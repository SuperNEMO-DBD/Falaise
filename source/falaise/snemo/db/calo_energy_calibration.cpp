
// Ourselves:
#include <falaise/snemo/db/calo_energy_calibration.hpp>

// This project:
#include <falaise/snemo/datamodels/geomid_utils.h>

namespace snemo {

  namespace db {

    namespace calo_energy_calibration {

      namespace version_1 {

	bool record_type::is_valid() const
	{
	  return this->om_number >= 0
	    and this->om_number < snemo::datamodel::om_map().size()
	    and not std::isnan(a)
	    and not std::isnan(b);
	}
	
	table::table()
	{
	  for (int i = 0; i < snemo::datamodel::om_map().size(); i++) {
	    record_type rec;
	    rec.om_number = i;
	    records.push_back(rec);
	  }
	  return;
	}

      } // end of namespace version_1

    } // end of namespace calo_energy_calibration

  } // end of namespace db

} // end of namespace snemo
