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
#include <falaise/snemo/datamodels/geomid_utils.h>

#include "FLGeoMap.hpp"

namespace FLGeoMap {

  void Config::print(std::ostream & out_) const
  {
    out_ << "geomConfigUrn      = " << geomConfigUrn << '\n';
    out_ << "variantConfigUrn   = " << variantConfigUrn << '\n';
    out_ << "variantConfigPath  = " << variantConfigPath << '\n';
    out_ << "variantProfileUrn  = " << variantProfileUrn << '\n';
    out_ << "variantProfilePath = " << variantProfilePath << '\n';
    out_ << "servicesConfigUrn = " << servicesConfigUrn << '\n';
    out_ << "servicesConfigPath = " << servicesConfigPath << '\n';
    out_ << "actionName = " << actionName << '\n';
    out_ << "findWhat = " << findWhat << '\n';
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
    if (datatools::logger::is_debug(config.verbosity)) {
      services.tree_dump(std::cerr, "Services", "[debug] ");
    }
  
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

  int Application::run()
  {
    int exitCode = 0;
    DT_LOG_DEBUG(config.verbosity, "Running...");
    const geomtools::geometry_service & geometryService
      = services.get<geomtools::geometry_service>(snemo::service_info::geometryServiceName());

    const geomtools::manager & geometryManager = geometryService.get_geom_manager();
     
    const geomtools::mapping & geometryMapping = geometryManager.get_mapping();
    const geomtools::id_mgr & geomIdManager = geometryMapping.get_id_manager();
    const auto & categories = geomIdManager.categories_by_name();
   
    const geomtools::model_factory & geometryModelFactory = geometryManager.get_factory();
    const geomtools::logical_volume::dict_type & logicalDist = geometryModelFactory.get_logicals();
   
    const auto & geomInfos = geometryMapping.get_geom_infos();

    if (datatools::logger::is_debug(config.verbosity)) {
      DT_LOG_DEBUG(config.verbosity, "Geometry manager");
      geometryManager.tree_dump(std::cerr);
      DT_LOG_DEBUG(config.verbosity, "categories.size = " << categories.size());
      DT_LOG_DEBUG(config.verbosity, "logicalDist.size = " << logicalDist.size());
      DT_LOG_DEBUG(config.verbosity, "geomInfos.size = " << geomInfos.size());
    }

    if (config.actionName == "om_map") {
      for (auto omNum = 0u; omNum < snemo::datamodel::number_of_oms(); omNum++) {
	geomtools::geom_id omGid = snemo::datamodel::om_gid(omNum, false, false);
	std::cout << omNum << ';' << omGid << '\n';
      }
    } else if (config.actionName == "gg_map") {
      for (auto ggNum = 0u; ggNum < snemo::datamodel::number_of_ggs(); ggNum++) {
	geomtools::geom_id ggGid = snemo::datamodel::gg_gid(ggNum, false);
	std::cout << ggNum << ';' << ggGid << '\n';
      }
    } else if (config.actionName == "om_find") {
      if (config.findWhat[0] == '[') {
	std::istringstream gIdIn(config.findWhat);
	geomtools::geom_id omGid;
	gIdIn >> omGid;
	DT_THROW_IF(not omGid.is_valid(), std::logic_error,
		    "Invalid OM GID format '" << config.findWhat << "'");
	DT_THROW_IF(not geometryMapping.validate_id(omGid), std::logic_error,
		    "Invalid OM GID '" << omGid << "'");
	int omNum = snemo::datamodel::om_num(omGid);
	if (omNum < 0) {
	  exitCode = 1;
	}
	std::cout << omGid << ';' << omNum << '\n';
      } else {
	std::istringstream omNumIn(config.findWhat);
	int omNum = -1;
	omNumIn >> omNum;
	DT_THROW_IF(omNum < 0 or not snemo::datamodel::om_num_is_valid(omNum),
		    std::logic_error,
		    "Invalid OM number format '" << config.findWhat << "'");
	geomtools::geom_id omGid = snemo::datamodel::om_gid(omNum, false, false);
	std::cout << omNum << ';' << omGid << '\n';
      }
    } else if (config.actionName == "gg_find") {
      if (config.findWhat[0] == '[') {
	std::istringstream gIdIn(config.findWhat);
	geomtools::geom_id ggGid;
	gIdIn >> ggGid;
	DT_THROW_IF(not ggGid.is_valid(), std::logic_error,
		    "Invalid tracker cell GID format '" << config.findWhat << "'");
	DT_THROW_IF(not geometryMapping.validate_id(ggGid), std::logic_error,
		    "Invalid tracker cell GID '" << ggGid << "'");
	int ggNum = snemo::datamodel::gg_num(ggGid);
	if (ggNum < 0) {
	  exitCode = 1;
	}
	std::cout << ggGid << ';' << ggNum << '\n';
      } else {
	std::istringstream ggNumIn(config.findWhat);
	int ggNum = -1;
	ggNumIn >> ggNum;
	DT_THROW_IF(ggNum < 0 or not snemo::datamodel::gg_num_is_valid(ggNum),
		    std::logic_error,
		    "Invalid tracker cell number format '" << config.findWhat << "'");
	geomtools::geom_id ggGid = snemo::datamodel::gg_gid(ggNum, false);
	std::cout << ggNum << ';' << ggGid << '\n';
      }       
    } else {
      DT_THROW(std::logic_error, "unsupported action name '" << config.actionName << "'");
    }
    
    return exitCode;
  }

}

