// Interface from Falaise
#include "falaise/processing/module.h"

class MyModule {
 public:
  // Default constructor
  MyModule() = default;

  // User-defined Constructor
  MyModule(std::string const& /*name*/,
           datatools::properties const& /*ps*/,
           datatools::service_manager& /*services*/) {
  }

  // Process event
  falaise::processing::status process(datatools::things& /*e*/) {
    std::cout << "MyModule::process called!\n"
    return PROCESS_OK;
  }
};

// Register module with Falaise's plugin system on load
FALAISE_REGISTER_MODULE(MyModule,"MyModule");

