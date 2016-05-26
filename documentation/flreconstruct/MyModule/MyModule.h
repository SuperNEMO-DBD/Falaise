//! \file    MyModule.h
//! \brief   Example processing module for flreconstruct
//! \details Process a things object

#ifndef MYMODULE_HH
#define MYMODULE_HH
// Standard Library

// Third Party
// - Bayeux
#include "bayeux/dpp/base_module.h"

// This Project

class MyModule : public dpp::base_module {
 public:
  //! Construct module
  MyModule();

  //! Destructor
  virtual ~MyModule();

  //! Configure the module
  virtual void initialize(const datatools::properties& myConfig,
                          datatools::service_manager& flServices,
                          dpp::module_handle_dict_type& moduleDict);

  //! Process supplied data record
  virtual dpp::base_module::process_status process(datatools::things& workItem);

  //! Reset the module
  virtual void reset();

 private:
  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(MyModule)
};

#endif // MYMODULE_HH
