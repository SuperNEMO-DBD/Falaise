/// \file falaise/snemo/db/database_description.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2025-04-29
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_DATABASE_DESCRIPTION_HPP
#define FALAISE_SNEMO_DB_DATABASE_DESCRIPTION_HPP 1

// Standard library:
#include <string>
#include <map>
#include <set>
// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/db/table_description.hpp>

namespace snemo {

  namespace db {

    /// \brief Description of a database
    class database_description
      : datatools::i_tree_dumpable
    {
    public:
      
      database_description(const std::string & name_);

      ~database_description() = default;

      const std::string & name() const;

      std::set<std::string> table_names() const;
      
      bool has_table(const std::string & name_) const;

      void add_table(const std::string & name_,
										 const std::string & text_,
										 const table_description & desc_);

      const table_entry & table(const std::string & name_) const;

      const table_map_type & tables() const;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;
	
    private:

      std::string _name_;
      table_map_type _tables_;

    };

    /// \brief Database instance
    struct database_entry
      : datatools::i_tree_dumpable
    {
    public:
      
      database_entry() = delete;
      
      database_entry(const std::string & name_,
										 const std::string & text_,
										 const database_description & description_);

      const std::string & name() const;

      const std::string & text() const;

      const database_description & description() const;
      
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:
      
      std::string _name_;
      std::string _text_;
      const database_description & _description_;
      
    };
     
    typedef std::map<std::string, database_entry> database_map_type;
  		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_DATABASE_DESCRIPTION_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
