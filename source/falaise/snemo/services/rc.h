//! \file falaise/snemo/services/rc.h
//! \brief Running conditions service in SuperNEMO
#ifndef SNEMO_RC_SVC_H
#define SNEMO_RC_SVC_H

// Standard library:
#include <string>
#include <memory>

// Bayeux:
#include "bayeux/datatools/base_service.h"

// Falaise:
#include "falaise/snemo/services/service_traits.h"

namespace snemo {

  class rc_record
  {
  };

  namespace rc {
    class run_list;
  }

  /// \brief Running conditions service
  class rc_svc
    : public datatools::base_service
  {
  public:

    rc_svc();

    virtual ~rc_svc();

    bool is_initialized() const override;
    
    int initialize(const datatools::properties &,
                   datatools::service_dict_type &) override;
    
    int reset() override;

    /**
     *  Run list:
     *
     *        run-42   run-43     run-44
     *    ---[------)-[-------)--[-------)------> t
     *       t1    t2 t3     t4  t5     t6
     *
     *
     */
    const rc::run_list & get_run_list() const;
    
  private:

    bool _initialized_ = false;
    struct pimpl_type;
    std::unique_ptr<pimpl_type> _pimpl_; // XXX
    
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(rc_svc)
    
  };

  template <>
  struct service_traits<rc_svc> {
    using label_type = BOOST_METAPARSE_STRING("rc_svc");
    using service_type = rc_svc;
    using instance_type = service_type;
    static instance_type* get(service_type& sm) { return &sm; }
  };
  
} // namespace snemo

#endif // SNEMO_RC_SVC_H