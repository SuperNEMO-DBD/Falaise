/// \file falaise/snemo/db/sndb.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2025-11-18
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_SNDB_HPP
#define FALAISE_SNEMO_DB_SNDB_HPP 1

// Standard library:
#include <string>
#include <map>
#include <memory>

// bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/db/database_description.hpp>
#include <falaise/snemo/db/factory.hpp>

namespace snemo {

  namespace db {

    /// \brief SuperNEMO Database description 
    class sndb
      : datatools::i_tree_dumpable
    {
    public:

      sndb() = default;
    
      bool is_initialized() const;

      /// Supported configuration parameters:
      ///
      /// - the list of database description files:
      ///  \code
      ///  database_descriptions.files : string[2] as path = "path/desc1.conf" "path/desc2.conf"
      ///  \endcode
      /// - the list of databases that can be accessed by the DB proxy:
      ///  \code
      ///  databases : string[2] = "nemo_rundb" "nemo_calibdb"
      ///  \endcode
      /// - the details about the listed databases:
      ///  \code
      ///  # Description of the database:
      ///  databases.nemo_rundb.text    : string = "The SuperNEMO database for information about runs"
      ///  # Model of the database:
      ///  databases.nemo_rundb.model   : string = "RunDbDatabaseDesc"
      ///
      ///  # Description of the database:
      ///  databases.nemo_calibdb.text  : string = "The SuperNEMO database for information about calibrations"
      ///  # Model of the database:
      ///  databases.nemo_calibdb.model : string = "CalibDbDatabaseDesc"
      ///  \endcode
      ///
      /// The description files themselves contain:
      ///
      /// - the database descriptive models of various types (list of tables),
      /// - the table descriptive models of various types (description of table columns...),
      /// - the field descriptive models of various types (type, length...)
      /// 
      void initialize(const datatools::properties & config_);

      void reset();
  
      bool has_database(const std::string & name_) const;

      void add_database(const std::string & name_,
                        const std::string & text_,
                        const database_description & desc_);

      const database_entry & database(const std::string & name_) const;

      const database_map_type & databases() const;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      bool _initialized_ = false; ///< Initialization flag
      factory _factory_; ///< Embedded DB descriptions factory
      database_map_type _databases_; ///< Database instances
           
    };

  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_SNDB_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
