#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"

namespace snemo {
using geometry = const geomtools::manager;

template <>
struct service_traits<geometry> {
  using label_type = BOOST_METAPARSE_STRING("geometry");
  using service_type = geomtools::geometry_service;
  using instance_type = geometry;

  static instance_type* get(service_type& sm) {
    return &(sm.get_geom_manager());
  }
};

} // namespace snemo

#endif // GEOMETRY_HH

