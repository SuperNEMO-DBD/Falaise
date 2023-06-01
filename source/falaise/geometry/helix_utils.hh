#ifndef FALAISE_GEOMETRY_HELIX_UTILS_HPP
#define FALAISE_GEOMETRY_HELIX_UTILS_HPP

// Standard library:
#include <iostream>
#include <tuple>

// This project:
#include <bayeux/geomtools/helix_3d.h>

namespace falaise {
  
  namespace geometry {

    bool matching_helices(const geomtools::helix_3d & h1_,
                          const geomtools::helix_3d & h2_,
                          const double xy_distance_tolerance_,
                          const double z_distance_tolerance_);
                
  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_GEOMETRY_HELIX_UTILS_HPP
