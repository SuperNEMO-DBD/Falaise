#ifndef SNEMO_GEOMETRY_SVC_HH
#define SNEMO_GEOMETRY_SVC_HH

#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"

namespace snemo {

// Naming is odd because of existence of snemo::geometry namespace
using geometry_svc = const geomtools::manager;

template <>
struct service_traits<geometry_svc> {
  using label_type = BOOST_METAPARSE_STRING("geometry");
  using service_type = geomtools::geometry_service;
  using instance_type = geometry_svc;

  static instance_type* get(service_type& sm) {
    return &(sm.get_geom_manager());
  }
};

} // namespace snemo

#endif // SNEMO_GEOMETRY_SVC_HH

