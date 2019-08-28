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
    // Print most basic information
    std::cout << "AccessThingsModule::process called!" << std::endl;
    std::cout << "[name]        : " << event.get_name() << std::endl;
    std::cout << "[description] : " << event.get_description() << std::endl;

    // Extract list of keys stored by the object
    std::vector<std::string> keys;
    event.get_names(keys);

    // Print each key and its concrete type
    for (const auto& k : keys) {
      std::cout << "- [key, serial_tag] : " << k << ", " << event.get_entry_serial_tag(k)
                << std::endl;
    }

    // Grab simulated data bank
    auto& simData = event.get<mctools::simulated_data>("SD");
    simData.tree_dump();

    return falaise::processing::status::PROCESS_OK;
  }
};

FALAISE_REGISTER_MODULE(AccessThingsModule)
