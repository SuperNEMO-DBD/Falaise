#include "AccessThingsModule.h"

#include <boost/foreach.hpp>

#include "bayeux/mctools/simulated_data.h"

DPP_MODULE_REGISTRATION_IMPLEMENT(AccessThingsModule,"AccessThingsModule");

AccessThingsModule::AccessThingsModule() : dpp::base_module()
{}

AccessThingsModule::~AccessThingsModule() {
  if (is_initialized()) this->reset();
}

void AccessThingsModule::initialize(const datatools::properties& /*myConfig*/,
                          datatools::service_manager& /*flServices*/,
                          dpp::module_handle_dict_type& /*moduleDict*/) {
  this->_set_initialized(true);
}

//! [AccessThingsModule::Process]
dpp::base_module::process_status
AccessThingsModule::process(datatools::things& workItem) {
  // Print most basic information
  std::cout << "AccessThingsModule::process called!" << std::endl;
  std::cout << "[name]        : " << workItem.get_name() << std::endl;
  std::cout << "[description] : " << workItem.get_description() << std::endl;

  // Extract list of keys stored by the object
  std::vector<std::string> workItemKeyList;
  workItem.get_names(workItemKeyList);

  // Iterate over keys, printing their name and the type of the object
  // they map to
  BOOST_FOREACH(std::string key, workItemKeyList) {
    std::cout << "- [key, serial_tag] : "
              << key
              << ", "
              << workItem.get_entry_serial_tag(key)
              << std::endl;
  }

  // Grab simulated data bank
  // Simulated data will only be present in simulation output files,
  // so wrap in a try block
  try {
    const mctools::simulated_data& simData = workItem.get<mctools::simulated_data>("SD");
    simData.tree_dump();
  } catch (std::logic_error& e) {
    std::cerr << "failed to grab SD bank : " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  }

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}
//! [AccessThingsModule::Process]

void AccessThingsModule::reset() {
  this->_set_initialized(false);
}
