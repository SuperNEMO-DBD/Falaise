// Interface from Falaise
#include "falaise/snemo/processing/module.h"

#include "falaise/snemo/services/geometry.h"

class MyModule {
 public:
  // Default constructor
  MyModule() = default;

  // User-defined Constructor
  MyModule(falaise::config::property_set const& /*ps*/, datatools::service_manager& sp)
      : geosvc{sp} {}

  // Process event
  falaise::processing::status process(datatools::things& /*e*/) {
    std::cout << "MyModule::process called!\n";
    geosvc->tree_dump(std::cout);
    return falaise::processing::status::PROCESS_OK;
  }

 private:
  snemo::service_handle<snemo::geometry_svc> geosvc;
};

// Register module with Falaise's plugin system on load
FALAISE_REGISTER_MODULE(MyModule)
