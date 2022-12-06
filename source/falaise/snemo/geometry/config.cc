// Ourselves:
#include <falaise/version.h>
#include <falaise/snemo/geometry/config.h>

namespace snemo {

  namespace geometry {

    const std::string & default_geometry_tag()
    {
      static const std::string _geomUrn(FALAISE_DEFAULT_GEOMETRY_TAG);
      return _geomUrn;
    }
  
  } // end of namespace geometry

} // end of namespace snemo
