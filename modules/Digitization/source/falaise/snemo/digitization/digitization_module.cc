/// \file falaise/snemo/digitization/digitization_module.cc
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/digitization_module.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>

// This project (Falaise):
//#include <falaise/snemo/processing/services.h>
#include <falaise/snemo/datamodels/data_model.h>

// This plugin (digitization_module) :
#include <falaise/snemo/digitization/digitization_driver.h>

namespace snemo {

namespace digitization {

// Registration instantiation macro
DPP_MODULE_REGISTRATION_IMPLEMENT(digitization_module, "snemo::digitization::digitization_module")

digitization_module::digitization_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_) {
  _set_defaults();
  return;
}

digitization_module::~digitization_module() {
  if (is_initialized()) digitization_module::reset();
  return;
}

void digitization_module::set_geometry_manager(const geomtools::manager& geo_mgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized/locked !");
  _geometry_manager_ = &geo_mgr_;
  return;
}

const geomtools::manager& digitization_module::get_geometry_manager() const {
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
  return *_geometry_manager_;
}

bool digitization_module::has_geometry_manager() const { return _geometry_manager_ != 0; }

void digitization_module::_set_defaults() {
  _SSD_label_ = snemo::datamodel::data_info::default_simulated_signal_data_label();
  _SDD_label_ = snemo::datamodel::data_info::default_simulated_digitized_data_label();
  _driver_->reset();
  return;
}

void digitization_module::initialize(const datatools::properties& setup_,
                                     datatools::service_manager& /* service_manager_*/,
                                     dpp::module_handle_dict_type& /* module_dict_ */)

{
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  dpp::base_module::_common_initialize(setup_);
  if (setup_.has_key("SSD_label")) {
    _SSD_label_ = setup_.fetch_string("SSD_label");
  }

  // Driver :
  _driver_->initialize(setup_);
  // Tag the module as initialized :
  _set_initialized(true);
}

void digitization_module::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  _set_initialized(false);
  if (_driver_.get() != 0) {
    if (_driver_->is_initialized()) {
      _driver_->reset();
    }
    _driver_.reset();
  }
  _set_defaults();
  return;
}

dpp::base_module::process_status digitization_module::process(datatools::things& data_record_) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  /*************************
   * Check simulated data *
   *************************/
  const bool abort_at_missing_input = true;
  // Check if some 'simulated_data' are available in the data model:
  if (!data_record_.has(_SSD_label_)) {
    DT_THROW_IF(abort_at_missing_input, std::logic_error,
                "Missing signal simulated data to be processed !");
    // leave the data unchanged.
    return dpp::base_module::PROCESS_ERROR;
  }
  // Get the 'simulated_data' entry from the data model :
  const mctools::signal::signal_data& the_signal_data =
      data_record_.get<mctools::signal::signal_data>(_SSD_label_);

  /********************
   * Process the data *
   ********************/
  // Main processing method :
  _process(the_signal_data);

  return dpp::base_module::PROCESS_SUCCESS;
}

void digitization_module::_process(const mctools::signal::signal_data& SSD_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  _driver_->process_digitization_algorithms(SSD_);

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

}  // end of namespace digitization

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::digitization::digitization_module, ocd_) {
  ocd_.set_class_name("snemo::digitization::digitization_module");
  ocd_.set_class_description(
      "A module that performs a selection on simulated data based on the number of PMT and GG "
      "cells hit");
  ocd_.set_class_library("Falaise_DigiTrigger");
  ocd_.set_class_documentation(
      " \n"
      " \n");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SSD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SSD_label")
        .set_terse_description("The label/name of the 'simulated data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used  \n"
            "after a simulation. \n")
        .set_default_value_string(
            snemo::datamodel::data_info::default_simulated_signal_data_label())
        .add_example(
            "Use an alternative name for the \n"
            "'signal data' bank::        \n"
            "                                \n"
            "  SSD_label : string = \"SSD2\"   \n"
            "                                \n");
  }

  ocd_.set_validation_support(true);
  ocd_.lock();

  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::digitization::digitization_module,
                               "snemo::digitization::digitization_module")

// end of falaise/snemo/digitization/digitization_module.cc
