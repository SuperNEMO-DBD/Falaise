// - Implementation of MyModule

// Ourselves
#include "MyModule.h"

// Standard Library

// Third Party
// - Bayeux
#include <bayeux/datatools/service_manager.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This Project

// Macro which automatically implements the interface needed
// to enable the module to be loaded at runtime
// The first argument is the typename
// The second is the string key used to access the module in pipeline
// scripts. This must be globally unique.
DPP_MODULE_REGISTRATION_IMPLEMENT(MyModule,"MyModule");

// Construct
MyModule::MyModule() : dpp::base_module(), geoManager_(0)
{}

// Destruct
MyModule::~MyModule() {
  // MUST reset module at destruction
  this->reset();
}

// Initialize
void MyModule::initialize(const datatools::properties& /*myConfig*/,
                          datatools::service_manager& flServices,
                          dpp::module_handle_dict_type& /*moduleDict*/) {
  if (flServices.has("geometry")) {
    // Service Manager can supply a geometry service, so get it and
    // then the corresponding manager
    geomtools::geometry_service& geoService = flServices.get<geomtools::geometry_service>("geometry");
    geoManager_ = &geoService.get_geom_manager();

    // Some output to demonstrate we have something
    geoService.tree_dump(std::cout,"Geometry Service");
    geoManager_->tree_dump(std::cout, "Geometry Manager");
  }
  this->_set_initialized(true);
}

// Process
dpp::base_module::process_status
MyModule::process(datatools::things& /*workItem*/) {
  // Hello world!
  std::cout << "MyModule::process called!" << std::endl;
  std::cout << "Using Geometry Manager instance at : " << geoManager_ << std::endl;

  // MUST return a status, see ref dpp::base_module::process_status
  return PROCESS_OK;
}

// Reset
void MyModule::reset() {
  geoManager_ = 0;
  this->_set_initialized(false);
}
