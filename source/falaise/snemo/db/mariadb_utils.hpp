/// \file falaise/snemo/db/mariadb_utils.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-30
 * Last modified : 2024-04-30
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_MARIADB_UTILS_HPP
#define FALAISE_SNEMO_DB_MARIADB_UTILS_HPP 1

// Standard library:
#include <string>
#include <vector>

// MariaDB C++:
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
#include <mariadb/conncpp.hpp>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <falaise/snemo/db/table_selection.hpp>
#include <falaise/snemo/db/database_description.hpp>

namespace snemo {

  namespace db {

    class mariadb_driver
    {
    public:
      
      mariadb_driver(sql::Connection & conn_,
										 const database_description & dbdesc_,
										 const datatools::logger::priority verbosity_ = datatools::logger::PRIO_FATAL);
      
      void process_select_statement(const std::string & tablename_,
																		const std::vector<std::string> & fieldnames_,
																		table_selection_type & selection_);
    private:

      sql::Connection & _conn_;
      const database_description & _dbdesc_;
			datatools::logger::priority _verbosity_ = datatools::logger::PRIO_FATAL;
			
    };
          		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_MARIADB_UTILS_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
