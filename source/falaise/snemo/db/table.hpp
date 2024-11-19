/// \file falaise/snemo/db/table.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-08-28
 * Last modified : 2024-08-28
 *
 * Copyright (C) 2024 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_TABLE_H
#define FALAISE_SNEMO_DB_TABLE_H 1

namespace snemo {

  namespace db {

    std::string snemo_database();
 
    std::string snemo_user_readonly();

    std::string snemo_calo_calib_table(const std::string & version_);

  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_TABLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
