// falaise/snemo/geometry/manager.h
/* Author(s) :    F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-06-16
 * Last modified: 2022-06-16
 *
 * License: GPL3
 *
 * Description:
 *
 *   A geometry manager wrapper for SuperNEMO.
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_MANAGER_H
#define FALAISE_SNEMO_GEOMETRY_MANAGER_H 1

// Standard library:
#include <memory>

// - Bayeux/geomtools:
#include <geomtools/manager.h>

namespace snemo {

  namespace geometry {

    void initialize_manager(geomtools::manager & gmgr_);

    std::unique_ptr<geomtools::manager> create_manager();
    
  }  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_MANAGER_H
