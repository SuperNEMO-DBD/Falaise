//! \file falaise/snemo/services/db_service.h
// Falaise service for cell status in SuperNEMO tracker

#ifndef SNEMO_DB_SERVICE_H
#define SNEMO_DB_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>
#include <cstdint>
#include <memory>

// - Bayeux:
#include <bayeux/datatools/base_service.h>
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>

// - Falaise:
#include "falaise/snemo/services/service_traits.h"
#include "falaise/snemo/db/table_selection.hpp"

namespace snemo {

  /// \brief Database service
  class db_service
    : public datatools::base_service
  {
  public:

    struct config_type
    {
      std::string   db_host;
      std::uint32_t db_port = 0;
      std::string   db_version;
      std::string   db_database;
      std::string   db_user;
      std::string   db_password;
    };

    db_service();

    virtual ~db_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &,
                   datatools::service_dict_type &) override;
    
    int reset() override;

    const config_type & config() const;

    bool has_table(const std::string & tablename_) const;

    void process_select_statement(const std::string & tablename_,
				  const std::vector<std::string> & fieldnames_,
				  snemo::db::table_selection_type & selection_) const;

    void process_select_all_statement(const std::string & tablename_,
				      snemo::db::table_selection_type & selection_) const;
     
    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;
         
  private:
 
    // Database operations:
   
    void _parse_config_(const std::string & path_);
   
    void _connect_();

    void _disconnect_();

    bool _initialized_ = false;
    config_type _config_; ///< Configuration

    struct pimpl_type;
    std::unique_ptr<pimpl_type> _pimpl_; ///< Pimpl

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(db_service)
      
  };

  //! Falaise service template  
  template <>
  struct service_traits<db_service>
  {
    using label_type = BOOST_METAPARSE_STRING("db");
    using service_type = db_service;
    using instance_type = service_type;
    static instance_type * get(service_type & sm) { return &sm; }
  };

} // namespace snemo

#endif  // SNEMO_DB_SERVICE_H
