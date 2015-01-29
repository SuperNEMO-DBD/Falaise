/// \file falaise/snemo/reconstruction/gamma_tracking_module.cc

// Ourselves:
#include <snemo/reconstruction/gamma_tracking_module.h>

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
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/processing/services.h>

// Gamma Tracking
#include <snemo/reconstruction/gamma_tracking_driver.h>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(gamma_tracking_module,
                                      "snemo::reconstruction::gamma_tracking_module");

    const geomtools::manager & gamma_tracking_module::get_geometry_manager() const
    {
      return *_geometry_manager_;
    }

    void gamma_tracking_module::set_geometry_manager(const geomtools::manager & gmgr_)
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

    void gamma_tracking_module::_set_defaults()
    {
      _geometry_manager_ = 0;
      _CD_label_ = snemo::datamodel::data_info::default_calibrated_data_label();
      _PTD_label_ = snemo::datamodel::data_info::default_tracker_clustering_data_label();
      _driver_.reset();
      return;
    }

    // Initialization :
    void gamma_tracking_module::initialize(const datatools::properties  & setup_,
                                           datatools::service_manager   & service_manager_,
                                           dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF (is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(setup_);

      if (setup_.has_key("CD_label")) {
        _CD_label_ = setup_.fetch_string("CD_label");
      }

      if (setup_.has_key("PTD_label")) {
        _PTD_label_ = setup_.fetch_string("PTD_label");
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
        geomtools::geometry_service & Geo
          = service_manager_.get<geomtools::geometry_service>(geo_label);
        set_geometry_manager(Geo.get_geom_manager());
      }

      // Gamma tracking algorithm :
      DT_THROW_IF(!setup_.has_key("driver"), std::logic_error, "Missing 'driver' algorithm");
      const std::string algorithm_id = setup_.fetch_string("driver");
      if (algorithm_id == "GT") {
        _driver_.reset(new snemo::reconstruction::gamma_tracking_driver);
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Unsupported '" << algorithm_id << "'gamma tracking algorithm ");
      }

      // Plug the geometry manager :
      _driver_.get()->set_geometry_manager(get_geometry_manager());

      // Initialize the clustering driver :
      _driver_.get()->initialize(setup_);

      _set_initialized(true);
      return;
    }

    void gamma_tracking_module::reset()
    {
      DT_THROW_IF (! is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      _set_defaults();
      return;
    }

    // Constructor :
    gamma_tracking_module::gamma_tracking_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    gamma_tracking_module::~gamma_tracking_module()
    {
      if (is_initialized()) gamma_tracking_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status gamma_tracking_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");

      const snemo::datamodel::calibrated_calorimeter_hit::collection_type * the_calos = 0;

      /*************************
       * Check calibrated data *
       *************************/

      // Check if some 'calibrated_data' are available in the data model:
      if (data_record_.has(_CD_label_)) {
        // Get the 'calibrated_data' entry from the data model :
        const snemo::datamodel::calibrated_data & the_calibrated_data
          = data_record_.get<snemo::datamodel::calibrated_data>(_CD_label_);
        the_calos = &the_calibrated_data.calibrated_calorimeter_hits();
        // DT_THROW_IF(abort_at_missing_input, std::logic_error,
        //             "Missing calibrated data to be processed !");
        // // leave the data unchanged.
        // return dpp::base_module::PROCESS_ERROR;
      }

      /*********************************
       * Check particle track data     *
       *********************************/
      snemo::datamodel::particle_track_data * ptr_particle_track_data = 0;
      if (! data_record_.has(_PTD_label_)) {
        ptr_particle_track_data
          = &(data_record_.add<snemo::datamodel::particle_track_data>(_PTD_label_));
      } else {
        ptr_particle_track_data
          = &(data_record_.grab<snemo::datamodel::particle_track_data>(_PTD_label_));
        the_calos = &ptr_particle_track_data->get_non_associated_calorimeters();
      }
      snemo::datamodel::particle_track_data & the_particle_track_data = *ptr_particle_track_data;

      /********************
       * Process the data *
       ********************/

      // Sanity check
      if (! the_calos) {
        DT_LOG_WARNING(get_logging_priority(), "No calorimeter hits to be processed !");
        return dpp::base_module::PROCESS_ERROR;
      }

      // Main processing method :
      _process(*the_calos, the_particle_track_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void gamma_tracking_module::_process(const snemo::datamodel::calibrated_calorimeter_hit::collection_type & hits_,
                                         snemo::datamodel::particle_track_data  & track_data_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // process the fitter driver :
      _driver_.get()->process(hits_, track_data_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::gamma_tracking_module, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::gamma_tracking_module");
  ocd_.set_class_description("A module that performs the gamma tracking using the Gamma_Tracking algorithms");
  ocd_.set_class_library("Falaise_GammaTracking");
  ocd_.set_class_documentation("This module uses the Gamma Tracking algorithms for.   \n"
                               "gamma involved in non associated calorimeter hits. See also OCD   \n"
                               "support for the ``snemo::reconstruction::gamma_tracking_driver`` class. \n");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'PTD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("PTD_label")
      .set_terse_description("The label/name of the 'particle track data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used as    \n"
                            "the source of calorimeter hits and reconstructed vertices. \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_particle_track_data_label())
      .add_example("Use an alternative name for the 'particle track data' bank:: \n"
                   "                                  \n"
                   "  PTD_label : string = \"PTD2\"   \n"
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
                               "  PTD_label : string = \"PTD\"           \n"
                               "  Geo_label : string = \"geometry\"      \n"
                               "                                         \n"
                               "Additional specific parameters are used to configure         \n"
                               "the embedded ``Gamma_Tracking`` driver itself; see the OCD support \n"
                               "of the ``snemo::reconstruction::gamma_tracking_driver`` class.     \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::gamma_tracking_module,
                               "snemo::reconstruction::gamma_tracking_module")
