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
dpp::base_module::process_status AccessThingsModule::process(
    datatools::things& workItem) {
  process_status readStatus = this->read(workItem);
  if (readStatus != PROCESS_OK) return readStatus;

  process_status writeStatus = this->write(workItem);

  // MUST return a status, see ref dpp::processing_status_flags_type
  return writeStatus;
}
//! [AccessThingsModule::Process]

void AccessThingsModule::reset() {
  this->_set_initialized(false);
}

dpp::base_module::process_status AccessThingsModule::read(datatools::things& workItem) {
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
    return PROCESS_INVALID;
  }

  return PROCESS_OK;
}

//! [AccessThingsModule::write]
dpp::base_module::process_status AccessThingsModule::write(datatools::things& workItem) {
  // Add a new entry to the things
  datatools::properties& atmProperties = workItem.add<datatools::properties>("ATMProperties");
  atmProperties.set_description("Properties added by the AccessThings Module");
  atmProperties.store("foo", "bar");
  atmProperties.store("baz", 3.14);

  return PROCESS_OK;
}
