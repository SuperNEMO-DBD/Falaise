/// \file falaise/snemo/db/table_description.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2025-04-29
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_TABLE_DESCRIPTION_HPP
#define FALAISE_SNEMO_DB_TABLE_DESCRIPTION_HPP 1

// Standard library:
#include <string>
#include <map>
#include <set>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/db/field_description.hpp>

namespace snemo {

  namespace db {

    /// \brief Description of a table
    class table_description
      : datatools::i_tree_dumpable
    {
    public:
      
      table_description(const std::string & name_);

      ~table_description() = default;

      const std::string & name() const;

      std::set<std::string> field_names() const;
      
      bool has_field(const std::string & name_) const;

      void add_field(const std::string & name_,
										 const std::string & text_,
										 const field_description & desc_,
										 const bool primary_key_ = false,
										 const bool null_allowed_ = false);

      const field_entry & field(const std::string & name_) const;

      const field_map_type & fields() const;
	
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      std::string _name_;
      field_map_type _fields_;

    };

    /// \brief Table instance
    struct table_entry
      : datatools::i_tree_dumpable
    {
    public:
      
      table_entry() = delete;
      
      table_entry(const std::string & name_,
									const std::string & text_,
									const table_description & description_);

      const std::string & name() const;

      const std::string & text() const;

      const table_description & description() const;
	
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:
      
      std::string _name_;
      std::string _text_;
      const table_description & _description_;
      
    };
     
    typedef std::map<std::string, table_entry> table_map_type;
   		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_TABLE_DESCRIPTION_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
