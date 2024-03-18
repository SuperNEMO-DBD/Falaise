// Ourselves
#include "FLSimulateArgs.h"

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/library_info.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/mygsl/random_utils.h>

// This Project:
#include "FLSimulateCommandLine.h"
#include "FLSimulateErrors.h"
#include "FLSimulateUtils.h"
#include "falaise/property_set.h"
#include "falaise/tags.h"

namespace FLSimulate {

  // static
  const std::string & FLSimulateArgs::default_file_for_output_metadata()
  {
    static const std::string _f("__flsimulate-metadata.log");
    return _f;
  }

  // static
  const std::string & FLSimulateArgs::default_file_for_seeds()
  {
    static const std::string _f("__flsimulate-seeds.log");
    return _f;
  }

  void do_postprocess(FLSimulateArgs & flSimParameters);

  // static
  FLSimulateArgs FLSimulateArgs::makeDefault()
  {
    FLSimulateArgs params;

    // Application specific parameters:
    params.logLevel = datatools::logger::PRIO_ERROR;
    params.userProfile = "normal";
    params.numberOfEvents = 1u;
    params.runNumber = datatools::event_id::ANY_RUN_NUMBER;
    params.firstEventNumber = 0u;
    params.doSimulation = true;
    params.doDigitization = false;
    // Identification of the experimental setup:
    params.experimentalSetupUrn = "";

    // Identification of the simulation setup:
    params.simulationSetupUrn = default_simulation_setup();
    // Simulation manager internal parameters:
    params.simulationManagerParams.set_defaults();
    params.simulationManagerParams.interactive = false;
    params.simulationManagerParams.logging = "error";
    params.simulationManagerParams.manager_config_filename = "";
    // Seeding is auto (from system) unless explicit file supplied
    params.simulationManagerParams.input_prng_seeds_file = "";
    params.simulationManagerParams.output_prng_seeds_file = "";
    params.simulationManagerParams.vg_seed =
      mygsl::random_utils::SEED_AUTO;  // PRNG for the vertex generator
    params.simulationManagerParams.eg_seed =
      mygsl::random_utils::SEED_AUTO;  // PRNG for the primary event generator
    params.simulationManagerParams.shpf_seed =
      mygsl::random_utils::SEED_AUTO;  // PRNG for the back end MC hit processors
    params.simulationManagerParams.mgr_seed =
      mygsl::random_utils::SEED_AUTO;  // PRNG for the Geant4 engine itself
    params.simulationManagerParams.output_profiles_activation_rule = "";
    params.saveRngSeeding = true;
    params.rngSeeding = "";

    // Variants support:
    params.variantConfigUrn = "";
    params.variantProfileUrn = "";
    params.variantServiceConfig.config_filename = "";
    params.saveVariantSettings = true;

    // Service support:
    params.servicesSubsystemConfigUrn = "";
    params.servicesSubsystemConfig = "";

    // I/O control:
    params.outputMetadataFile = "";
    params.embeddedMetadata = true;
    params.outputFile = "";

    // Plugins management:
    params.userLibConfig.reset();
    params.userLibConfig.set_key_label("name");
    params.userLibConfig.set_meta_label("filename");

    return params;
  }

  void do_configure(int argc_, char* argv_[], FLSimulateArgs& flSimParameters_)
  {
    // - Default Config
    flSimParameters_ = FLSimulateArgs::makeDefault();

    // - CL Dialog Config
    FLSimulateCommandLine args;
    try {
      do_cldialog(argc_, argv_, args);
    } catch (FLDialogHelpRequested& e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogOptionsError& e) {
      throw FLConfigUserError{"bad command line input"};
    }

    // Feed input from command line to params
    flSimParameters_.logLevel = args.logLevel;
    flSimParameters_.userProfile = args.userProfile;
    flSimParameters_.outputMetadataFile = args.outputMetadataFile;
    flSimParameters_.embeddedMetadata = args.embeddedMetadata;
    flSimParameters_.outputFile = args.outputFile;
    flSimParameters_.mountPoints = args.mountPoints;
    flSimParameters_.numberOfEvents = args.numberOfEvents;
    flSimParameters_.runNumber = args.runNumber;
    flSimParameters_.firstEventNumber = args.firstEventNumber;
 
    if (static_cast<unsigned int>(!flSimParameters_.mountPoints.empty()) != 0u) {
      // Apply mount points as soon as possible, because manually set file path below
      // may use this mechanism to locate files:
      datatools::kernel & dtk = datatools::kernel::instance();
      datatools::library_info & dtklLibInfo = dtk.grab_library_info_register();
      for (const std::string & mountDirective : flSimParameters_.mountPoints) {
        std::string theLibname;
        std::string theTopic;
        std::string thePath;
        std::string errMsg;
        bool parsed = datatools::library_info::parse_path_registration_directive(mountDirective,
										 theLibname,
										 theTopic,
										 thePath,
										 errMsg);
        DT_LOG_DEBUG(flSimParameters_.logLevel, "Mount directive: '" << mountDirective << "'");
        // DT_LOG_DEBUG(flSimParameters_.logLevel, "theLibname: " << theLibname);
        DT_THROW_IF(!parsed, FLConfigUserError,
                    "Cannot parse directory mount directive '" << mountDirective << "' : " << errMsg);
        if (theTopic.empty()) {
          theTopic = datatools::library_info::default_topic_label();
          DT_LOG_DEBUG(flSimParameters_.logLevel,
                       "Using default path registration topic: " << theTopic);
        }
        DT_LOG_DEBUG(flSimParameters_.logLevel, "Path registration: " << mountDirective);
        DT_LOG_DEBUG(flSimParameters_.logLevel, "  Library name : " << theLibname);
        DT_LOG_DEBUG(flSimParameters_.logLevel, "  Topic        : " << theTopic);
        DT_LOG_DEBUG(flSimParameters_.logLevel, "  Path         : " << thePath);
        try {
          dtklLibInfo.path_registration(theLibname, theTopic, thePath, false);
        } catch (std::exception& error) {
          DT_THROW(FLConfigUserError, "Cannot apply directory mount directive '"
                   << mountDirective << "': " << error.what());
        }
      }
    }

    if (!flSimParameters_.embeddedMetadata) {
      if (flSimParameters_.outputMetadataFile.empty()) {
        // Force a default metadata log file:
        flSimParameters_.outputMetadataFile = FLSimulateArgs::default_file_for_output_metadata();
      }
    }

    // If a script was supplied, use that to override params
    if (!args.configScript.empty()) {
      datatools::multi_properties flSimConfig("name", "type");
      std::string configScript = args.configScript;
      datatools::fetch_path_with_env(configScript);
      flSimConfig.read(configScript);
      DT_LOG_DEBUG(flSimParameters_.logLevel, "Simulation Configuration:");
      if (datatools::logger::is_debug(flSimParameters_.logLevel)) {
        flSimConfig.tree_dump(std::cerr, "", "[debug] ");
      }

      // Now extract and bind values as needed
      // Caution: some parameters are only available for specific user profile

      // Fetch plugins configuration:
      DT_LOG_DEBUG(flSimParameters_.logLevel, "Fetch plugins configuration...");
      if (flSimConfig.has_key_with_meta("flsimulate.plugins", "flsimulate::section")) {
        try {
	  // std::cerr << "[devel] Go fetch !\n";
          falaise::property_set userFLPlugins{flSimConfig.get_section("flsimulate.plugins")};
          flSimConfig.remove("flsimulate.plugins");

          auto pList = userFLPlugins.get<std::vector<std::string>>("plugins", {});
          for (const std::string& plugin_name : pList) {
            std::cerr << "[devel] => plugin '" << plugin_name << "'\n";
            auto pSection = userFLPlugins.get<falaise::property_set>(plugin_name, {});
            pSection.put("autoload", true);
            if (pSection.has_key("directory")) {
              //std::cerr << "[devel]   => key '" << "directory" << "'\n";
              auto libdirPath = pSection.get<falaise::path>("directory");
              //std::cerr << "[devel]   => libdirPath = '" << libdirPath << "'\n";
              std::string libdir(libdirPath);
              //std::cerr << "[devel]   => libdir = '" << libdir << "'\n";
              datatools::fetch_path_with_env(libdir);
              //std::cerr << "[devel]   => libdir = '" << libdir << "' (resolved)\n";
              falaise::path resolvedLibdirPath{libdir};
              pSection.put_or_replace<falaise::path>("directory", resolvedLibdirPath);
            }
          
            // if (pSection.has_key("filename")) {
            //   std::string libfilename = pSection.get<falaise::path>("filename");
            //   datatools::fetch_path_with_env(libfilename);
            //   pSection.put_or_replace<falaise::path>("filename", libfilename);          
            // }
          
            // if (pSection.has_key("full_path")) {
            //   std::string libfullpath = pSection.get<falaise::path>("full_path");
            //   datatools::fetch_path_with_env(libfullpath);
            //   pSection.put_or_replace<falaise::path>("full_path", libfullpath);         
            // }
          
            if (!pSection.has_key("directory")) {
              pSection.put("directory", std::string{"@falaise.plugins:"});
            }
            datatools::properties backProps{pSection};
            backProps.tree_dump(std::cerr, "Plugin section '" + plugin_name + "' : ", "[devel] ");
        
            flSimParameters_.userLibConfig.add(plugin_name, "", pSection);
          }
        
        } catch (std::logic_error& e) {
          DT_LOG_ERROR(flSimParameters_.logLevel, e.what());
          // do nothing for now because we can't distinguish errors, and
          // upcoming instantiation of library loader will handle
          // any syntax errors in the properties
        }
      }
 
      // Basic system:
      if (flSimConfig.has_key_with_meta("flsimulate", "flsimulate::section")) {
        falaise::property_set baseSystem{flSimConfig.get_section("flsimulate")};

        // Number of simulated events:
	int noe = baseSystem.get<int>("numberOfEvents", flSimParameters_.numberOfEvents);
	DT_THROW_IF(noe <= 0, FLConfigUserError, "Invalid number of events : " << noe);
	flSimParameters_.numberOfEvents = noe;

        // Run number:
        flSimParameters_.runNumber =
          baseSystem.get<int>("runNumber", flSimParameters_.runNumber);

        // First event number:
	int fen = baseSystem.get<int>("firstEventNumber", static_cast<int>(flSimParameters_.firstEventNumber));
	DT_THROW_IF(fen < 0, FLConfigUserError, "Invalid first event number : " << fen);
        flSimParameters_.firstEventNumber = static_cast<unsigned int>(fen);
	
        // Printing rate for events:
	int nem = baseSystem.get<int>("moduloEvents",
				      flSimParameters_.simulationManagerParams.number_of_events_modulo);
	DT_THROW_IF(nem < 0, FLConfigUserError, "Invalid event printing rate : " << nem);
        flSimParameters_.simulationManagerParams.number_of_events_modulo = nem;
	
		    
        // Do simulation:
        flSimParameters_.doSimulation =
          baseSystem.get<bool>("doSimulation", flSimParameters_.doSimulation);

        // Do digitization:
        flSimParameters_.doDigitization =
          baseSystem.get<bool>("doDigitization", flSimParameters_.doDigitization);
      }

      // Simulation subsystem:
      if (flSimConfig.has_key_with_meta("flsimulate.simulation", "flsimulate::section")) {
        falaise::property_set simSubsystem{flSimConfig.get_section("flsimulate.simulation")};
        // Bind properties in this section to the relevant ones in params:

        // Simulation setup URN:
        flSimParameters_.simulationSetupUrn =
          simSubsystem.get<std::string>("simulationSetupUrn", flSimParameters_.simulationSetupUrn);

        DT_LOG_DEBUG(flSimParameters_.logLevel,
                     "flSimParameters_.simulationSetupUrn=" << flSimParameters_.simulationSetupUrn);

        // Simulation manager main configuration file:
        if (flSimParameters_.userProfile == "production" &&
            simSubsystem.has_key("simulationSetupConfig")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "does not allow to use the '"
                   << "simulationSetupConfig"
                   << "' simulation configuration parameter!");
        }
        flSimParameters_.simulationManagerParams.manager_config_filename =
          simSubsystem.get<std::string>(
                                        "simulationSetupConfig",
                                        flSimParameters_.simulationManagerParams.manager_config_filename);

        // File for loading internal PRNG's seeds:
        if (flSimParameters_.userProfile == "production" && !simSubsystem.has_key("rngSeedFile")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "must use the '"
                   << "rngSeedFile"
                   << "' simulation configuration parameter!");
        }
        flSimParameters_.simulationManagerParams.input_prng_seeds_file
	  = simSubsystem.get<falaise::path>("rngSeedFile",
					    flSimParameters_.simulationManagerParams.input_prng_seeds_file);

        if (flSimParameters_.simulationManagerParams.input_prng_seeds_file.empty()) {
          if (flSimParameters_.userProfile == "production") {
            DT_THROW_IF(simSubsystem.has_key("rngEventGeneratorSeed"), FLConfigUserError,
                        "User profile '" << flSimParameters_.userProfile << "' "
                        << "does not allow to use the '"
                        << "rngEventGeneratorSeed"
                        << "' simulation configuration parameter!");
            DT_THROW_IF(simSubsystem.has_key("rngVertexGeneratorSeed"), FLConfigUserError,
                        "User profile '" << flSimParameters_.userProfile << "' "
                        << "does not allow to use the '"
                        << "rngVertexGeneratorSeed"
                        << "' simulation configuration parameter!");
            DT_THROW_IF(simSubsystem.has_key("rngHitPostprocessingGeneratorSeed"), FLConfigUserError,
                        "User profile '" << flSimParameters_.userProfile << "' "
                        << "does not allow to use the '"
                        << "rngHitProcessingGeneratorSeed"
                        << "' simulation configuration parameter!");
            DT_THROW_IF(simSubsystem.has_key("rngGeant4GeneratorSeed"), FLConfigUserError,
                        "User profile '" << flSimParameters_.userProfile << "' "
                        << "does not allow to use the '"
                        << "rngGeant4GeneratorSeed"
                        << "' simulation configuration parameter!");
          }
          flSimParameters_.simulationManagerParams.eg_seed
	    = simSubsystem.get<int>("rngEventGeneratorSeed",
				    flSimParameters_.simulationManagerParams.eg_seed);

          flSimParameters_.simulationManagerParams.vg_seed
	    = simSubsystem.get<int>("rngVertexGeneratorSeed",
				    flSimParameters_.simulationManagerParams.vg_seed);

          flSimParameters_.simulationManagerParams.shpf_seed
	    = simSubsystem.get<int>("rngHitProcessingGeneratorSeed",
				    flSimParameters_.simulationManagerParams.shpf_seed);

          flSimParameters_.simulationManagerParams.mgr_seed
	    = simSubsystem.get<int>("rngGeant4GeneratorSeed",
				    flSimParameters_.simulationManagerParams.mgr_seed);
        }

        flSimParameters_.simulationManagerParams.output_prng_seeds_file =
          simSubsystem.get<std::string>("rngSeedFileSave",
					flSimParameters_.simulationManagerParams.output_prng_seeds_file);

        // File for loading internal PRNG's states:
        if (flSimParameters_.userProfile != "expert" && simSubsystem.has_key("inputRngStateFile")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "does not allow to use the '"
                   << "inputRngStateFile"
                   << "' simulation configuration parameter!");
        }
        flSimParameters_.simulationManagerParams.input_prng_states_file =
          simSubsystem.get<std::string>("inputRngStateFile",
					flSimParameters_.simulationManagerParams.input_prng_states_file);

        // File for saving internal PRNG's states:
        flSimParameters_.simulationManagerParams.output_prng_states_file =
          simSubsystem.get<std::string>("outputRngStateFile",
                                        flSimParameters_.simulationManagerParams.output_prng_states_file);

        // Saving rate for internal PRNG's states:
        flSimParameters_.simulationManagerParams.prng_states_save_modulo
	  = simSubsystem.get<int>("rngStateModuloEvents",
				  flSimParameters_.simulationManagerParams.prng_states_save_modulo);
      }

      // Digitization subsystem:
      // if (flSimConfig.has_key_with_meta("flsimulate.digitization", "flsimulate::section")) {
      //  datatools::properties digiSubsystem = flSimConfig.get_section("flsimulate.digitization");
      // Bind properties in this section to the relevant ones in params:
      //}

      // Variants subsystem:
      if (flSimConfig.has_key_with_meta("flsimulate.variantService", "flsimulate::section")) {
        falaise::property_set variantSubsystem{flSimConfig.get_section("flsimulate.variantService")};
        // Bind properties to relevant ones on params

        // Variant configuration URN:
        flSimParameters_.variantConfigUrn =
          variantSubsystem.get<std::string>("configUrn", flSimParameters_.variantConfigUrn);

        // Variant configuration:
        if (flSimParameters_.userProfile == "production" && variantSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "does not allow to use the '"
                   << "config"
                   << "' variants configuration parameter!");
        }
        flSimParameters_.variantServiceConfig.config_filename
	  = variantSubsystem.get<std::string>("config", flSimParameters_.variantServiceConfig.config_filename);
	
        // Variant profile URN:
        flSimParameters_.variantProfileUrn =
          variantSubsystem.get<std::string>("profileUrn", flSimParameters_.variantProfileUrn);

        // Variant profile:
        flSimParameters_.variantServiceConfig.profile_load
	  = variantSubsystem.get<falaise::path>("profile",
						flSimParameters_.variantServiceConfig.profile_load);

        // Variant settings:
        if (flSimParameters_.userProfile == "production" && variantSubsystem.has_key("settings")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "does not allow to use the '"
                   << "settings"
                   << "' variants configuration parameter!");
        }
        flSimParameters_.variantServiceConfig.settings =
          variantSubsystem.get<std::vector<std::string>>("settings",
							 flSimParameters_.variantServiceConfig.settings);
      }

      // Services subsystem:
      if (flSimConfig.has_key_with_meta("flsimulate.services", "flsimulate::section")) {
        falaise::property_set servicesSubsystem{flSimConfig.get_section("flsimulate.services")};

        // Services manager configuration URN:
        flSimParameters_.servicesSubsystemConfigUrn
	  = servicesSubsystem.get<std::string>("configUrn", flSimParameters_.servicesSubsystemConfigUrn);

        // Services manager main configuration file:
        if (flSimParameters_.userProfile == "production" && servicesSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError, "User profile '" << flSimParameters_.userProfile << "' "
                   << "does not allow to use the '"
                   << "config"
                   << "' services configuration parameter!");
        }
        flSimParameters_.servicesSubsystemConfig =
          servicesSubsystem.get<std::string>("config", flSimParameters_.servicesSubsystemConfig);
      }

    }  // !args.configScript.empty()

    do_postprocess(flSimParameters_);
    return;
  }

  void do_postprocess(FLSimulateArgs & flSimParameters_)
  {
    DT_LOG_TRACE_ENTERING(flSimParameters_.logLevel);
    datatools::kernel& dtk = datatools::kernel::instance();
    const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

    if (flSimParameters_.simulationManagerParams.input_prng_seeds_file.empty()) {
      if (!flSimParameters_.saveRngSeeding &&
          flSimParameters_.simulationManagerParams.output_prng_seeds_file.empty()) {
        // Make sure PRNG seeds are stored in a default log file if
        // seeds are not stored in metadata:
        flSimParameters_.simulationManagerParams.output_prng_seeds_file =
          FLSimulateArgs::default_file_for_seeds();
      }
    }

    // Propagate verbosity to variant service:
    flSimParameters_.variantServiceConfig.logging =
      datatools::logger::get_priority_label(flSimParameters_.logLevel);

    if (flSimParameters_.simulationSetupUrn.empty()) {
      // Check for hardcoded path to the main simulation setup configuration file:
      if (!flSimParameters_.simulationManagerParams.manager_config_filename.empty()) {
        // Only for 'expert' of 'normal' user profiles.

        // Variant configuration:
        if (flSimParameters_.variantServiceConfig.config_filename.empty()) {
          DT_LOG_WARNING(flSimParameters_.logLevel, "No variant configuration file is provided!");
        }

        // Services configuration:
        if (flSimParameters_.servicesSubsystemConfig.empty()) {
          DT_LOG_WARNING(flSimParameters_.logLevel, "No services configuration file is provided!");
        }

      } else {
        DT_THROW(std::logic_error, "Missing simulation setup configuration file!");

        //   // Default simulation setup:
        //   if (flSimParameters_.simulationManagerParams.manager_config_filename.empty()) {
        //     flSimParameters_.simulationSetupUrn = default_simulation_setup();
        //     DT_LOG_WARNING(flSimParameters_.logLevel, "Use default simulation setup '" <<
        //     flSimParameters_.simulationSetupUrn << "'.");
        //   }
      }
    }

    datatools::urn_info simSetupUrnInfo;
    datatools::urn_info variantConfigUrnInfo;
    if (!flSimParameters_.simulationSetupUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters_.simulationSetupUrn,
                                                falaise::tags::simulation_setup_category()),
                    std::logic_error,
                    "Cannot query simulation setup URN='" << flSimParameters_.simulationSetupUrn << "'!");
      }
      simSetupUrnInfo = dtkUrnQuery.get_urn_info(flSimParameters_.simulationSetupUrn);

      if (flSimParameters_.experimentalSetupUrn.empty()) {
        // Automatically determine the experimental setup component:
        if (simSetupUrnInfo.has_topic("setup") &&
            simSetupUrnInfo.get_components_by_topic("setup").size() == 1) {
          flSimParameters_.experimentalSetupUrn = simSetupUrnInfo.get_component("setup");
        }
      }

      // Simulation:
      {
        // Resolve simulation config file path:
        std::string conf_simu_category = "configuration";
        std::string conf_simu_mime;
        std::string conf_simu_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters_.simulationSetupUrn,
                                                     conf_simu_category,
                                                     conf_simu_mime,
                                                     conf_simu_path),
                    std::logic_error, "Cannot resolve URN='" << flSimParameters_.simulationSetupUrn << "'!");
        flSimParameters_.simulationManagerParams.manager_config_filename = conf_simu_path;
      }

      // Variant setup:
      if (flSimParameters_.variantConfigUrn.empty()) {
        DT_LOG_DEBUG(flSimParameters_.logLevel, "variantConfigUrn is empty");
        // Automatically determine the variants configuration component:
        if (simSetupUrnInfo.has_topic("variants")) {
          DT_LOG_DEBUG(flSimParameters_.logLevel, "simSetupUrnInfo has 'variants'");
          const std::vector<std::string> & variantServiceComponents
            = simSetupUrnInfo.get_components_by_topic("variants");
          if (variantServiceComponents.size() == 1) {
            flSimParameters_.variantConfigUrn = variantServiceComponents.front();
          } else if (variantServiceComponents.size() > 1) {
            // Pickup the last one by default:
            flSimParameters_.variantConfigUrn = variantServiceComponents.back();      
          }
          DT_LOG_DEBUG(flSimParameters_.logLevel,
                       "Detected variantConfigUrn is '" << flSimParameters_.variantConfigUrn << "'");
        }
      }
      if (!flSimParameters_.variantConfigUrn.empty()) {
        // Check URN registration from the system URN query service:
          DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters_.variantConfigUrn,
                                                  falaise::tags::variant_service_category()),
                      std::logic_error,
                      "Cannot query variant setup URN='" << flSimParameters_.variantConfigUrn << "'!");
        variantConfigUrnInfo = dtkUrnQuery.get_urn_info(flSimParameters_.variantConfigUrn);
        if (datatools::logger::is_debug(flSimParameters_.logLevel)) {
          variantConfigUrnInfo.tree_dump(std::cerr, "Simulation Variant Service Info : ", "[debug] ");
        }
        // Resolve variant configuration file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters_.variantConfigUrn,
                                                     conf_variants_category,
                                                     conf_variants_mime,
                                                     conf_variants_path),
                    std::logic_error, "Cannot resolve URN='" << flSimParameters_.variantConfigUrn << "'!");
        flSimParameters_.variantServiceConfig.config_filename = conf_variants_path;
        DT_LOG_DEBUG(flSimParameters_.logLevel,
                     "Resolved variant service config file : '" << flSimParameters_.variantServiceConfig.config_filename << "'");
      }

      // Services:
      if (!flSimParameters_.servicesSubsystemConfig.empty()) {
        // Force the services configuration path:
        DT_THROW_IF(!flSimParameters_.servicesSubsystemConfigUrn.empty(), std::logic_error,
                    "Service configuration URN='" << flSimParameters_.servicesSubsystemConfigUrn
                    << "' conflicts with services configuration path='"
                    << flSimParameters_.servicesSubsystemConfig << "'!");
      } else {
        // Try to set the services setup from a blessed services configuration URN:
        if (flSimParameters_.servicesSubsystemConfigUrn.empty()) {
          if (simSetupUrnInfo.has_topic("services") &&
              simSetupUrnInfo.get_components_by_topic("services").size() == 1) {
            // If the simulation setup URN implies a "services" component, fetch it!
            flSimParameters_.servicesSubsystemConfigUrn = simSetupUrnInfo.get_component("services");
          }
        }
        if (!flSimParameters_.servicesSubsystemConfigUrn.empty()) {
          // Resolve services file:
          std::string conf_services_category = "configuration";
          std::string conf_services_mime;
          std::string conf_services_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters_.servicesSubsystemConfigUrn,
                                                       conf_services_category,
                                                       conf_services_mime,
                                                       conf_services_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flSimParameters_.servicesSubsystemConfigUrn << "'!");
          flSimParameters_.servicesSubsystemConfig = conf_services_path;
        }
      }
    } // !flSimParameters_.simulationSetupUrn.empty()

    if (!flSimParameters_.variantServiceConfig.profile_load.empty()) {
      // Check the variant config path:
      DT_THROW_IF(flSimParameters_.variantServiceConfig.config_filename.empty(), std::logic_error,
                  "Variant configuration file='"
                  << flSimParameters_.variantServiceConfig.config_filename << "' is missing!");
      // Force the variant profile path:
      DT_THROW_IF(!flSimParameters_.variantProfileUrn.empty(), std::logic_error,
                  "Variant profile URN='" << flSimParameters_.variantProfileUrn
                  << "' conflicts with variant profile path='"
                  << flSimParameters_.variantServiceConfig.profile_load
                  << "'!");
    } else {
      DT_LOG_DEBUG(flSimParameters_.logLevel, "No variant profile is set.");
      if (flSimParameters_.variantProfileUrn.empty()) {
        DT_LOG_DEBUG(flSimParameters_.logLevel, "No variant profile URN is set.");
        // No variant profile URN is set:
        if (simSetupUrnInfo.is_valid()) {
          DT_LOG_DEBUG(flSimParameters_.logLevel,
                       "Trying to find a default one from the current simulation setup...");
          // Try to find a default one from the current variant setup:
          if (simSetupUrnInfo.has_topic("defprofile") and
              simSetupUrnInfo.get_components_by_topic("defprofile").size() == 1) {
            // If the simulation setup URN implies a "services" component, fetch it!
            std::string defaultVariantProfileUrn = variantConfigUrnInfo.get_component("defprofile");
            DT_LOG_DEBUG(flSimParameters_.logLevel, "Using the default variant profile '"
                         << defaultVariantProfileUrn << "'"
                         << " associated to simulation setup '"
                         << variantConfigUrnInfo.get_urn() << "'.");
            DT_THROW_IF(!dtkUrnQuery.check_urn_info(defaultVariantProfileUrn,
                                                    falaise::tags::variant_profile_category()),
                        std::logic_error,
                        "Cannot query variant setup URN='" << flSimParameters_.variantConfigUrn << "'!");
            datatools::urn_info defaultVariantProfileConfigUrnInfo;
            defaultVariantProfileConfigUrnInfo = dtkUrnQuery.get_urn_info(defaultVariantProfileUrn);
            if (datatools::logger::is_debug(flSimParameters_.logLevel)) {
              defaultVariantProfileConfigUrnInfo.tree_dump(std::cerr,
                                                           "Simulation Default Variant Profile Info : ", "[debug] ");
            }
            DT_THROW_IF(! defaultVariantProfileConfigUrnInfo.has_topic("alias_of"),
                        std::logic_error,
                        "No alias for default input variant profile!");
            flSimParameters_.variantProfileUrn =
              defaultVariantProfileConfigUrnInfo.get_component("alias_of");
            DT_LOG_DEBUG(flSimParameters_.logLevel, "Using the default variant profile '"
                         << flSimParameters_.variantProfileUrn << "'"
                         << " associated to simulation setup '"
                         << simSetupUrnInfo.get_urn() << "'.");
          }
        }
      }
      if (!flSimParameters_.variantProfileUrn.empty()) {
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters_.variantProfileUrn,
                                                falaise::tags::variant_profile_category()),
                    std::logic_error,
                    "Cannot query variant setup URN='" << flSimParameters_.variantConfigUrn << "'!");
        datatools::urn_info variantProfileUrnInfo =
          dtkUrnQuery.get_urn_info(flSimParameters_.variantProfileUrn);
        if (datatools::logger::is_debug(flSimParameters_.logLevel)) {
          variantProfileUrnInfo.tree_dump(std::cerr,
                                          "Simulation Variant Profile Info : ", "[debug] ");
        }
        // Determine the variant profile path from a blessed variant profile URN:
        std::string conf_variantsProfile_category = "configuration";
        std::string conf_variantsProfile_mime;
        std::string conf_variantsProfile_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters_.variantProfileUrn,
                                                     conf_variantsProfile_category,
                                                     conf_variantsProfile_mime,
                                                     conf_variantsProfile_path),
                    std::logic_error,
                    "Cannot resolve variant profile URN='" << flSimParameters_.variantProfileUrn << "'!");
        flSimParameters_.variantServiceConfig.profile_load = conf_variantsProfile_path;
      }
    }

    if (flSimParameters_.variantServiceConfig.config_filename.empty()) {
      DT_LOG_WARNING(flSimParameters_.logLevel, "No variant configuration is provided.");
    } else {
      if (flSimParameters_.variantServiceConfig.profile_load.empty()) {
        DT_LOG_WARNING(flSimParameters_.logLevel, "No variant profile is provided.");
      } else {
        // Additional variants settings may be allowed but *must* be compatible
        // with selected variants config and optional variants profile.
      }
    }

    if (flSimParameters_.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flSimParameters_.logLevel, "No services configuration is provided.");
    }

    // Print:
    if (datatools::logger::is_debug(flSimParameters_.logLevel)) {
      flSimParameters_.print(std::cerr);
    }

    DT_LOG_TRACE_EXITING(flSimParameters_.logLevel);
    return;
  }

  void FLSimulateArgs::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLSimulate setup parameters: " << std::endl;
    out_ << tag
         << "logLevel                   = " << datatools::logger::get_priority_label(this->logLevel)
         << std::endl;
    out_ << tag << "userProfile                = " << userProfile << std::endl;
    out_ << tag << "numberOfEvents             = " << numberOfEvents << std::endl;
    out_ << tag << "runNumber                  = " << runNumber << std::endl;
    out_ << tag << "firstEventNumber           = " << firstEventNumber << std::endl;
    out_ << tag << "doSimulation               = " << std::boolalpha << doSimulation << std::endl;
    out_ << tag << "doDigitization             = " << std::boolalpha << doDigitization << std::endl;
    out_ << tag << "experimentalSetupUrn       = " << experimentalSetupUrn << std::endl;
    out_ << tag << "simulationSetupUrn         = " << simulationSetupUrn << std::endl;
    out_ << tag << "simulationSetupConfig      = " << simulationManagerParams.manager_config_filename
         << std::endl;
    out_ << tag << "saveRngSeeding             = " << std::boolalpha << saveRngSeeding << std::endl;
    out_ << tag << "rngSeeding                 = " << rngSeeding << std::endl;
    out_ << tag << "digitizationSetupUrn       = "
         << (digitizationSetupUrn.empty() ? "<not used>" : digitizationSetupUrn) << std::endl;
    out_ << tag << "variantConfigUrn           = " << variantConfigUrn << std::endl;
    out_ << tag << "variantProfileUrn          = " << variantProfileUrn << std::endl;
    out_ << tag << "variantServiceConfig.path  = " << variantServiceConfig.config_filename
         << std::endl;
    out_ << tag << "saveVariantSettings        = " << std::boolalpha << saveVariantSettings
         << std::endl;
    out_ << tag << "servicesSubsystemConfigUrn = " << servicesSubsystemConfigUrn << std::endl;
    out_ << tag << "servicesSubsystemConfig    = " << servicesSubsystemConfig << std::endl;
    out_ << tag << "outputMetadataFile         = " << outputMetadataFile << std::endl;
    out_ << tag << "embeddedMetadata           = " << std::boolalpha << embeddedMetadata << std::endl;
    out_ << last_tag << "outputFile                 = " << outputFile << std::endl;
    return;
  }

} // namespace FLSimulate
