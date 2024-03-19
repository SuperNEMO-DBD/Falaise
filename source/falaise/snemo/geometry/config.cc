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
 
    const std::string & default_locators_driver_name()
    {
      static const std::string _name("locators_driver");
      return _name;
    }
  
  } // end of namespace geometry

} // end of namespace snemo
