//! \file falaise/snemo/services/db_service.h
// Falaise service for cell status in SuperNEMO tracker

#ifndef SNEMO_DB_SERVICE_H
#define SNEMO_DB_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>

// - Bayeux:
#include <bayeux/datatools/base_service.h>
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>

// - Falaise:
#include "falaise/snemo/services/service_traits.h"

namespace snemo {

  /// \brief Database service
  class db_service
    : public datatools::base_service
  {
  public:

    db_service();

    virtual ~db_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &,
                   datatools::service_dict_type &) override;
    
    int reset() override;
    
  private:
 
    // Database operations:
   
    void _connect_();

    void _disconnect_();

    bool _initialized_ = false;
    std::string _db_host_;
    std::uint32_t _db_port_ = 0;
    std::string _db_version_;
    
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
