#ifndef ACCESSTHINGSMODULE_HH
#define ACCESSTHINGSMODULE_HH
#include "bayeux/dpp/base_module.h"

class AccessThingsModule : public dpp::base_module {
 public:
  AccessThingsModule();
  virtual ~AccessThingsModule();

  virtual void initialize(const datatools::properties& myConfig,
                          datatools::service_manager& flServices,
                          dpp::module_handle_dict_type& what);

  virtual int process(datatools::things& workItem);

  virtual void reset();

 private:
  DPP_MODULE_REGISTRATION_INTERFACE(AccessThingsModule);
};

#endif // ACCESSTHINGSMODULE_HH

