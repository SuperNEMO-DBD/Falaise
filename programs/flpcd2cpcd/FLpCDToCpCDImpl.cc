// Ourselves
#include "FLpCDToCpCDImpl.h"

// Standard Library
#include <memory>
#include <string>
#include <vector>

// Third Party
// - Bayeux
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/library_info.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>
#include "bayeux/bayeux.h"
#include "bayeux/datatools/factory_macros.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/input_module.h"
#include "bayeux/version.h"

// This Project
#include "FLpCDToCpCDCommandLine.h"
#include "FLpCDToCpCDParams.h"
#include "FLpCDToCpCDApplication.h"
#include "falaise/exitcodes.h"
#include "falaise/metadata_utils.h"
#include "falaise/property_set.h"
#include "falaise/resource.h"
#include "falaise/tags.h"
#include "falaise/version.h"
#include "falaise/snemo/processing/config.h"

namespace FLpCDToCpCD {

  void do_mount_points(const FLpCDToCpCDParams & flRecParams_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    const FLpCDToCpCDParams & flRecParameters = flRecParams_;
    // Apply mount points as soon as possible, because manually set file path below
    // may use this mechanism to locate files:
    datatools::kernel & dtk = datatools::kernel::instance();
    datatools::library_info & dtklLibInfo = dtk.grab_library_info_register();
    for (const std::string & mountDirective : flRecParameters.mountPoints) {
      DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing mount directive: '" << mountDirective << "'...");
      std::string theLibname;
      std::string theTopic;
      std::string thePath;
      std::string errMsg;
      bool parsed = datatools::library_info::parse_path_registration_directive(mountDirective,
                                                                               theLibname,
                                                                               theTopic,
                                                                               thePath,
                                                                               errMsg);
      DT_THROW_IF(!parsed, FLConfigUserError,
                  "Cannot parse directory mount directive '" << mountDirective << "' : " << errMsg);
      if (theTopic.empty()) {
        theTopic = datatools::library_info::default_topic_label();
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "Using default path registration topic: " << theTopic);
      }
      DT_LOG_DEBUG(flRecParameters.logLevel, "Path registration: " << mountDirective);
      DT_LOG_DEBUG(flRecParameters.logLevel, "  Library name : " << theLibname);
      DT_LOG_DEBUG(flRecParameters.logLevel, "  Topic        : " << theTopic);
      DT_LOG_DEBUG(flRecParameters.logLevel, "  Path         : " << thePath);
      try {
        dtklLibInfo.path_registration(theLibname, theTopic, thePath, false);
      } catch (std::exception& error) {
        DT_THROW(FLConfigUserError, "Cannot apply directory mount directive '"
                 << mountDirective << "': " << error.what());
      }
    }
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;
  } // do_mount_points

  ///! Parse command line arguments to configure the pcdtocpcd parameters
  void do_configure(int argc_, char * argv_[], FLpCDToCpCDApplication & flProcessApp_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    FLpCDToCpCDParams & flProcessParameters = flProcessApp_.parameters;
    // - Default Config
    flProcessParameters = FLpCDToCpCDParams::makeDefault();

    // - CL Dialog Config
    FLpCDToCpCDCommandLine clArgs;
    FLDialogState clDialogRet = DIALOG_OK;
    try {
      clDialogRet = do_cldialog(argc_, argv_, clArgs);

      if (clDialogRet == DIALOG_ERROR) {
        throw FLConfigUserError{"bad command line input"};
      }
      if (clDialogRet == DIALOG_QUERY) {
        throw FLConfigHelpHandled();
      }
    } catch (FLConfigHelpHandled & e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogHelpRequested & e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogOptionsError & e) {
      throw FLConfigUserError{"bad command line input"};
    } catch (std::exception & e) {
      throw FLConfigUserError{"bad command line input"};
    }
    DT_LOG_DEBUG(clArgs.logLevel, "FLpCDToCpCD command line parameters:");
    if (datatools::logger::is_debug(clArgs.logLevel)) {
      clArgs.print(std::cerr);
    }

    // Import parameters from the command line:
    flProcessParameters.logLevel = clArgs.logLevel;
    flProcessParameters.processConfig = clArgs.configScript;
    flProcessParameters.numberOfEvents = clArgs.maxNumberOfEvents;
    flProcessParameters.moduloEvents = clArgs.moduloEvents;
    flProcessParameters.userProfile = clArgs.userProfile;
    flProcessParameters.inputMetadataFile = clArgs.inputMetadataFile;
    flProcessParameters.inputFile = clArgs.inputFile;
    flProcessParameters.outputMetadataFile = clArgs.outputMetadataFile;
    flProcessParameters.outputFile = clArgs.outputFile;
    flProcessParameters.mountPoints = clArgs.mountPoints;
    DT_LOG_DEBUG(flProcessParameters.logLevel, "FLpCDToCpCD parameters after command line parsing:");
    if (datatools::logger::is_debug(flProcessParameters.logLevel)) {
      flProcessParameters.print(std::cerr);
    }

    if (not flProcessParameters.mountPoints.empty()) {
      do_mount_points(flProcessParameters);
    }

    if (flProcessParameters.userProfile.empty()) {
      // Force a default user profile:
      DT_LOG_DEBUG(flProcessParameters.logLevel, "Forcing user profile to default value 'normal'...");
      flProcessParameters.userProfile = "normal";
    }

    // First parsing pass of the FLpCDToCpCD configuration script:
    {
      DT_LOG_DEBUG(flProcessParameters.logLevel,
                   "First parsing pass of the FLpCDToCpCD configuration script...");
      datatools::multi_properties flProcessConfig1("name", "type");
      if (! flProcessParameters.processConfig.empty()) {
        std::string flProcessConfigScript = flProcessParameters.processConfig;
        datatools::fetch_path_with_env(flProcessConfigScript);
        flProcessConfig1.read(flProcessConfigScript);
      }
      if (datatools::logger::is_debug(flProcessParameters.logLevel)) {
        flProcessConfig1.tree_dump(std::cerr, "flProcessConfig1: ", "[debug] ");
      }
      // Fetch basic configuration from the script:
      if (flProcessConfig1.has_key_with_meta("flpcdtocpcd", "flreconstruct::section")) {
        DT_LOG_DEBUG(flProcessParameters.logLevel, "Parsing FLpCDToCpCD main configuration directives...");
        // pCDToCpCD basic subsystem:
        falaise::property_set basicSystem{flProcessConfig1.get_section("flpcdtocpcd")};
        flProcessConfig1.remove("flpcdtocpcd");
        
        // Fetch the pcdtocpcd process setup URN:
        flProcessParameters.processSetupUrn =
          basicSystem.get<std::string>("processSetupURN", flProcessParameters.processSetupUrn);
        if (flProcessParameters.processSetupUrn.empty()) {
          // Use the default one:
          flProcessParameters.processSetupUrn = // snemo::processing::
	    FLpCDToCpCD::FLpCDToCpCDApplication::default_pcd2cpcd_tag();
	}
        
	// Fetch the experimental setup URN:
	flProcessParameters.experimentalSetupUrn =
	  basicSystem.get<std::string>("experimentalSetupURN", flProcessParameters.experimentalSetupUrn);

	{
	  datatools::kernel& dtk = datatools::kernel::instance();
	  const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();
	  // Fetch the pcdtocpcd setup identifier/URN:
	  std::string confCategory = "recsetup";
	  DT_THROW_IF(!dtkUrnQuery.check_urn_info(flProcessParameters.processSetupUrn, confCategory),
		      std::logic_error,
		      "Cannot query URN='" << flProcessParameters.processSetupUrn << "'!");
	  // and extract the associated 'experimentalSetupUrn':
	  const datatools::urn_info & recSetupUrnInfo
	    = dtkUrnQuery.get_urn_info(flProcessParameters.processSetupUrn);
	  if (recSetupUrnInfo.has_topic("setup") &&
	      recSetupUrnInfo.get_components_by_topic("setup").size() == 1) {
	    std::string experimentalSetupUrn = recSetupUrnInfo.get_component("setup");
	    if (flProcessParameters.experimentalSetupUrn.empty()) {
	      flProcessParameters.experimentalSetupUrn = experimentalSetupUrn;
	    } else {
	      if (flProcessParameters.experimentalSetupUrn != experimentalSetupUrn) {
		DT_THROW(std::logic_error,
			 "Experimental setup URN='"
			 << flProcessParameters.experimentalSetupUrn << "' does not match "
			 << "the automatically resolved URN='" << experimentalSetupUrn
			 << "' from the pcdtocpcd setup '" << flProcessParameters.processSetupUrn
			 << "'!");
	      } 
	    }
	  }
	}

	// Number of events to be processed:
	flProcessParameters.numberOfEvents =
	  basicSystem.get<int>("numberOfEvents", flProcessParameters.numberOfEvents);

	// Printing rate for events:
	flProcessParameters.moduloEvents =
	  basicSystem.get<int>("moduloEvents", flProcessParameters.moduloEvents);

	// Printing rate for events:
	flProcessParameters.userProfile =
	  basicSystem.get<std::string>("userprofile", flProcessParameters.userProfile);
      }

      // Process input metadata:
      do_postprocess_input_metadata(flProcessParameters);
 
      // Fetch variant service configuration:
      if (flProcessConfig1.has_key_with_meta("flpcdtocpcd.variantService", "flreconstruct::section")) {
	DT_LOG_DEBUG(flProcessParameters.logLevel, "Parsing FLpCDToCpCD variant service's configuration directives...");
	falaise::property_set variantSubsystem{flProcessConfig1.get_section("flpcdtocpcd.variantService")};
	DT_LOG_DEBUG(flProcessParameters.logLevel, "Variant config: \n" << variantSubsystem.to_string());
      
	// Variant configuration URN:
	flProcessParameters.variantConfigUrn =
	  variantSubsystem.get<std::string>("configUrn", flProcessParameters.variantConfigUrn);
	DT_LOG_DEBUG(flProcessParameters.logLevel,
		     "variantConfigUrn: '" << flProcessParameters.variantConfigUrn << "'");

	// Variant configuration:
	if (flProcessParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
	  DT_THROW(FLConfigUserError, "User profile '" << flProcessParameters.userProfile << "' "
		   << "does not allow to use the '"
		   << "config"
		   << "' variants configuration parameter!");
	}
	flProcessParameters.variantSubsystemParams.config_filename =
	  variantSubsystem.get<std::string>("config", flProcessParameters.variantSubsystemParams.config_filename);

	DT_THROW_IF(! flProcessParameters.variantConfigUrn.empty() and ! flProcessParameters.variantSubsystemParams.config_filename.empty(),
		    FLConfigUserError,
		    "Explicit variant service configuration URN and variant service configuration file conflict!"); 

	// Variant profile URN:
	flProcessParameters.variantProfileUrn =
	  variantSubsystem.get<std::string>("profileUrn", flProcessParameters.variantProfileUrn);
	DT_LOG_DEBUG(flProcessParameters.logLevel, " -> variantProfileUrn=" << flProcessParameters.variantProfileUrn);
      
	// Variant profile:
	flProcessParameters.variantSubsystemParams.profile_load =
	  variantSubsystem.get<falaise::path>("profile", flProcessParameters.variantSubsystemParams.profile_load);
	DT_LOG_DEBUG(flProcessParameters.logLevel, " -> variant.profile_load='"
		     << flProcessParameters.variantSubsystemParams.profile_load << "'");

	// Variant settings:
	if (flProcessParameters.userProfile != "expert" && variantSubsystem.has_key("settings")) {
	  DT_THROW(FLConfigUserError, "User profile '" << flProcessParameters.userProfile << "' "
		   << "does not allow to use the '"
		   << "settings"
		   << "' variants configuration parameter!");
	}
	flProcessParameters.variantSubsystemParams.settings =
	  variantSubsystem.get<std::vector<std::string>>("settings",
							 flProcessParameters.variantSubsystemParams.settings);
      }
    } // First parsing pass

    // Variants support set up first because all other services will rely on it.
    DT_LOG_DEBUG(flProcessParameters.logLevel, "Configure variant subsystem...");
    do_configure_variant(flProcessApp_);

    {
      DT_LOG_DEBUG(flProcessParameters.logLevel, "Second parsing pass of the FLpCDToCpCD configuration script...");

      // Second parsing pass of the FLpCDToCpCD configuration script:
      datatools::multi_properties flProcessConfig2("name", "type");
      if (not flProcessParameters.processConfig.empty()) {
	std::string flProcessConfigScript = flProcessParameters.processConfig;
	datatools::fetch_path_with_env(flProcessConfigScript);
	flProcessConfig2.read(flProcessConfigScript);
      }
  
      // Fetch plugins configuration:
      if (flProcessConfig2.has_key_with_meta("flpcdtocpcd.plugins", "flreconstruct::section")) {
	DT_LOG_DEBUG(flProcessParameters.logLevel, "Parsing FLpCDToCpCD plugins' configuration directives...");
	try {
	  falaise::property_set userFLPlugins{flProcessConfig2.get_section("flpcdtocpcd.plugins")};
	  flProcessConfig2.remove("flpcdtocpcd.plugins");

	  auto pList = userFLPlugins.get<std::vector<std::string>>("plugins", {});

	  for (const std::string & plugin_name : pList) {
	    auto pSection = userFLPlugins.get<falaise::property_set>(plugin_name, {});
	    pSection.put("autoload", true);
	    if (!pSection.has_key("directory")) {
	      pSection.put("directory", std::string{"@falaise.plugins:"});
	    }

	    flProcessParameters.userLibConfig.add(plugin_name, "", pSection);
	  }
	} catch (std::logic_error & e) {
	  DT_LOG_ERROR(flProcessParameters.logLevel, e.what());
	  // do nothing for now because we can't distinguish errors, and
	  // upcoming instantiation of library loader will handle
	  // any syntax errors in the properties
	}
      }

      // Fetch services configuration:
      if (flProcessConfig2.has_key_with_meta("flpcdtocpcd.services", "flreconstruct::section")) {
	DT_LOG_DEBUG(flProcessParameters.logLevel, "Parsing FLpCDToCpCD services management directives...");
	falaise::property_set servicesSubsystem{flProcessConfig2.get_section("flpcdtocpcd.services")};
	flProcessConfig2.remove("flpcdtocpcd.services");
        
	// Services manager configuration URN:
	flProcessParameters.servicesSubsystemConfigUrn =
	  servicesSubsystem.get<std::string>("configUrn", flProcessParameters.servicesSubsystemConfigUrn);
	DT_LOG_DEBUG(flProcessParameters.logLevel,
		     "FLpCDToCpCD services management config URN = '" << flProcessParameters.servicesSubsystemConfigUrn << "'");
        
	// Services manager main configuration file:
	if (flProcessParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
	  // User profile 'production' must used official registered services configuration:
	  DT_THROW(FLConfigUserError, "User profile '" << flProcessParameters.userProfile << "' "
		   << "does not allow to use the '"
		   << "config"
		   << "' services configuration parameter!");
	}
	flProcessParameters.servicesSubsystemConfig =
	  servicesSubsystem.get<falaise::path>("config", flProcessParameters.servicesSubsystemConfig);
	DT_LOG_DEBUG(flProcessParameters.logLevel,
		     "FLpCDToCpCD services management config file = '" << flProcessParameters.servicesSubsystemConfig << "'");
 
	DT_THROW_IF(! flProcessParameters.servicesSubsystemConfigUrn.empty() and ! flProcessParameters.servicesSubsystemConfig.empty(),
		    FLConfigUserError,
		    "Explicit service configuration URN and service configuration file conflict!"); 
      }
    
      {
	// In principle all "flpcdtocpcd::section"(s) of interest should have
	// been processed now.
	// Clean the flProcessConfig2 from unused sections of type "flreconstruct::section":
	std::vector<std::string> section_keys = flProcessConfig2.keys();
	std::vector<std::string> unused_section_keys;
	for (const auto& section_key : section_keys) {
	  if (flProcessConfig2.has_key_with_meta(section_key, "flreconstruct::section")) {
	    DT_LOG_ERROR(flProcessParameters.logLevel, "Found an unused flpcdtocpcd section named '"
			 << section_key << "'! We will discard it!");
	    unused_section_keys.push_back(section_key);
	  }
	}
	for (std::size_t i = 0; i < unused_section_keys.size(); i++) {
	  flProcessConfig2.remove(unused_section_keys[i]);
	  DT_LOG_ERROR(flProcessParameters.logLevel, "Unused flpcdtocpcd section named '"
		       << section_keys[i] << "' has been removed.");
	}
      }

      // Check for allowed inline modules:
      if (flProcessParameters.userProfile == "production" && !flProcessConfig2.empty()) {
	DT_THROW(FLConfigUserError, "User profile '"
		 << flProcessParameters.userProfile << "' "
		 << "does not allow the definitions of inline modules!");
      }

      // From this point, no other section than inline processing pipeline modules definitions
      // should be present in the script. So we fetch modules configuration:
      
    }
  
    do_postprocess(flProcessParameters);
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
  }

  void do_postprocess_input_metadata(FLpCDToCpCDParams & flProcessParameters_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    DT_LOG_DEBUG(flProcessParameters_.logLevel, "Collect input metadata from input files");
    // Collect input metadata from input files:
    // we first try from some input metadata companion file, if provided,
    // then from the input data file itself, in case it contains metadata events.

    falaise::app::metadata_collector mc;
    if (not flProcessParameters_.inputMetadataFile.empty()) {
      // Fetch the metadata from the companion input metadata file, if any:
      mc.set_input_metadata_file(flProcessParameters_.inputMetadataFile);
      flProcessParameters_.inputMetadata = mc.get_metadata_from_metadata_file();
    } else if (not flProcessParameters_.inputFile.empty()) {
      // Fetch the metadata from the input data file:
      mc.set_input_data_file(flProcessParameters_.inputFile);
      flProcessParameters_.inputMetadata = mc.get_metadata_from_data_file();
    } else {
      // No metadata is available. Do nothing.
      // Probably we should handle this path in the future.
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
		     "No input metadata was provided. There is now way to check compatibility of the input data context and the configuration of the pcdtocpcd setup!");
    }
    if (datatools::logger::is_debug(flProcessParameters_.logLevel)) {
      flProcessParameters_.inputMetadata.tree_dump(std::cerr, "Input metadata: ", "[debug] ");
    }

    // Input metadata of interest:
    falaise::app::metadata_input iMeta;

    // Extract input metadata of interest:
    if (! flProcessParameters_.inputMetadata.empty()) {
      // Try to extract informations from the metadata:
      iMeta.scan(flProcessParameters_.inputMetadata);
      if (datatools::logger::is_notice(flProcessParameters_.logLevel)) {
	std::cerr << "[notice] Input metadata of interest:\n";
	iMeta.print(std::cerr);
      }
    } // End of using input metadata

      // Try to fetch the experimental setup URN
    if (! iMeta.experimentalSetupUrn.empty()) {
      DT_LOG_NOTICE(flProcessParameters_.logLevel,
		    "Experimental setup identifier (URN) from the input metadata is '"
		    << iMeta.experimentalSetupUrn << "'.");
    } else {
      DT_LOG_WARNING(flProcessParameters_.logLevel,
		     "No experimental setup identifier (URN) is set from input metadata.");
    }

    // Checks:
    {
      // Check the user profile:
      if (flProcessParameters_.userProfile == "production") {
	DT_THROW_IF(iMeta.userProfile != "production", FLConfigUserError,
		    "User profile '"
		    << flProcessParameters_.userProfile << "' "
		    << "is not compatible with input metadata production user profile '"
		    << iMeta.userProfile << "'!");
      } else if (flProcessParameters_.userProfile == "normal") {
	DT_THROW_IF(iMeta.userProfile == "expert", FLConfigUserError,
		    "User profile '"
		    << flProcessParameters_.userProfile << "' "
		    << "is not compatible with input metadata production user profile '"
		    << iMeta.userProfile << "'!");
      }

      // Check the experimental setup identifier:
      if (!flProcessParameters_.experimentalSetupUrn.empty()) {
	DT_THROW_IF(!iMeta.experimentalSetupUrn.empty() &&
		    (iMeta.experimentalSetupUrn != flProcessParameters_.experimentalSetupUrn),
		    std::logic_error,
		    "Experimental setup URN='" << flProcessParameters_.experimentalSetupUrn
		    << "' conflicts with experimental setup URN='"
		    << iMeta.experimentalSetupUrn
		    << "' extracted from input metadata!");
      }
      
    } // End of checks.

      // Settings:
    {
      if (flProcessParameters_.experimentalSetupUrn.empty()) {
	// Force the experimental setup identifier from the value found int the input metadata
	flProcessParameters_.experimentalSetupUrn = iMeta.experimentalSetupUrn;
	DT_LOG_NOTICE(flProcessParameters_.logLevel, "The experimental setup identifier (URN) is set to '"
		      << iMeta.experimentalSetupUrn
		      << "' from input metadata.");
      }
    } // End of settings.
    DT_LOG_DEBUG(flProcessParameters_.logLevel, "Done.");
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;
  }

  void do_configure_variant(FLpCDToCpCDApplication & recApplication_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    datatools::kernel& dtk = datatools::kernel::instance();
    const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

    FLpCDToCpCDParams & flProcessParameters_ = recApplication_.parameters;

    if (flProcessParameters_.experimentalSetupUrn.empty()) {
      // If experimental setup URN is not set..
      DT_LOG_NOTICE(flProcessParameters_.logLevel, "No experimental setup identifier (URN) is set.");

      // Variants service configuration can be hardcoded:
      if (!flProcessParameters_.variantSubsystemParams.config_filename.empty()) {
	DT_LOG_NOTICE(flProcessParameters_.logLevel,
		      "Using a manually set variant service configuration file : '"
		      << flProcessParameters_.variantSubsystemParams.config_filename << "'");
      }
    }
  
    // The experimental setup URN is set, we try to extract automatically the path of the
    // components associated to it (variants)
    if (!flProcessParameters_.experimentalSetupUrn.empty()) {
      DT_LOG_DEBUG(flProcessParameters_.logLevel, " -> experimentalSetupUrn='" << flProcessParameters_.experimentalSetupUrn << "'");
      // Check URN registration from the system URN query service:
      {
	const std::string & conf_category = falaise::tags::experimental_setup_category();
	DT_THROW_IF(!dtkUrnQuery.check_urn_info(flProcessParameters_.experimentalSetupUrn, conf_category),
		    std::logic_error,
		    "Cannot query URN='" << flProcessParameters_.experimentalSetupUrn << "'!");
      }
      const datatools::urn_info & expSetupUrnInfo =
	dtkUrnQuery.get_urn_info(flProcessParameters_.experimentalSetupUrn);
      if (datatools::logger::is_debug(flProcessParameters_.logLevel)) {
	expSetupUrnInfo.tree_dump(std::cerr, "expSetupUrnInfo", "[debug] ");
      }
    
      // Variants:
      // Automatically determine the variants configuration component:
      std::string variantConfigUrn;
      if (expSetupUrnInfo.has_topic("variants")) {
	const std::vector<std::string> & vv = expSetupUrnInfo.get_components_by_topic("variants");
	if (vv.size() == 1) {
	  DT_LOG_DEBUG(flProcessParameters_.logLevel, " -> Found 'variants' component");
	  variantConfigUrn = expSetupUrnInfo.get_component("variants");
	}
	for (const std::string & v : vv) {
	  DT_LOG_DEBUG(flProcessParameters_.logLevel, " -> Variant service's configuration URN from setup is '" << v << "'");    
	}
      }
      DT_LOG_DEBUG(flProcessParameters_.logLevel, " -> variantConfigUrn='" << variantConfigUrn << "'");
      DT_LOG_DEBUG(flProcessParameters_.logLevel, " -> params.variantConfigUrn='" << flProcessParameters_.variantConfigUrn << "'");   
      if (! flProcessParameters_.variantConfigUrn.empty()) {
	DT_THROW_IF(flProcessParameters_.variantConfigUrn != variantConfigUrn, std::logic_error,
		    "Variant config URN='" << flProcessParameters_.variantConfigUrn << "' does not match "
		    << "the automatically resolved URN='" << variantConfigUrn
		    << "' from experimental setup '"
		    << flProcessParameters_.experimentalSetupUrn << "'!");
      } else {
	DT_LOG_DEBUG(flProcessParameters_.logLevel, "Using the variant service's configuration '" << variantConfigUrn << "' from the setup...");
	flProcessParameters_.variantConfigUrn = variantConfigUrn;
      }
      if (! flProcessParameters_.variantConfigUrn.empty()) {
	// Resolve variants file:
	std::string conf_variants_category = "configuration";
	std::string conf_variants_mime;
	std::string conf_variants_path;
	DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flProcessParameters_.variantConfigUrn,
						      conf_variants_category,
						      conf_variants_mime,
						      conf_variants_path),
		    std::logic_error,
		    "Cannot resolve URN='" << flProcessParameters_.variantConfigUrn << "'!");
	flProcessParameters_.variantSubsystemParams.config_filename = conf_variants_path;
	DT_LOG_DEBUG(flProcessParameters_.logLevel, "Resolved variant service's configuration file is '" << conf_variants_path << "'");
      }
    }

    // Variants profile:
    if (! flProcessParameters_.variantSubsystemParams.profile_load.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(! flProcessParameters_.variantProfileUrn.empty(),
		  std::logic_error,
		  "Required variants profile URN='"
		  << flProcessParameters_.variantProfileUrn << "' "
		  << "conflicts with required variants profile path='"
		  << flProcessParameters_.variantSubsystemParams.profile_load << "'!");
    } else if (! flProcessParameters_.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flProcessParameters_.variantProfileUrn,
						    conf_variantsProfile_category, conf_variantsProfile_mime,
						    conf_variantsProfile_path),
		  std::logic_error,
		  "Cannot resolve variants profile URN='" << flProcessParameters_.variantProfileUrn << "'!");
      flProcessParameters_.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }

    // Warnings:
    if (flProcessParameters_.variantSubsystemParams.config_filename.empty()) {
      DT_LOG_WARNING(flProcessParameters_.logLevel, "No variants configuration is provided.");
    } else {
      if (flProcessParameters_.variantSubsystemParams.profile_load.empty()) {
	DT_LOG_WARNING(flProcessParameters_.logLevel, "No variants profile is provided.");
      }
    }

    // Start the variant service:
    datatools::configuration::variant_service & variantService = recApplication_.variantService;
    if (! flProcessParameters_.variantSubsystemParams.logging.empty()) {
      variantService.set_logging(datatools::logger::get_priority(flProcessParameters_.variantSubsystemParams.logging));
    }
    if (flProcessParameters_.variantSubsystemParams.is_active()) {
      DT_LOG_DEBUG(flProcessParameters_.logLevel, "Starting the variant service...");
      variantService.configure(flProcessParameters_.variantSubsystemParams);
      variantService.start();
    } else {
      DT_LOG_WARNING(flProcessParameters_.logLevel, "Do not start the variant service!");
    }
  
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;  
  }

  void do_postprocess(FLpCDToCpCDParams& flProcessParameters_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    /*
    if (! flProcessParameters_.pcdtocpcdPipelineUrn.empty()) {
      DT_LOG_DEBUG(flProcessParameters_.logLevel,
		   "pcd2cpcd pipeline category = '" << falaise::tags::reconstruction_pipeline_category() << "'");
      // Check URN registration from the system URN query service:
      DT_THROW_IF(! dtkUrnQuery.check_urn_info(flProcessParameters_.pcdtocpcdPipelineUrn,
					       falaise::tags::reconstruction_pipeline_category()),
		  std::logic_error,
		  "Cannot query pcdtocpcd pipeline URN='"
		  << flProcessParameters_.pcdtocpcdPipelineUrn << "'!");
      // Resolve pcdtocpcd config file path:
      std::string conf_rec_category = "configuration";
      std::string conf_rec_mime;
      std::string conf_rec_path;
      DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flProcessParameters_.pcdtocpcdPipelineUrn,
						    conf_rec_category,
						    conf_rec_mime,
						    conf_rec_path),
		  std::logic_error,
		  "Cannot resolve URN='" << flProcessParameters_.pcdtocpcdPipelineUrn << "'!");
      DT_LOG_DEBUG(flProcessParameters_.logLevel, "Resolved pipeline definition file '" << conf_rec_path << "' from URN='"
		   << flProcessParameters_.pcdtocpcdPipelineUrn << "'");
      flProcessParameters_.pcdtocpcdPipelineConfig = conf_rec_path;
    }
    */

    // if (! flProcessParameters_.pcdtocpcdPipelineConfig.empty()) {
    //   if (! flProcessParameters_.modulesConfig.empty()) {
    // 	DT_THROW(std::logic_error,
    // 		 "Pipeline module definition file '"
    // 		 << flProcessParameters_.pcdtocpcdPipelineConfig << "' "
    // 		 << "conflicts with pipeline inline configuration provided by the main configuration script!");
    //   }
    //   std::string pipelineConfigFilename = flProcessParameters_.pcdtocpcdPipelineConfig;
    //   datatools::fetch_path_with_env(pipelineConfigFilename);
    //   flProcessParameters_.modulesConfig.read(pipelineConfigFilename);
    //   if (datatools::logger::is_debug(flProcessParameters_.logLevel)) {
    // 	flProcessParameters_.modulesConfig.tree_dump(std::cerr, "Pipeline configuration: ", "[debug] ");
    //   }
    // } else {
    //   DT_LOG_NOTICE(flProcessParameters_.logLevel,
    // 		    "No pcdtocpcd pipeline configuration (URN/path) is set.");
    // }

    if (flProcessParameters_.experimentalSetupUrn.empty()) {
      // If experimental setup URN is not set..
      DT_LOG_NOTICE(flProcessParameters_.logLevel, "No experimental setup identifier (URN) is set.");
      // Services configuration must be hardcoded (typically with at least the geometry service):
      DT_THROW_IF(flProcessParameters_.servicesSubsystemConfig.empty(), std::logic_error,
		  "Missing services configuration file!");
    }

    // The experimental setup URN is set, we try to extract automatically the path of the
    // components associated to it (services)
    if (not flProcessParameters_.experimentalSetupUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
	const std::string & confCategory = falaise::tags::experimental_setup_category();
	DT_THROW_IF(not dtkUrnQuery.check_urn_info(flProcessParameters_.experimentalSetupUrn, confCategory),
		    std::logic_error,
		    "Cannot query URN='" << flProcessParameters_.experimentalSetupUrn << "'!");
      }
      const datatools::urn_info & expSetupUrnInfo =
	dtkUrnQuery.get_urn_info(flProcessParameters_.experimentalSetupUrn);

      // Services:
      if (not flProcessParameters_.servicesSubsystemConfig.empty()) {
	// Force the services config path:
	DT_THROW_IF(not flProcessParameters_.servicesSubsystemConfigUrn.empty(), std::logic_error,
		    "Service configuration URN='" << flProcessParameters_.servicesSubsystemConfigUrn
		    << "' "
		    << "conflicts with services configuration path='"
		    << flProcessParameters_.servicesSubsystemConfig << "'!");
      } else {
	DT_LOG_DEBUG(flProcessParameters_.logLevel, "Trying to fetch a services' configuration from the experimental setup...");
	// Try to set the services setup from a blessed services configuration URN:
	std::string servicesSubsystemConfigUrn;
	if (flProcessParameters_.servicesSubsystemConfigUrn.empty()) {
	  if (expSetupUrnInfo.has_topic("services") &&
	      expSetupUrnInfo.get_components_by_topic("services").size() == 1) {
	    // If the experimental setup URN implies a "services" component, fetch it!
	    servicesSubsystemConfigUrn = expSetupUrnInfo.get_component("services");
	    DT_LOG_DEBUG(flProcessParameters_.logLevel, "Found services' configuration '" << servicesSubsystemConfigUrn << "' from the experimental setup...");
	  }
	}
	if (not flProcessParameters_.servicesSubsystemConfigUrn.empty()) {
	  DT_THROW_IF(flProcessParameters_.servicesSubsystemConfigUrn != servicesSubsystemConfigUrn,
		      std::logic_error,
		      "Services config URN='"
		      << flProcessParameters_.servicesSubsystemConfigUrn << "' does not match "
		      << "the automatically resolved URN='" << servicesSubsystemConfigUrn
		      << "' from experimental setup '" << flProcessParameters_.experimentalSetupUrn
		      << "'!");
	} else {
	  flProcessParameters_.servicesSubsystemConfigUrn = servicesSubsystemConfigUrn;
	}
	if (not flProcessParameters_.servicesSubsystemConfigUrn.empty()) {
	  // Resolve services file:
	  std::string conf_services_category = "configuration";
	  std::string conf_services_mime;
	  std::string conf_services_path;
	  DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flProcessParameters_.servicesSubsystemConfigUrn,
						       conf_services_category,
						       conf_services_mime,
						       conf_services_path),
		      std::logic_error,
		      "Cannot resolve URN='" << flProcessParameters_.servicesSubsystemConfigUrn << "'!");
	  flProcessParameters_.servicesSubsystemConfig = conf_services_path;
	  DT_LOG_DEBUG(flProcessParameters_.logLevel, "Resolved services' configuration is '" << conf_services_path << "'");
	}
      }
    }

    if (flProcessParameters_.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flProcessParameters_.logLevel, "No services configuration is provided.");
    }

    // Print:
    if (datatools::logger::is_debug(flProcessParameters_.logLevel)) {
      flProcessParameters_.print(std::cerr);
    }

    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;
  }

  falaise::exit_code do_metadata(const FLpCDToCpCDParams & flProcessParameters_,
				 datatools::multi_properties & flProcessMetadata_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    falaise::exit_code code = falaise::EXIT_OK;

    {
      // System section:
      datatools::properties & system_props =
	flProcessMetadata_.add_section("flpcdtocpcd", "flreconstruct::section");
      system_props.set_description("flpcdtocpcd basic system informations");

      system_props.store_string("bayeux.version", bayeux::version::get_version(), "Bayeux version");

      system_props.store_string("falaise.version", falaise::version::get_version(),
				"Falaise version");
      system_props.store_string("falaise.version.commit", falaise::version::get_commit(),
				"Falaise commit");
      system_props.store_boolean("falaise.version.state", falaise::version::is_dirty(),
				 "Falaise commit state");

      system_props.store_string("application", "flpcdtocpcd",
				"The flpcdtocpcd application used to produce pcdtocpcded data");

      system_props.store_string("application.version", falaise::version::get_version(),
				"The version of the pcdtocpcd application");

      system_props.store_string("userProfile", flProcessParameters_.userProfile, "User profile");

      if (flProcessParameters_.numberOfEvents > 0) {
	system_props.store_integer("numberOfEvents", flProcessParameters_.numberOfEvents,
				   "Number of pcdtocpcded events");
      }
      system_props.store_string("processSetupUrn", flProcessParameters_.processSetupUrn,
				"pcdtocpcd process setup URN");

      if (not flProcessParameters_.experimentalSetupUrn.empty()) {
	system_props.store_string("experimentalSetupUrn", flProcessParameters_.experimentalSetupUrn,
				  "Experimental setup URN");
      }
    }

    {
      // Variants section:
      datatools::properties & variants_props =
	flProcessMetadata_.add_section("flpcd2cpcd.variantService", "flreconstruct::section");
      variants_props.set_description("Variant setup");

      if (!flProcessParameters_.variantConfigUrn.empty()) {
	variants_props.store_string("configUrn", flProcessParameters_.variantConfigUrn,
				    "Variants setup configuration URN");
      } else if (!flProcessParameters_.variantSubsystemParams.config_filename.empty()) {
	variants_props.store_path("config", flProcessParameters_.variantSubsystemParams.config_filename,
				  "Variants setup configuration path");
      }

      if (!flProcessParameters_.variantProfileUrn.empty()) {
	variants_props.store_string("profileUrn", flProcessParameters_.variantProfileUrn,
				    "Variants profile URN");
      } else if (!flProcessParameters_.variantSubsystemParams.profile_load.empty()) {
	variants_props.store_path("profile", flProcessParameters_.variantSubsystemParams.profile_load,
				  "Variants profile path");
      }

      if (!flProcessParameters_.variantSubsystemParams.settings.empty()) {
	// Not with "production" user profile:
	variants_props.store("settings", flProcessParameters_.variantSubsystemParams.settings,
			     "Variants settings");
      }
    }

    {
      // Services section:
      datatools::properties & services_props =
	flProcessMetadata_.add_section("flpcd2cpcd.services", "flreconstruct::section");
      services_props.set_description("Services configuration");

      if (!flProcessParameters_.servicesSubsystemConfigUrn.empty()) {
	services_props.store_string("configUrn", flProcessParameters_.servicesSubsystemConfigUrn,
				    "Services setup configuration URN");
      } else if (!flProcessParameters_.servicesSubsystemConfig.empty()) {
	services_props.store_path("config", flProcessParameters_.servicesSubsystemConfig,
				  "Services setup configuration path");
      }
    }

    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return code;
  }

  void do_terminate_variant(FLpCDToCpCDApplication & recApplication_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    if (recApplication_.variantService.is_started()) {
      // Terminate the variant service:
      recApplication_.variantService.stop();
    }
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;
  }
 
  void do_terminate(FLpCDToCpCDApplication & recApplication_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    do_terminate_variant(recApplication_);
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
    return;
  }
  
} // namespace FLpCDToCpCD
