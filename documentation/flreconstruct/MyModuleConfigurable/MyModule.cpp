// Interface from Falaise
#include "falaise/snemo/processing/module.h"

class MyModule {
 public:
  MyModule() = default;

  MyModule(falaise::property_set const& ps, datatools::service_manager& /*services*/)
      : message(ps.get<std::string>("message")) {}

  falaise::processing::status process(datatools::things& /*workItem*/) {
    std::cout << "MyModule::process says '" << message << "'\n";
    return falaise::processing::status::PROCESS_OK;
  }

 private:
  std::string message{};
};

FALAISE_REGISTER_MODULE(MyModule);
