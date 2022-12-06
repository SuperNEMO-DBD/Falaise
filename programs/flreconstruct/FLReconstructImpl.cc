// Ourselves
#include "FLReconstructImpl.h"

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
#include "FLReconstructCommandLine.h"
#include "FLReconstructParams.h"
#include "falaise/exitcodes.h"
#include "falaise/metadata_utils.h"
#include "falaise/property_set.h"
#include "falaise/resource.h"
#include "falaise/tags.h"
#include "falaise/version.h"
#include "falaise/snemo/processing/config.h"

namespace FLReconstruct {

  void do_mount_points(const FLReconstructParams & flRecParams_)
  {
    const FLReconstructParams & flRecParameters = flRecParams_;
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
    return;
  }

  ///! Parse command line arguments to configure the reconstruction parameters
  void do_configure(int argc_, char * argv_[], FLReconstructApplication & flRecApp_)
  {
    FLReconstructParams & flRecParameters = flRecApp_.parameters;
    // - Default Config
    flRecParameters = FLReconstructParams::makeDefault();

    // - CL Dialog Config
    FLReconstructCommandLine clArgs;
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
    DT_LOG_DEBUG(clArgs.logLevel, "FLReconstruct command line parameters:");
    if (datatools::logger::is_debug(clArgs.logLevel)) {
      clArgs.print(std::cerr);
    }

    // Import parameters from the command line:
    flRecParameters.logLevel = clArgs.logLevel;
    flRecParameters.reconstructionConfig = clArgs.configScript;
    flRecParameters.numberOfEvents = clArgs.maxNumberOfEvents;
    flRecParameters.moduloEvents = clArgs.moduloEvents;
    flRecParameters.userProfile = clArgs.userProfile;
    flRecParameters.inputMetadataFile = clArgs.inputMetadataFile;
    flRecParameters.inputFile = clArgs.inputFile;
    flRecParameters.outputMetadataFile = clArgs.outputMetadataFile;
    flRecParameters.outputFile = clArgs.outputFile;
    flRecParameters.mountPoints = clArgs.mountPoints;
    //  flRecParameters.reconstructionPipelineConfig = clArgs.configScript;
    DT_LOG_DEBUG(flRecParameters.logLevel, "FLReconstruct parameters after command line parsing:");
    if (datatools::logger::is_debug(flRecParameters.logLevel)) {
      flRecParameters.print(std::cerr);
    }

    if (not flRecParameters.mountPoints.empty()) {
      do_mount_points(flRecParameters);
    }

    if (flRecParameters.userProfile.empty()) {
      // Force a default user profile:
      DT_LOG_DEBUG(flRecParameters.logLevel, "Forcing user profile to default value 'normal'...");
      flRecParameters.userProfile = "normal";
    }

    // First parsing pass of the FLReconstruct configuration script:
    {
      DT_LOG_DEBUG(flRecParameters.logLevel,
                   "First parsing pass of the FLReconstruct configuration script...");
      datatools::multi_properties flRecConfig("name", "type");
      if (! flRecParameters.reconstructionConfig.empty()) {
        std::string flRecConfigScript = flRecParameters.reconstructionConfig;
        datatools::fetch_path_with_env(flRecConfigScript);
        flRecConfig.read(flRecConfigScript);
      }
      if (datatools::logger::is_debug(flRecParameters.logLevel)) {
        flRecConfig.tree_dump(std::cerr, "flRecConfig: ", "[debug] ");
      }
      // Fetch basic configuration from the script:
      if (flRecConfig.has_key_with_meta("flreconstruct", "flreconstruct::section")) {
        DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing FLReconstruct main configuration directives...");
        // Reconstruction basic subsystem:
        falaise::property_set basicSystem{flRecConfig.get_section("flreconstruct")};
        flRecConfig.remove("flreconstruct");
        
        // Fetch the reconstruction setup URN:
        flRecParameters.reconstructionSetupUrn =
          basicSystem.get<std::string>("reconstructionSetupURN", flRecParameters.reconstructionSetupUrn);
        if (flRecParameters.reconstructionSetupUrn.empty()) {
          // Use the default one:
          flRecParameters.reconstructionSetupUrn = snemo::processing::default_reconstruction_tag();
        }
        
        // Fetch the experimental setup URN:
        flRecParameters.experimentalSetupUrn =
          basicSystem.get<std::string>("experimentalSetupURN", flRecParameters.experimentalSetupUrn);

        {
          datatools::kernel& dtk = datatools::kernel::instance();
          const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();
          // Fetch the reconstruction setup identifier/URN:
          std::string confCategory = "recsetup";
          DT_THROW_IF(!dtkUrnQuery.check_urn_info(flRecParameters.reconstructionSetupUrn, confCategory),
                      std::logic_error,
                      "Cannot query URN='" << flRecParameters.reconstructionSetupUrn << "'!");
             // and extract the associated 'experimentalSetupUrn':
          const datatools::urn_info & recSetupUrnInfo = dtkUrnQuery.get_urn_info(flRecParameters.reconstructionSetupUrn);
          if (recSetupUrnInfo.has_topic("setup") &&
              recSetupUrnInfo.get_components_by_topic("setup").size() == 1) {
            std::string experimentalSetupUrn = recSetupUrnInfo.get_component("setup");
            if (flRecParameters.experimentalSetupUrn.empty()) {
              flRecParameters.experimentalSetupUrn = experimentalSetupUrn;
            } else {
              if (flRecParameters.experimentalSetupUrn != experimentalSetupUrn) {
                DT_THROW(std::logic_error,
                         "Experimental setup URN='"
                         << flRecParameters.experimentalSetupUrn << "' does not match "
                         << "the automatically resolved URN='" <<experimentalSetupUrn
                         << "' from the reconstruction setup '" << flRecParameters.reconstructionSetupUrn
                         << "'!");
              } 
            }
          }
        }

        // Number of events to be processed:
        flRecParameters.numberOfEvents =
          basicSystem.get<int>("numberOfEvents", flRecParameters.numberOfEvents);

        // Printing rate for events:
        flRecParameters.moduloEvents =
          basicSystem.get<int>("moduloEvents", flRecParameters.moduloEvents);

        // Printing rate for events:
        flRecParameters.userProfile =
          basicSystem.get<std::string>("userprofile", flRecParameters.userProfile);
      }

      // Process input metadata:
      do_postprocess_input_metadata(flRecParameters);
 
      // Fetch variant service configuration:
      if (flRecConfig.has_key_with_meta("flreconstruct.variantService", "flreconstruct::section")) {
        DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing FLReconstruct variant service's configuration directives...");
        falaise::property_set variantSubsystem{flRecConfig.get_section("flreconstruct.variantService")};
        DT_LOG_DEBUG(flRecParameters.logLevel, "Variant config: \n" << variantSubsystem.to_string());
      
        // Variant configuration URN:
        flRecParameters.variantConfigUrn =
          variantSubsystem.get<std::string>("configUrn", flRecParameters.variantConfigUrn);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "variantConfigUrn: '" << flRecParameters.variantConfigUrn << "'");

        // Variant configuration:
        if (flRecParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError, "User profile '" << flRecParameters.userProfile << "' "
                   << "does not allow to use the '"
                   << "config"
                   << "' variants configuration parameter!");
        }
        flRecParameters.variantSubsystemParams.config_filename =
          variantSubsystem.get<std::string>("config", flRecParameters.variantSubsystemParams.config_filename);

        DT_THROW_IF(! flRecParameters.variantConfigUrn.empty() and ! flRecParameters.variantSubsystemParams.config_filename.empty(),
                    FLConfigUserError,
                    "Explicit variant service configuration URN and variant service configuration file conflict!"); 

        // Variant profile URN:
        flRecParameters.variantProfileUrn =
          variantSubsystem.get<std::string>("profileUrn", flRecParameters.variantProfileUrn);
        DT_LOG_DEBUG(flRecParameters.logLevel, " -> variantProfileUrn=" << flRecParameters.variantProfileUrn);
      
        // Variant profile:
        flRecParameters.variantSubsystemParams.profile_load =
          variantSubsystem.get<falaise::path>("profile", flRecParameters.variantSubsystemParams.profile_load);
        DT_LOG_DEBUG(flRecParameters.logLevel, " -> variant.profile_load='"
                     << flRecParameters.variantSubsystemParams.profile_load << "'");

        // Variant settings:
        if (flRecParameters.userProfile != "expert" && variantSubsystem.has_key("settings")) {
          DT_THROW(FLConfigUserError, "User profile '" << flRecParameters.userProfile << "' "
                   << "does not allow to use the '"
                   << "settings"
                   << "' variants configuration parameter!");
        }
        flRecParameters.variantSubsystemParams.settings =
          variantSubsystem.get<std::vector<std::string>>("settings",
                                                         flRecParameters.variantSubsystemParams.settings);
      }
    }

    // Variants support set up first because all other services will
    // rely on it.
    DT_LOG_DEBUG(flRecParameters.logLevel, "Configure variant subsystem...");
    do_configure_variant(flRecApp_);

    {
      DT_LOG_DEBUG(flRecParameters.logLevel, "Second parsing pass of the FLReconstruct configuration script...");

     // Second parsing pass of the FLReconstruct configuration script:
      datatools::multi_properties flRecConfig("name", "type");
      if (! flRecParameters.reconstructionConfig.empty()) {
        std::string flRecConfigScript = flRecParameters.reconstructionConfig;
        datatools::fetch_path_with_env(flRecConfigScript);
        flRecConfig.read(flRecConfigScript);
      }
  
      // Fetch plugins configuration:
      if (flRecConfig.has_key_with_meta("flreconstruct.plugins", "flreconstruct::section")) {
        DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing FLReconstruct plugins' configuration directives...");
        try {
          falaise::property_set userFLPlugins{flRecConfig.get_section("flreconstruct.plugins")};
          flRecConfig.remove("flreconstruct.plugins");

          auto pList = userFLPlugins.get<std::vector<std::string>>("plugins", {});

          for (const std::string & plugin_name : pList) {
            auto pSection = userFLPlugins.get<falaise::property_set>(plugin_name, {});
            pSection.put("autoload", true);
            if (!pSection.has_key("directory")) {
              pSection.put("directory", std::string{"@falaise.plugins:"});
            }

            flRecParameters.userLibConfig.add(plugin_name, "", pSection);
          }
        } catch (std::logic_error & e) {
          DT_LOG_ERROR(flRecParameters.logLevel, e.what());
          // do nothing for now because we can't distinguish errors, and
          // upcoming instantiation of library loader will handle
          // any syntax errors in the properties
        }
      }

      // Fetch services configuration:
      if (flRecConfig.has_key_with_meta("flreconstruct.services", "flreconstruct::section")) {
        DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing FLReconstruct services management directives...");
        falaise::property_set servicesSubsystem{flRecConfig.get_section("flreconstruct.services")};
        flRecConfig.remove("flreconstruct.services");
        
        // Services manager configuration URN:
        flRecParameters.servicesSubsystemConfigUrn =
          servicesSubsystem.get<std::string>("configUrn", flRecParameters.servicesSubsystemConfigUrn);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "FLReconstruct services management config URN = '" << flRecParameters.servicesSubsystemConfigUrn << "'");
        
        // Services manager main configuration file:
        if (flRecParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
          // User profile 'production' must used official registered services configuration:
          DT_THROW(FLConfigUserError, "User profile '" << flRecParameters.userProfile << "' "
                   << "does not allow to use the '"
                   << "config"
                   << "' services configuration parameter!");
        }
        flRecParameters.servicesSubsystemConfig =
          servicesSubsystem.get<std::string>("config", flRecParameters.servicesSubsystemConfig);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "FLReconstruct services management config file = '" << flRecParameters.servicesSubsystemConfig << "'");
 
        DT_THROW_IF(! flRecParameters.servicesSubsystemConfigUrn.empty() and ! flRecParameters.servicesSubsystemConfig.empty(),
                    FLConfigUserError,
                    "Explicit service configuration URN and service configuration file conflict!"); 
      }

      // Fetch pipeline configuration:
      if (flRecConfig.has_key_with_meta("flreconstruct.pipeline", "flreconstruct::section")) {
        DT_LOG_DEBUG(flRecParameters.logLevel, "Parsing FLReconstruct pipeline's directives...");
        falaise::property_set pipelineSubsystem{flRecConfig.get_section("flreconstruct.pipeline")};
        flRecConfig.remove("flreconstruct.pipeline");

        if (datatools::logger::is_debug(flRecParameters.logLevel)) {
          std::cerr << "Pipeline subsystem: \n" << pipelineSubsystem.to_string() << std::endl;
        }

        flRecParameters.reconstructionPipelineUrn =
          pipelineSubsystem.get<std::string>("configUrn", flRecParameters.reconstructionPipelineUrn);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "FLReconstruct pipeline config URN = '" << flRecParameters.reconstructionPipelineUrn << "'");

        flRecParameters.reconstructionPipelineConfig =
          pipelineSubsystem.get<std::string>("config", flRecParameters.reconstructionPipelineConfig);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "FLReconstruct pipeline config file = '" << flRecParameters.reconstructionPipelineConfig << "'");

        DT_THROW_IF(! flRecParameters.reconstructionPipelineUrn.empty() and ! flRecParameters.reconstructionPipelineConfig.empty(),
                    FLConfigUserError,
                    "Explicit pipeline configuration URN and pipeline configuration file conflict!"); 

        // Name of the top level pipeline module:
        flRecParameters.reconstructionPipelineModule =
          pipelineSubsystem.get<std::string>("module", flRecParameters.reconstructionPipelineModule);
        DT_LOG_DEBUG(flRecParameters.logLevel,
                     "Explicit name of the top level pipeline module = '" << flRecParameters.reconstructionPipelineModule << "'");
      }
  
      {
        // In principle all "flreconstruct::section"(s) of interest should have
        // been processed now.
        // Clean the flRecConfig from unused sections of type "flreconstruct::section":
        std::vector<std::string> section_keys = flRecConfig.keys();
        std::vector<std::string> unused_section_keys;
        for (const auto& section_key : section_keys) {
          if (flRecConfig.has_key_with_meta(section_key, "flreconstruct::section")) {
            DT_LOG_ERROR(flRecParameters.logLevel, "Found an unused flreconstruct section named '"
                         << section_key << "'! We will discard it!");
            unused_section_keys.push_back(section_key);
          }
        }
        for (std::size_t i = 0; i < unused_section_keys.size(); i++) {
          flRecConfig.remove(unused_section_keys[i]);
          DT_LOG_ERROR(flRecParameters.logLevel, "Unused flreconstruct section named '"
                       << section_keys[i] << "' has been removed.");
        }
      }

      // Check for allowed inline modules:
      if (flRecParameters.userProfile == "production" && !flRecConfig.empty()) {
        DT_THROW(FLConfigUserError, "User profile '"
                 << flRecParameters.userProfile << "' "
                 << "does not allow the definitions of inline modules!");
      }

      // From this point, no other section than inline processing pipeline modules definitions
      // should be present in the script. So we fetch modules configuration:
      flRecParameters.modulesConfig = flRecConfig;
      
    }
  
    do_postprocess(flRecParameters);
  }

  void do_postprocess_input_metadata(FLReconstructParams & flRecParameters_)
  {
    // Collect input metadata from input files:
    // we first try from some input metadata companion file, if provided,
    // then from the input data file itself, in case it contains metadata records.

    falaise::app::metadata_collector mc;
    if (!flRecParameters_.inputMetadataFile.empty()) {
      // Fetch the metadata from the companion input metadata file, if any:
      mc.set_input_metadata_file(flRecParameters_.inputMetadataFile);
      flRecParameters_.inputMetadata = mc.get_metadata_from_metadata_file();
    } else if (!flRecParameters_.inputFile.empty()) {
      // Fetch the metadata from the input data file:
      mc.set_input_data_file(flRecParameters_.inputFile);
      flRecParameters_.inputMetadata = mc.get_metadata_from_data_file();
    } else {
      // No metadata is available. Do nothing.
      // Probably we should handle this path in the future.
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "No input metadata was provided. There is now way to check compatibility of the input data context and the configuration of the reconstruction setup!");
    }
    if (datatools::logger::is_debug(flRecParameters_.logLevel)) {
      flRecParameters_.inputMetadata.tree_dump(std::cerr, "Input metadata: ", "[debug] ");
    }

    // Input metadata of interest:
    falaise::app::metadata_input iMeta;

    // Extract input metadata of interest:
    if (! flRecParameters_.inputMetadata.empty()) {
      // Try to extract informations from the metadata:
      iMeta.scan(flRecParameters_.inputMetadata);
      if (datatools::logger::is_notice(flRecParameters_.logLevel)) {
        std::cerr << "[notice] Input metadata of interest:\n";
        iMeta.print(std::cerr);
      }
    } // End of using input metadata

    // Try to fetch the experimental setup URN
    if (!iMeta.experimentalSetupUrn.empty()) {
      DT_LOG_NOTICE(flRecParameters_.logLevel,
                    "Experimental setup identifier (URN) from the input metadata is '"
                    << iMeta.experimentalSetupUrn << "'.");
    } else {
      DT_LOG_WARNING(flRecParameters_.logLevel,
                     "No experimental setup identifier (URN) is set from input metadata.");
    }

    // Checks:
    {
      // Check the user profile:
      if (flRecParameters_.userProfile == "production") {
        DT_THROW_IF(iMeta.userProfile != "production", FLConfigUserError,
                    "User profile '"
                    << flRecParameters_.userProfile << "' "
                    << "is not compatible with input metadata production user profile '"
                    << iMeta.userProfile << "'!");
      } else if (flRecParameters_.userProfile == "normal") {
        DT_THROW_IF(iMeta.userProfile == "expert", FLConfigUserError,
                    "User profile '"
                    << flRecParameters_.userProfile << "' "
                    << "is not compatible with input metadata production user profile '"
                    << iMeta.userProfile << "'!");
      }

      // Check the experimental setup identifier:
      if (!flRecParameters_.experimentalSetupUrn.empty()) {
        DT_THROW_IF(!iMeta.experimentalSetupUrn.empty() &&
                    (iMeta.experimentalSetupUrn != flRecParameters_.experimentalSetupUrn),
                    std::logic_error,
                    "Experimental setup URN='" << flRecParameters_.experimentalSetupUrn
                    << "' conflicts with experimental setup URN='"
                    << iMeta.experimentalSetupUrn
                    << "' extracted from input metadata!");
      }
    } // End of checks.

    // Settings:
    {
      if (flRecParameters_.experimentalSetupUrn.empty()) {
        // Force the experimental setup identifier from the value found int the input metadata
        flRecParameters_.experimentalSetupUrn = iMeta.experimentalSetupUrn;
        DT_LOG_NOTICE(flRecParameters_.logLevel, "The experimental setup identifier (URN) is set to '"
                      << iMeta.experimentalSetupUrn
                      << "' from input metadata.");
      }
    } // End of settings.
    return;
  }

  void do_configure_variant(FLReconstructApplication & recApplication_)
  {
    datatools::kernel& dtk = datatools::kernel::instance();
    const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

    FLReconstructParams & flRecParameters_ = recApplication_.parameters;

    if (flRecParameters_.experimentalSetupUrn.empty()) {
      // If experimental setup URN is not set..
      DT_LOG_NOTICE(flRecParameters_.logLevel, "No experimental setup identifier (URN) is set.");

      // Variants service configuration can be hardcoded:
      if (!flRecParameters_.variantSubsystemParams.config_filename.empty()) {
        DT_LOG_NOTICE(flRecParameters_.logLevel,
                      "Using a manually set variant service configuration file : '"
                      << flRecParameters_.variantSubsystemParams.config_filename << "'");
      }
    }
  
    // The experimental setup URN is set, we try to extract automatically the path of the
    // components associated to it (variants)
    if (!flRecParameters_.experimentalSetupUrn.empty()) {
      DT_LOG_DEBUG(flRecParameters_.logLevel, " -> experimentalSetupUrn='" << flRecParameters_.experimentalSetupUrn << "'");
      // Check URN registration from the system URN query service:
      {
        const std::string & conf_category = falaise::tags::experimental_setup_category();
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flRecParameters_.experimentalSetupUrn, conf_category),
                    std::logic_error,
                    "Cannot query URN='" << flRecParameters_.experimentalSetupUrn << "'!");
      }
      const datatools::urn_info & expSetupUrnInfo =
        dtkUrnQuery.get_urn_info(flRecParameters_.experimentalSetupUrn);
      if (datatools::logger::is_debug(flRecParameters_.logLevel)) {
        expSetupUrnInfo.tree_dump(std::cerr, "expSetupUrnInfo", "[debug] ");
      }
    
      // Variants:
      // Automatically determine the variants configuration component:
      std::string variantConfigUrn;
      if (expSetupUrnInfo.has_topic("variants")) {
        const std::vector<std::string> & vv = expSetupUrnInfo.get_components_by_topic("variants");
        if (vv.size() == 1) {
          DT_LOG_DEBUG(flRecParameters_.logLevel, " -> Found 'variants' component");
          variantConfigUrn = expSetupUrnInfo.get_component("variants");
        }
        for (const std::string & v : vv) {
          DT_LOG_DEBUG(flRecParameters_.logLevel, " -> Variant service's configuration URN from setup is '" << v << "'");    
        }
      }
      DT_LOG_DEBUG(flRecParameters_.logLevel, " -> variantConfigUrn='" << variantConfigUrn << "'");
      DT_LOG_DEBUG(flRecParameters_.logLevel, " -> params.variantConfigUrn='" << flRecParameters_.variantConfigUrn << "'");   
      if (! flRecParameters_.variantConfigUrn.empty()) {
        DT_THROW_IF(flRecParameters_.variantConfigUrn != variantConfigUrn, std::logic_error,
                    "Variant config URN='" << flRecParameters_.variantConfigUrn << "' does not match "
                    << "the automatically resolved URN='" << variantConfigUrn
                    << "' from experimental setup '"
                    << flRecParameters_.experimentalSetupUrn << "'!");
      } else {
        DT_LOG_DEBUG(flRecParameters_.logLevel, "Using the variant service's configuration '" << variantConfigUrn << "' from the setup...");
        flRecParameters_.variantConfigUrn = variantConfigUrn;
      }
      if (! flRecParameters_.variantConfigUrn.empty()) {
        // Resolve variants file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flRecParameters_.variantConfigUrn,
                                                      conf_variants_category,
                                                      conf_variants_mime,
                                                      conf_variants_path),
                    std::logic_error,
                    "Cannot resolve URN='" << flRecParameters_.variantConfigUrn << "'!");
        flRecParameters_.variantSubsystemParams.config_filename = conf_variants_path;
        DT_LOG_DEBUG(flRecParameters_.logLevel, "Resolved variant service's configuration file is '" << conf_variants_path << "'");
      }
    }

    // Variants profile:
    if (! flRecParameters_.variantSubsystemParams.profile_load.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(! flRecParameters_.variantProfileUrn.empty(),
                  std::logic_error,
                  "Required variants profile URN='"
                  << flRecParameters_.variantProfileUrn << "' "
                  << "conflicts with required variants profile path='"
                  << flRecParameters_.variantSubsystemParams.profile_load << "'!");
    } else if (! flRecParameters_.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flRecParameters_.variantProfileUrn,
                                                    conf_variantsProfile_category, conf_variantsProfile_mime,
                                                    conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << flRecParameters_.variantProfileUrn << "'!");
      flRecParameters_.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }

    // Warnings:
    if (flRecParameters_.variantSubsystemParams.config_filename.empty()) {
      DT_LOG_WARNING(flRecParameters_.logLevel, "No variants configuration is provided.");
    } else {
      if (flRecParameters_.variantSubsystemParams.profile_load.empty()) {
        DT_LOG_WARNING(flRecParameters_.logLevel, "No variants profile is provided.");
      }
    }

    // Start the variant service:
    datatools::configuration::variant_service & variantService = recApplication_.variantService;
    if (! flRecParameters_.variantSubsystemParams.logging.empty()) {
      variantService.set_logging(datatools::logger::get_priority(flRecParameters_.variantSubsystemParams.logging));
    }
    if (flRecParameters_.variantSubsystemParams.is_active()) {
      DT_LOG_DEBUG(flRecParameters_.logLevel, "Starting the variant service...");
      variantService.configure(flRecParameters_.variantSubsystemParams);
      variantService.start();
    } else {
      DT_LOG_WARNING(flRecParameters_.logLevel, "Do not start the variant service!");
    }
  
    return;  
  }

  void do_postprocess(FLReconstructParams& flRecParameters_)
  {
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    if (! flRecParameters_.reconstructionPipelineUrn.empty()) {
      DT_LOG_DEBUG(flRecParameters_.logLevel,
                   "Reconstruction pipeline category = '" << falaise::tags::reconstruction_pipeline_category() << "'");
       // Check URN registration from the system URN query service:
      DT_THROW_IF(! dtkUrnQuery.check_urn_info(flRecParameters_.reconstructionPipelineUrn,
                                               falaise::tags::reconstruction_pipeline_category()),
                  std::logic_error,
                  "Cannot query reconstruction pipeline URN='"
                  << flRecParameters_.reconstructionPipelineUrn << "'!");
      // Resolve reconstruction config file path:
      std::string conf_rec_category = "configuration";
      std::string conf_rec_mime;
      std::string conf_rec_path;
      DT_THROW_IF(! dtkUrnQuery.resolve_urn_to_path(flRecParameters_.reconstructionPipelineUrn,
                                                    conf_rec_category,
                                                    conf_rec_mime,
                                                    conf_rec_path),
                  std::logic_error,
                  "Cannot resolve URN='" << flRecParameters_.reconstructionPipelineUrn << "'!");
      DT_LOG_DEBUG(flRecParameters_.logLevel, "Resolved pipeline definition file '" << conf_rec_path << "' from URN='"
                   << flRecParameters_.reconstructionPipelineUrn << "'");
      flRecParameters_.reconstructionPipelineConfig = conf_rec_path;
    }

    if (! flRecParameters_.reconstructionPipelineConfig.empty()) {
      if (! flRecParameters_.modulesConfig.empty()) {
        DT_THROW(std::logic_error,
                 "Pipeline module definition file '"
                 << flRecParameters_.reconstructionPipelineConfig << "' "
                 << "conflicts with pipeline inline configuration provided by the main configuration script!");
      }
      std::string pipelineConfigFilename = flRecParameters_.reconstructionPipelineConfig;
      datatools::fetch_path_with_env(pipelineConfigFilename);
      flRecParameters_.modulesConfig.read(pipelineConfigFilename);
      if (datatools::logger::is_debug(flRecParameters_.logLevel)) {
        flRecParameters_.modulesConfig.tree_dump(std::cerr, "Pipeline configuration: ", "[debug] ");
      }
    } else {
      DT_LOG_NOTICE(flRecParameters_.logLevel,
                    "No reconstruction pipeline configuration (URN/path) is set.");
    }

    if (flRecParameters_.experimentalSetupUrn.empty()) {
      // If experimental setup URN is not set..
      DT_LOG_NOTICE(flRecParameters_.logLevel, "No experimental setup identifier (URN) is set.");

      // Services configuration must be hardcoded (typically with at least the geometry service):
      DT_THROW_IF(flRecParameters_.servicesSubsystemConfig.empty(), std::logic_error,
                  "Missing services configuration file!");
    }

    // The experimental setup URN is set, we try to extract automatically the path of the
    // components associated to it (services)
    if (!flRecParameters_.experimentalSetupUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
        const std::string & confCategory = falaise::tags::experimental_setup_category();
        DT_THROW_IF(! dtkUrnQuery.check_urn_info(flRecParameters_.experimentalSetupUrn, confCategory),
                    std::logic_error,
                    "Cannot query URN='" << flRecParameters_.experimentalSetupUrn << "'!");
      }
      const datatools::urn_info & expSetupUrnInfo =
        dtkUrnQuery.get_urn_info(flRecParameters_.experimentalSetupUrn);

      // Services:
      if (!flRecParameters_.servicesSubsystemConfig.empty()) {
        // Force the services config path:
        DT_THROW_IF(! flRecParameters_.servicesSubsystemConfigUrn.empty(), std::logic_error,
                    "Service configuration URN='" << flRecParameters_.servicesSubsystemConfigUrn
                    << "' "
                    << "conflicts with services configuration path='"
                    << flRecParameters_.servicesSubsystemConfig << "'!");
      } else {
        DT_LOG_DEBUG(flRecParameters_.logLevel, "Trying to fetch a services' configuration from the experimental setup...");
        // Try to set the services setup from a blessed services configuration URN:
        std::string servicesSubsystemConfigUrn;
        if (flRecParameters_.servicesSubsystemConfigUrn.empty()) {
          if (expSetupUrnInfo.has_topic("services") &&
              expSetupUrnInfo.get_components_by_topic("services").size() == 1) {
            // If the experimental setup URN implies a "services" component, fetch it!
            servicesSubsystemConfigUrn = expSetupUrnInfo.get_component("services");
            DT_LOG_DEBUG(flRecParameters_.logLevel, "Found services' configuration '" << servicesSubsystemConfigUrn << "' from the experimental setup...");
          }
        }
        if (!flRecParameters_.servicesSubsystemConfigUrn.empty()) {
          DT_THROW_IF(flRecParameters_.servicesSubsystemConfigUrn != servicesSubsystemConfigUrn,
                      std::logic_error,
                      "Services config URN='"
                      << flRecParameters_.servicesSubsystemConfigUrn << "' does not match "
                      << "the automatically resolved URN='" << servicesSubsystemConfigUrn
                      << "' from experimental setup '" << flRecParameters_.experimentalSetupUrn
                      << "'!");
        } else {
          flRecParameters_.servicesSubsystemConfigUrn = servicesSubsystemConfigUrn;
        }
        if (!flRecParameters_.servicesSubsystemConfigUrn.empty()) {
          // Resolve services file:
          std::string conf_services_category = "configuration";
          std::string conf_services_mime;
          std::string conf_services_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flRecParameters_.servicesSubsystemConfigUrn,
                                                       conf_services_category,
                                                       conf_services_mime,
                                                       conf_services_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flRecParameters_.servicesSubsystemConfigUrn << "'!");
          flRecParameters_.servicesSubsystemConfig = conf_services_path;
          DT_LOG_DEBUG(flRecParameters_.logLevel, "Resolved services' configuration is '" << conf_services_path << "'");
        }
      }
    }

    if (flRecParameters_.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flRecParameters_.logLevel, "No services configuration is provided.");
    }

    // Print:
    if (datatools::logger::is_debug(flRecParameters_.logLevel)) {
      flRecParameters_.print(std::cerr);
    }

    return;
  }

  falaise::exit_code do_metadata(const FLReconstructParams & flRecParameters_,
                                 datatools::multi_properties & flRecMetadata_)
  {
    falaise::exit_code code = falaise::EXIT_OK;

    {
      // System section:
      datatools::properties & system_props =
        flRecMetadata_.add_section("flreconstruct", "flreconstruct::section");
      system_props.set_description("flreconstruct basic system informations");

      system_props.store_string("bayeux.version", bayeux::version::get_version(), "Bayeux version");

      system_props.store_string("falaise.version", falaise::version::get_version(),
                                "Falaise version");
      system_props.store_string("falaise.version.commit", falaise::version::get_commit(),
                                "Falaise commit");
      system_props.store_boolean("falaise.version.state", falaise::version::is_dirty(),
                                 "Falaise commit state");

      system_props.store_string("application", "flreconstruct",
                                "The flreconstruct application used to produce reconstructed data");

      system_props.store_string("application.version", falaise::version::get_version(),
                                "The version of the reconstruction application");

      system_props.store_string("userProfile", flRecParameters_.userProfile, "User profile");

      if (flRecParameters_.numberOfEvents > 0) {
        system_props.store_integer("numberOfEvents", flRecParameters_.numberOfEvents,
                                   "Number of reconstructed events");
      }
      system_props.store_string("reconstructionSetupUrn", flRecParameters_.reconstructionSetupUrn,
                                "Reconstruction setup URN");

      if (!flRecParameters_.experimentalSetupUrn.empty()) {
        system_props.store_string("experimentalSetupUrn", flRecParameters_.experimentalSetupUrn,
                                  "Experimental setup URN");
      }
    }

    {
      // Variants section:
      datatools::properties & variants_props =
        flRecMetadata_.add_section("flreconstruct.variantService", "flreconstruct::section");
      variants_props.set_description("Variant setup");

      if (!flRecParameters_.variantConfigUrn.empty()) {
        variants_props.store_string("configUrn", flRecParameters_.variantConfigUrn,
                                    "Variants setup configuration URN");
      } else if (!flRecParameters_.variantSubsystemParams.config_filename.empty()) {
        variants_props.store_path("config", flRecParameters_.variantSubsystemParams.config_filename,
                                  "Variants setup configuration path");
      }

      if (!flRecParameters_.variantProfileUrn.empty()) {
        variants_props.store_string("profileUrn", flRecParameters_.variantProfileUrn,
                                    "Variants profile URN");
      } else if (!flRecParameters_.variantSubsystemParams.profile_load.empty()) {
        variants_props.store_path("profile", flRecParameters_.variantSubsystemParams.profile_load,
                                  "Variants profile path");
      }

      if (!flRecParameters_.variantSubsystemParams.settings.empty()) {
        // Not with "production" user profile:
        variants_props.store("settings", flRecParameters_.variantSubsystemParams.settings,
                             "Variants settings");
      }
    }

    {
      // Services section:
      datatools::properties & services_props =
        flRecMetadata_.add_section("flreconstruct.services", "flreconstruct::section");
      services_props.set_description("Services configuration");

      if (!flRecParameters_.servicesSubsystemConfigUrn.empty()) {
        services_props.store_string("configUrn", flRecParameters_.servicesSubsystemConfigUrn,
                                    "Services setup configuration URN");
      } else if (!flRecParameters_.servicesSubsystemConfig.empty()) {
        services_props.store_path("config", flRecParameters_.servicesSubsystemConfig,
                                  "Services setup configuration path");
      }
    }

    {
      // Pipeline section:
      datatools::properties& reconstruction_props =
        flRecMetadata_.add_section("flreconstruct.pipeline", "flreconstruct::section");
      reconstruction_props.set_description("Reconstruction setup parameters");

      if (!flRecParameters_.reconstructionPipelineUrn.empty()) {
        reconstruction_props.store_string("configUrn", flRecParameters_.reconstructionPipelineUrn,
                                          "Reconstruction setup URN");
      } else if (!flRecParameters_.reconstructionPipelineConfig.empty()) {
        reconstruction_props.store_path("config", flRecParameters_.reconstructionPipelineConfig,
                                        "Reconstruction setup main configuration file");
      }

      if (!flRecParameters_.reconstructionPipelineModule.empty()) {
        reconstruction_props.store_string("module", flRecParameters_.reconstructionPipelineModule,
                                          "Reconstruction pipeline top module");
      }
    }

    return code;
  }

  void do_terminate_variant(FLReconstructApplication & recApplication_)
  {
    if (recApplication_.variantService.is_started()) {
      // Terminate the variant service:
      recApplication_.variantService.stop();
    }
    return;
  }
 
  void do_terminate(FLReconstructApplication & recApplication_)
  {
    do_terminate_variant(recApplication_);
    return;
  }
  
} // namespace FLReconstruct
