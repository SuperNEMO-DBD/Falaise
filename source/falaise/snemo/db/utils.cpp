
// Ourselves:
#include <falaise/snemo/db/utils.hpp>

// Standard library:
#include <iomanip>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace db {

    std::string snemo_database_name()
    {
      return std::string("snemodb");
    }
 
    std::string snemo_readonly_user_name()
    {
      return std::string("snemoro");
    }

    std::string snemo_calo_energy_calibration_table_name(const std::string & version_)
    {
      std::string version = version_;
      if (version == "1.0") {
	return std::string("calo_calib");
      } else {
	DT_THROW(std::logic_error, "Unsupported version " << std::quoted(version_));
      }
    }

  } // end of namespace db

} // end of namespace snemo
