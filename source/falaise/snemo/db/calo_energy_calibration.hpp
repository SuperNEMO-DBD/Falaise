/// \file falaise/snemo/db/calo_energy_calibration.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-08-28
 * Last modified : 2024-08-28
 *
 * Copyright (C) 2024 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_CALO_ENERGY_CALIBRATION_HPP
#define FALAISE_SNEMO_DB_CALO_ENERGY_CALIBRATION_HPP 1

// Standard library:
#include <string>
#include <limits>

namespace snemo {

  namespace db {

    namespace calo_energy_calibration {

      namespace version_1 {

	// From DocDB #5985 and https://github.com/konarfil/CalibrationTools
	struct record_type
	{
	  uint32_t om_number = -1; ///< OM unique identifier
	  double a = std::numeric_limits<double>::quiet_NaN(); ///< First calibration parameter
	  double b = std::numeric_limits<double>::quiet_NaN(); ///< Second calibration parameter
	  double chi2_A = std::numeric_limits<double>::quiet_NaN(); ///< Chi-2 per NDF for A
	  double chi2_B = std::numeric_limits<double>::quiet_NaN(); ///< Chi-2 per NDF for B
	  double loss = std::numeric_limits<double>::quiet_NaN(); ///< Lowest found loss function value

	  bool is_valid() const;
	};

	struct table
	{
	  table();

	  void load(snemo::db_service & dbserv_);
	  
	  std::vector<record_type> records;
	}

      } // end of namespace version_1

    } // end of namespace calo_energy_calibration

  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_CALO_ENERGY_CALIBRATION_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

