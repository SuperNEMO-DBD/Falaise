// Ourselves:
#include <falaise/version.h>
#include <falaise/snemo/processing/config.h>

namespace snemo {

  namespace processing {

    const std::string & default_reconstruction_tag()
    {
      static const std::string _recoUrn(FALAISE_DEFAULT_RECONSTRUCTION_TAG);
      return _recoUrn;
    }
  
  } // end of namespace processing

} // end of namespace snemo
