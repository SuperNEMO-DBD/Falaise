#ifndef FLGEOMAP_HPP
#define FLGEOMAP_HPP

#include <string>
#include <iostream>

#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/service_manager.h"

namespace FLGeoMap {

  struct Config
  {
    datatools::logger::priority verbosity = datatools::logger::PRIO_ERROR;
    
    /// Geometry configuration:
    std::string geomConfigUrn{"urn:snemo:demonstrator:geometry:default"};

    /// Variants configuration:   
    std::string variantConfigUrn{"urn:snemo:demonstrator:geometry:variants:service:default"};  
    std::string variantConfigPath;  
    std::string variantProfileUrn;  
    std::string variantProfilePath;  
    std::string servicesConfigUrn{"urn:snemo:demonstrator:geometry:6.0:services"};  
    std::string servicesConfigPath;

    std::string actionName;
    std::string findWhat;
    
    void print(std::ostream & out_) const;
    
  };

  struct Application
  {
    Config config;
    datatools::configuration::variant_service variantService;
    datatools::service_manager services;
    
    void init(const Config & config);

    void reset();

    int run();
    
  };
}

#endif // FLGEOMAP_HPP
