// Interface from Falaise
#include "falaise/processing/module.h"

// Current problem in the below is how to initialize with
// an optional parameter. fetch_string will throw if the
// key isn't present. Fine for required parameters, but
// many will be optional. Refer to Art docs to see how FHiCL
// handles this (with PSes). Might also be able to introduce
// type like:
//OptionalProperty<std::string>(key, ps, default)
//Property<T>(key, ps)
//Table(prefix,ps)


class MyModule {
 public:
  MyModule() = default;

  MyModule(std::string const& /*name*/, datatools::properties const& ps,
           datatools::service_manager& /*services*/)
      : message(ps.fetch_string("message")) {}

  falaise::processing::status process(datatools::things& /*workItem*/) {
    std::cout << "MyModule::process says \"" << message "\"\n";
    return falaise::processing::PROCESS_OK;
  }

 private:
  std::string message;
};

FALAISE_REGISTER_MODULE(MyModule, "MyModule");
