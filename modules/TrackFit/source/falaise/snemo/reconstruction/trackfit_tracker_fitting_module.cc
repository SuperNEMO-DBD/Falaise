/// \file falaise/snemo/reconstruction/trackfit_tracker_fitting_module.cc

// Ourselves:
#include <snemo/reconstruction/trackfit_tracker_fitting_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/processing/services.h>

// TrackFit
#include <snemo/reconstruction/trackfit_driver.h>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(trackfit_tracker_fitting_module,
                                      "snemo::reconstruction::trackfit_tracker_fitting_module");

    const geomtools::manager & trackfit_tracker_fitting_module::get_geometry_manager() const
    {
      return *_geometry_manager_;
    }

    void trackfit_tracker_fitting_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geometry_manager_->get_setup_label();
      DT_THROW_IF (setup_label != "snemo::demonstrator" &&
                   setup_label != "snemo::tracker_commissioning",
                   std::logic_error,
                   "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void trackfit_tracker_fitting_module::_set_defaults()
    {
      _geometry_manager_ = 0;
      _TCD_label_.clear();
      _TTD_label_.clear();
      _driver_.reset(0);
      return;
    }

    // Initialization :
    void trackfit_tracker_fitting_module::initialize(const datatools::properties  & setup_,
                                                     datatools::service_manager   & service_manager_,
                                                     dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF (is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(setup_);

      if (_TCD_label_.empty()) {
        if (setup_.has_key("TCD_label")) {
          _TCD_label_ = setup_.fetch_string("TCD_label");
        }
      }
      // Default label:
      if (_TCD_label_.empty()) {
        _TCD_label_ = snemo::datamodel::data_info::default_tracker_clustering_data_label();
      }

      if (_TTD_label_.empty()) {
        if (setup_.has_key("TTD_label")) {
          _TTD_label_ = setup_.fetch_string("TTD_label");
        }
      }
      // Default label:
      if (_TTD_label_.empty()) {
        _TTD_label_ = snemo::datamodel::data_info::default_tracker_trajectory_data_label();
      }

      // Geometry manager :
      if (_geometry_manager_ == 0) {
        std::string geo_label = snemo::processing::service_info::default_geometry_service_label();
        if (setup_.has_key("Geo_label")) {
          geo_label = setup_.fetch_string("Geo_label");
        }
        DT_THROW_IF (geo_label.empty(), std::logic_error,
                     "Module '" << get_name() << "' has no valid '" << "Geo_label" << "' property !");
        DT_THROW_IF (! service_manager_.has(geo_label) ||
                     ! service_manager_.is_a<geomtools::geometry_service>(geo_label),
                     std::logic_error,
                     "Module '" << get_name() << "' has no '" << geo_label << "' service !");
        const geomtools::geometry_service & Geo
          = service_manager_.get<geomtools::geometry_service>(geo_label);
        set_geometry_manager(Geo.get_geom_manager());
      }

      // Tracking algorithm :
      std::string algorithm_id = trackfit_driver::trackfit_id();
      // Initialize the fitting algo:
      _driver_.reset(new trackfit_driver);
      DT_THROW_IF (! _driver_, std::logic_error,
                   "Module '" << get_name() << "' could not instantiate the '"
                   << algorithm_id << "' tracker fitting algorithm !");

      // Plug the geometry manager :
      _driver_.get()->set_geometry_manager(get_geometry_manager());

      // Initialize the clustering driver :
      _driver_.get()->initialize(setup_);

      _set_initialized(true);
      return;
    }

    void trackfit_tracker_fitting_module::reset()
    {
      DT_THROW_IF (! is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the fitter driver :
      if (_driver_) {
        if (_driver_->is_initialized()) {
          _driver_->reset();
        }
        _driver_.reset();
      }
      _set_defaults();
      return;
    }

    // Constructor :
    trackfit_tracker_fitting_module::trackfit_tracker_fitting_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    trackfit_tracker_fitting_module::~trackfit_tracker_fitting_module()
    {
      if (is_initialized()) trackfit_tracker_fitting_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status trackfit_tracker_fitting_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");

      ////////////////////////////////////
      // Check tracker clustering data  //
      ////////////////////////////////////

      bool abort_at_missing_input = true;

      // Check if some 'tracker_clustering_data' are available in the data model:
      if (! data_record_.has(_TCD_label_)) {
        DT_THROW_IF (abort_at_missing_input, std::logic_error,
                     "Missing tracker clustering data to be processed !");
        // leave the data unchanged.
        return dpp::base_module::PROCESS_ERROR;
      }
      // grab the 'tracker_clustering_data' entry from the data model :
      snemo::datamodel::tracker_clustering_data & the_tracker_clustered_data
        = data_record_.grab<snemo::datamodel::tracker_clustering_data>(_TCD_label_);

      ////////////////////////////////////
      // Check tracker trajectory data  //
      ////////////////////////////////////

      bool abort_at_former_output = false;
      bool preserve_former_output = false;

      // check if some 'tracker_trajectory_data' are available in the data model:
      snemo::datamodel::tracker_trajectory_data * ptr_trajectory_data = 0;
      if (! data_record_.has(_TTD_label_)) {
          ptr_trajectory_data = &(data_record_.add<snemo::datamodel::tracker_trajectory_data>(_TTD_label_));
        } else {
          ptr_trajectory_data = &(data_record_.grab<snemo::datamodel::tracker_trajectory_data>(_TTD_label_));
        }
      snemo::datamodel::tracker_trajectory_data & the_tracker_trajectory_data = *ptr_trajectory_data;
      if (the_tracker_trajectory_data.has_solutions()) {
          DT_THROW_IF (abort_at_former_output, std::logic_error,
                       "Already has processed tracker trajectory data !");
          if (! preserve_former_output) {
              the_tracker_trajectory_data.reset();
            }
        }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      _process(the_tracker_clustered_data, the_tracker_trajectory_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void trackfit_tracker_fitting_module::_process(const snemo::datamodel::tracker_clustering_data & clustering_data_,
                                                   snemo::datamodel::tracker_trajectory_data       & trajectory_data_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Process isolated tracks using external resource:
      if (! clustering_data_.has_solutions()) {
          DT_LOG_DEBUG(get_logging_priority(), "No clustered solution to be fitted");
          return;
        }

      // process the fitter driver :
      _driver_.get()->process(clustering_data_, trajectory_data_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::trackfit_tracker_fitting_module, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::trackfit_tracker_fitting_module");
  ocd_.set_class_description("A module that performs the track fitting using the TrackFit algorithms");
  ocd_.set_class_library("Falaise_TrackFit");
  ocd_.set_class_documentation("This module uses the TrackFit trjectory fitting algorithms for.   \n"
                               "charged particles traversing the tracking chamber. See also OCD   \n"
                               "support for the ``snemo::reconstruction::trackfit_driver`` class. \n");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
      .set_terse_description("The label/name of the 'tracker clustering data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used as    \n"
                            "the source of input clusters of tracker hits. \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_tracker_clustering_data_label())
      .add_example("Use an alternative name for the 'tracker clustering data' bank:: \n"
                   "                                  \n"
                   "  TCD_label : string = \"TCD2\"   \n"
                   "                                  \n"
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
      .set_long_description("This is the name of the bank to be used as \n"
                            "the sink of output tracker trajectories.   \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_tracker_trajectory_data_label())
      .add_example("Use an alternative name for the 'tracker trajectory data' bank:: \n"
                   "                                  \n"
                   "  TTD_label : string = \"TTD2\"   \n"
                   "                                  \n"
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
                   "                                     \n"
                   "  Geo_label : string = \"geometry2\" \n"
                   "                                     \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                         \n"
                               "  TCD_label : string = \"TCD\"           \n"
                               "  TTD_label : string = \"TTD\"           \n"
                               "  Geo_label : string = \"geometry\"      \n"
                               "                                         \n"
                               "Additional specific parameters are used to configure         \n"
                               "the embedded ``TrackFit`` driver itself; see the OCD support \n"
                               "of the ``snemo::reconstruction::trackfit_driver`` class.     \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::trackfit_tracker_fitting_module,
                               "snemo::reconstruction::trackfit_tracker_fitting_module")
