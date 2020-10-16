Writing FLReconstruct Services {#writingflreconstructservices}
==============================

A _Service_ in Falaise is a class/object that is shared between all modules and
typically provides data/calculations common to a _Run_, i.e. a set of _Events_.
_Aside_: Bayeux, and hence Falaise, do not implement the _Run_ concept explicitly
in C++, rather each input file is considered as a _Run_.

A good example of a _Service_ is the `snemo::geometry_svc` that provides an interface
to query the detector geometry for answers to things like "where is this point?".
The FLReconstruct documentation on [writing service-aware modules](@ref usingservices)
covers the case of accessing and using services in modules. Here we describe how to
write new services for Falaise, but it should be noted that due to the current implementation
of services provided by Bayeux they _cannot_ be plugged into Falaise in the same way as modules.
Whilst this is a use case under consideration, it's expected that all services will be
supplied by Falaise to simplify use and ensure coherence.

Implementing a new Service {#implementingnewservice}
====================
Headers and source files for Falaise services should go in the `source/falaise/snemo/services` folder
for consistency. The minimal C++ code required to implement a service is shown in the `hello_world.hh`
and `hello_world.cc` files:

~~~~~
#ifndef SNEMO_HELLO_WORLD_SVC_HH
#define SNEMO_HELLO_WORLD_SVC_HH

#include "bayeux/datatools/base_service.h"
#include "falaise/snemo/services/service_traits.h"

namespace snemo {

class hello_world_svc : public datatools::base_service {
 public:
  // Inherited pure virtual member functions that must be overriden
  int initialize(const datatools::properties&, datatools::service_dict_type&) override;
  int reset() override;
  bool is_initialized() const override;

  // Actual implementation of the service interface
  // Can be several functions as required to implement the interface
  std::string say_hello(const std::string& to_who) const;

 private:
  DATATOOLS_SERVICE_REGISTRATION_INTERFACE(hello_world_svc)
};

template <>
struct service_traits<hello_world_svc> {
  using label_type = BOOST_METAPARSE_STRING("hello_world_svc");
  using service_type = hello_world_svc;
  using instance_type = service_type;
  static instance_type* get(service_type& sm) { return &sm; }
};
}  // namespace snemo

#endif  // SNEMO_HELLO_WORLD_SVC_HH
~~~~~

The service may be named anything you like, though a semi-convention is to append `_svc`
to the type name for clarity. All services must inherit from the Bayeux `datatools::base_service` interface and:

-  Implement overrides for the `initialize`, `reset`, and `is_initialized` member functions
-  Add the `DATATOOLS_SERVICE_REGISTRATION_INTERFACE` macro for the class name in the `private` section

The remaining member functions and data can be implemented as needed for your use case(s).

To integrate the service class in Falaise and allow easy construction and use via
`service_handle`s, a specialization of the `snemo::service_traits` struct template must be provided.
Almost all services can use the form shown above with all specialization points using the service typename
(as type or string). The `service_type/instance_type` split is neccessary to support legacy
Bayeux interfaces.

The implementation then follows as needed for the overriden and concrete member functions:

~~~~~
// Ourselves:
#include <falaise/snemo/services/hello_world.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::hello_world_svc, "snemo::hello_world_svc")

namespace snemo {

int hello_world_svc::initialize(const datatools::properties&, datatools::service_dict_type&) {
  // Where you configure the service if it needs/allows it
  return 0;
}

int hello_world_svc::reset() {
  // Where you free any resources the service holds (you would also need to implement a
  // destructor!!) We don't do anything special so just return "success"
  return 0;
}

bool hello_world_svc::is_initialized() const {
  // Return true/false to indicate initialization state
  // Here we are always initialized, so return true
  return true;
}

std::string hello_world_svc::say_hello(const std::string& to_who) const {
  // Implement the service implementation in one of more functions
  return std::string("Hello " + to_who);
}

}  // namespace snemo
~~~~~

The `DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT` macro must be placed in the global namespace
with the fully qualified name of your service to complete the registration/integration mechanism.
The implementation of the three overriden member functions will depend on the details of your
service. The example above shows the simplest possible case of no allowable configuration and
no resource management (i.e. allocation/deallocation of memory). You should generally code
the `initialize/reset` pair as you would a constructor/destructor pair, and implement the
latter two if sensible defaults for data members are needed.

Member functions implementing the actual service functionality can be implemented as required
by the use cases for the service.


Integrating a new Service in Falaise's CMake Build {#integratingnewservice}
==================================================
Simply add the `.h` and `.cc` files to the `FalaiseLibrary_HEADERS` and `FalaiseLibrary_SOURCES`
CMake variables in `source/falaise/snemo/snemo.cmake`. If your implementation requires use of
a new external library, please raise an Issue on GitHub to discuss this first. New dependencies
require review before they will be approved to be used in Falaise.
