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
    ~pimpl_type();
    rc::run_list runs;
  };

  rc_svc::pimpl_type::~pimpl_type()
  {
  }

  rc_svc::rc_svc()
  {
    
  }

  // virtual
  rc_svc::~rc_svc()
  {
  }

  bool rc_svc::is_initialized() const
  {
    return _initialized_;
  }
   
  int rc_svc::initialize(const datatools::properties & config_,
                         datatools::service_dict_type & /* service_dict_ */)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!")
    _pimpl_ = std::make_unique<pimpl_type>();

    if (config_.has_key("run_lists")) {
      std::vector<std::string> runListFilenames;
      config_.fetch("run_lists", runListFilenames);
      for (std::string filename : runListFilenames) {
        datatools::fetch_path_with_env(filename);
        datatools::multi_properties runListConfig("run", "type");
        runListConfig.read(filename);
        _pimpl_->runs.load(runListConfig);
      }
    }

    // Fallback to a mock list of runs:
    if (_pimpl_->runs.is_empty()) {
      std::string filename("@falaise:snemo/demonstrator/rc/run_lists/mock_runs-1.conf");
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties runListConfig("run", "type");
      runListConfig.read(filename);
      _pimpl_->runs.load(runListConfig);
    }
    if (datatools::logger::is_debug(get_logging_priority())) {
      DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "This run list:\n");
      _pimpl_->runs.print_tree(std::clog);
    }
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

  const rc::run_list & rc_svc::get_run_list() const
  {
    return _pimpl_->runs;
  }

  
 
} // namespace snemo

