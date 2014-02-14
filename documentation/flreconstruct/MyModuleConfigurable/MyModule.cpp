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
MyModule::MyModule() : dpp::base_module(), fudgeFactor_(1.0) {
  this->reset();
}

// Destruct
MyModule::~MyModule() {
  // MUST reset module at destruction
  if (this->is_initialized()) this->reset();
}

// Initialize
void MyModule::initialize(const datatools::properties& myConfig,
                          datatools::service_manager& /*flServices*/,
                          dpp::module_handle_dict_type& /*moduleDict*/) {
  // Throw logic exception if we've already initialized this instance
  DT_THROW_IF(this->is_initialized(),
              std::logic_error,
              "MyModule already initialized");

  // Extract the fudge_factor key from the supplied config, if
  // the key exists. datatools::properties throws an exception if
  // the key isn't in the config, so catch this if thrown and don't do
  // anything
  try {
    myConfig.fetch("fudge_factor",this->fudgeFactor_);
  } catch (std::logic_error& e) {
  }

  this->_set_initialized(true);
}

// Process
dpp::base_module::process_status
MyModule::process(datatools::things& /*workItem*/) {
  // Hello world!
  std::cout << "MyModule::process using fudgeFactor("
            << fudgeFactor_ << ")" << std::endl;

  // MUST return a status, see ref dpp::processing_status_flags_type
  return PROCESS_OK;
}

// Reset
void MyModule::reset() {
  this->_set_initialized(false);
  fudgeFactor_ = 1.0;
}
