//! \file falaise/snemo/services/dead_cells.h
//! \brief Canonical example of a service in SuperNEMO
#ifndef SNEMO_DEAD_CELLS_SVC_HH
#define SNEMO_DEAD_CELLS_SVC_HH

#include "bayeux/datatools/base_service.h"
#include "falaise/snemo/services/service_traits.h"

namespace snemo {

class dead_cells_svc : public datatools::base_service {
 public:
  // Inherited pure virtual member functions that must be overriden
  int initialize(const datatools::properties&, datatools::service_dict_type&) override;
  int reset() override;
  bool is_initialized() const override;

  // Actual implementation of the service interface
  // Can be several functions as required to implement the interface
  std::string say_hello(const std::string& to_who) const;

 private:
  DATATOOLS_SERVICE_REGISTRATION_INTERFACE(dead_cells_svc)
};

template <>
struct service_traits<dead_cells_svc> {
  using label_type = BOOST_METAPARSE_STRING("dead_cells_svc");
  using service_type = dead_cells_svc;
  using instance_type = service_type;
  static instance_type* get(service_type& sm) { return &sm; }
};
}  // namespace snemo

#endif  // SNEMO_DEAD_CELLS_SVC_HH
