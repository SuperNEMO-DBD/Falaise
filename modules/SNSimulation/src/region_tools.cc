/// \file mctools/region_tools.cc

// Ourselves:
#include <mctools/g4/region_tools.h>

// Standard library:
#include <sstream>

// Third party:
//- Boost:
#include <boost/regex.hpp>
//- Geant4:
#include <G4LogicalVolumeStore.hh>
#include <G4RegionStore.hh>
// - Bayeux/datatools :
#include <datatools/detail/command_macros.h>
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/materials :
#include <materials/manager.h>
// - Bayeux/geomtools :
#include <geomtools/materials_plugin.h>

namespace mctools {

namespace g4 {

region_info::region_info() { _initialized_ = false; }

region_info::~region_info() {
  if (is_initialized()) {
    reset();
  }
}

auto region_info::is_initialized() const -> bool { return _initialized_; }

void region_info::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Region is not initialized !");
  _initialized_ = false;
  _logical_ids_.clear();
  this->datatools::enriched_base::clear();
}

void region_info::initialize(const datatools::properties& config_,
                             const geomtools::manager* geom_manager_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Region is already initialized !");
  this->datatools::enriched_base::initialize(config_);

  std::set<std::string> the_log_volumes_regexp;

  // Fetch the explicit list of volumes belonging to this region:
  if (config_.has_key("volumes")) {
    std::vector<std::string> the_log_volumes;
    config_.fetch("volumes", the_log_volumes);
    for (const auto& log_name : the_log_volumes) {
      if (log_name != "world.log") {
        add_logical_volume(log_name);
      }
    }
  }

  // Fetch a list of regexps on volume names belonging to this region:
  if (config_.has_key("volumes_regexp")) {
    config_.fetch("volumes_regexp", the_log_volumes_regexp);
  }

  if (geom_manager_ != nullptr) {
    // Additional criteria to determine which volumes are in the region:
    // Only one of these criteria can be used:
    bool use_materials =
        false;  // volume use a specific material from an explicit list of materials
    bool use_density_range = false;  // volume use a material with density in a specific range
    bool use_state = false;  // volume use a material with a specific state (solid/liquid/gas)

    // Search for logical volumes with specific materials:
    std::vector<std::string> requested_material_refs;
    if (config_.has_key("materials")) {
      config_.fetch("materials", requested_material_refs);
      use_materials = true;
    }

    // Search for logical volumes with specific materials with density in a given range:
    double default_density_unit = CLHEP::g / CLHEP::cm3;
    double requested_material_min_density = datatools::invalid_real();
    double requested_material_max_density = datatools::invalid_real();
    if (!use_materials) {
      if (config_.has_key("material.max_density")) {
        requested_material_max_density = config_.fetch_real("material.max_density");
        if (!config_.has_explicit_unit("material.max_density")) {
          requested_material_max_density *= default_density_unit;
        }
        use_density_range = true;
      }
      if (config_.has_key("material.requested_material_min_density")) {
        requested_material_min_density = config_.fetch_real("material.min_density");
        if (!config_.has_explicit_unit("material.min_density")) {
          requested_material_min_density *= default_density_unit;
        }
        use_density_range = true;
      }
      if (use_density_range) {
        if (!datatools::is_valid(requested_material_min_density)) {
          requested_material_min_density = 0.0;
        }
        if (!datatools::is_valid(requested_material_max_density)) {
          datatools::plus_infinity(requested_material_max_density);
        }
        DT_THROW_IF(requested_material_min_density >= requested_material_max_density,
                    std::range_error,
                    "Invalid material density range ["
                        << requested_material_min_density / default_density_unit
                        << " >=" << requested_material_max_density / default_density_unit << "]!");
      }
    }

    // Search for logical volumes with materials with specific state:
    materials::material::state_type requested_material_state = materials::material::STATE_UNKNOWN;
    if (!use_materials && !use_density_range) {
      if (config_.has_key("material.state")) {
        use_state = true;
        std::string state_label = config_.fetch_string("material.state");
        requested_material_state = materials::material::label_to_state(state_label);
        DT_THROW_IF(requested_material_state != materials::material::STATE_UNKNOWN,
                    std::logic_error, "Invalid material state '" << state_label << "'!");
      }
    }

    const materials::manager* mat_mgr_ref = nullptr;
    {
      // Attempt to access to some embedded material manager from the geometry manager:
      std::string materials_plugin_name;
      DT_LOG_DEBUG(get_logging_priority(),
                   "Trying to find a materials plugin associated to the geometry manager...");
      using dict_type = geomtools::manager::plugins_dict_type;
      const dict_type& plugins = geom_manager_->get_plugins();
      for (const auto& plugin : plugins) {
        const std::string& plugin_name = plugin.first;
        if (geom_manager_->is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
          materials_plugin_name = plugin_name;
          break;
        }
      }
      // Access to a given plugin by name and type :
      if (geom_manager_->has_plugin(materials_plugin_name) &&
          geom_manager_->is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Found materials plugin named '" << materials_plugin_name << "'");
        const auto& mgp =
            geom_manager_->get_plugin<geomtools::materials_plugin>(materials_plugin_name);
        const materials::manager& mat_mgr = mgp.get_manager();
        mat_mgr_ref = &mat_mgr;
      }
      DT_THROW_IF((use_density_range || use_state) && mat_mgr_ref == nullptr, std::logic_error,
                  "Cannot find any material plugin in the geometry manager !");
    }

    // Search for logical volumes with specific traits:
    for (const auto& ilogical : geom_manager_->get_factory().get_logicals()) {
      // Get a reference to the associated logical volume :
      const std::string& log_name = ilogical.first;
      const geomtools::logical_volume& log = *(ilogical.second);
      bool add_it = false;

      // Process regex on logical volume names:
      for (const std::string& log_name_regex : the_log_volumes_regexp) {
        boost::regex e1(log_name_regex, boost::regex::extended);
        if (boost::regex_match(log_name, e1)) {
          // std::cerr << " ************* DEVEL ************ Logical '" << log_name << "' matches
          // the region regex '" << log_name_regex << "' \n";
          add_it = true;
        }
      }

      // Check if it is tagged with a material :
      std::string mr;
      if (log.has_material_ref()) {
        mr = log.get_material_ref();
      } else if (log.get_parameters().has_key("material.ref")) {
        mr = log.get_parameters().fetch_string("material.ref");
      }
      if (!mr.empty()) {
        if (use_materials) {
          // Check is a listed material matched the one in the logical volume:
          for (const auto& material_ref : requested_material_refs) {
            if (mr == material_ref) {
              if (log.get_name() != "world.log") {
                add_it = true;
                break;
              }
            }
          }
        } else {
          std::string effective_mat_ref = mr;
          if (mat_mgr_ref->is_alias(mr)) {
            effective_mat_ref = mat_mgr_ref->alias_of(mr);
          }
          DT_THROW_IF(!mat_mgr_ref->has_material(effective_mat_ref), std::logic_error,
                      "Unknown material '" << effective_mat_ref << "' in the material manager!");
          const materials::material& mat = mat_mgr_ref->get_material(effective_mat_ref);

          if (use_density_range) {
            // Check if the density of the material of the logical volume is in the range:
            bool valid_density = true;
            if (mat.get_density() < requested_material_min_density) {
              valid_density = false;
            }
            if (mat.get_density() > requested_material_max_density) {
              valid_density = false;
            }
            if (valid_density) {
              add_it = true;
            }
          } else if (use_state) {
            // Check if the state of the material of the logical volume matches the requested one:
            if (mat.get_state() == requested_material_state) {
              add_it = true;
            }
          }
        }
      }
      if (add_it && log.get_name() != "world.log") {
        // Never add the top-level logical volume in a region for Geant4 already
        // set the "DefaultRegionForWorld" region.
        add_logical_volume(log.get_name());
      }
    }  // End of loop on logical volumes.
  }    // Geometry manager was set.

  // Remove explicitely some logical volumes from the region if previously added
  // by one of the critera above:
  if (config_.has_key("volumes.excluded")) {
    // std::cerr << "DEVEL: Region '" << get_name() << "' : checking excluded volumes..." <<
    // std::endl;
    std::vector<std::string> excluded_volumes;
    config_.fetch("volumes.excluded", excluded_volumes);
    for (const auto& log_name : excluded_volumes) {
      // std::cerr << "DEVEL: Region '" << get_name() << "' : processing excluded volume '" <<
      // log_name << "' ..." << std::endl;
      if (has_logical_volume(log_name)) {
        // std::cerr << "DEVEL: Region '" << get_name() << "' : removing logical '" << log_name <<
        // "'..." << std::endl;
        remove_logical_volume(log_name);
      }
    }
  } else {
    // std::cerr << "DEVEL: Region '" << get_name() << "' : no excluded volumes." << std::endl;
  }
  // config_.tree_dump(std::cerr, "Region info config: ", "DEVEL: ");

  DT_THROW_IF(_logical_ids_.empty(), std::logic_error,
              "The region '" << get_name() << "' has no logical volumes !");

  // std::cerr << " ************* DEVEL ************ Region '" << get_name() << "' has '" <<
  // _logical_ids_.size() << "' logical volume(s). \n";

  /*
  if (_logical_ids_.size() == 0) {
    DT_LOG_WARNING(get_logging_priority(),
                   "The region '" << get_name() << "' has not logical volumes !");
  }
  */

  _initialized_ = true;
}

auto region_info::is_active() const -> bool {
  return is_initialized() && (static_cast<unsigned int>(!_logical_ids_.empty()) != 0U);
}

void region_info::add_logical_volume(const std::string& logical_id_, bool allow_duplicate_) {
  DT_THROW_IF(!allow_duplicate_ && _logical_ids_.count(logical_id_), std::logic_error,
              "Logical volume '" << logical_id_ << "' already belong to the region '" << get_name()
                                 << "'!");
  _logical_ids_.insert(logical_id_);
}

void region_info::remove_logical_volume(const std::string& logical_id_)

{
  DT_THROW_IF(!has_logical_volume(logical_id_), std::logic_error,
              "Region '" << get_name() << "' has no logical volume named '" << logical_id_ << "'!");
  _logical_ids_.erase(logical_id_);
}

auto region_info::has_logical_volume(const std::string& logical_id_) const -> bool {
  return _logical_ids_.count(logical_id_) > 0;
}

auto region_info::get_logical_ids() const -> const std::set<std::string>& { return _logical_ids_; }

auto region_info::grab_logical_ids() -> std::set<std::string>& { return _logical_ids_; }

void region_info::tree_dump(std::ostream& out_, const std::string& title_,
                            const std::string& indent_, bool inherit_) const {
  this->datatools::enriched_base::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Logical volumes : ";
  if (static_cast<unsigned int>(!_logical_ids_.empty()) != 0U) {
    out_ << '[' << _logical_ids_.size() << ']';
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;
  for (auto i = _logical_ids_.begin(); i != _logical_ids_.end(); i++) {
    auto j = i;
    j++;
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (j == _logical_ids_.end()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Logical volume '" << *i << "'" << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Initialized : " << (is_initialized() ? "Yes" : "No") << "" << std::endl;
}

// Regions setup

regions_setup::regions_setup() { _geom_manager_ = nullptr; }

regions_setup::~regions_setup() { reset(); }

void regions_setup::set_geometry_manager(const geomtools::manager& geom_mgr_) {
  _geom_manager_ = &geom_mgr_;
}

auto regions_setup::get_geometry_manager() const -> const geomtools::manager& {
  DT_THROW_IF(_geom_manager_ == nullptr, std::logic_error, "No geoemtry manager is referenced!");
  return *_geom_manager_;
}

auto regions_setup::has_region(const std::string& region_id_) const -> bool {
  return _region_infos_.find(region_id_) != _region_infos_.end();
}

void regions_setup::initialize(const datatools::properties& config_) {
  DT_THROW_IF(_geom_manager_ == nullptr, std::logic_error, "Missing referenced geometry manager!");

  std::vector<std::string> regions;
  if (config_.has_key("regions.names")) {
    config_.fetch("regions.names", regions);
  }
  for (const auto& the_region_name : regions) {
    DT_THROW_IF(_region_infos_.find(the_region_name) != _region_infos_.end(), std::logic_error,
                "Duplicate region '" << the_region_name << "'!");
    {
      region_info dummy;
      _region_infos_[the_region_name] = dummy;
    }
    region_info& ri = _region_infos_[the_region_name];
    ri.set_name(the_region_name);
    // ri.set_display_name(std::string("Region '") + the_region_name + "'");
    // ri.set_terse_description("A Geant4 region");
    datatools::properties region_config;
    std::ostringstream region_key_prefix;
    region_key_prefix << "regions." << the_region_name << '.';
    config_.export_and_rename_starting_with(region_config, region_key_prefix.str(), "");
    ri.initialize(region_config, _geom_manager_);
    // ri.tree_dump(std::cerr, "Region (2): ", "DEVEL: ");
  }
  datatools::command::returned_info cri = _check();
  DT_THROW_IF(cri.is_failure(), std::logic_error, cri.get_error_message());
}

auto regions_setup::_check() -> datatools::command::returned_info {
  datatools::command::returned_info cri;
  std::set<std::string> associated_logicals;
  std::map<std::string, std::string> logical_to_region_associations;
  for (region_infos_dict_type::const_iterator i = _region_infos_.begin(); i != _region_infos_.end();
       i++) {
    const std::string& the_region_name = i->first;
    for (const auto& log_id : i->second.get_logical_ids()) {
      if (associated_logicals.count(log_id) > 0) {
        // Logical volume already belongs to another region:
        DT_COMMAND_RETURNED_ERROR(cri, datatools::command::CEC_FAILURE,
                                  "Logical volume '"
                                      << log_id << "' associated to region '" << the_region_name
                                      << "' is already associated to region '"
                                      << logical_to_region_associations[log_id] << "'!");
        break;
      }
      associated_logicals.insert(log_id);
      logical_to_region_associations[log_id] = the_region_name;
    }
    if (cri.is_failure()) {
      break;
    }
  }
  return cri;
}

void regions_setup::reset() {
  _region_infos_.clear();
  _geom_manager_ = nullptr;
}

auto regions_setup::get_region_infos() const -> const region_infos_dict_type& {
  return _region_infos_;
}

auto regions_setup::grab_region_infos() -> region_infos_dict_type& { return _region_infos_; }

void regions_setup::tree_dump(std::ostream& out_, const std::string& title_,
                              const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Region infos: ";
  if (static_cast<unsigned int>(!_region_infos_.empty()) != 0U) {
    out_ << '[' << _region_infos_.size() << ']';
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;
  for (auto i = _region_infos_.begin(); i != _region_infos_.end(); i++) {
    auto j = i;
    j++;
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    std::ostringstream indent2;
    indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (j == _region_infos_.end()) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "Region '" << i->first << "'" << std::endl;
    /*
    std::ostringstream indent2;
    indent2 << indent_;
    indent2 << datatools::i_tree_dumpable::skip_tag;
    if (j == _region_infos_.end()) {
      indent2 << datatools::i_tree_dumpable::last_tag;
    } else {
      indent2 << datatools::i_tree_dumpable::tag;
    }
    */
    i->second.tree_dump(out_, "", indent2.str());
  }

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Geometry manager: "
       << "[@" << _geom_manager_ << "]" << std::endl;
}

/*
// static
bool region_info::validate_for_geant4(const region_infos_dict_type & region_infos_)
{
bool valid = true;
for (region_infos_dict_type::const_iterator i = region_infos_.begin();
i != region_infos_.end();
i++) {
const std::string & rname = i->first;
const region_info & ri = i->second;


}
return valid;
}

*/
}  // end of namespace g4

}  // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::region_info, ocd_) {
  // The class name :
  ocd_.set_class_name("mctools::g4::region_info");

  // The class terse description :
  ocd_.set_class_description("The description of a Geant4 region attached to the virtual geometry");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation(
      "The Geant4 simulation engine/manager is able to define *region* objects       \n"
      "that are attached to the virtual geometry setup.                              \n"
      "A region consists in a set of logical volumes that are considered to share    \n"
      "some common traits (being parts of the same devices, having similar           \n"
      "physical characteristics...). This concept is generally used to setup         \n"
      "specific behaviour of the tracking depending of the regions traversed by      \n"
      "tracked particles: set special cuts on steps, tracks, generation of secondary \n"
      "particles, set special electromagnetic fields or processes...).               \n"
      "Practically a region is explicitely defined by a set of logical volumes in    \n"
      "the setup. However the present class is equipped with a high-level interface  \n"
      "which allows users to define a region from various useful criteria:           \n"
      "                                                                              \n"
      " * Explicit list of logical volumes (given by names) the user must manually   \n"
      "   specify the list of volumes to be added in the region.                     \n"
      " * Explicit list of materials (given by names) of which the volumes in the    \n"
      "   region are made of.                                                        \n"
      " * Explicit state of the material of which the volumes in the region are made \n"
      "   of. Supported values are: ``\"solid\"``, ``\"liquid\"``, ``\"gas\"``.      \n"
      " * Explicit density range of the material of which the volumes in the region  \n"
      "   are made of.                                                               \n"
      "                                                                              \n"
      "Given one of the criteria above to build a region, it is still possible to    \n"
      "explicitely remove some extra logical volumes that turns to be excluded from  \n"
      "a region.                                                                     \n"
      "                                                                              \n"
      "Note that the top-level *world* volume must not be set to any user region.    \n"
      "Also a logical volume must not belongs to differents regions.                 \n");

  logger::declare_ocd_logging_configuration(ocd_, "");

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Typical configurations for regions follows.                        \n"
      "                                                                   \n"
      " * A region defined by explicit list of volumes: ::                \n"
      "                                                                   \n"
      "     volumes : string[2] = \"tracker.log\" \"lab.log\"             \n"
      "                                                                   \n"
      "At user's discretion, one of the following criteria can also be    \n"
      "used to set the list of volumes belonging to the region:           \n"
      "                                                                   \n"
      " * Add volumes with specific materials: ::                         \n"
      "                                                                   \n"
      "     materials :  string[2] = \"copper\" \"iron\"                  \n"
      "                                                                   \n"
      " * Add volumes with low density materials: ::                      \n"
      "                                                                   \n"
      "     material.max_density : real = 0.1 g/cm3                       \n"
      "                                                                   \n"
      " * Add volumes with high density materials: ::                     \n"
      "                                                                   \n"
      "     material.min_density : real = 3.0 g/cm3                       \n"
      "                                                                   \n"
      " * Add volumes with materials in a range of density: ::            \n"
      "                                                                   \n"
      "     material.min_density : real = 1.2 g/cm3                       \n"
      "     material.min_density : real = 3.4 g/cm3                       \n"
      "                                                                   \n"
      " * Add volumes with given state excluding specific volumes: ::     \n"
      "                                                                   \n"
      "     material.state : string = \"gas\"                             \n"
      "     volumes.excluded : string[2] = \"screw.log\" \"muvertex.log\" \n"
      "                                                                   \n");

  ocd_.set_validation_support(false);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'mctools::g4::region_info' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::region_info, "mctools::g4::region_info")
