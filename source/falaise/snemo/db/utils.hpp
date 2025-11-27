/// \file falaise/snemo/db/utils.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-08-28
 * Last modified : 2024-08-28
 *
 * Copyright (C) 2024 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_UTILS_HPP
#define FALAISE_SNEMO_DB_UTILS_HPP 1

// Standard library:
#include <string>

namespace snemo {

  namespace db {

    std::string snemo_database_name();
 
    std::string snemo_readonly_user_name();

    std::string snemo_calo_energy_calibration_table_name(const std::string & version_ = "");

  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_UTILS_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
