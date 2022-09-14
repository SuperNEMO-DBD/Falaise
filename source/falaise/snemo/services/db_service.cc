// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/db_service.cc */

// Ourselves:
#include <falaise/snemo/services/db_service.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// This project:
#include <falaise/snemo/services/services.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::db_service, "snemo::db_service")

namespace snemo {

  db_service::db_service()
    : datatools::base_service("db",
                              "Database service",
                              "Service accessing the SuperNEMO database")
  {
    return;
  }

  db_service::~db_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool db_service::is_initialized() const
  {
    return _initialized_;
  }

  int db_service::initialize(const datatools::properties & config_,
                             datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");

    if (config_.has_key("host")) {
      _db_host_ = config_.fetch_string("host");
     }
    if (config_.has_key("port")) {
      _db_port_ = config_.fetch_positive_integer("port");
     }
    if (config_.has_key("version")) {
      _db_version_ = config_.fetch_string("version");
     }
    DT_THROW_IF(_db_host_.empty(), std::logic_error, "Missing DB hostname!");
    DT_THROW_IF(_db_port_ == 0, std::logic_error, "Missing DB port!");
    _connect_();
    _initialized_ = true;
    return 0;
  }

  int db_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    _initialized_ = false;
    _disconnect_();
    return 0;
  }
   
  void db_service::_connect_()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Connecting the SuperNEMO database...");
    return;
  }

  void db_service::_disconnect_()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Disconnecting the SuperNEMO database...");
    return;
  }

}  // namespace snemo
