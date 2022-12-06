// falaise/snemo/simulation/simrc_module.cc

// Ourselves:
#include "simrc_module.hpp"

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/logger.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// - Falaise:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/property_set.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/resource.h>
#include <falaise/snemo/services/services.h>

namespace snemo {

  namespace simulation {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(simrc_module, "snemo::simulation::simrc_module")

    // Constructor :
    simrc_module::simrc_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    simrc_module::~simrc_module()
    {
      if (is_initialized()) {
        simrc_module::reset();
      }
      return;
    }

    void simrc_module::_set_defaults()
    {
      _geoManager_ = nullptr;
      _runInfos_ = nullptr;
      _runInfoServiceName_.clear();
      _EHTag_.clear();
      _SDTag_.clear();
      return;
    }

    const geomtools::manager & simrc_module::get_geometry_manager() const
    {
      return *_geoManager_;
    }

    void simrc_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geoManager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geoManager_->get_setup_label();
      DT_THROW_IF(setup_label != "snemo::demonstrator",
                  std::logic_error, "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void simrc_module::initialize(const datatools::properties & config_,
                                  datatools::service_manager & services_,
                                  dpp::module_handle_dict_type & /* unused */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(config_);

      falaise::property_set ps{config_};
      _EHTag_ = ps.get<std::string>("EH_label", snedm::labels::event_header());
      _SDTag_ = ps.get<std::string>("SD_label", snedm::labels::simulated_data());

      snemo::service_handle<snemo::geometry_svc> geoService{services_};
      set_geometry_manager(*(geoService.operator->()));
      _caloTypes_ = {"calo", "xcalo", "gveto"};
      _ggType_ = "gg";

      // Search for a service named 'run_info"
      _runInfoServiceName_ = ps.get<std::string>("run_info_label", snemo::service_info::runInfoServiceName());
      if (services_.has(_runInfoServiceName_) and services_.is_a<snemo::run_info_service>(_runInfoServiceName_)) {
        _runInfos_ = &services_.get<snemo::run_info_service>(_runInfoServiceName_);
      }
     
      // snemo::service_handle<snemo::run_info_service> runInfoService{services_};
      // _runInfos_ = runInfoService.operator->();
      // snemo::service_handle<snemo::tracker_cell_status_service> cellStatusService{services_};
      // snemo::service_handle<snemo::calorimeter_om_status_service> omStatusService{services_};
     
      if (config_.has_key("timestamp_event")) {
        _event_timestamping_ = config_.fetch_boolean("timestamp_event");
      }
      
      if (config_.has_key("tag_tracker_cell")) {
        _tracker_cell_tag_ = config_.fetch_boolean("tag_tracker_cell");
      }

      if (config_.has_key("tag_calorimeter_om")) {
        _calorimeter_om_tag_ = config_.fetch_boolean("tag_calorimeter_om");
      }
      
      if (_event_timestamping_) {
        DT_LOG_DEBUG(get_logging_priority(), "Activate MC event timestamping");
        _event_timestamper_ = std::make_unique<event_timestamper>();
        if (_runInfos_ != nullptr) {
          // Use the run info service from the event timestamper
         DT_LOG_DEBUG(get_logging_priority(), "MC event timestamping uses the run_info service");
         _event_timestamper_->set_run_info_service(*_runInfos_);
        }
        datatools::properties eventTimestamperConfig;
        config_.export_and_rename_starting_with(eventTimestamperConfig, "event_timestamper.", "");
        _event_timestamper_->initialize(eventTimestamperConfig);
      }

      if (_tracker_cell_tag_) {
        DT_LOG_DEBUG(get_logging_priority(), "Activate MC event tracker cell tagging");
        _tracker_cell_tagger_ = std::make_unique<tracker_cell_tagger>();
        datatools::properties cellTaggerConfig;
        config_.export_and_rename_starting_with(cellTaggerConfig, "tracker_cell_tagger.", "");
        _tracker_cell_tagger_->set_geometry_manager(*_geoManager_);
        snemo::service_handle<snemo::tracker_cell_status_service> cellStatusService{services_};
         DT_LOG_DEBUG(get_logging_priority(), "MC event tracker cell tagging uses the cell status service");
        _tracker_cell_tagger_->set_tracker_cell_status_service(*(cellStatusService.operator->()));
        _tracker_cell_tagger_->initialize(cellTaggerConfig);
      }
      
      if (_calorimeter_om_tag_) {
        DT_LOG_DEBUG(get_logging_priority(), "Activate MC event calorimeter OM tagging");
        _calorimeter_om_tagger_ = std::make_unique<calorimeter_om_tagger>();
        datatools::properties omTaggerConfig;
        config_.export_and_rename_starting_with(omTaggerConfig, "calorimeter_om_tagger.", "");
        _calorimeter_om_tagger_->set_geometry_manager(*_geoManager_);
        snemo::service_handle<snemo::calorimeter_om_status_service> omStatusService{services_};
         DT_LOG_DEBUG(get_logging_priority(), "MC event calorimeter om tagging uses the om status service");
        _calorimeter_om_tagger_->set_calorimeter_om_status_service(*(omStatusService.operator->()));
        _calorimeter_om_tagger_->initialize(omTaggerConfig);
      }
  
      _set_initialized(true);
      return;
    }

    void simrc_module::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      
      if (_event_timestamper_) {
        if (_event_timestamper_->is_initialized()) {
          _event_timestamper_->reset();
        }
        _event_timestamper_.reset();
      }

      // Reset the calo OM tagger :
      if (_calorimeter_om_tagger_) {
        if (_calorimeter_om_tagger_->is_initialized()) {
          _calorimeter_om_tagger_->reset();
        }
        _calorimeter_om_tagger_.reset();
      }
      
      // Reset the tracker cell tagger :
      if (_tracker_cell_tagger_) {
        if (_tracker_cell_tagger_->is_initialized()) {
          _tracker_cell_tagger_->reset();
        }
        _tracker_cell_tagger_.reset();
      }

      _set_defaults();
      return;
    }

    dpp::base_module::process_status simrc_module::process(datatools::things & event_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
     
      // Check event header:
      if (!event_.has(_EHTag_)) {
        DT_THROW(std::logic_error, "Missing event header data to be processed !");
      }
      snemo::datamodel::event_header & eh = event_.grab<snemo::datamodel::event_header>(_EHTag_);
       
      // Check simulated data:
      if (!event_.has(_SDTag_)) {
        DT_THROW(std::logic_error, "Missing simulated data to be processed !");
      }
      mctools::simulated_data & sd = event_.grab<mctools::simulated_data>(_SDTag_);
      
      // Main processing method :
      _process_impl(eh, sd);
      
      return dpp::base_module::PROCESS_SUCCESS;
    }

    void simrc_module::_process_impl(snemo::datamodel::event_header & eh_,
                                     mctools::simulated_data & sd_)
    {
  
      if (_event_timestamper_) {
        _event_timestamper_->process(eh_);
      }

      if (_tracker_cell_tagger_) {
        _tracker_cell_tagger_->process(eh_, sd_);
      }

      if (_calorimeter_om_tagger_) {
        _calorimeter_om_tagger_->process(eh_, sd_);
      }
        
      return;
    }

  }  // end of namespace simulation

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::simulation::simrc_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::simulation::simrc_module, ocd_) {
  ocd_.set_class_name("snemo::simulation::simrc_s2c_module");
  ocd_.set_class_description(
      "A module that applies real running conditions to simulated data");
  ocd_.set_class_library("Falaise_SimRC");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
                               "Example: \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::simulation::simrc_module,
                               "snemo::simulation::simrc_module")

// end
    
