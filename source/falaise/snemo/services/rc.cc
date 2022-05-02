// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/rc.h */

// Ourselves:
#include <falaise/snemo/services/rc.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::rc_svc, "snemo::rc_svc")

namespace snemo {

  struct rc_svc::pimpl_type
  {
    pimpl_type() = default;
    ~pimpl_type() = default;
    
  };
  
  int rc_svc::initialize(const datatools::properties & /* config_ */,
                         datatools::service_dict_type & /* service_dict_ */)
  {

    _pimpl_ = std::make_unique<pimpl_type>();
    
    _initialized_ = true;
    return 0;
  }
 
  bool rc_svc::is_initialized() const
  {
    return _initialized_;
  }
 
  int rc_svc::reset()
  {
    _initialized_ = false;
    
    _pimpl_.reset();
    
    return 0;
  }
 
} // namespace snemo

