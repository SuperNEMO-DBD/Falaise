// snemo/digitization/mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <string>

// Ourselves:
#include <falaise/snemo/digitization/mapping.h>

namespace snemo {

  namespace digitization {

    const std::string mapping::GEIGER_TYPE    = "geiger";
    const std::string mapping::MAIN_CALO_TYPE = "mcalo";
    const std::string mapping::X_WALL_TYPE    = "xwall";
    const std::string mapping::GVETO_TYPE     = "gveto";

  } // end of namespace digitization

} // end of namespace snemo
