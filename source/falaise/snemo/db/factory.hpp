/// \file falaise/snemo/db/factory.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2024-04-29
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_FACTORY_HPP
#define FALAISE_SNEMO_DB_FACTORY_HPP 1

// Standard library:
#include <string>
#include <memory>

// This project:
#include <falaise/snemo/db/field_description.hpp>
#include <falaise/snemo/db/table_description.hpp>
#include <falaise/snemo/db/database_description.hpp>

// - Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/datatools/i_tree_dump.h>

namespace snemo {

  namespace db {
   
    /// \brief Factory for description objects
    class factory
      : datatools::i_tree_dumpable
    {
    public:
      
      factory() = default;

      std::unique_ptr<field_description>
      make_field_description(const std::string & name_,
			     const datatools::properties & config_);

      std::unique_ptr<table_description>
      make_table_description(const std::string & name_,
			     const datatools::properties & config_);

      std::unique_ptr<database_description>
      make_database_description(const std::string & name_,
				const datatools::properties & config_);

      std::set<std::string> field_description_keys() const;
 
      std::set<std::string> table_description_keys() const;
 
      std::set<std::string> database_description_keys() const;

			bool has_field_description(const std::string & name_) const;

			bool has_table_description(const std::string & name_) const;

			bool has_database_description(const std::string & name_) const;
			
      const field_description &
      get_field_description(const std::string & name_) const;
 
      const table_description &
      get_table_description(const std::string & name_) const;
 
      const database_description &
      get_database_description(const std::string & name_) const;

      void build(const datatools::multi_properties & mconfig_);

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;

      
    private:

      std::map<std::string,std::shared_ptr<const field_description>> _field_descriptions_;
      std::map<std::string,std::shared_ptr<const table_description>> _table_descriptions_;
      std::map<std::string,std::shared_ptr<const database_description>> _database_descriptions_;
      
    };
      		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_FACTORY_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
