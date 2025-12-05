#include <falaise/tags.h>

#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/mapping.h>

#include <falaise/snemo/services/services.h>

#include "FLScanGeom.hpp"

namespace FLScanGeom {

  void Config::print(std::ostream & out_) const
  {
    out_ << "geomConfigUrn      = " << geomConfigUrn << '\n';
    out_ << "variantConfigUrn   = " << variantConfigUrn << '\n';
    out_ << "variantConfigPath  = " << variantConfigPath << '\n';
    out_ << "variantProfileUrn  = " << variantProfileUrn << '\n';
    out_ << "variantProfilePath = " << variantProfilePath << '\n';
    out_ << "servicesConfigUrn = " << servicesConfigUrn << '\n';
    out_ << "servicesConfigPath = " << servicesConfigPath << '\n';
    return;
  }
 
  void Application::init(const Config & config_)
  {
    this->config = config_;
    datatools::kernel & bxKernel = datatools::kernel::instance();
    const datatools::urn_query_service & bxUrnQuery = bxKernel.get_urn_query();
    {
      const std::string & geomConfigCategory = falaise::tags::geometry_setup_category();
      DT_THROW_IF(!bxUrnQuery.check_urn_info(config.geomConfigUrn, geomConfigCategory),
		  std::logic_error,
		  "Cannot query URN='" << config.geomConfigUrn << "'!");
    }
    datatools::urn_info geomUrnInfo =
      bxUrnQuery.get_urn_info(config.geomConfigUrn);
    if (datatools::logger::is_debug(config.verbosity)) {
      geomUrnInfo.tree_dump(std::cerr, "geomUrnInfo", "[debug] ");
    }

    if (geomUrnInfo.has_topic("alias_of")) {
      std::string aliasOf = geomUrnInfo.get_component("alias_of");
      DT_LOG_DEBUG(config.verbosity, "found aliasOf = " << aliasOf);
      config.geomConfigUrn = aliasOf;
      geomUrnInfo = bxUrnQuery.get_urn_info(config.geomConfigUrn);
      if (datatools::logger::is_debug(config.verbosity)) {
	geomUrnInfo.tree_dump(std::cerr, "geomUrnInfo", "[debug] ");
      }
   }

    if (geomUrnInfo.has_topic("variants")) {
      DT_LOG_DEBUG(config.verbosity, "has variants");
      const std::vector<std::string> & vv = geomUrnInfo.get_components_by_topic("variants");
      std::string variantConfigUrn;
      if (vv.size() == 1) {
	variantConfigUrn = geomUrnInfo.get_component("variants");
	DT_LOG_DEBUG(config.verbosity, "variantConfigUrn = " << variantConfigUrn);
      }
      config.variantConfigUrn = variantConfigUrn;
      if (not config.variantConfigUrn.empty()) {
        // Resolve variants file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.variantConfigUrn,
						     conf_variants_category,
						     conf_variants_mime,
						     conf_variants_path),
                    std::logic_error,
                    "Cannot resolve URN='" << config.variantConfigUrn << "'!");
        config.variantConfigPath = conf_variants_path;
        DT_LOG_DEBUG(config.verbosity,
		     "Resolved variant service's configuration file is '" << conf_variants_path << "'");
      }
    }

    // Variants profile:
    if (not config.variantProfilePath.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(not config.variantProfileUrn.empty(),
                  std::logic_error,
                  "Required variants profile URN='"
                  << config.variantProfileUrn << "' "
                  << "conflicts with required variants profile path='"
                  << config.variantProfilePath << "'!");
    } else if (not config.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.variantProfileUrn,
						   conf_variantsProfile_category,
						   conf_variantsProfile_mime,
						   conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << config.variantProfileUrn << "'!");
      config.variantProfilePath = conf_variantsProfile_path;
    }

    DT_LOG_DEBUG(config.verbosity, "variantProfilePath = " << config.variantProfilePath);

    // Start the variant service:
    datatools::configuration::variant_service & theVariantService = this->variantService;
    datatools::configuration::variant_service::config variantParams;
    variantParams.config_filename = config.variantConfigPath;
    variantParams.profile_load = config.variantProfilePath;
    theVariantService.configure(variantParams);
    theVariantService.start();
    {
      const std::string & servicesConfigCategory = falaise::tags::services_category();
      DT_THROW_IF(!bxUrnQuery.check_urn_info(config.servicesConfigUrn, servicesConfigCategory),
		  std::logic_error,
		  "Cannot query URN='" << config.servicesConfigUrn << "'!");
    }
    datatools::urn_info servicesUrnInfo =
      bxUrnQuery.get_urn_info(config.servicesConfigUrn);
    if (datatools::logger::is_debug(config.verbosity)) {
      servicesUrnInfo.tree_dump(std::cerr, "servicesUrnInfo", "[debug] ");
    }
    // Determine the variant profile path from a blessed variant profile URN:
    std::string conf_services_category = "configuration";
    std::string conf_services_mime;
    std::string conf_services_path;
    DT_THROW_IF(! bxUrnQuery.resolve_urn_to_path(config.servicesConfigUrn,
						 conf_services_category,
						 conf_services_mime,
						 conf_services_path),
		std::logic_error,
		"Cannot resolve profile URN='" << config.servicesConfigUrn << "'!");
    config.servicesConfigPath = conf_services_path;

    
    services.set_logging_priority(config.verbosity);
    datatools::multi_properties servicesConfig;
    datatools::fetch_path_with_env(config.servicesConfigPath);
    servicesConfig.read(config.servicesConfigPath);
    services.load(servicesConfig);
    services.initialize();
    services.tree_dump(std::cerr, "Services", "[debug] ");
  
    return;
  }

  void Application::reset()
  {
    if (services.is_initialized()) {
      services.reset();
    }

    if (variantService.is_started()) {
      // Terminate the variant service:
      variantService.stop();
    }
    
    return;
  }

  void Application::run()
  {
    DT_LOG_DEBUG(config.verbosity, "Running...");
    const geomtools::geometry_service & geometryService
      = services.get<geomtools::geometry_service>(snemo::service_info::geometryServiceName());

    const geomtools::manager & geometryManager = geometryService.get_geom_manager();
    geometryManager.tree_dump(std::cerr, "Geometry manager", "[debug] ");
    
    const geomtools::mapping & geometryMapping = geometryManager.get_mapping();
    const geomtools::id_mgr & geomIdManager = geometryMapping.get_id_manager();
    const auto & categories = geomIdManager.categories_by_name();
    std::cerr << "categories.size = " << categories.size() << std::endl;
   
    const geomtools::model_factory & geometryModelFactory = geometryManager.get_factory();
    const geomtools::logical_volume::dict_type & logicalDist = geometryModelFactory.get_logicals();
    std::cerr << "logicalDist.size = " << logicalDist.size() << std::endl;
   
    const auto & geomInfos = geometryMapping.get_geom_infos();
    std::cerr << "geomInfos.size = " << geomInfos.size() << std::endl;

    // coil : "coil_wire" "coil_wall_support" 
    // ART : "art_wall"
    //  "art_panel_ce_panel_full_build.model" 

    
    return;
  }

}

