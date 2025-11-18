/// \file falaise/snemo/db/table_selection.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-05-05
 * Last modified : 2025-05-05
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_TABLE_SELECTION_HPP
#define FALAISE_SNEMO_DB_TABLE_SELECTION_HPP 1

// Standard library:
#include <string>
#include <vector>
#include <variant>

namespace snemo {

  namespace db {

    typedef std::variant<std::monostate,
												 bool,
												 std::int32_t,
												 double,
												 std::string> record_field_type;

    typedef std::vector<record_field_type> record_type;

    typedef std::vector<record_type> table_selection_type;
    
          		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_TABLE_SELECTION_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
