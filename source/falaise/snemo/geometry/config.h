// falaise/snemo/geometry/config.h
/* Author(s) :    F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-10-26
 * Last modified: 2022-10-26
 *
 * License: GPL3
 *
 * Description:
 *
 *   Geometry configuration utilities for SuperNEMO.
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_CONFIG_H
#define FALAISE_SNEMO_GEOMETRY_CONFIG_H 1

// Standard library:
#include <string>

namespace snemo {

  namespace geometry {

    const std::string & default_geometry_tag();

    const std::string & default_locators_driver_name();
    
  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_CONFIG_H
