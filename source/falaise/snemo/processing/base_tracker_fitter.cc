/// \file falaise/snemo/processing/base_tracker_fitter.cc

// Ourselves:
#include "falaise/snemo/processing/base_tracker_fitter.h"

// Standard library:
#include <set>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace snemo {

namespace processing {

datatools::logger::priority base_tracker_fitter::get_logging_priority() const {
  return _logging_priority;
}  // end of base_tracker_fitter::get_logging_priority

void base_tracker_fitter::set_logging_priority(datatools::logger::priority priority_) {
  _logging_priority = priority_;
  return;
}  // end of base_tracker_fitter::set_logging_priority

const std::string& base_tracker_fitter::get_id() const {
  return _id_;
}  // end of base_tracker_fitter::get_id

bool base_tracker_fitter::is_initialized() const {
  return _initialized_;
}  // end of base_tracker_fitter::is_initialized

void base_tracker_fitter::_set_initialized(bool i_) {
  _initialized_ = i_;
  return;
}  // end of base_tracker_fitter::_set_initialized

const snemo::geometry::gg_locator& base_tracker_fitter::get_gg_locator() const {
  return *_gg_locator_;
}  // end of base_tracker_fitter::get_gg_locator

void base_tracker_fitter::_initialize(const datatools::properties& setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  DT_THROW_IF(_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");
  DT_THROW_IF(!_geometry_manager_->is_initialized(), std::logic_error,
              "Geometry manager is not initialized !");

  /* Parse configuration parameters */

  // Extract the setup of the base tracker fitter :
  datatools::properties btf_setup;
  setup_.export_and_rename_starting_with(btf_setup, "BTF.", "");

  // Logging priority:
  datatools::logger::priority lp = datatools::logger::extract_logging_configuration(btf_setup);
  DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
              "Invalid logging priority level for base tracker fitter !");
  set_logging_priority(lp);

  // Maximum number of fit to be saved :
  if (btf_setup.has_key("maximum_number_of_fits")) {
    _maximum_number_of_fits_ = btf_setup.fetch_integer("maximum_number_of_fits");
  }

  /* initialization stuff */

  const std::string& geo_setup_label = _geometry_manager_->get_setup_label();
  DT_THROW_IF(
      geo_setup_label != "snemo::demonstrator" && geo_setup_label != "snemo::tracker_commissioning",
      std::logic_error, "Invalid geometry setup label '" << geo_setup_label << "' !");

  // Get the Geiger cell locator from geometry plugins :
  const geomtools::manager& geo_mgr = get_geometry_manager();
  std::string locator_plugin_name;
  if (btf_setup.has_key("locator_plugin_name")) {
    locator_plugin_name = btf_setup.fetch_string("locator_plugin_name");
  }
  // If no locator plugin name is set, then search for the first one
  if (locator_plugin_name.empty()) {
    const geomtools::manager::plugins_dict_type& plugins = geo_mgr.get_plugins();
    for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
         ip != plugins.end(); ip++) {
      const std::string& plugin_name = ip->first;
      if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
        locator_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  if (geo_mgr.has_plugin(locator_plugin_name) &&
      geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Found locator plugin named '" << locator_plugin_name << "'");
    const snemo::geometry::locator_plugin& locplug =
        geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
    // Set the Geiger cell locator :
    _gg_locator_ = &(locplug.get_gg_locator());
  }
  DT_THROW_IF(_gg_locator_ == 0, std::logic_error, "Cannot find Geiger locator !");
  if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
    DT_LOG_DEBUG(get_logging_priority(), "Geiger locator :");
    _gg_locator_->tree_dump(std::clog, "", "[debug]: ");
  }

  return;
}  // end of base_tracker_fitter::_initialize

void base_tracker_fitter::_reset() {
  _set_initialized(false);
  this->base_tracker_fitter::_set_defaults();
  return;
}  // end of base_tracker_fitter::_reset

void base_tracker_fitter::set_geometry_manager(const geomtools::manager& gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized/locked !");
  _geometry_manager_ = &gmgr_;
  return;
}  // end of base_tracker_fitter::set_geometry_manager

const geomtools::manager& base_tracker_fitter::get_geometry_manager() const {
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
  return *_geometry_manager_;
}  // end of base_tracker_fitter::get_geometry_manager

bool base_tracker_fitter::has_geometry_manager() const {
  return _geometry_manager_ != 0;
}  // end of base_tracker_fitter::has_geometry_manager

void base_tracker_fitter::_set_defaults() {
  _logging_priority = datatools::logger::PRIO_WARNING;

  _geometry_manager_ = 0;
  _gg_locator_ = 0;

  _maximum_number_of_fits_ = 0;
  return;
}  // end of base_tracker_fitter::_set_defaults

// Constructor
base_tracker_fitter::base_tracker_fitter(const std::string& id_) {
  _id_ = id_;
  _set_initialized(false);
  _set_defaults();
  return;
}  // end of base_tracker_fitter::base_tracker_fitter

base_tracker_fitter::~base_tracker_fitter() {
  if (is_initialized()) {
    _reset();
  }
  return;
}  // end of base_tracker_fitter::~base_tracker_fitter

void base_tracker_fitter::tree_dump(std::ostream& out_, const std::string& title_,
                                    const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
       << datatools::logger::get_priority_label(_logging_priority) << "'" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << _geometry_manager_
       << std::endl;
  if (has_geometry_manager()) {
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
         << _geometry_manager_->get_setup_label() << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
         << _geometry_manager_->get_setup_version() << "'" << std::endl;
  }
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Max number of fit to be saved : " << _maximum_number_of_fits_ << std::endl;

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;
  return;
}  // end of base_tracker_fitter::tree_dump

int base_tracker_fitter::process(const snemo::datamodel::tracker_clustering_data& clustering_,
                                 snemo::datamodel::tracker_trajectory_data& trajectory_) {
  int status = 0;
  DT_THROW_IF(!is_initialized(), std::logic_error, "Fitter '" << _id_ << "' is not initialized !");

  trajectory_.invalidate_solutions();

  status = _process_algo(clustering_, trajectory_);
  if (status != 0) {
    DT_LOG_ERROR(get_logging_priority(),
                 "Processing of cluster hits by '" << _id_ << "' algorithm has failed !");
    return status;
  }

  _post_process(trajectory_);
  return status;
}  // end of base_tracker_fitter::process

int base_tracker_fitter::_post_process(snemo::datamodel::tracker_trajectory_data& trajectory_) {
  int status = 0;

  // Loop over all trajectory solutions
  snemo::datamodel::tracker_trajectory_data::solution_col_type& the_solutions =
      trajectory_.grab_solutions();

  for (snemo::datamodel::tracker_trajectory_data::solution_col_type::iterator isolution =
           the_solutions.begin();
       isolution != the_solutions.end(); ++isolution) {
    // Grab current solution
    snemo::datamodel::tracker_trajectory_solution& a_solution = isolution->grab();

    // Grab all trajectories attached to the solution
    snemo::datamodel::tracker_trajectory_solution::trajectory_col_type& the_trajectories =
        a_solution.grab_trajectories();

    // Define a map with index based on chi2 value : first elements are the
    // best fitted tracks
    typedef std::map<double, snemo::datamodel::tracker_trajectory::handle_type> chi2_dict_type;
    typedef std::map<int32_t, chi2_dict_type> trajectory_dict_type;
    trajectory_dict_type vtrajs;
    for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::iterator itrajectory =
             the_trajectories.begin();
         itrajectory != the_trajectories.end(); ++itrajectory) {
      snemo::datamodel::tracker_trajectory& a_trajectory = itrajectory->grab();

      // Store associated cluster id if any
      if (!a_trajectory.has_cluster()) {
        DT_LOG_DEBUG(get_logging_priority(), "No cluster has been found !");
        continue;
      }
      const int32_t a_cluster_id = a_trajectory.get_cluster().get_hit_id();

      // Grab trajectory chi2
      const datatools::properties& prop = a_trajectory.get_auxiliaries();
      if (!prop.has_key("chi2") && !prop.has_key("ndof")) {
        DT_LOG_DEBUG(get_logging_priority(), "No chi2 and degree of freedom have been stored !");
        continue;
      }

      // TODO: 2013-08-30 XG: Either use the chi2 alone or chi2
      // reduced i.e. chi2/ndof
      const double chi2 = prop.fetch_real("chi2");
      /* const size_t ndof = prop.fetch_integer("ndof"); */
      vtrajs[a_cluster_id].insert(std::make_pair(chi2, *itrajectory));
    }

    // Tag helix trajectory as default(best chi2) and remove the useless one
    // based on the maximum number of fits to be kept.
    for (trajectory_dict_type::iterator i = vtrajs.begin(); i != vtrajs.end(); ++i) {
      chi2_dict_type& a_trajectory_dict = i->second;

      // Tag default solution i.e. the first one in the trajectory dictionnary
      snemo::datamodel::tracker_trajectory& a_trajectory = a_trajectory_dict.begin()->second.grab();
      a_trajectory.grab_auxiliaries().update("default", true);

      if (_maximum_number_of_fits_ == 0) continue;

      // Remove solution based on _maximum_number_of_fits_. Also keep
      // trajectory with chi2 value pretty close to the previous saved
      // trajectory
      size_t count = 0;
      double prev_chi2 = 0.0;
      for (chi2_dict_type::iterator j = a_trajectory_dict.begin(); j != a_trajectory_dict.end();
           ++j, ++count) {
        const double chi2_tolerance = 0.1;
        if (count >= _maximum_number_of_fits_ && std::abs(j->first - prev_chi2) > chi2_tolerance) {
          j->second.grab().grab_auxiliaries().store_flag("__remove");
        } else {
          prev_chi2 = j->first;
        }
      }
    }

    geomtools::base_hit::has_flag_predicate remove_pred("__remove");
    // wrapper predicates :
    datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                            snemo::datamodel::tracker_trajectory>
        pred_M2D(remove_pred);
    datatools::handle_predicate<snemo::datamodel::tracker_trajectory> pred_via_handle(pred_M2D);
    the_trajectories.erase(
        std::remove_if(the_trajectories.begin(), the_trajectories.end(), pred_via_handle),
        the_trajectories.end());
  }

  return status;
}  // end of base_tracker_fitter::_post_process

// static
void base_tracker_fitter::ocd_support(datatools::object_configuration_description& ocd_,
                                      const std::string& prefix_) {
  datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BTF.");

  {
    // Description of the 'locator_plugin_name' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("BTF.locator_plugin_name")
        .set_terse_description("The name of the geometry Geiger locator plugin to be used")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description("Special value: an empty string means automatic search   \n")
        .add_example(
            "Set the default value::                         \n"
            "                                                \n"
            "  BTF.locator_plugin_name : string = \"gg_loc\" \n"
            "                                                \n");
  }

  {
    // Description of the 'maximum_number_of_fits' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("BTF.maximum_number_of_fits")
        .set_terse_description(
            "The maximum number of fitting solutions to be addressed by the algorithm")
        .set_traits(datatools::TYPE_INTEGER)
        .set_long_description("Special value: ``0`` means no limit     \n")
        .set_default_value_integer(0)
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  BTF.maximum_number_of_fits : integer = 0       \n"
            "                                                 \n");
  }

  return;
}

}  // end of namespace processing

}  // end of namespace snemo
