// Interface from Falaise
#include "falaise/snemo/processing/module.h"

// #include "MyNewDataType.h"

class AccessThingsModule {
 public:
  AccessThingsModule() = default;

  AccessThingsModule(falaise::property_set const& /*ps*/,
                     datatools::service_manager& /*services*/) {}

  // Process event
  falaise::processing::status process(datatools::things& event) {
    // Add our custom type to the event
    //MyNewDataType & atmCounter = workItem.add<MyNewDataType>("ATMCounter");
    //atmCounter.increment()

    return falaise::processing::status::PROCESS_OK;
  }
};

FALAISE_REGISTER_MODULE(AccessThingsModule)
