// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/hello_world.h */

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