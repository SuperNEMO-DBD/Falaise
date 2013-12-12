#include "AccessThingsModule.h"

#include <boost/foreach.hpp>

#include "bayeux/mctools/simulated_data.h"

DPP_MODULE_REGISTRATION_IMPLEMENT(AccessThingsModule,"AccessThingsModule");

AccessThingsModule::AccessThingsModule() : dpp::base_module()
{}

AccessThingsModule::~AccessThingsModule() {
  this->reset();
}

void AccessThingsModule::initialize(const datatools::properties& /*myConfig*/,
                          datatools::service_manager& /*flServices*/,
                          dpp::module_handle_dict_type& /*moduleDict*/) {
  this->_set_initialized(true);
}

//! [AccessThingsModule::Process]
int AccessThingsModule::process(datatools::things& workItem) {
  // Print most basic information
  std::cout << "AccessThingsModule::process called!" << std::endl;
  std::cout << "[name]        : " << workItem.get_name() << std::endl;
  std::cout << "[description] : " << workItem.get_description() << std::endl;
  // Extract list of keys stored by the object
  std::vector<std::string> workItemKeyList;
  workItem.get_names(workItemKeyList);
  BOOST_FOREACH(std::string k, workItemKeyList) {
    std::cout << "- [key] : " << k <<std::endl;
    std::cout << "  - [serial_tag] : " << workItem.get_entry_serial_tag("SD") << std::endl;
  }

  try {
    const mctools::simulated_data& simData = workItem.grab<mctools::simulated_data>("SD");
  } catch (std::logic_error& e) {
    std::cerr << "failed to grab SD bank : " << e.what() << std::endl;
    return dpp::PROCESS_INVALID;
  }

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::PROCESS_OK;
}
//! [AccessThingsModule::Process]

void AccessThingsModule::reset() {
  this->_set_initialized(false);
}

