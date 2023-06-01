// - Implementation of UddAnalysis

// Ourselves
#include "UddAnalysis.hpp"

DPP_MODULE_REGISTRATION_IMPLEMENT(UddAnalysis, "UddAnalysis");

// Construct
UddAnalysis::UddAnalysis()
  : dpp::base_module()
{
}

// Destruct
UddAnalysis::~UddAnalysis()
{
  // MUST reset module at destruction
  if (this->is_initialized()) {
    this->UddAnalysis::reset();
  }
  return;
}

// Initialize
void UddAnalysis::initialize(const datatools::properties & /*config_*/,
                             datatools::service_manager & /*services_*/,
                             dpp::module_handle_dict_type & /*modules_*/)
{
  // Throw logic exception if we've already initialized this instance
  DT_THROW_IF(this->is_initialized(), std::logic_error, "UddAnalysis already initialized");

  
  this->_set_initialized(true);
  return;
}
 
// Reset
void UddAnalysis::reset() {
  // Throw logic exception if we've not initialized this instance
  DT_THROW_IF(!this->is_initialized(), std::logic_error, "UddAnalysis not initialized");
  this->_set_initialized(false);

  
  return;
}

// Process
dpp::base_module::process_status UddAnalysis::process(datatools::things & event_record_)
{
  if (event_record_.has("UDD")) {
    
  }
  return dpp::base_module::PROCESS_OK;
}
