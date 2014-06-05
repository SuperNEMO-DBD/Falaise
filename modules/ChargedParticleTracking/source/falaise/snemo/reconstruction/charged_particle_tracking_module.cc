// \file falaise/snemo/reconstruction/charged_particle_tracking_module.cc

// Ourselves:
#include <snemo/reconstruction/charged_particle_tracking_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/particle_track_data.h>

// This plugin (ChargedParticleTracking):
#include <snemo/reconstruction/vertex_extrapolation_driver.h>
#include <snemo/reconstruction/charge_computation_driver.h>
#include <snemo/reconstruction/calorimeter_association_driver.h>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro
    DPP_MODULE_REGISTRATION_IMPLEMENT(charged_particle_tracking_module,
                                      "snemo::reconstruction::charged_particle_tracking_module");

    const geomtools::manager & charged_particle_tracking_module::get_geometry_manager () const
    {
      return *_geometry_manager_;
    }

    void charged_particle_tracking_module::set_geometry_manager (const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;
      return;
    }

    void charged_particle_tracking_module::_set_defaults ()
    {
      _CD_label_  = snemo::datamodel::data_info::default_calibrated_data_label();
      _TTD_label_ = snemo::datamodel::data_info::default_tracker_trajectory_data_label();
      _PTD_label_ = snemo::datamodel::data_info::default_particle_track_data_label();

      _geometry_manager_  = 0;

      _VED_.reset();
      _CCD_.reset();
      _CAD_.reset();
      return;
    }

    void charged_particle_tracking_module::initialize(const datatools::properties  & setup_,
                                                      datatools::service_manager   & service_manager_,
                                                      dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(setup_);

      if (setup_.has_key("CD_label"))
        {
          _CD_label_ = setup_.fetch_string("CD_label");
        }

      if (setup_.has_key("TTD_label"))
        {
          _TTD_label_ = setup_.fetch_string("TTD_label");
        }

      if (setup_.has_key("PTD_label"))
        {
          _PTD_label_ = setup_.fetch_string("PTD_label");
        }

      std::string geometry_label;
      if (setup_.has_key("Geo_label"))
        {
          geometry_label = setup_.fetch_string("Geo_label");
        }
      // Geometry manager :
      if (_geometry_manager_ == 0)
        {
          DT_THROW_IF(geometry_label.empty(), std::logic_error,
                      "Module '" << get_name() << "' has no valid '" << "Geo_label" << "' property !");
          DT_THROW_IF(! service_manager_.has(geometry_label) ||
                      ! service_manager_.is_a<geomtools::geometry_service>(geometry_label),
                      std::logic_error,
                      "Module '" << get_name() << "' has no '" << geometry_label << "' service !");
          geomtools::geometry_service & Geo
            = service_manager_.get<geomtools::geometry_service>(geometry_label);
          set_geometry_manager(Geo.get_geom_manager());
        }

      // Drivers :
      DT_THROW_IF(! setup_.has_key ("drivers"), std::logic_error, "Missing 'drivers' key !");
      std::vector<std::string> driver_names;
      setup_.fetch("drivers", driver_names);
      for (std::vector<std::string>::const_iterator
             idriver = driver_names.begin();
           idriver != driver_names.end(); ++idriver)
        {
          const std::string & a_driver_name = *idriver;

          if (a_driver_name == "VED")
            {
              // Initialize Vertex Extrapolation Driver
              _VED_.reset(new snemo::reconstruction::vertex_extrapolation_driver);
              _VED_->set_geometry_manager(get_geometry_manager ());
              datatools::properties VED_config;
              setup_.export_and_rename_starting_with(VED_config, std::string(a_driver_name + "."), "");
              _VED_->initialize(VED_config);
            }
          else if (a_driver_name == "CCD")
            {
              // Initialize Charge Computation Driver
              _CCD_.reset(new snemo::reconstruction::charge_computation_driver);
              datatools::properties CCD_config;
              setup_.export_and_rename_starting_with(CCD_config, std::string(a_driver_name + "."), "");
              _CCD_->initialize(CCD_config);
            }
          else if (a_driver_name == "CAD")
            {
              // Initialize Calorimeter Association Driver
              _CAD_.reset(new snemo::reconstruction::calorimeter_association_driver);
              _CAD_->set_geometry_manager(get_geometry_manager ());
              datatools::properties CAD_config;
              setup_.export_and_rename_starting_with(CAD_config, std::string(a_driver_name + "."), "");
              _CAD_->initialize(CAD_config);
            }
          else
            {
              DT_THROW_IF (true, std::logic_error, "Driver '" << a_driver_name << "' does not exist !");
            }
        }

      // Tag the module as initialized :
      _set_initialized (true);
      return;
    }

    void charged_particle_tracking_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      _set_initialized(false);
      _set_defaults();
      return;
    }

    // Constructor :
    charged_particle_tracking_module::charged_particle_tracking_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    charged_particle_tracking_module::~charged_particle_tracking_module()
    {
      if (is_initialized()) charged_particle_tracking_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status charged_particle_tracking_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      /*************************
       * Check calibrated data *
       *************************/

      const bool abort_at_missing_input = true;
      // Check if some 'calibrated_data' are available in the data model:
      if (! data_record_.has(_CD_label_))
        {
          DT_THROW_IF(abort_at_missing_input, std::logic_error,
                      "Missing calibrated data to be processed !");
          // leave the data unchanged.
          return dpp::base_module::PROCESS_ERROR;
        }
      // Get the 'calibrated_data' entry from the data model :
      const snemo::datamodel::calibrated_data & the_calibrated_data
        = data_record_.get<snemo::datamodel::calibrated_data>(_CD_label_);

      /*********************************
       * Check tracker trajectory data *
       *********************************/

      // Check if some 'tracker_trajectory_data' are available in the data model:
      if (! data_record_.has(_TTD_label_))
        {
          DT_THROW_IF(abort_at_missing_input, std::logic_error,
                      "Missing tracker trajectory data to be processed !");
          // leave the data unchanged.
          return dpp::base_module::PROCESS_ERROR;
        }
      // Get the 'tracker_trajectory_data' entry from the data model :
      const snemo::datamodel::tracker_trajectory_data & the_tracker_trajectory_data
        = data_record_.get<snemo::datamodel::tracker_trajectory_data>(_TTD_label_);

      /*********************************
       * Check particle track data     *
       *********************************/
      const bool abort_at_former_output = false;
      const bool preserve_former_output = false;

      // check if some 'particle_track_data' are available in the data model:
      snemo::datamodel::particle_track_data * ptr_particle_track_data = 0;
      if (! data_record_.has(_PTD_label_))
        {
          ptr_particle_track_data
            = &(data_record_.add<snemo::datamodel::particle_track_data>(_PTD_label_));
        }
      else
        {
          ptr_particle_track_data
            = &(data_record_.grab<snemo::datamodel::particle_track_data>(_PTD_label_));
        }
      snemo::datamodel::particle_track_data & the_particle_track_data = *ptr_particle_track_data;
      if (the_particle_track_data.has_particles())
        {
          DT_THROW_IF(abort_at_former_output, std::logic_error,
                      "Already has processed particle track data !");
          if (! preserve_former_output)
            {
              the_particle_track_data.reset();
            }
        }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      this->_process (the_calibrated_data, the_tracker_trajectory_data, the_particle_track_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void charged_particle_tracking_module::_process
    (const snemo::datamodel::calibrated_data         & calibrated_data_,
     const snemo::datamodel::tracker_trajectory_data & tracker_trajectory_data_,
     snemo::datamodel::particle_track_data           & particle_track_data_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Process trajectories using external resource:
      if (!tracker_trajectory_data_.has_default_solution())
        {
          DT_LOG_WARNING(get_logging_priority(),
                         "No default trajectory solution has been found");
          return;
        }

      const snemo::datamodel::tracker_trajectory_solution & def_solution
        = tracker_trajectory_data_.get_default_solution();
      const snemo::datamodel::tracker_trajectory_solution::trajectory_col_type & trajectories
        = def_solution.get_trajectories();
      for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::const_iterator
             itrajectory = trajectories.begin();
           itrajectory != trajectories.end(); ++itrajectory)
        {
          const snemo::datamodel::tracker_trajectory & a_trajectory = itrajectory->get();

          // Look into properties to find the default
          // trajectory. Here, default means the one with the best
          // chi2. This flag is set by the 'fitting' module.
          if (!a_trajectory.get_auxiliaries().has_flag("default")) continue;

          // Add a new particle_track
          snemo::datamodel::particle_track::handle_type hPT(new snemo::datamodel::particle_track);
          hPT.grab().set_trajectory_handle(*itrajectory);
          particle_track_data_.add_particle(hPT);

          // Compute particle charge
          _CCD_->process(a_trajectory, hPT.grab());

          // Determine track vertices
          _VED_->process(a_trajectory, hPT.grab());

          // Associate vertices to calorimeter hits
          _CAD_->process(calibrated_data_.calibrated_calorimeter_hits(), hPT.grab());
        }

      // Grab non associated calorimeters :
      geomtools::base_hit::has_flag_predicate asso_pred("__associated");
      geomtools::base_hit::negates_predicate not_asso_pred(asso_pred);
      // Wrapper predicates :
      datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                              snemo::datamodel::calibrated_calorimeter_hit> pred_M2D(not_asso_pred);
      datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);
      const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & chits
        = calibrated_data_.calibrated_calorimeter_hits();
      snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator ihit
        = std::find_if (chits.begin(), chits.end(), pred_via_handle);
      while (ihit != chits.end())
        {
          particle_track_data_.grab_non_associated_calorimeters().push_back(*ihit);
          ihit = std::find_if(++ihit, chits.end(), pred_via_handle);
        }

      DT_LOG_TRACE (get_logging_priority (), "Exiting.");
      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

// end of falaise/snemo/reconstruction/charged_particle_tracking_module.cc
