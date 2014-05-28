/// \file falaise/snemo/processing/base_tracker_fitter.cc

// Ourselves:
#include <snemo/processing/base_tracker_fitter.h>

// Standard library:
#include <sstream>
#include <stdexcept>
#include <set>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project:
#include <snemo/geometry/locator_plugin.h>
#include <snemo/geometry/gg_locator.h>
#include <snemo/datamodels/tracker_clustering_data.h>
#include <snemo/datamodels/tracker_trajectory_data.h>
#include <snemo/datamodels/helix_trajectory_pattern.h>
#include <snemo/datamodels/line_trajectory_pattern.h>

namespace snemo {

  namespace processing {

    datatools::logger::priority base_tracker_fitter::get_logging_priority() const
    {
      return _logging_priority;
    } // end of base_tracker_fitter::get_logging_priority

    void base_tracker_fitter::set_logging_priority(datatools::logger::priority priority_)
    {
      _logging_priority = priority_;
      return;
    } // end of base_tracker_fitter::set_logging_priority

    const std::string & base_tracker_fitter::get_id() const
    {
      return _id_;
    } // end of base_tracker_fitter::get_id

    bool base_tracker_fitter::is_initialized() const
    {
      return _initialized_;
    } // end of base_tracker_fitter::is_initialized

    void base_tracker_fitter::_set_initialized(bool i_)
    {
      _initialized_ = i_;
      return;
    } // end of base_tracker_fitter::_set_initialized

    const snemo::geometry::gg_locator & base_tracker_fitter::get_gg_locator() const
    {
      return *_gg_locator_;
    } // end of base_tracker_fitter::get_gg_locator

    void base_tracker_fitter::_initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

      DT_THROW_IF (_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");
      DT_THROW_IF (!_geometry_manager_->is_initialized(), std::logic_error, "Geometry manager is not initialized !");

      /* Parse configuration parameters */

      // Logging priority:
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration(setup_);
      DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      // Maximum number of fit to be saved :
      if (setup_.has_key("maximum_number_of_fits")) {
        _maximum_number_of_fits_ = setup_.fetch_integer("maximum_number_of_fits");
      }

      /* initialization stuff */

      const std::string & geo_setup_label = _geometry_manager_->get_setup_label();
      DT_THROW_IF (geo_setup_label != "snemo::demonstrator" &&
                   geo_setup_label != "snemo::tracker_commissioning",
                   std::logic_error,
                   "Invalid geometry setup label '" << geo_setup_label << "' !");

      // Get the Geiger cell locator from geometry plugins :
      const geomtools::manager & geo_mgr = get_geometry_manager();
      std::string locator_plugin_name;
      if (setup_.has_key("locator_plugin_name")) {
        locator_plugin_name = setup_.fetch_string("locator_plugin_name");
      }
      // If no locator plugin name is set, then search for the first one
      if (locator_plugin_name.empty()) {
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
            DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      if (geo_mgr.has_plugin(locator_plugin_name)
          && geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
        DT_LOG_NOTICE(get_logging_priority(), "Found locator plugin named '" << locator_plugin_name << "'");
        const snemo::geometry::locator_plugin & lp
          = geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
        // Set the Geiger cell locator :
        _gg_locator_ = &(lp.get_gg_locator());
      }
      DT_THROW_IF (_gg_locator_ == 0,  std::logic_error,
                   "Cannot find Geiger locator !");
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG(get_logging_priority(), "Geiger locator :");
        _gg_locator_->dump(std::clog);
      }

      return;
    } // end of base_tracker_fitter::_initialize

    void base_tracker_fitter::_reset()
    {
      _set_initialized(false);
      this->base_tracker_fitter::_set_defaults();
      return;
    } // end of base_tracker_fitter::_reset

    void base_tracker_fitter::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized/locked !");
      _geometry_manager_ = &gmgr_;
      return;
    } // end of base_tracker_fitter::set_geometry_manager

    const geomtools::manager & base_tracker_fitter::get_geometry_manager() const
    {
      DT_THROW_IF (! has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    } // end of base_tracker_fitter::get_geometry_manager


    bool base_tracker_fitter::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    } // end of base_tracker_fitter::has_geometry_manager

    void base_tracker_fitter::_set_defaults()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;

      _geometry_manager_ = 0;
      _gg_locator_   = 0;

      _maximum_number_of_fits_ = 0;
      return;
    } // end of base_tracker_fitter::_set_defaults

      // Constructor
    base_tracker_fitter::base_tracker_fitter(const std::string & id_)
    {
      _id_ = id_;
      _set_initialized(false);
      _set_defaults();
      return;
    } // end of base_tracker_fitter::base_tracker_fitter

    base_tracker_fitter::~base_tracker_fitter()
    {
      if (is_initialized()) {
        _reset();
      }
      return;
    } // end of base_tracker_fitter::~base_tracker_fitter

    void base_tracker_fitter::tree_dump(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      if ( ! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logging          : '"
           << datatools::logger::get_priority_label(_logging_priority) << "'"
           << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Initialized      : " << is_initialized() << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Geometry manager : " << _geometry_manager_ << std::endl;
      if (has_geometry_manager()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup label   : '"
             << _geometry_manager_->get_setup_label() << "'" << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup version : '"
             << _geometry_manager_->get_setup_version() << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Max number of fit to be saved : "
           << _maximum_number_of_fits_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "End." << std::endl;
      return;
    } // end of base_tracker_fitter::tree_dump


    int base_tracker_fitter::_post_process(snemo::datamodel::tracker_trajectory_data & trajectory_)
    {
      int status = 0;

      // Loop over all trajectory solutions
      snemo::datamodel::tracker_trajectory_data::solution_col_type & the_solutions
        = trajectory_.grab_solutions();

      for (snemo::datamodel::tracker_trajectory_data::solution_col_type::iterator
             isolution = the_solutions.begin();
           isolution != the_solutions.end(); ++isolution) {
        // Grab current solution
        snemo::datamodel::tracker_trajectory_solution & a_solution = isolution->grab();

        // Grab all trajectories attached to the solution
        snemo::datamodel::tracker_trajectory_solution::trajectory_col_type & the_trajectories
          = a_solution.grab_trajectories();

        typedef std::map<double, snemo::datamodel::tracker_trajectory::handle_type> traj_dict_type;
        std::map<int32_t, traj_dict_type> vline, vhelix;
        std::set<int32_t> vcluster_id;
        for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::iterator
               itrajectory = the_trajectories.begin();
             itrajectory != the_trajectories.end(); ++itrajectory) {
          // Grab current trajectory
          snemo::datamodel::tracker_trajectory & a_trajectory = itrajectory->grab();

          // Store associated cluster id if any
          if (! a_trajectory.has_cluster()) {
            DT_LOG_DEBUG(get_logging_priority(), "No cluster has been found !");
            continue;
          }
          vcluster_id.insert(a_trajectory.get_cluster().get_hit_id());

          // Grab trajectory chi2
          datatools::properties & prop = a_trajectory.grab_auxiliaries();
          if (! prop.has_key("chi2") && ! prop.has_key("ndof")) {
            DT_LOG_DEBUG(get_logging_priority(), "No chi2 and degree of freedom have been stored !");
            continue;
          }

          // TODO: 2013-08-30 XG: Either use the chi2 alone or chi2
          // reduced i.e. chi2/ndof
          const double chi2 = prop.fetch_real("chi2");
          /* const size_t ndof = prop.fetch_integer("ndof"); */

          // Look first if trajectory pattern is an helix or not
          snemo::datamodel::base_trajectory_pattern & a_track_pattern = a_trajectory.grab_pattern();

          if (a_track_pattern.get_pattern_id() == snemo::datamodel::line_trajectory_pattern::pattern_id()) {
            vline[a_trajectory.get_cluster().get_hit_id()].insert(std::make_pair(chi2, *itrajectory));
          }

          if (a_track_pattern.get_pattern_id() == snemo::datamodel::helix_trajectory_pattern::pattern_id()) {
            vhelix[a_trajectory.get_cluster().get_hit_id()].insert(std::make_pair(chi2, *itrajectory));
          }

        }

        // Tag helix trajectory as default(best chi2) and remove the
        // useless one based on the maximum number of solutions to be
        // kept. We allow the save of helix beyond the
        // _maximum_number_of_saved_trajectory_ if the chi2 is pretty closed
        // to the previous trajectory kept
        for (std::set<int32_t>::const_iterator icluster_id = vcluster_id.begin();
             icluster_id != vcluster_id.end(); ++icluster_id) {
          // Tag default solution:
          const int32_t cluster_id = *icluster_id;
          std::map<int32_t, traj_dict_type>::iterator helix_dict = vhelix.find(cluster_id);
          std::map<int32_t, traj_dict_type>::iterator line_dict  = vline.find (cluster_id);

          if (helix_dict != vhelix.end() && line_dict == vline.end()) {
            // Only helix solution
            traj_dict_type::iterator best_helix = helix_dict->second.begin();
            best_helix->second.grab().grab_auxiliaries().update("default", true);
          } else if (helix_dict == vhelix.end() && line_dict != vline.end()) {
            // Only line solution
            traj_dict_type::iterator best_line = line_dict->second.begin();
            best_line->second.grab().grab_auxiliaries().update("default", true);
          } else {
            traj_dict_type::iterator best_helix = helix_dict->second.begin();
            traj_dict_type::iterator best_line  = line_dict->second.begin();
            const double chi2_helix = best_helix->first;
            const double chi2_line  = best_line->first;
            if (chi2_helix < chi2_line) {
              best_helix->second.grab().grab_auxiliaries().update("default", true);
            } else {
              best_line->second.grab().grab_auxiliaries().update("default", true);
            }
          }

          if (_maximum_number_of_fits_ == 0) continue;

          // Remove solution based on
          // _maximum_number_of_saved_trajectory_. Also keep trajectory
          // with chi2 value pretty close to the previous saved trajectory
          if (helix_dict != vhelix.end()) {
            size_t count     = 0;
            double prev_chi2 = 0.0;
            for (traj_dict_type::iterator i = helix_dict->second.begin();
                 i != helix_dict->second.end(); ++i, ++count) {
              const double chi2_tolerance = 0.1;
              if (count >= _maximum_number_of_fits_ &&
                  std::abs(i->first - prev_chi2) > chi2_tolerance) {
                i->second.grab().grab_auxiliaries().store_flag("__remove");
              } else {
                prev_chi2 = i->first;
              }
            }
          }

          if (line_dict != vline.end()) {
            size_t count     = 0;
            double prev_chi2 = 0.0;
            for (traj_dict_type::iterator i = line_dict->second.begin();
                 i != line_dict->second.end(); ++i, ++count) {
              const double chi2_tolerance = 0.1;
              if (count >= _maximum_number_of_fits_ &&
                  std::abs(i->first - prev_chi2) > chi2_tolerance) {
                i->second.grab().grab_auxiliaries().store_flag("__remove");
              } else {
                prev_chi2 = i->first;
              }
            }
          }
        }

        geomtools::base_hit::has_flag_predicate remove_pred("__remove");
        // wrapper predicates :
        datatools::mother_to_daughter_predicate<geomtools::base_hit, snemo::datamodel::tracker_trajectory> pred_M2D(remove_pred);
        datatools::handle_predicate<snemo::datamodel::tracker_trajectory> pred_via_handle(pred_M2D);
        the_trajectories.erase(std::remove_if (the_trajectories.begin(), the_trajectories.end(), pred_via_handle),
                               the_trajectories.end());
      }

      return status;
    }// end of base_tracker_fitter::_post_process

    int base_tracker_fitter::process(const snemo::datamodel::tracker_clustering_data & clustering_,
                                     snemo::datamodel::tracker_trajectory_data       & trajectory_)
    {
      int status = 0;
      DT_THROW_IF (! is_initialized(), std::logic_error, "Fitter '" << _id_ << "' is not initialized !");

      trajectory_.invalidate_solutions();

      status = _process_algo(clustering_, trajectory_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of cluster hits by '" << _id_ << "' algorithm has failed !");
        return status;
      }

      _post_process(trajectory_);
      return status;
    } // end of base_tracker_fitter::process

    // static
    void base_tracker_fitter::ocd_support(datatools::object_configuration_description & ocd_,
                                          const std::string & prefix_)
    {

      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_);

      {
        // Description of the 'maximum_number_of_fits' configuration property :
        datatools::configuration_property_description & cpd = ocd_.add_property_info();
        cpd.set_name_pattern("maximum_number_of_fits")
          .set_terse_description("The maximum number of fitting solutions to be addressed by the algorithm")
          .set_traits(datatools::TYPE_INTEGER)
          .set_long_description("Special value: ``0`` means no limit     \n")
          .set_default_value_integer(0)
          .add_example("Set the default value::                          \n"
                       "                                                 \n"
                       "  maximum_number_of_fits : integer = 0           \n"
                       "                                                 \n"
                       )
          ;
      }

      {
        // Description of the 'locator_plugin_name' configuration property :
        datatools::configuration_property_description & cpd = ocd_.add_property_info();
        cpd.set_name_pattern("locator_plugin_name")
          .set_terse_description("The name of the geometry Geiger locator plugin to be used")
          .set_traits(datatools::TYPE_STRING)
          .set_long_description("Special value: an empty string means automatic search   \n")
          .add_example("Set the default value::                          \n"
                       "                                                 \n"
                       "  locator_plugin_name : string = \"gg_loc\"      \n"
                       "                                                 \n"
                       )
          ;
      }

      return;
    }

  }  // end of namespace processing

}  // end of namespace snemo
