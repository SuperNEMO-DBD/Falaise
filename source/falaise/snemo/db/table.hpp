/// \file falaise/snemo/db/table.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2024-04-29
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_TABLE_HPP
#define FALAISE_SNEMO_DB_TABLE_HPP 1

namespace snemo {

  namespace db {

		class table
		{
		public:

			table(const std::string & name_);

			~table() = default;

			const std::string & name() const;
		
			
		private:

			std::string _name_;
			
		};
		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_TABLE_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
