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
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/property_set.h>

namespace snemo {

  namespace processing {
    
    // Constructor
    base_tracker_fitter::base_tracker_fitter(const std::string & name)
    {
      id_ = name;
      _set_initialized(false);
      _set_defaults();
    }

    base_tracker_fitter::~base_tracker_fitter() = default;

    void base_tracker_fitter::_set_defaults()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
      geoManager_ = nullptr;
      geigerLocator_ = nullptr;
      maxFitsToSave_ = 0;
    }

    datatools::logger::priority base_tracker_fitter::get_logging_priority() const
    {
      return _logging_priority;
    }

    void base_tracker_fitter::set_logging_priority(datatools::logger::priority priority_)
    {
      DT_THROW_IF(priority_ == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority level for base gamma builder !");
      _logging_priority = priority_;
    }

    const std::string & base_tracker_fitter::get_id() const { return id_; }

    bool base_tracker_fitter::is_initialized() const { return isInitialized_; }

    void base_tracker_fitter::_set_initialized(bool state) { isInitialized_ = state; }

    const snemo::geometry::gg_locator & base_tracker_fitter::get_gg_locator() const
    {
      return *geigerLocator_;
    }

    void base_tracker_fitter::_initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
      DT_THROW_IF(not has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(not geoManager_->is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      /* Parse configuration parameters */
      falaise::property_set localSetup{setup_};
      auto ps = localSetup.get<falaise::property_set>("BTF", {});

      // Extract the setup of the base tracker fitter :
      auto lp = datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
      set_logging_priority(lp);

      maxFitsToSave_ = ps.get<int>("maximum_number_of_fits", 0);

      auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
      auto snLocator = snemo::geometry::getSNemoLocator(get_geometry_manager(), locator_plugin_name);
      geigerLocator_ = &(snLocator->geigerLocator());
      DT_THROW_IF(geigerLocator_ == nullptr, std::logic_error, "Cannot find Geiger locator !");
    }

    void base_tracker_fitter::_reset()
    {
      _set_initialized(false);
      this->base_tracker_fitter::_set_defaults();
    }

    void base_tracker_fitter::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized/locked !");
      geoManager_ = &gmgr_;
    }

    const geomtools::manager& base_tracker_fitter::get_geometry_manager() const
    {
      DT_THROW_IF(not has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *geoManager_;
    }

    bool base_tracker_fitter::has_geometry_manager() const
    {
      return geoManager_ != nullptr;
    }

    int base_tracker_fitter::process(const snemo::datamodel::tracker_clustering_data & clustering_,
                                     snemo::datamodel::tracker_trajectory_data & trajectory_)
    {
      int status = 0;
      DT_THROW_IF(not is_initialized(), std::logic_error, "Fitter '" << id_ << "' is not initialized !");

      trajectory_.invalidate_solutions();

      status = _process_algo(clustering_, trajectory_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of cluster hits by '" << id_ << "' algorithm has failed !");
        return status;
      }

      _post_process(trajectory_);
      return status;
    }

    int base_tracker_fitter::_post_process(snemo::datamodel::tracker_trajectory_data & trajectory_)
    {
      int status = 0;

      for (auto & a_solution : trajectory_.get_solutions()) {
        // Grab all trajectories attached to the solution
        auto & the_trajectories = a_solution->grab_trajectories();

        // Define a map with index based on chi2 value : first elements are the
        // best fitted tracks
        using chi2_dict_type = std::map<double, snemo::datamodel::TrackerTrajectoryHdl>;
        using trajectory_dict_type = std::map<int32_t, chi2_dict_type>;
        trajectory_dict_type vtrajs;

        for (auto & a_trajectory : the_trajectories) {
          // Store associated cluster id if any
          if (!a_trajectory->has_cluster()) {
            continue;
          }
          const int32_t a_cluster_id = a_trajectory->get_cluster().get_hit_id();

          // Grab trajectory chi2
          const datatools::properties& prop = a_trajectory->get_auxiliaries();
          if (!prop.has_key("chi2") && !prop.has_key("ndof")) {
            continue;
          }

          // TODO: 2013-08-30 XG: Either use the chi2 alone or chi2
          // reduced i.e. chi2/ndof
          const double chi2 = prop.fetch_real("chi2");
          /* const size_t ndof = prop.fetch_integer("ndof"); */
          vtrajs[a_cluster_id].insert(std::make_pair(chi2, a_trajectory));
        }

        // Tag helix trajectory as default(best chi2) and remove the useless one
        // based on the maximum number of fits to be kept.
        for (auto & i : vtrajs) {
          // for (auto i = vtrajs.begin(); i != vtrajs.end(); ++i) {
          chi2_dict_type & a_trajectory_dict = i.second;

          // Tag default solution i.e. the first one in the trajectory dictionnary
          snemo::datamodel::tracker_trajectory & a_trajectory = a_trajectory_dict.begin()->second.grab();
          a_trajectory.grab_auxiliaries().update("default", true);

          if (maxFitsToSave_ == 0) {
            continue;
          }

          // Remove solution based on maxFitsToSave_. Also keep
          // trajectory with chi2 value pretty close to the previous saved
          // trajectory
          size_t count = 0;
          double prev_chi2 = 0.0;
          for (auto j = a_trajectory_dict.begin(); j != a_trajectory_dict.end(); ++j, ++count) {
            const double chi2_tolerance = 0.1;
            if (count >= maxFitsToSave_ && std::abs(j->first - prev_chi2) > chi2_tolerance) {
              (j->second)->grab_auxiliaries().store_flag("__remove");
            } else {
              prev_chi2 = j->first;
            }
          }
        }

        geomtools::base_hit::has_flag_predicate remove_pred("__remove");
        // wrapper predicates :
        typedef datatools::mother_to_daughter_predicate<geomtools::base_hit,
							snemo::datamodel::tracker_trajectory> predicate_type;
        predicate_type pred_M2D(remove_pred);
        datatools::handle_predicate<snemo::datamodel::tracker_trajectory> pred_via_handle(pred_M2D);
        the_trajectories.erase(std::remove_if(the_trajectories.begin(),
					      the_trajectories.end(),
					      pred_via_handle),
                               the_trajectories.end());
      }

      return status;
    }

    void base_tracker_fitter::tree_dump(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent,
                                        bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
           << datatools::logger::get_priority_label(_logging_priority) << "'" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
           << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << geoManager_
           << std::endl;
      if (has_geometry_manager()) {
        out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
             << geoManager_->get_setup_label() << "'" << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
             << geoManager_->get_setup_version() << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Max number of fit to be saved : " << maxFitsToSave_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;
    }

    // static
    void base_tracker_fitter::ocd_support(datatools::object_configuration_description & ocd_,
                                          const std::string & prefix_)
    {
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

  } // end of namespace processing

} // end of namespace snemo
