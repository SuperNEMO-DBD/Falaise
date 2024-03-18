//! \file falaise/snemo/services/geometry.h
//! \brief Types and traits for the SuperNEMO Geometry Service
#ifndef SNEMO_GEOMETRY_SVC_HH
#define SNEMO_GEOMETRY_SVC_HH

#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"

#include "falaise/snemo/services/service_traits.h"

namespace snemo {
  
// NB: Document in terms of inheritance purely to fool Doxygen...
/*! \class snemo::geometry_svc
 *  \brief Locate, track, and trace elements of the SuperNEMO detector geometry
 *  \implements geomtools::manager
 *
 * It provides an implementation of the `const` parts of the @ref geomtools::manager interface,
 * and you should consult documentation for that class for full details.
 *
 * The class should only be created through obtaining a @ref snemo::service_handle
 * using a @ref datatools::service_manager, e.g.
 *
 * ```cpp
 * void example(datatools::service_manager& s) {
 *   snemo::service_handle<snemo::geometry_svc> geosvc{s};
 *   ...
 * }
 * ```
 *
 * Direct construction is not supported and cannot be guaranteed to work.
 *
 * \sa geomtools::manager
 */
using geometry_svc = const geomtools::manager;

//! Specialization of service_traits for geometry_svc
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

