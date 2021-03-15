/// \file mctools/detector_construction.cc

// Ourselves:
#include <snsim/detector_construction.h>

// Standard library:
#include <clocale>

// Third party:
// - Boost:
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/units.h>
#include <bayeux/datatools/utils.h>
// - Bayeux/mygsl :
#include <bayeux/mygsl/random_utils.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/color.h>
#include <bayeux/geomtools/gdml_export.h>
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/materials_plugin.h>
#include <bayeux/geomtools/materials_utils.h>
#include <bayeux/geomtools/resource.h>
#include <bayeux/geomtools/sensitive.h>
#include <bayeux/geomtools/visibility.h>
// - Bayeux/emfield :
#include <bayeux/emfield/base_electromagnetic_field.h>
#include <bayeux/emfield/electromagnetic_field_manager.h>
#include <bayeux/emfield/emfield_geom_plugin.h>
#include <bayeux/emfield/geom_map.h>

// G4 stuff:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#include <G4ChordFinder.hh>
#include <G4Color.hh>
#include <G4FieldManager.hh>
#include <G4GDMLParser.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4RegionStore.hh>
#include <G4SDManager.hh>
#include <G4TransportationManager.hh>
#include <G4UserLimits.hh>
#include <G4VisAttributes.hh>
#include <globals.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
// This project:
#include <snsim/biasing_manager.h>
#include <snsim/electromagnetic_field.h>
#include <snsim/em_field_g4_stuff.h>
#include <snsim/magnetic_field.h>
#include <snsim/manager.h>
#include <snsim/sensitive_detector.h>

namespace snsim {

const double detector_construction::DEFAULT_MISS_DISTANCE = 0.5 * CLHEP::mm;

auto detector_construction::get_sensitive_detectors() const
    -> const detector_construction::sensitive_detector_dict_type & {
  return _sensitive_detectors_;
}

auto detector_construction::grab_step_hit_processor_factory()
    -> detector_construction::SHPF_type & {
  return _SHPF_;
}

auto detector_construction::get_manager() const -> const manager & {
  DT_THROW_IF(_g4_manager_ == nullptr, std::logic_error, "Operation prohibited !");
  return *_g4_manager_;
}

auto detector_construction::grab_manager() -> manager & {
  DT_THROW_IF(_g4_manager_ == nullptr, std::logic_error, "Operation prohibited !");
  return *_g4_manager_;
}

auto detector_construction::get_geometry_manager() const -> const geomtools::manager & {
  return *_geom_manager_;
}

void detector_construction::_set_default() {
  _abort_on_error_ = true;

  _g4_manager_ = nullptr;
  _geom_manager_ = nullptr;
  _materials_geom_plugin_name_ = "";

  _generate_gdml_file_ = true;
  _gdml_filename_ = "";
  _gdml_file_dir_ = "";
  _gdml_schema_ = geomtools::gdml_writer::default_remote_gdml_schema();
  _gdml_validation_ = true;

  _using_vis_attributes_ = true;
  _using_user_limits_ = true;
  _using_regions_ = true;
  _using_sensitive_detectors_ = true;
  _using_em_field_ = false;
  _using_biasing_ = false;
  _em_field_manager_ = nullptr;
  _miss_distance_unit_ = CLHEP::mm;
  _general_miss_distance_ = DEFAULT_MISS_DISTANCE;
  _SD_params_.set_key_label("name");
  _SD_params_.set_meta_label("type");
  _SD_params_.set_description("Sensitive detectors' configuration (snsim::detector_construction)");
}

detector_construction::detector_construction() {
  _initialized_ = false;
  _set_default();
  _g4_manager_ = nullptr;
}

detector_construction::detector_construction(manager &g4_mgr_) {
  _initialized_ = false;
  _set_default();
  _g4_manager_ = &g4_mgr_;
}

detector_construction::~detector_construction() {
  if (is_initialized()) {
    reset();
  }
}

auto detector_construction::is_initialized() const -> bool { return _initialized_; }

auto detector_construction::has_geometry_manager() const -> bool {
  return _geom_manager_ != nullptr;
}

void detector_construction::set_geometry_manager(const geomtools::manager &gm_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Operation prohibited !");
  _geom_manager_ = &gm_;
}

void detector_construction::reset() {
  DT_LOG_TRACE(_logprio(), "Entering...");
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
  _initialized_ = false;

  if (_biasing_manager_) {
    DT_LOG_TRACE(_logprio(), "Reset biaising manager...");
    _biasing_manager_.reset();
    DT_LOG_TRACE(_logprio(), "Reset biaising manager... done.");
  }

  // Destroy G4 EM field data
  DT_LOG_TRACE(_logprio(), "Destroy electromagnetic field support...");
  _destroy_electromagnetic_field();
  DT_LOG_TRACE(_logprio(), "Destroy electromagnetic field support... done.");

  // Clear visualization attributes:
  DT_LOG_TRACE(_logprio(), "Clear visualization attributes...");
  for (auto &_vis_attribute : _vis_attributes_) {
    if (_vis_attribute.second != 0) {
      delete _vis_attribute.second;
      _vis_attribute.second = 0;
    }
  }
  _vis_attributes_.clear();
  DT_LOG_TRACE(_logprio(), "Clear visualization attributes... done.");

  // Clear user limits:
  DT_LOG_TRACE(_logprio(), "Clear user limits...");
  for (auto &i : _user_limits_col_) {
    delete i;
  }
  _user_limits_col_.clear();
  _SD_params_.reset();
  DT_LOG_TRACE(_logprio(), "Clear user limits... done.");

  _set_default();

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void detector_construction::initialize(const datatools::properties &config_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  /*** parsing properties from 'config_'  ***/
  loggable_support::_initialize_logging_support(config_);

  if (config_.has_key("gdml.tmp_dir")) {
    std::string gdml_tmp_dir = config_.fetch_string("gdml.tmp_dir");
    datatools::fetch_path_with_env(gdml_tmp_dir);
    _gdml_file_dir_ = gdml_tmp_dir;
  }

  if (config_.has_key("gdml.validation")) {
    _gdml_validation_ = config_.fetch_boolean("gdml.validation");
  }

  if (config_.has_key("gdml.schema_location")) {
    std::string gdml_schema_location = config_.fetch_string("gdml.schema_location");
    if (gdml_schema_location == "remote") {
      _gdml_schema_ = geomtools::gdml_writer::default_remote_gdml_schema();
    } else if (gdml_schema_location == "local") {
      _gdml_schema_ = geomtools::get_resource("gdml_schema/gdml.xsd", true);
      datatools::fetch_path_with_env(_gdml_schema_);
    } else {
      DT_THROW_IF(true, std::logic_error,
                  "Missing property '"
                      << "gdml.schema_location"
                      << "' !");
    }
  }

  if (config_.has_key("materials.plugin_name")) {
    std::string mpn = config_.fetch_string("materials.plugin_name");
    set_materials_geom_plugin_name(mpn);
  }

  config_.export_and_rename_starting_with(_SHPF_config_, "hit_processor_factory.", "");
  DT_THROW_IF(_SHPF_config_.empty(), std::logic_error,
              "Missing property '"
                  << "hit_processor_factory"
                  << "' !");

  if (config_.has_key("using_user_limits")) {
    _using_user_limits_ = config_.fetch_boolean("using_user_limits");
  }

  config_.export_starting_with(_limits_config_, "limits.");

  // end of fetching.

  DT_THROW_IF(_geom_manager_ == nullptr, std::logic_error, "Missing geometry manager !");

  /*************************
   *                       *
   *  Sensitive detectors  *
   *                       *
   *************************/

  if (config_.has_key("sensitive.definitions")) {
    std::vector<std::string> sd_defs;
    config_.fetch("sensitive.definitions", sd_defs);
    for (auto sd_def : sd_defs) {
      datatools::fetch_path_with_env(sd_def);
      _SD_params_.read(sd_def);
    }
  }

  if (config_.has_key("sensitive.detectors")) {
    std::vector<std::string> list_of_sensitive_detectors;
    config_.fetch("sensitive.detectors", list_of_sensitive_detectors);

    // Loop on the list of official sensitive categories/detectors :
    for (const auto &SD_category : list_of_sensitive_detectors) {
      if (!_SD_params_.has_key(SD_category)) {
        _SD_params_.add(SD_category, "SD");
      }

      // Extract properties starting with the
      // 'sensitive.CATEGORY_SD' prefix and store them with a new
      // prefix starting only with 'sensitive.'
      const std::string key_oss = "sensitive." + SD_category;
      const std::string new_key_oss = "sensitive";
      config_.export_and_rename_starting_with(_SD_params_.grab(SD_category).grab_properties(),
                                              key_oss, new_key_oss);
    }  // for

    DT_LOG_NOTICE(_logprio(), "Info for sensitive detectors : ");
    for (std::vector<std::string>::const_iterator i = list_of_sensitive_detectors.begin();
         i != list_of_sensitive_detectors.end(); ++i) {
      auto j = i;
      if (++j == list_of_sensitive_detectors.end()) {
        DT_LOG_NOTICE(_logprio(), "`-- "
                                      << "Sensitive detectors '" << *i << "'");
      } else {
        DT_LOG_NOTICE(_logprio(), "|-- "
                                      << "Sensitive detectors '" << *i << "'");
      }
      if (is_debug()) {
        DT_LOG_DEBUG(_logprio(), "    ==> "
                                     << "Sensitive parameters for '" << *i << "' : ");
        _SD_params_.get(*i).get_properties().tree_dump(std::clog, "", "[debug]: ");
      }
    }
  }

  /*************************
   *                       *
   *        Regions        *
   *                       *
   *************************/

  if (config_.has_key("using_regions")) {
    _using_regions_ = config_.fetch_boolean("using_regions");
  }

  datatools::properties regions_setup_config;
  if (config_.has_key("regions")) {
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                   "Obsolete 'regions' property, please use 'regions.names'!");
    std::vector<std::string> region_names;
    config_.fetch("regions", region_names);
    regions_setup_config.store("regions.names", region_names);
  }
  config_.export_starting_with(regions_setup_config, "regions.");
  _region_definitions_.set_geometry_manager(*_geom_manager_);
  _region_definitions_.initialize(regions_setup_config);
  DT_LOG_NOTICE(_logprio(), "Definitions of regions: ");
  if (_logprio() >= datatools::logger::PRIO_NOTICE) {
    _region_definitions_.tree_dump(std::clog, "", "[notice]: ");
  }

  /*************************
   *                       *
   *    Magnetic field     *
   *                       *
   *************************/

  if (config_.has_key("using_electromagnetic_field")) {
    _using_em_field_ = config_.fetch_boolean("using_electromagnetic_field");
    DT_LOG_DEBUG(_logprio(), "Using electromagnetic field(s) : " << _using_em_field_);
  } else if (config_.has_key("using_magnetic_field")) {
    // Obsolete:
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                   "Configuration property 'using_magnetic_field' is obsolete, use "
                   "'using_electromagnetic_field'!");
    _using_em_field_ = config_.fetch_boolean("using_magnetic_field");
  }

  if (_using_em_field_) {
    DT_LOG_DEBUG(_logprio(), "Using electromagnetic field...");

    if (config_.has_key("electromagnetic_field.plugin_name")) {
      std::string fpn = config_.fetch_string("electromagnetic_field.plugin_name");
      set_emfield_geom_plugin_name(fpn);
    } else if (config_.has_key("magnetic_field.plugin_name")) {
      // Obsolete:
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                     "Configuration property 'magnetic_field.plugin_name' is obsolete, use "
                     "'electromagnetic_field.plugin_name'!");
      _using_em_field_ = config_.fetch_boolean("using_magnetic_field");
      std::string fpn = config_.fetch_string("magnetic_field.plugin_name");
      set_emfield_geom_plugin_name(fpn);
    }

    DT_LOG_DEBUG(_logprio(),
                 "Electromagnetic fields geometry plugin = '" << _emfield_geom_plugin_name_ << "'");

    if (config_.has_key("electromagnetic_field.miss_distance.unit")) {
      std::string md_unit_str = config_.fetch_string("electromagnetic_field.miss_distance.unit");
      _miss_distance_unit_ = datatools::units::get_length_unit_from(md_unit_str);
    } else if (config_.has_key("magnetic_field.miss_distance.unit")) {
      // Obsolete:
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                     "Configuration property 'magnetic_field.miss_distance.unit' is obsolete, use "
                     "'electromagnetic_field.miss_distance.unit'!");
      std::string md_unit_str = config_.fetch_string("magnetic_field.miss_distance.unit");
      _miss_distance_unit_ = datatools::units::get_length_unit_from(md_unit_str);
    }

    if (config_.has_key("electromagnetic_field.miss_distance")) {
      _general_miss_distance_ = config_.fetch_real("electromagnetic_field.miss_distance");
      if (!config_.has_explicit_unit("electromagnetic_field.miss_distance")) {
        _general_miss_distance_ *= _miss_distance_unit_;
      }
    } else if (config_.has_key("magnetic_field.miss_distance")) {
      // Obsolete:
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                     "Configuration property 'magnetic_field.miss_distance' is obsolete, use "
                     "'electromagnetic_field.miss_distance'!");
      _general_miss_distance_ = config_.fetch_real("magnetic_field.miss_distance");
      if (!config_.has_explicit_unit("magnetic_field.miss_distance")) {
        _general_miss_distance_ *= _miss_distance_unit_;
      }
    }
    DT_LOG_DEBUG(_logprio(),
                 "General miss distance = " << _general_miss_distance_ / CLHEP::mm << " mm");

    const geomtools::manager &geo_mgr = get_geometry_manager();
    std::string emfield_geom_plugin_name = _emfield_geom_plugin_name_;
    if (_emfield_geom_plugin_name_.empty()) {
      // We try to find a emfield plugin associated to the geometry manager :
      using dict_type = geomtools::manager::plugins_dict_type;
      const dict_type &plugins = geo_mgr.get_plugins();
      for (const auto &plugin : plugins) {
        const std::string &plugin_name = plugin.first;
        if (geo_mgr.is_plugin_a<emfield::emfield_geom_plugin>(plugin_name)) {
          emfield_geom_plugin_name = plugin_name;
          DT_LOG_DEBUG(_logprio(), "Found EM field plugin = '" << emfield_geom_plugin_name << "'");
          break;
        }
      }
    }
    DT_THROW_IF(_emfield_geom_plugin_name_.empty(), std::logic_error,
                "No geometry EM field plugin name is provided!");
    // Access to a given plugin by name and type :
    DT_LOG_DEBUG(_logprio(), "Trying to access to an EM field geometry plugin with name '"
                                 << emfield_geom_plugin_name << "'");
    DT_LOG_DEBUG(_logprio(), "A geometry plugin with name '"
                                 << emfield_geom_plugin_name
                                 << "' exists: " << geo_mgr.has_plugin(emfield_geom_plugin_name));
    DT_LOG_DEBUG(_logprio(), "Geometry plugin named '"
                                 << emfield_geom_plugin_name << "' is a emfield_geom_plugin: "
                                 << geo_mgr.is_plugin_a<emfield::emfield_geom_plugin>(
                                        emfield_geom_plugin_name));
    if (geo_mgr.has_plugin(emfield_geom_plugin_name) &&
        geo_mgr.is_plugin_a<emfield::emfield_geom_plugin>(emfield_geom_plugin_name)) {
      DT_LOG_DEBUG(_logprio(),
                   "Found EM field geometry plugin with name '" << emfield_geom_plugin_name << "'");
      DT_LOG_NOTICE(_logprio(),
                    "Found EM field geometry plugin named '" << emfield_geom_plugin_name << "'");
      const auto &fgp = geo_mgr.get_plugin<emfield::emfield_geom_plugin>(emfield_geom_plugin_name);
      const emfield::electromagnetic_field_manager &emf_mgr = fgp.get_manager();
      DT_LOG_TRACE(_logprio(), "EM field manager: ");
      if (_logprio() >= datatools::logger::PRIO_TRACE) {
        emf_mgr.tree_dump(std::clog, "", "[trace]: ");
      }
      set_em_field_manager(emf_mgr);
    }

    _em_field_aux_.clear();
    config_.export_and_rename_starting_with(_em_field_aux_, "electromagnetic_field.", "");
    if (_em_field_aux_.empty()) {
      // Try with the legacy (obsolete) prefix if the first one was sterile...
      config_.export_and_rename_starting_with(_em_field_aux_, "magnetic_field.",
                                              "");  // to be obsoleted
    }
    DT_LOG_NOTICE(_logprio(), "Electromagnetic field support is configured.");
  }

  /******************
   *                *
   *    Biasing     *
   *                *
   ******************/

  if (config_.has_key("using_biasing")) {
    _using_biasing_ = config_.fetch_boolean("using_biasing");
  }

  if (_using_biasing_) {
    config_.export_and_rename_starting_with(_biasing_config_, "biasing.", "");
    DT_THROW_IF(_biasing_config_.empty(), std::logic_error,
                "Missing property '"
                    << "biasing"
                    << "' !");
  }

  _initialized_ = true;
}

auto detector_construction::Construct() -> G4VPhysicalVolume * {
  DT_LOG_TRACE(_logprio(), "Entering...");

  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  G4VPhysicalVolume *world_physical_volume = nullptr;

  world_physical_volume = _g4_construct();

  if (world_physical_volume == nullptr) {
    G4Exception("snsim::detector_construction::Construct", "GeometryError", FatalException,
                "Missing world physical volume !");
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
  return world_physical_volume;
}

auto detector_construction::_g4_construct() -> G4VPhysicalVolume * {
  DT_LOG_TRACE(_logprio(), "Entering...");

  // Automaticaly construct the geometry tree:
  G4VPhysicalVolume *world_physical_volume = nullptr;
  // try {
  std::setlocale(LC_NUMERIC, "C");
  G4GDMLParser parser;
  parser.SetStripFlag(false);
  if (_generate_gdml_file_) {
    write_tmp_gdml_file();
    parser.Read(_gdml_filename_, _gdml_validation_);
  } else {
    parser.Read("test_manager.gdml");
  }
  world_physical_volume = parser.GetWorldVolume();
  // }
  // catch (std::exception & g4_error) {
  //   DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
  //                "GDML Parser failed: " << g4_error.what());
  //   throw;
  // }

  if (is_debug()) {
    DT_LOG_DEBUG(_logprio(), "G4LogicalVolumeStore: ");
    G4LogicalVolumeStore *g4_LV_store = G4LogicalVolumeStore::GetInstance();
    for (G4LogicalVolumeStore::const_iterator i = g4_LV_store->begin(); i != g4_LV_store->end();
         ++i) {
      G4LogicalVolume *g4_log = *i;
      DT_LOG_DEBUG(_logprio(), "Geant4 logical: '" << g4_log->GetName() << "'");
    }
  }

  // Automaticaly construct regions:
  if (_using_regions_) {
    DT_LOG_DEBUG(_logprio(), "Constructing regions...");
    _construct_regions();
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not construct any regions.");
  }

  // Automaticaly construct sensitive detectors:
  if (_using_sensitive_detectors_) {
    DT_LOG_DEBUG(_logprio(), "Constructing sensitive detectors...");
    _construct_sensitive_detectors();
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not construct any sensitive detectors.");
  }

  // Automaticaly construct the electromagnetic field :
  if (_using_em_field_) {
    DT_LOG_DEBUG(_logprio(), "Constructing the electromagnetic field(s)...");
    _construct_electromagnetic_field();
    DT_LOG_DEBUG(_logprio(), "Electromagnetic fields are constructed.");
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not construct any electromagnetic field(s).");
  }

  // Automaticaly set the visibility attributes:
  if (_using_vis_attributes_) {
    DT_LOG_DEBUG(_logprio(), "Set the visualization attributes.");
    _set_visualization_attributes();
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not use any visualization attributes.");
  }

  // Automaticaly set the user limits:
  if (_using_user_limits_) {
    DT_LOG_DEBUG(_logprio(), "Using the user limits(max step)...");
    _set_user_limits();
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not construct any user limits.");
  }

  // Automaticaly construct the biasing algos :
  if (_using_biasing_) {
    DT_LOG_DEBUG(_logprio(), "Construct the biasing algorithms.");
    _construct_biasing();
  } else {
    DT_LOG_DEBUG(_logprio(), "Do not use any biasing algorithms.");
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
  return world_physical_volume;
}

auto detector_construction::has_em_field_manager() const -> bool {
  return _em_field_manager_ != nullptr;
}

auto detector_construction::has_mag_field_manager() const -> bool { return has_em_field_manager(); }

void detector_construction::set_mag_field_manager(
    const emfield::electromagnetic_field_manager &emf_mgr_) {
  set_em_field_manager(emf_mgr_);
}

void detector_construction::set_em_field_manager(
    const emfield::electromagnetic_field_manager &emf_mgr_) {
  DT_THROW_IF(_initialized_, std::logic_error, "Already initialized !");
  _em_field_manager_ = &emf_mgr_;
}

auto detector_construction::get_mag_field_manager() const
    -> const emfield::electromagnetic_field_manager & {
  return get_em_field_manager();
}

auto detector_construction::get_em_field_manager() const
    -> const emfield::electromagnetic_field_manager & {
  DT_THROW_IF(_em_field_manager_ == nullptr, std::logic_error,
              "No electromagnetic field manager is defined !");
  return *_em_field_manager_;
}

void detector_construction::_construct_biasing() {
  _biasing_manager_.reset(new biasing_manager);
  _biasing_manager_->set_detector_construction(*this);
  _biasing_manager_->initialize(_biasing_config_);
}

void detector_construction::_destroy_electromagnetic_field() {
  for (auto i = _em_field_g4_data_.begin(); i != _em_field_g4_data_.end(); i++) {
    DT_LOG_TRACE(_logprio(), "Destroy G4 support for the '"
                                 << i->first << "' geometry/EM-field association...");
    em_field_g4_stuff *emf_working = i->second;
    if (emf_working != nullptr) {
      if (emf_working->is_initialized()) {
        emf_working->reset();
      }
      delete emf_working;
    }
    DT_LOG_TRACE(_logprio(), "Destroy G4 support for the '"
                                 << i->first << "' geometry/EM-field association... done.");
  }
  _em_field_g4_data_.clear();
}

void detector_construction::_construct_electromagnetic_field() {
  datatools::logger::priority logging = _logprio();
  DT_LOG_TRACE_ENTERING(logging);
  DT_LOG_DEBUG(logging, "Has EM field manager : " << has_em_field_manager());
  DT_LOG_DEBUG(logging, "Has EM field geom map : " << (has_em_field_manager() &&
                                                       _em_field_manager_->has_geom_map()));

  if (has_em_field_manager() && _em_field_manager_->has_geom_map()) {
    DT_LOG_NOTICE(logging, "Processing geometry/EM-field association map...");

    G4LogicalVolumeStore *g4_LV_store = G4LogicalVolumeStore::GetInstance();
    const emfield::geom_map &geomap = _em_field_manager_->get_geom_map();

    using gma_type = emfield::geom_map::association_dict_type;
    using gma_entry_type = emfield::geom_map::association_entry;
    const gma_type &mfamap = geomap.get_associations();
    for (const auto &i : mfamap) {
      const std::string &association_name = i.first;
      const gma_entry_type &gefa = i.second;
      if (!gefa.has_logvol()) {
        std::ostringstream message;
        message << "Missing G4 logical volume for geom/EM-field association '" << association_name
                << "' !";
        if (_abort_on_error_) {
          DT_THROW_IF(true, std::logic_error, message.str());
        }
        continue;
      }
      G4String g4_log_name = gefa.get_logical_volume_name().c_str();
      G4LogicalVolume *g4_module_log = g4_LV_store->GetVolume(g4_log_name, false);
      if (g4_module_log == nullptr) {
        std::ostringstream message;
        message << "Missing G4 logical volume with name '" << g4_log_name << "' !";
        if (_abort_on_error_) {
          DT_THROW(std::logic_error, message.str());
        }
        DT_LOG_WARNING(logging, message.str());
        continue;
      }
      DT_LOG_NOTICE(logging, " -> G4 logical volume '"
                                 << g4_log_name << "' has an electromagnetic field labelled '"
                                 << gefa.get_field_name() << "'.");

      // Create a new data structure for the handling of G4 EM field
      // associated to this geom/field association...
      {
        auto *new_emf_working = new em_field_g4_stuff;
        _em_field_g4_data_[association_name] = new_emf_working;
      }
      em_field_g4_stuff *emf_working = _em_field_g4_data_.find(association_name)->second;
      if (!gefa.field->is_magnetic_field() && !gefa.field->is_electric_field()) {
        DT_THROW(std::logic_error,
                 "Field '" << association_name << "' is not an electromagnetic field !");
      }
      if (gefa.field->is_magnetic_field() && !gefa.field->is_electric_field()) {
        // Magnetic field only:
        auto *b_field = new magnetic_field;
        b_field->set_name(association_name);
        b_field->set_field(*gefa.field);
        datatools::properties field_config;
        _em_field_aux_.export_and_rename_starting_with(field_config, association_name + ".", "");
        b_field->initialize(field_config);
        emf_working->set_g4_magnetic_field(b_field);
        emf_working->initialize(field_config);
        DT_LOG_NOTICE(
            logging, "Initializing pure magnetic field with label '" << association_name << "'...");
      } else {
        // General case: electromagnetic field
        auto *eb_field = new electromagnetic_field;
        eb_field->set_name(association_name);
        eb_field->set_field(*gefa.field);
        datatools::properties field_config;
        _em_field_aux_.export_and_rename_starting_with(field_config, association_name + ".", "");
        eb_field->initialize(field_config);
        emf_working->set_g4_electromagnetic_field(eb_field);
        emf_working->initialize(field_config);
        DT_LOG_NOTICE(logging, "Initializing general electromagnetic field with label '"
                                   << association_name << "'...");
      }

      // Use the default miss distance :
      double miss_distance = _general_miss_distance_;
      std::string md_log_key = "miss_distance." + association_name;
      if (_em_field_aux_.has_key(md_log_key)) {
        miss_distance = _em_field_aux_.fetch_real(md_log_key);
        if (!_em_field_aux_.has_explicit_unit(md_log_key)) {
          miss_distance *= _miss_distance_unit_;
        }
      }
      DT_LOG_TRACE(logging, "EM field stuff: ");
      if (logging >= datatools::logger::PRIO_TRACE) {
        emf_working->tree_dump(std::clog, "", "[trace]: ");
      }
      g4_module_log->SetFieldManager(emf_working->grab_field_manager(),
                                     emf_working->is_propagate_to_daughters());
      DT_LOG_NOTICE(logging, "G4 logical volume '" << g4_log_name
                                                   << "' has an electromagnetic field named '"
                                                   << gefa.get_field_name() << "' !");
      DT_LOG_NOTICE(logging, "Miss distance is " << miss_distance / CLHEP::mm << " mm");
    }
    DT_LOG_NOTICE(logging, "Geometry/EM field map has been processed.");
  }

  DT_LOG_TRACE_EXITING(logging);
}

void detector_construction::_construct_sensitive_detectors() {
  DT_LOG_TRACE_ENTERING(_logprio());
  G4SDManager *SDman = G4SDManager::GetSDMpointer();
  G4LogicalVolumeStore *g4_LV_store = G4LogicalVolumeStore::GetInstance();

  /*********************************************
   * Setup the "offical" sensitive categories  *
   *********************************************/

  /// The association between logical volumes and sensitive category
  std::map<std::string, std::string> lv_sd_associations;

  // Loop on sensitive logical volumes :
  for (const auto &ilogical : _geom_manager_->get_factory().get_logicals()) {
    // Get a reference to the associated logical volume :
    const geomtools::logical_volume &log = *(ilogical.second);
    // Search a "sensitive.category" from the geometry definition of the volume:
    if (geomtools::sensitive::is_sensitive(log.get_parameters())) {
      DT_LOG_DEBUG(_logprio(), "Logical volume '" << log.get_name() << "' is sensitive !");
      lv_sd_associations[log.get_name()] =
          geomtools::sensitive::get_sensitive_category(log.get_parameters());
    }
  }

  // Loop on the definitions of official sensitive categories:
  for (const auto &i : _SD_params_.entries()) {
    const std::string &sensitive_category_name = i.first;
    const datatools::multi_properties::entry e = i.second;
    const datatools::properties &sd_config = e.get_properties();
    if (sd_config.has_key("sensitive.volumes")) {
      std::vector<std::string> lv_names;
      sd_config.fetch("sensitive.volumes", lv_names);
      for (const auto &lv_name : lv_names) {
        // Check if the logical volume association matches the proper SD configuration
        DT_THROW_IF(lv_sd_associations.find(lv_name) != lv_sd_associations.end() &&
                        lv_sd_associations[lv_name] != sensitive_category_name,
                    std::logic_error,
                    "Logical volume '" << lv_name << "' is already associated to category '"
                                       << lv_sd_associations[lv_name] << "'!");
        lv_sd_associations[lv_name] = sensitive_category_name;
      }
    }
  }

  // Here the 'lv_sd_associations' dictionary should describe all
  // official "logical_volume->sensitive category" associations...

  // Check that defined associations are valid:
  for (std::map<std::string, std::string>::const_iterator i = lv_sd_associations.begin();
       i != lv_sd_associations.end(); i++) {
    const std::string &lv_name = i->first;
    const std::string &sd_name = i->second;
    // std::cerr << "DEVEL: "
    //           << "detector_construction::_construct_sensitive_detectors: "
    //           << "Logical='" << lv_name << "' Sensitive='" << sd_name << "'"
    //           << std::endl;
    // We throw if some logical volumes do not exist and thus cannot be associated to
    // a given requested sensitive category (error in some configuration file):
    DT_THROW_IF(_geom_manager_->get_factory().get_logicals().find(lv_name) ==
                    _geom_manager_->get_factory().get_logicals().end(),
                std::logic_error,
                "Logical volume '"
                    << lv_name
                    << "' does not exists (supposed to be associated to sensitive category '"
                    << sd_name << "'!");
    // DT_THROW_IF(! _SD_params_.has_key(sd_name),
    //             std::logic_error,
    //             "Sensitive category '" << sd_name << "' does not exists (supposed to be
    //             associated to logical volume '"
    //             << lv_name << "'!");
    if (!_SD_params_.has_key(sd_name)) {
      // Is some logical volume cannot find its associated sensitive category, we only warn:
      DT_LOG_WARNING(_logprio(),
                     "Sensitive category '"
                         << sd_name
                         << "' does not exists but is supposed to be associated to logical volume '"
                         << lv_name << "'! Ignoring this association !");
    }
  }

  // Loop on logical volumes, processing only ones associated to some sensitive category:
  for (const auto &ilogical : _geom_manager_->get_factory().get_logicals()) {
    // Get a reference to the associated logical volume :
    const geomtools::logical_volume &log = *(ilogical.second);

    // Search for an association with a sensitive category:
    if (lv_sd_associations.find(log.get_name()) == lv_sd_associations.end()) {
      DT_LOG_DEBUG(_logprio(), "Logical volume '" << log.get_name() << "' is not sensitive !");
      continue;
    }

    // // Check if it is tagged as a 'sensitive' detector :
    // if (! geomtools::sensitive::is_sensitive(log.get_parameters())) {
    //   DT_LOG_DEBUG(_logprio(),"Logical volume '" << log.get_name() << "' is not sensitive !");
    //   continue;
    // }

    // Get the 'sensitive category' for this detector :
    std::string sensitive_category = lv_sd_associations[log.get_name()];

    // // Get the 'sensitive category' for this detector :
    // std::string sensitive_category
    //   = geomtools::sensitive::get_sensitive_category(log.get_parameters());
    // if (sensitive_category.empty()) {
    //   DT_LOG_WARNING(_logprio(),"Empty sensitive category name ! Ignore !");
    //   continue;
    // }

    DT_LOG_NOTICE(_logprio(), "Logical volume '" << log.get_name()
                                                 << "' is sensitive with category '"
                                                 << sensitive_category << "' !");

    // Pickup the corresponding G4 logical volume :
    G4LogicalVolume *g4_log = nullptr;
    G4String log_name = log.get_name().c_str();
    g4_log = g4_LV_store->GetVolume(log_name, false);
    if (g4_log == nullptr) {
      DT_LOG_WARNING(_logprio(), "Logical volume '"
                                     << log.get_name() << "' is not used in G4LogicalVolumeStore ! "
                                     << "Ignore this association with sensitive category '"
                                     << sensitive_category << "'!");
      continue;
    }
    sensitive_detector *SD = nullptr;
    // Search for the sensitive detector that uses this category:
    auto found = _sensitive_detectors_.find(sensitive_category);
    bool already_present = false;
    if (found == _sensitive_detectors_.end()) {
      DT_LOG_NOTICE(_logprio(),
                    "Create a new sensitive detector with category '" << sensitive_category << "'");
      SD = new sensitive_detector(sensitive_category);
      if (_g4_manager_ != nullptr) {
        SD->set_manager(*_g4_manager_);
      } else {
        DT_LOG_FATAL(_logprio(), "Missing G4 manager for sensitive detectors!");
      }
      SDman->AddNewDetector(SD);
      _sensitive_detectors_[sensitive_category] = SD;
    } else {
      already_present = true;
      DT_LOG_NOTICE(_logprio(),
                    "Use the sensitive detector with category '" << sensitive_category << "'");
      SD = found->second;
    }

    {
      DT_LOG_NOTICE(_logprio(), "Make the G4 logical volume '"
                                    << log.get_name() << "' a sensitive detector with category '"
                                    << sensitive_category << "'");
    }

    // Update the list of logical volumes(identified by their names) attached
    // to this sensitive detector:
    _sensitive_detectors_[sensitive_category]->attach_logical_volume(log.get_name());

    if (!already_present) {
      // Setup special behaviour of the new sensitive detector:
      if (_SD_params_.has_key(sensitive_category)) {
        const datatools::properties &this_SD_params =
            _SD_params_.get(sensitive_category).get_properties();
        SD->configure(this_SD_params);
      }
    }

    // Associate this sensitive detector to the G4 logical volume:
    g4_log->SetSensitiveDetector(_sensitive_detectors_[sensitive_category]);

  }  // for (logical_volumes)

  /****************************************
   * Setup the step hit processor factory *
   ****************************************/

  // Pass the address of the geometry manager to step hit processors :
  DT_LOG_NOTICE(_logprio(), "SHPF: Preparing the step hit processor factory (SHPF)...");
  // Logging priority:
  datatools::logger::priority lp = datatools::logger::extract_logging_configuration(_SHPF_config_);
  if (lp != datatools::logger::PRIO_UNDEFINED) {
    _SHPF_.set_logging_priority(lp);
  }
  if (this->has_geometry_manager()) {
    _SHPF_.set_geometry_manager(this->get_geometry_manager());
  }

  // Set the set of activated output profiles from the manager:
  if (_g4_manager_ != nullptr) {
    _SHPF_.set_output_profiles(_g4_manager_->get_activated_output_profile_ids());
  } else {
    DT_LOG_FATAL(_logprio(), "Missing G4 manager for output profiles!");
  }

  std::vector<std::string> config_files;
  if (_SHPF_config_.has_key("definitions")) {
    _SHPF_config_.fetch("definitions", config_files);
  } else if (_SHPF_config_.has_key("configs")) {
    _SHPF_config_.fetch("configs", config_files);
  } else if (_SHPF_config_.has_key("config")) {
    std::string config_file = _SHPF_config_.fetch_string("config");
    config_files.push_back(config_file);
  }
  DT_THROW_IF(config_files.empty(), std::logic_error,
              "Missing configuration file(s) for the step hit processor factory !");
  for (auto config_file : config_files) {
    datatools::fetch_path_with_env(config_file);
    datatools::multi_properties mconfig("name", "type");
    DT_LOG_NOTICE(_logprio(), "SHPF: Parsing the SHPF configuration file...");
    mconfig.read(config_file);
    DT_LOG_NOTICE(_logprio(), "SHPF: The SHPF configuration file has been parsed.");
    DT_LOG_NOTICE(_logprio(), "SHPF: Loading the hit processors from the SHPF...");
    _SHPF_.load(mconfig);
  }
  _SHPF_.initialize();
  DT_LOG_NOTICE(_logprio(), "SHPF: The step hit processors have been loaded from the SHPF.");
  if (_logprio() >= datatools::logger::PRIO_DEBUG) {
    _SHPF_.tree_dump(std::clog, "SHPF: ", "[debug]: ");
  }

  /*****************************************************************
   * Install some step hit processors into the sensitive detectors *
   *****************************************************************/

  /* Look for "non-offical" sensitive categories from the SHPF factory itself.
   * Possibly extends the list of "offical" sensitive detectors
   * from the list of step hit processors found in the SHPF factory
   * and creates the proper association to some logical volumes which must
   * be specified using their respective names.
   */
  for (const auto &iSHP : _SHPF_.get_processors()) {
    const mctools::base_step_hit_processor *processor = iSHP.second;
    const std::string &hit_category = processor->get_hit_category();
    if (_g4_manager_ != nullptr) {
      if (_g4_manager_->forbids_private_hits()) {
        if (boost::starts_with(hit_category, "_")) {
          DT_LOG_WARNING(_logprio(), "SHPF: Sensitive detector '"
                                         << processor->get_name()
                                         << "' initialized with private hit category '"
                                         << hit_category << "' is not used !");
          continue;
        }
      }
    } else {
      DT_LOG_FATAL(_logprio(), "Missing G4 manager!!!");
    }

    const std::string &from_processor_sensitive_category = iSHP.second->get_sensitive_category();

    // If the sensitive_category already exists (from the geometry model setup):
    if (_sensitive_detectors_.find(from_processor_sensitive_category) !=
        _sensitive_detectors_.end()) {
      DT_LOG_WARNING(_logprio(), "SHPF: A sensitive detector '"
                                     << processor->get_name() << "' with category '"
                                     << from_processor_sensitive_category
                                     << "' already exists ! Ignore this rule !");
      continue;
    }
    // At this point, we know that some geometry volumes(s)
    // are attached to the newly created sensitive detector:
    DT_LOG_NOTICE(_logprio(), "SHPF: Create a new sensitive detector with category '"
                                  << from_processor_sensitive_category << "'");

    auto *SD = new sensitive_detector(from_processor_sensitive_category);
    if (_g4_manager_ != nullptr) {
      SD->set_manager(*_g4_manager_);
    } else {
      DT_LOG_FATAL(_logprio(), "Missing G4 manager for sensitive detectors!!!");
    }

    // Setup special behaviour of the sensitive detector:
    SD->configure(processor->get_auxiliaries());

    SDman->AddNewDetector(SD);
    _sensitive_detectors_[from_processor_sensitive_category] = SD;

    // Extract the logical volumes this sensitive detector is attached to:
    std::vector<std::string> logicals;

    bool all_logicals = false;
    if (processor->get_auxiliaries().has_flag("geometry.volumes.all")) {
      all_logicals = true;
    }
    if (all_logicals) {
      for (const auto &ilogical : _geom_manager_->get_factory().get_logicals()) {
        const std::string &logical_name = ilogical.first;
        if (std::find(logicals.begin(), logicals.end(), logical_name) == logicals.end()) {
          logicals.push_back(logical_name);
        }
      }
    }

    // Fetch the list of logical volumes associated to this 'sensitive category' :
    if (!all_logicals) {
      if (processor->get_auxiliaries().has_key("geometry.volumes")) {
        DT_LOG_NOTICE(_logprio(),
                      "SHPF: Fetch the list of logical volumes associated to a sensitive category '"
                          << from_processor_sensitive_category << "'");
        // Logicals are given by name :
        processor->get_auxiliaries().fetch("geometry.volumes", logicals);
      }
    }

    DT_LOG_NOTICE(_logprio(), "SHPF: Number of logical volumes associated to sensitive category '"
                                  << from_processor_sensitive_category
                                  << "' : " << logicals.size());

    // Also fetch the list of logical volumes associated to this 'sensitive category'
    // and *with* a given list of materials :
    if (processor->get_auxiliaries().has_key("geometry.volumes.with_materials")) {
      // Volumes are given by material(s) :
      std::vector<std::string> materials;
      DT_LOG_NOTICE(_logprio(), "SHPF: Fetch the list of materials for sensitive category '"
                                    << from_processor_sensitive_category << "'");
      processor->get_auxiliaries().fetch("geometry.volumes.with_materials", materials);

      std::ostringstream message;
      message << "SHPF: Searching for logical volumes with material in (";
      for (size_t j = 0; j < materials.size(); j++) {
        if (j != 0) {
          message << ',';
        }
        message << ' ' << '"' << materials[j] << '"';
      }
      message << ") to be associated to the new sensitive detector with category '"
              << from_processor_sensitive_category << "'";
      DT_LOG_NOTICE(_logprio(), message.str());

      // Traverse all logical volumes :
      for (const auto &ilogical : _geom_manager_->get_factory().get_logicals()) {
        const std::string &logical_name = ilogical.first;
        const geomtools::logical_volume &log = *(ilogical.second);
        ;
        bool checked_material = false;
        std::string mat_name;
        for (const auto &material : materials) {
          mat_name = material;
          if (log.get_material_ref() == mat_name) {
            checked_material = true;
            break;
          }
        }
        if (checked_material) {
          if (std::find(logicals.begin(), logicals.end(), logical_name) == logicals.end()) {
            std::ostringstream local_message;
            message << "SHPF: Associate the logical volume '" << logical_name << "' with material '"
                    << mat_name << "' to the new sensitive detector with category '"
                    << from_processor_sensitive_category << "'";
            DT_LOG_NOTICE(_logprio(), local_message.str());
            logicals.push_back(logical_name);
          }
        }
      }
    }
    DT_LOG_NOTICE(_logprio(), "SHPF: Number of logical volumes associated to sensitive category '"
                                  << from_processor_sensitive_category
                                  << "' : " << logicals.size());

    // Skip the logical volumes that should be excluded :
    if (processor->get_auxiliaries().has_key("geometry.volumes.excluded")) {
      std::vector<std::string> excluded_logicals;
      processor->get_auxiliaries().fetch("geometry.volumes.excluded", excluded_logicals);
      std::ostringstream message;
      message << "SHPF: Excluding logical volumes in (";
      for (size_t j = 0; j < excluded_logicals.size(); j++) {
        if (j != 0) {
          message << ',';
        }
        message << ' ' << '"' << excluded_logicals[j] << '"';
      }
      message << ") from the list of volumes to be associated to the new sensitive detector from "
                 "SHPF with category '"
              << from_processor_sensitive_category << "'";
      DT_LOG_NOTICE(_logprio(), message.str());

      for (const auto &excluded_logical : excluded_logicals) {
        logicals.erase(std::remove(logicals.begin(), logicals.end(), excluded_logical),
                       logicals.end());
      }
    }

    if (logicals.empty()) {
      DT_LOG_WARNING(_logprio(), "SHPF: New sensitive category '"
                                     << from_processor_sensitive_category
                                     << "' has no associated logical volumes !");
      continue;
    }

    // Loop on the specified logical volumes and make them sensitive
    // with the new sensitive detector.
    // Detect conflict (more than one sensitive detector per logical volume).
    for (std::vector<std::string>::const_iterator ilogical = logicals.begin();
         ilogical != logicals.end(); ++ilogical) {
      const std::string &logical_name = *ilogical;
      auto found = _geom_manager_->get_factory().get_logicals().find(logical_name);
      if (found == _geom_manager_->get_factory().get_logicals().end()) {
        DT_LOG_WARNING(_logprio(), "SHPF: No logical volume with name '"
                                       << logical_name
                                       << "' exists in the geometry manager ! Ignore !");
        continue;
      }
      const geomtools::logical_volume &log = *(found->second);

      // Check sensitivity of the logical volume):
      if (geomtools::sensitive::is_sensitive(log.get_parameters())) {
        DT_LOG_WARNING(_logprio(),
                       "SHPF: Logical volume '"
                           << logical_name << "' is already associated to sensitive category '"
                           << geomtools::sensitive::get_sensitive_category(log.get_parameters())
                           << "' !");
        continue;
      }

      // Pickup the corresponding G4 logical volume:
      G4LogicalVolume *g4_log = nullptr;
      G4String log_name = log.get_name().c_str();
      g4_log = g4_LV_store->GetVolume(log_name, false);
      if (g4_log == nullptr) {
        DT_LOG_WARNING(_logprio(), "SHPF: No logical volume has been found '"
                                       << log.get_name() << "' in the G4LogicalVolumeStore !");
        continue;
      }

      // Local scope:
      {
        // Makes the logical sensitive (using a trick because of the const-ness
        // of the logical volume instance after building the model factory from
        // the geometry manager) :
        auto *mutable_log = const_cast<geomtools::logical_volume *>(&log);
        geomtools::sensitive::set_sensitive_category(mutable_log->grab_parameters(),
                                                     from_processor_sensitive_category);
      }

      std::ostringstream message;
      message << "SHPF: Make the G4 logical volume '" << log.get_name()
              << "' a sensitive detector with category '" << from_processor_sensitive_category
              << "'";
      DT_LOG_NOTICE(_logprio(), message.str());
      _sensitive_detectors_[from_processor_sensitive_category]->attach_logical_volume(
          log.get_name());

      g4_log->SetSensitiveDetector(_sensitive_detectors_[from_processor_sensitive_category]);
    }

  }  // for (hit processors)

  // Associate a sensitive detector to a particular step hit processor
  // from the factory:
  for (const auto &iSHP : _SHPF_.get_processors()) {
    const std::string &processor_name = iSHP.first;
    mctools::base_step_hit_processor *processor = iSHP.second;
    const std::string &hit_category = processor->get_hit_category();
    if (_g4_manager_ != nullptr) {
      if (_g4_manager_->forbids_private_hits()) {
        if (boost::starts_with(hit_category, "_")) {
          DT_LOG_WARNING(_logprio(), "SHPF: Sensitive detector '"
                                         << processor->get_name()
                                         << "' initialized from SHPF with private hit category '"
                                         << hit_category << "' is not used !");
          continue;
        }
      }
    } else {
      DT_LOG_FATAL(_logprio(), "Missing G4 manager!!!");
    }
    const std::string &from_processor_sensitive_category = processor->get_sensitive_category();
    auto iSD = _sensitive_detectors_.find(from_processor_sensitive_category);
    if (iSD == _sensitive_detectors_.end()) {
      continue;
    }
    sensitive_detector *SD = iSD->second;
    SD->add_hit_processor(processor_name, *processor);
  }  // SHPF

  // Dump:
  if (is_verbose()) {
    for (auto &_sensitive_detector : _sensitive_detectors_) {
      const std::string &sensitive_category = _sensitive_detector.first;
      sensitive_detector *SD = _sensitive_detector.second;
      DT_LOG_NOTICE(_logprio(), "Sensitive detector '" << sensitive_category << "' : ");
      SD->tree_dump(std::clog, "", "[notice]: ");
    }
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void detector_construction::set_materials_geom_plugin_name(const std::string &mpn_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Operation prohibited !");
  _materials_geom_plugin_name_ = mpn_;
}

void detector_construction::set_emfield_geom_plugin_name(const std::string &fpn_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Operation prohibited !");
  _emfield_geom_plugin_name_ = fpn_;
}

void detector_construction::write_tmp_gdml_file() {
  DT_THROW_IF(!_geom_manager_->is_initialized(), std::logic_error,
              "Geometry manager is not initialized !");
  std::ostringstream tmp_file_template_oss;
  if (!_gdml_file_dir_.empty()) {
    if (!boost::filesystem::is_directory(_gdml_file_dir_)) {
      bool do_mkdir = true;
      if (do_mkdir) {
        DT_LOG_WARNING(_logprio(),
                       "Attempt to create the temporary directory '" << _gdml_file_dir_ << "'...");
        std::ostringstream syscmd;
        syscmd << "mkdir -p " << _gdml_file_dir_;
        int syserr = system(syscmd.str().c_str());
        DT_THROW_IF(syserr == -1, std::runtime_error,
                    "Cannot create temporary directory '" << _gdml_file_dir_ << "' !");
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Temporary directory '" << _gdml_file_dir_ << "' does not exist !");
      }
    }
    tmp_file_template_oss << _gdml_file_dir_ << '/';
  }
  tmp_file_template_oss << "mctools_g4_geometry.gdml."
                        << "XXXXXX" << std::ends;
  std::string tmp_file_template = tmp_file_template_oss.str();
  char tmp_gdml_filename[1024];
  copy(tmp_file_template.begin(), tmp_file_template.end(), tmp_gdml_filename);

  int err = mkstemp(tmp_gdml_filename);
  DT_THROW_IF(err == -1, std::runtime_error, "Cannot create GDML temporary file !");
  _gdml_filename_ = tmp_gdml_filename;

  // The GDML export object :
  geomtools::gdml_export GDML;

  geomtools::gdml_writer materials_writer;  // GDML writer for materials
  const materials::manager *mat_mgr_ref = nullptr;
  std::string materials_plugin_name = _materials_geom_plugin_name_;
  const geomtools::manager &geo_mgr = get_geometry_manager();
  if (!materials_plugin_name.empty()) {
    DT_LOG_NOTICE(_logprio(),
                  "Requested materials plugin name is '" << materials_plugin_name << "'");
  } else {
    DT_LOG_NOTICE(_logprio(), "No explicit requested materials plugin name");
    // We try to find a material plugin associated to the manager :
    DT_LOG_NOTICE(_logprio(), "Trying to find a materials plugin associated to the manager...");
    using dict_type = geomtools::manager::plugins_dict_type;
    const dict_type &plugins = geo_mgr.get_plugins();
    for (const auto &plugin : plugins) {
      const std::string &plugin_name = plugin.first;
      if (geo_mgr.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
        materials_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  if (geo_mgr.has_plugin(materials_plugin_name) &&
      geo_mgr.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
    DT_LOG_NOTICE(_logprio(), "Found materials plugin named '" << materials_plugin_name << "'");
    const auto &mgp = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
    const materials::manager &mat_mgr = mgp.get_manager();
    mat_mgr_ref = &mat_mgr;
  }
  DT_THROW_IF(mat_mgr_ref == nullptr, std::logic_error,
              "Cannot find a material plugin in the geometry manager !");
  DT_LOG_NOTICE(_logprio(), "Export GDML materials from the materials plugin: ");
  // mat_mgr_ref->tree_dump(std::cerr, "Material manager: ", "DEVEL: ");
  geomtools::export_gdml(*mat_mgr_ref, materials_writer);
  GDML.attach_external_materials(
      materials_writer.get_stream(geomtools::gdml_writer::materials_section()));
  DT_LOG_TRACE(
      _logprio(),
      "Materials writer: \n"
          << materials_writer.get_stream(geomtools::gdml_writer::materials_section()).str());

  GDML.add_auxiliary_support(false);
  GDML.add_replica_support(false);
  GDML.parameters().store("xml_version", geomtools::gdml_writer::default_xml_version());
  GDML.parameters().store("xml_encoding", geomtools::gdml_writer::default_xml_encoding());

  GDML.parameters().store("gdml_schema", _gdml_schema_);

  GDML.parameters().store("length_unit", geomtools::gdml_export::default_length_unit());
  GDML.parameters().store("angle_unit", geomtools::gdml_export::default_angle_unit());
  GDML.export_gdml(_gdml_filename_, _geom_manager_->get_factory(), "world");
}

void detector_construction::_set_user_limits() {
  DT_LOG_TRACE(_logprio(), "Entering...");

  std::set<std::string> log_volumes;
  if (_limits_config_.has_key("limits.list_of_volumes")) {
    _limits_config_.fetch("limits.list_of_volumes", log_volumes);
  }

  /*
  std::set<std::string> log_volumes_regexp;
  if (_limits_config_.has_key("limits.volume_regexps")) {
    _limits_config_.fetch("limits.volume_regexps", log_volume_regexps);
  }

  if (log_volumes_regexp.size()) {
    for (geomtools::logical_volume::dict_type::const_iterator ilogical
           = _geom_manager_->get_factory().get_logicals().begin();
         ilogical != _geom_manager_->get_factory().get_logicals().end();
         ++ilogical) {
      bool add_it = false;
      const std::string & log_name = ilogical->first;
      for (const std::string & log_name_regex : log_volumes_regexp) {
        boost::regex e1(log_name_regex, boost::regex::extended);
        if (boost::regex_match(log_name, e1)) {
          add_it = true;
        }
      }
      if (add_it) {
        log_volumes.insert(log_name);
      }
    }
  }
  */

  std::set<std::string> regions;
  if (_limits_config_.has_key("limits.list_of_regions")) {
    _limits_config_.fetch("limits.list_of_regions", regions);
  }

  std::map<std::string, std::string> targets;
  for (const std::string &region_name : regions) {
    targets[region_name] = "region";
  }
  for (const std::string &log_name : log_volumes) {
    targets[log_name] = "logical_volume";
  }

  // Loop on regions and logical volumes:
  for (std::map<std::string, std::string>::const_iterator i = targets.begin(); i != targets.end();
       i++) {
    DT_LOG_NOTICE(_logprio(),
                  "Fetching user limits for G4 logical volume/region '" << i->first << "'...");
    G4Region *g4_region = nullptr;
    G4LogicalVolume *g4_logical = nullptr;
    const std::string &target_name = i->first;
    if (i->second == "logical_volume") {
      /*
      geomtools::logical_volume::dict_type::const_iterator found
        = _geom_manager_->get_factory().get_logicals().find(target_name);
      if (found == _geom_manager_->get_factory().get_logicals().end()) {
        DT_THROW_IF(true, std::logic_error,
                    "Cannot find logical volume '" << target_name << "' for user limits !");
      }
      */
      DT_LOG_NOTICE(_logprio(), "Searching G4 logical volume '" << target_name.c_str() << "'...");
      g4_logical = G4LogicalVolumeStore::GetInstance()->GetVolume(target_name.c_str(), false);
      DT_THROW_IF(g4_logical == nullptr, std::logic_error,
                  "Missing G4 logical volume with name '" << target_name << "' for user limits !");
    } else if (i->second == "region") {
      DT_LOG_NOTICE(_logprio(), "Searching G4 region '" << target_name.c_str() << "'...");
      g4_region = G4RegionStore::GetInstance()->GetRegion(target_name.c_str());
      DT_LOG_DEBUG(_logprio(), "G4 region [@" << g4_region << "]");
      DT_THROW_IF(g4_region == nullptr, std::logic_error,
                  "Cannot find region named '" << target_name << "' !");
    }

    // Search for maximum step length:
    double max_step = DBL_MAX;
    double max_track_length = DBL_MAX;
    double max_track_time = DBL_MAX;
    double min_track_ekin = 0.0;
    double min_track_range = 0.0;
    bool ulimits_used = false;
    double default_length_unit = CLHEP::mm;
    double default_time_unit = CLHEP::nanosecond;
    double default_energy_unit = CLHEP::keV;
    {
      std::ostringstream prop_name;
      prop_name << "limits.max_step." << target_name;
      if (_limits_config_.has_key(prop_name.str())) {
        max_step = _limits_config_.fetch_real(prop_name.str());
        if (!_limits_config_.has_explicit_unit(prop_name.str())) {
          max_step *= default_length_unit;
        }
        ulimits_used = true;
      }
    }
    {
      std::ostringstream prop_name;
      prop_name << "limits.max_track_length." << target_name;
      if (_limits_config_.has_key(prop_name.str())) {
        max_track_length = _limits_config_.fetch_real(prop_name.str());
        if (!_limits_config_.has_explicit_unit(prop_name.str())) {
          max_track_length *= default_length_unit;
        }
        ulimits_used = true;
      }
    }
    {
      std::ostringstream prop_name;
      prop_name << "limits.max_track_time." << target_name;
      if (_limits_config_.has_key(prop_name.str())) {
        max_track_time = _limits_config_.fetch_real(prop_name.str());
        if (!_limits_config_.has_explicit_unit(prop_name.str())) {
          max_track_time *= default_time_unit;
        }
        ulimits_used = true;
      }
    }
    {
      std::ostringstream prop_name;
      prop_name << "limits.min_track_ekin." << target_name;
      if (_limits_config_.has_key(prop_name.str())) {
        min_track_ekin = _limits_config_.fetch_real(prop_name.str());
        if (!_limits_config_.has_explicit_unit(prop_name.str())) {
          min_track_ekin *= default_energy_unit;
        }
        ulimits_used = true;
      }
    }
    {
      std::ostringstream prop_name;
      prop_name << "limits.min_track_range." << target_name;
      if (_limits_config_.has_key(prop_name.str())) {
        min_track_range = _limits_config_.fetch_real(prop_name.str());
        if (!_limits_config_.has_explicit_unit(prop_name.str())) {
          min_track_range *= default_length_unit;
        }
        ulimits_used = true;
      }
    }
    if (ulimits_used) {
      // 2014-12-12, FM:
      // max_step implies G4StepLimiter pseudo process to be associated to the particle.
      // Other params imply G4UserSpecialCuts pseudo process to be associated to the
      // particle (not implemented yet).
      auto *g4_user_limits = new G4UserLimits(max_step, max_track_length, max_track_time,
                                              min_track_ekin, min_track_range);
      _user_limits_col_.push_back(g4_user_limits);
      if (g4_logical != nullptr) {
        g4_logical->SetUserLimits(g4_user_limits);
        DT_LOG_NOTICE(_logprio(), "Installing user limits for G4 logical volume named '"
                                      << target_name << "' with max step = " << max_step / CLHEP::mm
                                      << " mm");
      } else if (g4_region != nullptr) {
        g4_region->SetUserLimits(g4_user_limits);
        DT_LOG_NOTICE(_logprio(), "Installing user limits for G4 region named '"
                                      << target_name << "' with max step = " << max_step / CLHEP::mm
                                      << " mm");
      }
    } else {
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                     "No explicit limits has been set for G4 "
                         << i->second << " named '" << target_name
                         << "' while it was subscribed to user limits!");
    }
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void detector_construction::_construct_regions() {
  G4LogicalVolumeStore *g4_LV_store = G4LogicalVolumeStore::GetInstance();

  // {
  //   for (std::vector<G4LogicalVolume*>::const_iterator i = g4_LV_store->begin();
  //        i != g4_LV_store->end();
  //        i++) {
  //     std::cerr << "DEVEL: dœetector_construction::_construct_regions: "
  //               << "Logical volume '" << (*i)->GetName() << "'" << std::endl;
  //   }
  // }

  for (const auto &i : _region_definitions_.get_region_infos()) {
    const std::string &the_region_name = i.first;
    const region_info &the_region_info = i.second;
    auto *the_region = new G4Region(the_region_name);
    for (const auto &logical_volume_name : the_region_info.get_logical_ids()) {
      G4String g4_logical_volume_name = logical_volume_name.c_str();
      G4LogicalVolume *a_logical = g4_LV_store->GetVolume(g4_logical_volume_name, false);
      if (a_logical != nullptr) {
        the_region->AddRootLogicalVolume(a_logical);
      } else {
        // DT_LOG_WARNING(_logprio(),"No logical volume named '"
        //                << logical_volume_name << "' to be added in the '"
        //                << the_region_name << "' region !");
        DT_THROW(std::logic_error, "No logical volume named '" << logical_volume_name
                                                               << "' to be added in the '"
                                                               << the_region_name << "' region !");
      }
    }
  }
}

void detector_construction::_set_visualization_attributes() {
  DT_LOG_TRACE(_logprio(), "Entering...");

  // Look for visibility properties :
  /*
    for (geomtools::models_col_type::const_iterator i
    = _geom_manager_->get_factory().get_models().begin();
    i != _geom_manager_->get_factory().get_models().end();
    ++i) {
    const geomtools::i_model & model = *(i->second);
    const geomtools::logical_volume & log = model.get_logical();
  */
  for (auto ilogical = _geom_manager_->get_factory().get_logicals().begin();
       ilogical != _geom_manager_->get_factory().get_logicals().end(); ++ilogical) {
    // Get a reference to the associated logical volume :
    const geomtools::logical_volume &log = *(ilogical->second);
    bool visible = true;
    bool daughters_visible = true;
    std::string display_color;
    bool force_wire_frame = true;
    bool force_solid = false;
    bool force_aux_edge_visible = false;
    double display_opacity = geomtools::visibility::default_opacity();
    if (geomtools::visibility::is_hidden(log.get_parameters())) {
      visible = false;
      daughters_visible = false;
    } else {
      if (geomtools::visibility::is_hidden_envelope(log.get_parameters())) {
        visible = false;
      }
      if (geomtools::visibility::is_daughters_hidden(log.get_parameters())) {
        daughters_visible = false;
      }
      if (geomtools::visibility::has_color(log.get_parameters())) {
        display_color = geomtools::visibility::get_color(log.get_parameters());
      }
      if (geomtools::visibility::has_opacity(log.get_parameters())) {
        display_opacity = geomtools::visibility::get_opacity(log.get_parameters());
        DT_THROW_IF(!geomtools::visibility::validate_opacity(display_opacity), std::range_error,
                    "Invalid display opacity (" << display_opacity << " for logical volume '"
                                                << ilogical->first << "'!");
      }
      if (geomtools::visibility::is_visible_edges(log.get_parameters())) {
        force_aux_edge_visible = true;
      }
    }
    G4LogicalVolume *g4_log = nullptr;
    G4LogicalVolumeStore *g4_LV_store = G4LogicalVolumeStore::GetInstance();
    G4String log_name = log.get_name().c_str();
    g4_log = g4_LV_store->GetVolume(log_name, false);
    if (g4_log != nullptr) {
      auto *va = new G4VisAttributes;
      if (!visible) {
        va->SetVisibility(false);
      } else {
        va->SetVisibility(true);
      }
      if (!daughters_visible) {
        va->SetDaughtersInvisible(true);
      } else {
        va->SetDaughtersInvisible(false);
      }
      va->SetForceWireframe(force_wire_frame);
      va->SetForceSolid(force_solid);
      va->SetForceAuxEdgeVisible(force_aux_edge_visible);
      G4Colour color;  // default == white
      if (!display_color.empty()) {
        const geomtools::color::constants &gcc = geomtools::color::constants::instance();
        if (display_color == gcc.white) {
          color = G4Colour::White();
        }
        if (display_color == gcc.grey) {
          color = G4Colour::Grey();
        }
        if (display_color == gcc.black) {
          color = G4Colour::Black();
        }
        if (display_color == gcc.red) {
          color = G4Colour::Red();
        }
        if (display_color == gcc.green) {
          color = G4Colour::Green();
        }
        if (display_color == gcc.blue) {
          color = G4Colour::Blue();
        }
        if (display_color == gcc.cyan) {
          color = G4Colour::Cyan();
        }
        if (display_color == gcc.magenta) {
          color = G4Colour::Magenta();
        }
        if (display_color == gcc.yellow) {
          color = G4Colour::Yellow();
        }
        if (display_color == gcc.brown) {
          color = G4Colour::Brown();
        }
        if (display_color == gcc.orange) {
          color = G4Colour(1.0, 0.66, 0.0);
        }
      }
      if (visible) {
        va->SetColor(G4Colour(color.GetRed(), color.GetGreen(), color.GetBlue(), display_opacity));
      }
      // push the G4VisAttributes in the map for persistency in memory:
      _vis_attributes_[log.get_name()] = va;
      g4_log->SetVisAttributes(va);
    } else {
      DT_LOG_DEBUG(_logprio(), "Cannot find logical volume '" << log.get_name()
                                                              << "' in G4LogicalVolumeStore !");
    }
  }
  DT_LOG_TRACE(_logprio(), "Exiting.");
}

}  // namespace snsim

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snsim::detector_construction, ocd_) {
  // The class name :
  ocd_.set_class_name("snsim::detector_construction");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation mandatory detector construction");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation(
      "The Geant4 simulation engine/manager needs to be given a *detector construction* \n"
      "object. This complex object is responsible of some fundamental operations\n"
      "before to run the simulation process :                                   \n"
      "                                                                         \n"
      " * initialization of the virtual geometry tree thanks to                 \n"
      "   the GDML export feature of the ``geomtools::manager`` class           \n"
      "   used here as the main geometry modelling tools.                       \n"
      " * initialization of detector *regions* where one can apply special      \n"
      "   user cuts for the production of secondary particles.                  \n"
      " * initialization of special *user limits* in some geometry volumes      \n"
      "   (use maximum step length).                                            \n"
      " * initialization of magnetic field associated to some geometry volumes  \n"
      "   using a ``emfield::electromagnetic_field_manager`` object provided    \n"
      "   a dedicated plugin in the geometry manager.                           \n"
      " * initialization of the *step hit processor factory* for post-processing\n"
      "   of truth/step hits generated by sensitive detector within their       \n"
      "   associated volumes.                                                   \n"
      " * initialization of visualization attributes for Geant4 3D rendering.   \n"
      "                                                                         \n");

  logger::declare_ocd_logging_configuration(ocd_, "");

  /********
   * GDML *
   ********/

  {
    // Description of the 'gdml.tmp_dir' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.tmp_dir")
        .set_terse_description(
            "The directory where the GDML file is generated by the ``geomtools::manager``")
        .set_traits(datatools::TYPE_STRING)
        .set_path(true)
        .set_long_description(
            "Default value: empty (match the current directory)            \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  gdml.tmp_dir : string as path = \"/tmp/g4_work\"            \n"
            "                                                              \n");
  }

  {
    // Description of the 'gdml.no_validation' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.validation")
        .set_terse_description("Flag to proceed to the validation of the GDML file")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_long_description(
            "Default value: 1                                              \n"
            "                                                              \n"
            "Example::                                                     \n"
            "                                                              \n"
            "  gdml.validation : boolean = 1                               \n"
            "                                                              \n");
  }

  {
    // Description of the 'gdml.schema_location' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.schema_location")
        .set_terse_description("The label indicating where to find the GDML schema resources")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description(
            "Allowed values: ``\"local\"`` , ``\"remote\"``                \n"
            "                                                              \n"
            "Default value: ``\"remote\"`` (needs access to the Internet)  \n")
        .add_example(
            "Use local schema::                                                     \n"
            "                                                              \n"
            "  gdml.schema_location : string = \"local\"                   \n"
            "                                                              \n");
  }

  {
    // Description of the 'materials.plugin_name' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("materials.plugin_name")
        .set_terse_description(
            "The name of the materials plugin embedded in the geometry manager for materials "
            "export in GDML")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description(
            "Default value: empty (plugin is autodetected from the geometry manager)\n")
        .add_example(
            "Use a plugin for materials with a specific name::             \n"
            "                                                              \n"
            "  materials.plugin_name : string = \"materials_driver\"       \n"
            "                                                              \n");
  }

  {
    // Description of the 'hit_processor_factory.configs' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("hit_processor_factory.configs")
        .set_terse_description(
            "The names of the files that contain the configurations\n"
            "of the back-end processors of truth hits.             \n")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_long_description("Default value: empty")
        .add_example(
            "Use two configuration files for back-end step hits processors:: \n"
            "                                                              \n"
            "  hit_processor_factory.configs : string[2] as path = \\      \n"
            "    \"truth_hits_processors-official.conf\" \\                \n"
            "    \"truth_hits_processors-visu.conf\"                       \n"
            "                                                              \n");
  }

  {
    // Description of the 'hit_processor_factory.config' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("hit_processor_factory.config")
        .set_terse_description(
            "The name of the file that contains the configurations \n"
            "of all back-end processors of truth hits.             \n")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description(
            "Default value: empty                                   \n"
            "                                                       \n"
            "This property is not parsed if the \"hit_processor_factory.configs\" \n"
            "is provided.                                           \n"
            "                                                       \n")
        .add_example(
            "Use only one configuration file for all step hits processors::  \n"
            "                                                                \n"
            "  hit_processor_factory.config : string as path = \\            \n"
            "    \"truth_hits_processors.conf\"                              \n"
            "                                                                \n");
  }

  /*************************
   *  Sensitive detectors  *
   *************************/

  {
    // Description of the 'sensitive.detectors' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.detectors")
        .set_terse_description("The list of sensitive detectors attached to the geometry")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .set_long_description(
            "This is the list of so-called *official* sensitive detectors in the   \n"
            "geometry setup. Each sensitive detectors is identified by its unique  \n"
            "name.                                                                 \n")
        .add_example(
            "Use two sensitive detector volumes::                                  \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "                                                                      \n"
            "The configuration parameters for a given sensitive detector are        \n"
            "provided through additionnal properties with the following syntax ::  \n"
            "                                                                      \n"
            " sensitive.SDNAME.KEY1 : TYPE1 = VALUE1                     \n"
            " sensitive.SDNAME.KEY2 : TYPE2 = VALUE2                     \n"
            "                                                                      \n"
            "where ``SDNAME`` is the name of the sensitive detector                \n"
            "and ``KEY1``, ``TYPE1`` and  ``VALUE1`` are the property's identifier, type \n"
            "and value respectively. Please refer to the OCD manual of the         \n"
            "``snsim::sensitive_detector`` class for further documentation   \n"
            "about the available configuration parameters for *sensitive detector* objects.\n");
  }

  {
    // Description of the 'sensitive.XXX.record_track_id' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_track_id")
        .set_terse_description("Record the *track ID* attached to a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Record the track Id for the 'tracker' hits::                          \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_track_id    : boolean = 0                  \n"
            "  sensitive.tracker_SD.record_track_id : boolean = 1                  \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_primary_particle' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_primary_particle")
        .set_terse_description(
            "Record *primary particle* flag attached to a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Record the 'primary particle' flag::                                  \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_primary_particle    : boolean = 1          \n"
            "  sensitive.tracker_SD.record_primary_particle : boolean = 0          \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_alpha_quenching' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_alpha_quenching")
        .set_terse_description(
            "Record the *alpha quenching* flag attached to a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Activate alpha quenching for the 'calo' hits::                        \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_alpha_quenching    : boolean = 1           \n"
            "  sensitive.tracker_SD.record_alpha_quenching : boolean = 0           \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_major_track' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_major_track")
        .set_terse_description(
            "Record the *major track* flag attached to a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_major_track    : boolean = 1               \n"
            "  sensitive.tracker_SD.record_major_track : boolean = 0               \n"
            "                                                                      \n"
            "See OCD manual of the ``snsim::sensitive_detector`` class.      \n");
  }

  {
    // Description of the 'sensitive.XXX.major_track_minimum_energy' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.major_track_minimum_energy")
        .set_terse_description("Energy threshold to consider a track as a *major* one")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_default_value_real(0.0)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                                 \n"
            "                                                                          \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"            \n"
            "  sensitive.calo_SD.record_major_track         : boolean = 1              \n"
            "  sensitive.calo_SD.major_track_minimum_energy : real as energy = 50 keV  \n"
            "                                                                          \n");
  }

  {
    // Description of the 'sensitive.XXX.record_category' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_category")
        .set_terse_description(
            "Record the *sensitive category* attached to a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_category    : boolean = 1                  \n"
            "  sensitive.tracker_SD.record_category : boolean = 1                  \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_momentum' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_momentum")
        .set_terse_description(
            "Record the start and stop *momenta* of the step for a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_momentum    : boolean = 0                  \n"
            "  sensitive.tracker_SD.record_momentum : boolean = 1                  \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_kinetic_energy' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_kinetic_energy")
        .set_terse_description(
            "Record the start and stop *kinetic energy* of the step for a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_kinetic_energy    : boolean = 1            \n"
            "  sensitive.tracker_SD.record_kinetic_energy : boolean = 0            \n"
            "                                                                      \n"
            "See OCD manual of the ``snsim::sensitive_detector`` class.      \n");
  }

  {
    // Description of the 'sensitive.XXX.hits_buffer_capacity' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.hits_buffer_capacity")
        .set_terse_description(
            "Pre-allocate the initial capacity of a buffer of step hits associated to a given "
            "sensitive detector")
        .set_traits(datatools::TYPE_INTEGER)
        .set_default_value_integer(1000)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.hits_buffer_capacity    : integer = 30            \n"
            "  sensitive.tracker_SD.hits_buffer_capacity : integer = 100           \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.drop_zero_energy_deposit_steps' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.drop_zero_energy_deposit_steps")
        .set_terse_description("Drop zero energy deposit step hits for a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.drop_zero_energy_deposit_steps    : boolean = 1   \n"
            "  sensitive.tracker_SD.drop_zero_energy_deposit_steps : boolean = 0   \n"
            "                                                                      \n");
  }

  {
    // Description of the 'sensitive.XXX.record_g4_volume_infos' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_g4_volume_infos")
        .set_terse_description(
            "Record the Geant4 geometry volume informations for a given sensitive detector")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .set_long_description("See OCD manual of the ``snsim::sensitive_detector`` class.\n")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
            "  sensitive.calo_SD.record_g4_volume_infos    : boolean = 1           \n"
            "  sensitive.tracker_SD.record_g4_volume_infos : boolean = 1           \n"
            "                                                                      \n");
  }

  /***********
   * Regions *
   ***********/

  {
    // Description of the 'using_regions' volumes' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("using_regions")
        .set_terse_description("Flag to activate regions")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(true)
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  using_regions : boolean = 1                                         \n"
            "                                                                      \n");
  }

  {
    // Description of the 'regions' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("regions")
        .set_terse_description("The list of regions attached to the geometry")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  regions : string[2] = \"calo\" \"tracker\"                          \n"
            "                                                                      \n");
  }

  {
    // Description of the 'regions' volumes' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("regions.${regions}.volumes")
        .set_terse_description(
            "The list of volumes attached to a region referenced through its name")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .set_mandatory(false)
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  regions : string[2] = \"calo\" \"tracker\"                          \n"
            "  regions.calo.volumes    : string[2] = \"calo0.log\" \"calo1.log\"   \n"
            "  regions.tracker.volumes : string[1] = \"cells.log\"                 \n"
            "                                                                      \n");
  }

  {
    // Description of the 'regions' volumes.with_material' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("regions.${regions}.volumes.with_materials")
        .set_terse_description(
            "The list of volumes with some given materials attached to a region referenced through "
            "its name")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .set_mandatory(false)
        .add_example(
            "Example::                                                                \n"
            "                                                                         \n"
            "  regions : string[2] = \"calo\" \"tracker\"                             \n"
            "  regions.calo.volumes.with_materials    : string[2] = \"PVT\" \"NE110\" \n"
            "  regions.tracker.volumes.with_materials : string = \"tracking_gas\"     \n"
            "                                                                         \n");
  }

  /***************
   * User limits *
   ***************/

  {
    // Description of the 'using_user_limits' volumes' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("using_user_limits")
        .set_terse_description("Flag to activate user limits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  using_user_limits : boolean = 1                                     \n"
            "                                                                      \n");
  }

  {
    // Description of the 'limits.list_of_volumes' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("limits.list_of_volumes")
        .set_terse_description("The list of volumes where to apply user limits")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .add_example(
            "Example::                                                                      \n"
            "                                                                               \n"
            "  limits.list_of_volumes : string[2] = \"calo_block.log\" \"tracker_cell.log\" \n"
            "                                                                               \n");
  }

  {
    // Description of the 'limits.max_step.XXX' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("limits.max_step.${limits.list_of_volumes}")
        .set_terse_description("The maximum tracking step length associated to a given volume")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .add_example(
            "Example::                                                                      \n"
            "                                                                               \n"
            "  limits.list_of_volumes : string[2] = \"calo_block.log\" \"tracker_cell.log\" \n"
            "  limits.max_step.calo_block.log   : real as length = 0.5 mm                   \n"
            "  limits.max_step.tracker_cell.log : real as length = 5 mm                     \n"
            "                                                                               \n");
  }

  /******************
   * Magnetic field *
   ******************/

  {
    // Description of the 'using_magnetic_field' volumes' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("using_magnetic_field")
        .set_terse_description("Flag to activate a magnetic field")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  using_magnetic_field : boolean = 0                                  \n"
            "                                                                      \n");
  }

  {
    // Description of the 'magnetic_field.plugin_name' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.plugin_name")
        .set_terse_description(
            "The name of the geometry magnetic field plugin embedded in the geometry manager")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .set_triggered_by_flag("using_magnetic_field")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  magnetic_field.plugin_name : string = \"coil_driver\"               \n"
            "                                                                      \n");
  }

  {
    // Description of the 'magnetic_field.miss_distance.unit' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance.unit")
        .set_terse_description("The default implicit magnetic field miss distance unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_triggered_by_flag("using_magnetic_field")
        .set_default_value_string("``\"mm\"``")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  magnetic_field.miss_distance.unit : string = \"mm\"                 \n"
            "                                                                      \n");
  }

  {
    // Description of the 'magnetic_field.miss_distance.unit' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance")
        .set_terse_description("The global magnetic field miss distance")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_triggered_by_flag("using_magnetic_field")
        .add_example(
            "Example::                                                             \n"
            "                                                                      \n"
            "  magnetic_field.miss_distance : real as length = 1.2 mm              \n"
            "                                                                      \n")
        .set_explicit_unit(true)
        .set_unit_label("length");
  }

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.logging.priority")
        .set_terse_description("Logging priority threshold of the magnetic field manager")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("``\"warning\"``")
        .set_long_description(
            "Allowed values are:                                      \n"
            "                                                         \n"
            " * ``\"fatal\"``       : print fatal error messages      \n"
            " * ``\"critical\"``    : print critical error messages   \n"
            " * ``\"error\"``       : print error messages            \n"
            " * ``\"warning\"``     : print warnings                  \n"
            " * ``\"notice\"``      : print notice messages           \n"
            " * ``\"information\"`` : print informational messages    \n"
            " * ``\"debug\"``       : print debug messages            \n"
            " * ``\"trace\"``       : print trace messages            \n"
            "                                                         \n"
            "Example::                                                \n"
            "                                                         \n"
            "  magnetic_field.logging.priority : string = \"warning\" \n"
            "                                                         \n");
  }

  {
    // Description of the 'magnetic_field.associations' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.associations")
        .set_terse_description("List of geom./mag. field associations with special criteria")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .set_mandatory(false)
        .add_example(
            "Example::                                                           \n"
            "                                                                    \n"
            "  magnetic_field.associations : string[2] = \"coil\" \"lab\"        \n"
            "                                                                    \n"
            "Note: the geometry volume/magnetic field association must exist in  \n"
            "the dedicated plugin.                                               \n");
  };

  {
    // Description of the 'magnetic_field.check_pos_time.XXX' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance.${magnetic_field.associations}")
        .set_terse_description(
            "Set a specific miss distance for a given geom/mag. field association")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_long_description(
            "Apply a special miss distance for a given geom. volume/mag. field association. \n")
        .add_example(
            "Example::                                                           \n"
            "                                                                    \n"
            "  magnetic_field.associations       : string[2] = \"coil\" \"lab\"  \n"
            "  magnetic_field.miss_distance.coil : real as length = 1 mm         \n"
            "  magnetic_field.miss_distance.lab  : real as length = 1 cm         \n"
            "                                                                    \n"
            "Note: the geometry volume/magnetic field association must exist in  \n"
            "the dedicated plugin.                                               \n");
  }

  {
    // Description of the 'magnetic_field.check_pos_time.XXX' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.check_pos_time.${magnetic_field.associations}")
        .set_terse_description(
            "Flag to force the checking of position/time while computing mag. field for a given "
            "geom/mag. field association")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .set_long_description(
            "Activate the checking of position/time while computing the           \n"
            "magnetic field referenced by a given geom. volume/mag. field association. \n")
        .add_example(
            "Example::                                                           \n"
            "                                                                    \n"
            "  magnetic_field.associations        : string[2] = \"coil\" \"lab\" \n"
            "  magnetic_field.check_pos_time.coil : boolean = 1                  \n"
            "  magnetic_field.check_pos_time.lab  : boolean = 0                  \n"
            "                                                                    \n"
            "Note: the geometry volume/magnetic field association must exist in  \n"
            "the dedicated plugin.                                               \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Typical configuration from the Geant4 simulation manager is::                 \n"
      "                                                                             \n"
      "  [name=\"detector_construction\"]                                           \n"
      "  #@config Main configuration file for the detector construction user object \n"
      "  logging.priority : string = \"debug\"                                      \n"
      "  gdml.tmp_dir : string as path = \"/tmp/${USER}/mctools_g4.d\"              \n"
      "  gdml.schema_location : string = \"local\"                                  \n"
      "  gdml.no_validation   : boolean = 0                                         \n"
      "  materials.plugin_name : string = \"materials_driver\"                      \n"
      "  sensitive.detectors : string[2] = \"calo_SD\"  \"tracker_SD\"              \n"
      "  sensitive.calo_SD.hits_buffer_capacity : integer = 50                      \n"
      "  sensitive.calo_SD.record_track_id : boolean = 1                            \n"
      "  sensitive.tracker_SD.hits_buffer_capacity : integer = 200                  \n"
      "  sensitive.tracker_SD.record_track_id : boolean = 1                         \n"
      "                                                                             \n"
      "Additional properties for *sensitive detector* are detailed in the           \n"
      "OCD manual for the ``snsim::sensitive_detector`` class.                \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snsim::detector_construction' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::detector_construction, "snsim::detector_construction")
