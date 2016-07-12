#include "AccessThingsModule.h"

#include <boost/foreach.hpp>

#include "bayeux/mctools/simulated_data.h"

#include "MyDataType.h"

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
  // Add our custom type to the item
  MyDataType & atmCounter = workItem.add<MyDataType>("ATMCounter");
  atmCounter.increment();

  return PROCESS_OK;

}
//! [AccessThingsModule::Process]

void AccessThingsModule::reset() {
  this->_set_initialized(false);
}
