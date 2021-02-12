//! \file falaise/snemo/services/hello_world.h
//! \brief Canonical example of a service in SuperNEMO
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
