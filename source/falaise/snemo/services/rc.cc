// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/rc.h */

// Ourselves:
#include <falaise/snemo/services/rc.h>

// Bayeux:
#include "bayeux/datatools/exception.h"

// Falaise:
#include "falaise/snemo/rc/run_list.h"

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::rc_svc, "snemo::rc_svc")

namespace snemo {

  struct rc_svc::pimpl_type
  {
    pimpl_type() = default;
    ~pimpl_type() = default;
    run_list runs;
  };

  bool rc_svc::is_initialized() const
  {
    return _initialized_;
  }
   
  int rc_svc::initialize(const datatools::properties & /* config_ */,
                         datatools::service_dict_type & /* service_dict_ */)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!")
    _pimpl_ = std::make_unique<pimpl_type>();
    
    _initialized_ = true;
    return 0;
  }
 
  int rc_svc::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Service is not initialized!")
    _initialized_ = false;
    
    _pimpl_.reset();
    
    return 0;
  }

  const run_list & rc_svc::get_run_list() const
  {
    return _pimpl_->runs;
  }
 
} // namespace snemo

