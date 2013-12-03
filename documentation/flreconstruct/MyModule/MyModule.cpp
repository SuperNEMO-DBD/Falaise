// - Implementation of MyModule

// Ourselves
#include "MyModule.h"

// Standard Library

// Third Party
// - A

// This Project

// Macro which automatically implements the interface needed
// to enable the module to be loaded at runtime
// The first argument is the typename
// The second is the string key used to access the module in pipeline
// scripts. This must be globally unique.
DPP_MODULE_REGISTRATION_IMPLEMENT(MyModule,"MyModule");

// Construct
MyModule::MyModule() : dpp::base_module()
{}

// Destruct
MyModule::~MyModule()
{}

// Initialize
void MyModule::initialize(const datatools::properties& myConfig,
                          datatools::service_manager& /*notused*/,
                          dpp::module_handle_dict_type& /*notused*/) {
  this->_set_initialized(true);
}

// Process
int MyModule::process(datatools::things& /*workItem*/) {
  return dpp::PROCESS_OK;
}

// Reset
void MyModule::reset() {
  this->_set_initialized(false);
}

