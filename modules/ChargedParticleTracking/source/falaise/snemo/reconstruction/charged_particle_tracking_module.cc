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
#include <falaise/snemo/processing/services.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
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

    const geomtools::manager & charged_particle_tracking_module::get_geometry_manager() const
    {
      return *_geometry_manager_;
    }

    void charged_particle_tracking_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;
      return;
    }

    void charged_particle_tracking_module::_set_defaults()
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

      if (setup_.has_key("CD_label")) {
        _CD_label_ = setup_.fetch_string("CD_label");
      }

      if (setup_.has_key("TTD_label")) {
        _TTD_label_ = setup_.fetch_string("TTD_label");
      }

      if (setup_.has_key("PTD_label")) {
        _PTD_label_ = setup_.fetch_string("PTD_label");
      }

      std::string geometry_label;
      if (setup_.has_key("Geo_label")) {
        geometry_label = setup_.fetch_string("Geo_label");
      }
      // Geometry manager :
      if (_geometry_manager_ == 0) {
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
      DT_THROW_IF(! setup_.has_key("drivers"), std::logic_error, "Missing 'drivers' key !");
      std::vector<std::string> driver_names;
      setup_.fetch("drivers", driver_names);
      for (std::vector<std::string>::const_iterator
             idriver = driver_names.begin();
           idriver != driver_names.end(); ++idriver) {
        const std::string & a_driver_name = *idriver;

        if (a_driver_name == snemo::reconstruction::vertex_extrapolation_driver::get_id()) {
          // Initialize Vertex Extrapolation Driver
          _VED_.reset(new snemo::reconstruction::vertex_extrapolation_driver);
          _VED_->set_geometry_manager(get_geometry_manager());
          datatools::properties VED_config;
          setup_.export_and_rename_starting_with(VED_config, a_driver_name + ".", "");
          _VED_->initialize(VED_config);
        } else if (a_driver_name == snemo::reconstruction::charge_computation_driver::get_id()) {
          // Initialize Charge Computation Driver
          _CCD_.reset(new snemo::reconstruction::charge_computation_driver);
          datatools::properties CCD_config;
          setup_.export_and_rename_starting_with(CCD_config, a_driver_name + ".", "");
          _CCD_->initialize(CCD_config);
        } else if (a_driver_name == snemo::reconstruction::calorimeter_association_driver::get_id()) {
          // Initialize Calorimeter Association Driver
          _CAD_.reset(new snemo::reconstruction::calorimeter_association_driver);
          _CAD_->set_geometry_manager(get_geometry_manager());
          datatools::properties CAD_config;
          setup_.export_and_rename_starting_with(CAD_config, a_driver_name + ".", "");
          _CAD_->initialize(CAD_config);
        } else {
          DT_THROW_IF(true, std::logic_error, "Driver '" << a_driver_name << "' does not exist !");
        }
      }

      // Tag the module as initialized :
      _set_initialized(true);
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
      if (! data_record_.has(_CD_label_)) {
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
      if (! data_record_.has(_TTD_label_)) {
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
      if (! data_record_.has(_PTD_label_)) {
        ptr_particle_track_data
          = &(data_record_.add<snemo::datamodel::particle_track_data>(_PTD_label_));
      } else {
        ptr_particle_track_data
          = &(data_record_.grab<snemo::datamodel::particle_track_data>(_PTD_label_));
      }
      snemo::datamodel::particle_track_data & the_particle_track_data = *ptr_particle_track_data;
      if (the_particle_track_data.has_particles()) {
        DT_THROW_IF(abort_at_former_output, std::logic_error,
                    "Already has processed particle track data !");
        if (! preserve_former_output) {
          the_particle_track_data.reset();
        }
      }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      this->_process(the_calibrated_data, the_tracker_trajectory_data, the_particle_track_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void charged_particle_tracking_module::_process
    (const snemo::datamodel::calibrated_data         & calibrated_data_,
     const snemo::datamodel::tracker_trajectory_data & tracker_trajectory_data_,
     snemo::datamodel::particle_track_data           & particle_track_data_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Process trajectories using external resource:
      if (!tracker_trajectory_data_.has_default_solution()) {
        DT_LOG_DEBUG(get_logging_priority(),
                     "No default trajectory solution has been found");
        // Fill non associated calorimeter hits
        const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & chits
          = calibrated_data_.calibrated_calorimeter_hits();
        for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator
               ihit = chits.begin(); ihit != chits.end(); ++ihit) {
          particle_track_data_.grab_non_associated_calorimeters().push_back(*ihit);
        }
        return;
      }

      const snemo::datamodel::tracker_trajectory_solution & a_solution
        = tracker_trajectory_data_.get_default_solution();
      const snemo::datamodel::tracker_trajectory_solution::trajectory_col_type & trajectories
        = a_solution.get_trajectories();
      for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::const_iterator
             itrajectory = trajectories.begin();
           itrajectory != trajectories.end(); ++itrajectory) {
        const snemo::datamodel::tracker_trajectory & a_trajectory = itrajectory->get();

        // Look into properties to find the default
        // trajectory. Here, default means the one with the best
        // chi2. This flag is set by the 'fitting' module.
        if (! a_trajectory.get_auxiliaries().has_flag("default")) continue;

        // Add a new particle_track
        snemo::datamodel::particle_track::handle_type hPT(new snemo::datamodel::particle_track);
        hPT.grab().set_trajectory_handle(*itrajectory);
        hPT.grab().set_track_id(particle_track_data_.get_number_of_particles());
        particle_track_data_.add_particle(hPT);

        // Compute particle charge
        if (_CCD_) _CCD_->process(a_trajectory, hPT.grab());

        // Determine track vertices
        if (_VED_) _VED_->process(a_trajectory, hPT.grab());

        // Associate vertices to calorimeter hits
        if (_CAD_) _CAD_->process(calibrated_data_.calibrated_calorimeter_hits(), hPT.grab());
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
        = std::find_if(chits.begin(), chits.end(), pred_via_handle);
      while (ihit != chits.end()) {
        particle_track_data_.grab_non_associated_calorimeters().push_back(*ihit);
        ihit = std::find_if(++ihit, chits.end(), pred_via_handle);
      }

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::charged_particle_tracking_module, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::charged_particle_tracking_module");
  ocd_.set_class_description("A module that performs the physical interpretation of tracker trajectory");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation("This module uses some dedicated drivers to reconstruct physical quantities   \n"
                               "such as electric charge or track vertices.                                   \n"
                               "It uses 3 dedicated drivers to perform reconstruction steps:                 \n"
                               " 1) Charge Computation Driver determines the electric charge of the track    \n"
                               " 2) Vertex Extrapolation Driver builds the list of vertices such as          \n"
                               "    foil vertex or calorimeter wall vertices                                 \n"
                               " 3) Calorimeter Association Driver associates a track with a calorimeter hit \n");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
      .set_terse_description("The label/name of the 'calibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used  \n"
                            "as the source of input calorimeter hits. \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_calibrated_data_label())
      .add_example("Use an alternative name for the \n"
                   "'calibrated data' bank::        \n"
                   "                                \n"
                   "  CD_label : string = \"CD2\"   \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'TTD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("TTD_label")
      .set_terse_description("The label/name of the 'tracker trajectory data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used      \n"
                            "as the source of input tracker trajectories. \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_tracker_trajectory_data_label())
      .add_example("Use an alternative name for the  \n"
                   "'tracker trajectory data' bank:: \n"
                   "                                 \n"
                   "  TTD_label : string = \"TTD2\"  \n"
                   "                                 \n"
                   )
      ;
  }

  {
    // Description of the 'PTD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("PTD_label")
      .set_terse_description("The label/name of the 'particle track data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used as \n"
                            "the sink of output particle tracks.        \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_particle_track_data_label())
      .add_example("Use an alternative name for the \n"
                   "'particle track data' bank::    \n"
                   "                                \n"
                   "  PTD_label : string = \"PTD2\" \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'Geo_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("Geo_label")
      .set_terse_description("The label/name of the geometry service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the service to be used as the \n"
                            "geometry service.                                 \n"
                            "This property is only used if no geometry manager \n"
                            "as been provided to the module.                   \n"
                            )
      .set_default_value_string(snemo::processing::service_info::default_geometry_service_label())
      .add_example("Use an alternative name for the geometry service:: \n"
                   "                                                   \n"
                   "  Geo_label : string = \"geometry2\"               \n"
                   "                                                   \n"
                   )
      ;
  }

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::vertex_extrapolation_driver::init_ocd(ocd_);
  ::snemo::reconstruction::charge_computation_driver::init_ocd(ocd_);
  ::snemo::reconstruction::calorimeter_association_driver::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the ``datatools::properties`` \n"
                               "ASCII format::                                                        \n"
                               "                                                                      \n"
                               "  CD_label                     : string = \"CD\"                      \n"
                               "  TTD_label                    : string = \"TTD\"                     \n"
                               "  PTD_label                    : string = \"PTD\"                     \n"
                               "  Geo_label                    : string = \"geometry\"                \n"
                               "  VED.logging.priority         : string = \"fatal\"                   \n"
                               "  VED.use_linear_extrapolation : boolean = 0                          \n"
                               "  CCD.logging_priority         : string = \"fatal\"                   \n"
                               "  CCD.charge_from_source       : boolean = 1                          \n"
                               "  CAD.logging_priority         : string = \"fatal\"                   \n"
                               "  CAD.matching_tolerance       : real as length = 50 mm               \n"
                               "                                                                      \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::charged_particle_tracking_module,
                               "snemo::reconstruction::charged_particle_tracking_module")

// end of falaise/snemo/reconstruction/charged_particle_tracking_module.cc
