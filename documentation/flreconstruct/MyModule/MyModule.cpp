// Interface from Falaise
#include "falaise/snemo/processing/module.h"

class MyModule {
 public:
  // Default constructor
  MyModule() = default;

  // User-defined Constructor
  MyModule(falaise::property_set const& /*ps*/, datatools::service_manager& /*services*/) {}

  // Process event
  falaise::processing::status process(datatools::things& /*e*/) {
    std::cout << "MyModule::process called!\n";
    return falaise::processing::status::PROCESS_OK;
  }
};

// Register module with Falaise's plugin system on load
FALAISE_REGISTER_MODULE(MyModule)
