// Interface from Falaise
#include "falaise/snemo/processing/module.h"

// Any data models we need
#include <bayeux/mctools/simulated_data.h>

class AccessThingsModule {
 public:
  AccessThingsModule() = default;

  AccessThingsModule(falaise::property_set const& /*ps*/,
                     datatools::service_manager& /*services*/) {}

  // Process event
  falaise::processing::status process(datatools::things& event) {
    std::cout << "AccessThingsModule::process called!" << std::endl;
    read(event);
    write(event);
    return falaise::processing::status::PROCESS_OK;
  }

 private:
  // read data
  void read(datatools::things& event) {
    // Extract list of keys stored by the object
    std::vector<std::string> keys;
    event.get_names(keys);

    // Print each key and its concrete type
    for (const auto& k : keys) {
      std::cout << "- [key, serial_tag] : " << k << ", " << event.get_entry_serial_tag(k)
                << std::endl;
    }
  }

  // write data
  void write(datatools::things& event) {
    // Add a new data bank to the event
    auto& atmProperties = event.add<datatools::properties>("ATMProperties");
    atmProperties.set_description("Properties added by the AccessThings Module");
    atmProperties.store("foo", "bar");
  }
};

FALAISE_REGISTER_MODULE(AccessThingsModule)
