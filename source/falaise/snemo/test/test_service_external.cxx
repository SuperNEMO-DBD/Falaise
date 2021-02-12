#include <iostream>

#include <bayeux/datatools/base_service.h>
#include <bayeux/datatools/library_loader.h>
#include <bayeux/datatools/service_manager.h>

#include <falaise/snemo/services/service_handle.h>

// Plugin interface
// NB: means we need to link to the library as well (just like in Art)
//#include "plugin_service.h"

// Compiled in, so should always be present,
class builtin_service : public datatools::base_service {
 public:
  virtual int initialize(const datatools::properties&, datatools::service_dict_type&) {return 0;}
  virtual int reset() { return 0;}

  virtual bool is_initialized() const { return false; }

  void builtin_impl() { std::cout << "builtin_impl" << std::endl; }

 private:
  DATATOOLS_SERVICE_REGISTRATION_INTERFACE(builtin_service)
};
DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(builtin_service, "builtin_service")

#define SNEMO_SERVICE_TRAITS_DIRECT(T)                          \
  namespace snemo {                                             \
  template <>                                                   \
  struct service_traits<T> {                                    \
    using label_type = BOOST_METAPARSE_STRING(#T);              \
    using service_type = T;                                     \
    using instance_type = service_type;                         \
    static instance_type* get(service_type& sm) { return &sm; } \
  };                                                            \
  }

SNEMO_SERVICE_TRAITS_DIRECT(builtin_service)

int main() {
  std::cout << "start" << std::endl;

  // service_manager has a private factory, and begins
  // by importing the global one. When we load a plugin,
  // that's where it'll appear, so must load plugin **before** creating
  // service_manager (also mean the library loader for services requires
  // same lifetime as service_manager)
  // Now try and load the test plugin
  //datatools::library_loader svc_loader{};
  //svc_loader.load("plugin_service");

  // Did it load?
  //svc_loader.print(std::cout);

  // Construct service manager
  datatools::service_manager svc{};
  svc.set_allow_dynamic_services(true);

  // What's it got by default?
  svc.tree_dump(std::cout, "As Constructed:");

  // MUST load service before it can be used.
  svc.load("builtin_service", "builtin_service", {});
  //svc.load("plugin_service", "plugin_service", {});

  // Can we extract the builtin service?
  snemo::service_handle<builtin_service> bs{svc};
  bs->builtin_impl();

  // Can we extract the plugged service?
  // Yes, but only if we link to it
  // Remember, here we're mocking falaise, which loads the service,
  // modules will access the service, and be linked to it.
  // snemo::service_handle<plugin_service> ps{svc};
  // bs->builtin_impl();

  // What's it got after loading?
  svc.tree_dump(std::cout, "As Constructed:");

  return 0;
}