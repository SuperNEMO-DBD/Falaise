// Ourselves
#include "FLSimulateArgs.h"

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/mygsl/random_utils.h>

// This Project:
#include "falaise/property_reader.h"
#include "FLSimulateCommandLine.h"
#include "FLSimulateErrors.h"

namespace FLSimulate {

  void do_postprocess(FLSimulateArgs & flSimParameters);

  // static
  FLSimulateArgs FLSimulateArgs::makeDefault()
  {
    FLSimulateArgs params;

    // Application specific parameters:
    params.logLevel       = datatools::logger::PRIO_ERROR;
    params.userProfile    = "normal";
    params.numberOfEvents = 1;
    params.doSimulation   = true;
    params.doDigitization = false;
    // Identification of the experimental setup:
    params.experimentalSetupUrn = "";

    // Identification of the simulation setup:
    params.simulationSetupUrn   = "urn:snemo:demonstrator:simulation:2.1";
    // Simulation manager internal parameters:
    params.simulationManagerParams.set_defaults();
    params.simulationManagerParams.interactive = false;
    params.simulationManagerParams.logging = "error";
    params.simulationManagerParams.manager_config_filename = "";
    // Seeding is auto (from system) unless explicit file supplied
    params.simulationManagerParams.input_prng_seeds_file = "";
    params.simulationManagerParams.vg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the vertex generator
    params.simulationManagerParams.eg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the primary event generator
    params.simulationManagerParams.shpf_seed = mygsl::random_utils::SEED_AUTO; // PRNG for the back end MC hit processors
    params.simulationManagerParams.mgr_seed  = mygsl::random_utils::SEED_AUTO; // PRNG for the Geant4 engine itself
    params.simulationManagerParams.output_profiles_activation_rule = "";

    // Variants support:
    params.variantConfigUrn = "";
    params.variantProfileUrn = "";
    params.variantSubsystemParams.config_filename = "";

    // Service support:
    params.servicesSubsystemConfigUrn = "";
    params.servicesSubsystemConfig = "";

    // I/O control:
    params.outputMetadataFile = "";
    params.embeddedMetadata   = false;
    params.outputFile         = "";

    return params;
  }

  void do_configure(int argc, char *argv[], FLSimulateArgs& flSimParameters) {
    // - Default Config
    flSimParameters = FLSimulateArgs::makeDefault();

    // - CL Dialog Config
    FLSimulateCommandLine args;
    try {
      do_cldialog(argc, argv, args);
    } catch (FLDialogHelpRequested& e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogOptionsError& e) {
      throw FLConfigUserError {"bad command line input"};
    }

    // Feed input from command line to params
    flSimParameters.logLevel           = args.logLevel;
    flSimParameters.userProfile        = args.userProfile;
    flSimParameters.outputMetadataFile = args.outputMetadataFile;
    flSimParameters.embeddedMetadata   = args.embeddedMetadata;
    flSimParameters.outputFile         = args.outputFile;

    if (flSimParameters.outputMetadataFile.empty()) {
      // Force storage of metadata in the output data file:
      flSimParameters.embeddedMetadata = true;
      // Force a default metadata log file:
      flSimParameters.outputMetadataFile = "flsimulate-metadata.log";
    }

    // If a script was supplied, use that to override params
    if (!args.configScript.empty()) {
      datatools::multi_properties flSimConfig("name", "type");
      std::string configScript = args.configScript;
      datatools::fetch_path_with_env(configScript);
      flSimConfig.read(configScript);
      DT_LOG_DEBUG(flSimParameters.logLevel, "Simulation Configuration:");
      if (datatools::logger::is_debug(flSimParameters.logLevel)) {
        flSimConfig.tree_dump(std::cerr, "", "[debug] ");
      }

      // Now extract and bind values as needed
      // Caution: some parameters are only available for specific user profile

      // Basic system:
      if(flSimConfig.has_key_with_meta("flsimulate", "flsimulate::section")) {
        datatools::properties baseSystem = flSimConfig.get_section("flsimulate");
        // Bind properties in this section to the relevant ones in params:

        // Number of simulated events:
        flSimParameters.numberOfEvents = falaise::properties::getValueOrDefault<int>(baseSystem,
                                                                                     "numberOfEvents",
                                                                                     flSimParameters.numberOfEvents);


        // Printing rate for events:
        flSimParameters.simulationManagerParams.number_of_events_modulo =
          falaise::properties::getValueOrDefault<int>(baseSystem,
                                                      "moduloEvents",
                                                      flSimParameters.simulationManagerParams.number_of_events_modulo);

        // Do simulation:
        flSimParameters.doSimulation = falaise::properties::getValueOrDefault<bool>(baseSystem,
                                                                                    "doSimulation",
                                                                                    flSimParameters.doSimulation);

        // Do digitization:
        flSimParameters.doDigitization = falaise::properties::getValueOrDefault<bool>(baseSystem,
                                                                                      "doDigitization",
                                                                                      flSimParameters.doDigitization);

      }

      // Simulation subsystem:
      if(flSimConfig.has_key_with_meta("flsimulate.simulation", "flsimulate::section")) {
        datatools::properties simSubsystem = flSimConfig.get_section("flsimulate.simulation");
        // Bind properties in this section to the relevant ones in params:

        // Simulation setup URN:
        flSimParameters.simulationSetupUrn
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "simulationSetupUrn",
                                                                flSimParameters.simulationSetupUrn);

        DT_LOG_DEBUG(flSimParameters.logLevel,
                     "flSimParameters.simulationSetupUrn=" << flSimParameters.simulationSetupUrn);

        // Simulation manager main configuration file:
        if (flSimParameters.userProfile == "production" && simSubsystem.has_key("simulationConfig")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "simulationConfig" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.manager_config_filename
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "simulationConfig",
                                                                flSimParameters.simulationManagerParams.manager_config_filename);

        // File for loading internal PRNG's seeds:
        if (flSimParameters.userProfile != "expert" && !simSubsystem.has_key("rngSeedFile")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "must use the '" << "rngSeedFile" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.input_prng_seeds_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "rngSeedFile",
                                                              flSimParameters.simulationManagerParams.input_prng_seeds_file);

        // File for loading internal PRNG's states:
        if (flSimParameters.userProfile != "expert" && simSubsystem.has_key("inputRngStateFile")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "inputRngStateFile" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.input_prng_states_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "inputRngStateFile",
                                                              flSimParameters.simulationManagerParams.input_prng_states_file);

        // File for saving internal PRNG's states:
        flSimParameters.simulationManagerParams.output_prng_states_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "outputRngStateFile",
                                                              flSimParameters.simulationManagerParams.output_prng_states_file);

        // Saving rate for internal PRNG's states:
        flSimParameters.simulationManagerParams.prng_states_save_modulo =
          falaise::properties::getValueOrDefault<int>(simSubsystem,
                                                      "rngStateModuloEvents",
                                                      flSimParameters.simulationManagerParams.prng_states_save_modulo);
      }

      // Digitization subsystem:
      if(flSimConfig.has_key_with_meta("flsimulate.digitization", "flsimulate::section")) {
        datatools::properties digiSubsystem = flSimConfig.get_section("flsimulate.digitization");
        // Bind properties in this section to the relevant ones in params:

      }

      // Variants subsystem:
      if (flSimConfig.has_key_with_meta("flsimulate.variantService", "flsimulate::section")) {
        datatools::properties variantSubsystem = flSimConfig.get_section("flsimulate.variantService");
        // Bind properties to relevant ones on params

        // Variant configuration URN:
        flSimParameters.variantConfigUrn
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "configUrn",
                                                                flSimParameters.variantConfigUrn);

        // Variant configuration:
        if (flSimParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "config" << "' variants configuration parameter!");
        }
        flSimParameters.variantSubsystemParams.config_filename
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "config",
                                                                flSimParameters.variantSubsystemParams.config_filename);

        // Variant profile URN:
        flSimParameters.variantProfileUrn
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "profileUrn",
                                                                flSimParameters.variantProfileUrn);


        // Variant profile:
        flSimParameters.variantSubsystemParams.profile_load
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "profile",
                                                                flSimParameters.variantSubsystemParams.profile_load);

        // Variant settings:
        if (flSimParameters.userProfile != "expert" && variantSubsystem.has_key("settings")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "settings" << "' variants configuration parameter!");
        }
        flSimParameters.variantSubsystemParams.settings
          = falaise::properties::getValueOrDefault<std::vector<std::string> >(variantSubsystem,
                                                                              "settings",
                                                                              flSimParameters.variantSubsystemParams.settings);
      }

      // Services subsystem:
      if (flSimConfig.has_key_with_meta("flsimulate.services", "flsimulate::section")) {
        datatools::properties servicesSubsystem = flSimConfig.get_section("flsimulate.services");

        // Services manager configuration URN:
        flSimParameters.servicesSubsystemConfigUrn =
          falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                              "configUrn",
                                                              flSimParameters.servicesSubsystemConfigUrn);

        // Services manager main configuration file:
        if (flSimParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "config" << "' services configuration parameter!");
        }
        flSimParameters.servicesSubsystemConfig =
          falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                              "config",
                                                              flSimParameters.servicesSubsystemConfig);
      }

    } // !args.configScript.empty()

    do_postprocess(flSimParameters);
    return;
  }


  void do_postprocess(FLSimulateArgs & flSimParameters)
  {
    DT_LOG_TRACE_ENTERING(flSimParameters.logLevel);
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    if (flSimParameters.simulationSetupUrn.empty()) {

      // Check for hardcoded path to the main simulation setup configuration file:
      if (! flSimParameters.simulationManagerParams.manager_config_filename.empty()) {
        // Only for 'expert' of 'normal' user profiles.

        // Variants configuration:
        if (flSimParameters.variantSubsystemParams.config_filename.empty()) {
          DT_LOG_WARNING(flSimParameters.logLevel, "No variants configuration file is provided!");
        }

        // Services configuration:
        if (flSimParameters.servicesSubsystemConfig.empty()) {
          DT_LOG_WARNING(flSimParameters.logLevel, "No services configuration file is provided!");
        }

      } else {
        DT_THROW(std::logic_error, "Missing simulation setup configuration file!");

        //   // Default simulation setup:
        //   if (flSimParameters.simulationManagerParams.manager_config_filename.empty()) {
        //     flSimParameters.simulationSetupUrn = "urn:snemo:demonstrator:simulation:2.1";
        //     DT_LOG_WARNING(flSimParameters.logLevel, "Use default simulation setup '" << flSimParameters.simulationSetupUrn << "'.");
        //   }

      }
    }

    if (!flSimParameters.simulationSetupUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters.simulationSetupUrn, "simsetup"),
                    std::logic_error,
                    "Cannot query simulation setup URN='" << flSimParameters.simulationSetupUrn << "'!");
      }
      const datatools::urn_info & simSetupUrnInfo = dtkUrnQuery.get_urn_info(flSimParameters.simulationSetupUrn);

      if (flSimParameters.experimentalSetupUrn.empty()) {
        // Automatically determine the experimental setup component:
        if (simSetupUrnInfo.has_topic("expsetup") &&
            simSetupUrnInfo.get_components_by_topic("expsetup").size() == 1) {
          flSimParameters.experimentalSetupUrn = simSetupUrnInfo.get_component("expsetup");
        }
      }

      // Simulation:
      {
        // Resolve simulation config file path:
        std::string conf_simu_category = "configuration";
        std::string conf_simu_mime;
        std::string conf_simu_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.simulationSetupUrn,
                                                     conf_simu_category,
                                                     conf_simu_mime,
                                                     conf_simu_path),
                    std::logic_error,
                    "Cannot resolve URN='" << flSimParameters.simulationSetupUrn << "'!");
        flSimParameters.simulationManagerParams.manager_config_filename = conf_simu_path;
      }

      // Variants:
      if (flSimParameters.variantConfigUrn.empty()) {
        // Automatically determine the variants configuration component:
        if (simSetupUrnInfo.has_topic("variants") &&
            simSetupUrnInfo.get_components_by_topic("variants").size() == 1) {
          flSimParameters.variantConfigUrn = simSetupUrnInfo.get_component("variants");
        }
      }
      {
        // Resolve variants file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantConfigUrn,
                                                     conf_variants_category,
                                                     conf_variants_mime,
                                                     conf_variants_path),
                    std::logic_error,
                    "Cannot resolve URN='" << flSimParameters.variantConfigUrn << "'!");
        flSimParameters.variantSubsystemParams.config_filename = conf_variants_path;
      }

      // Services:
      if (!flSimParameters.servicesSubsystemConfig.empty()) {
        // Force the services configuration path:
        DT_THROW_IF(!flSimParameters.servicesSubsystemConfigUrn.empty(),
                    std::logic_error,
                    "Service configuration URN='" << flSimParameters.servicesSubsystemConfigUrn << "' conflicts with services configuration path='"
                    << flSimParameters.servicesSubsystemConfig << "'!");
      } else {
        // Try to set the services setup from a blessed services configuration URN:
        if (flSimParameters.servicesSubsystemConfigUrn.empty()) {
          if (simSetupUrnInfo.has_topic("services") &&
              simSetupUrnInfo.get_components_by_topic("services").size() == 1) {
            // If sthe simulation setup URN implies a "services" component, fetch it!
            flSimParameters.servicesSubsystemConfigUrn = simSetupUrnInfo.get_component("services");
          }
        }
        if (!flSimParameters.servicesSubsystemConfigUrn.empty()) {
          // Resolve services file:
          std::string conf_services_category = "configuration";
          std::string conf_services_mime;
          std::string conf_services_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.servicesSubsystemConfigUrn,
                                                       conf_services_category,
                                                       conf_services_mime,
                                                       conf_services_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flSimParameters.servicesSubsystemConfigUrn << "'!");
          flSimParameters.servicesSubsystemConfig = conf_services_path;
        }
      }
    } // !flSimParameters.simulationSetupUrn.empty()

    // Search for Variants profile:
    if (!flSimParameters.variantSubsystemParams.profile_load.empty()) {
      // Check the variant config path:
      DT_THROW_IF(flSimParameters.variantSubsystemParams.config_filename.empty(),
                  std::logic_error,
                  "Variants configuration file='" << flSimParameters.variantSubsystemParams.config_filename << "' is missing!");
      // Force the variant profile path:
      DT_THROW_IF(!flSimParameters.variantProfileUrn.empty(),
                  std::logic_error,
                  "Variants profile URN='" << flSimParameters.variantProfileUrn << "' conflicts with variants profile path='"
                  << flSimParameters.variantSubsystemParams.profile_load << "'!");
    } else if (!flSimParameters.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantProfileUrn,
                                                   conf_variantsProfile_category,
                                                   conf_variantsProfile_mime,
                                                   conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << flSimParameters.variantProfileUrn << "'!");
      flSimParameters.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }

    if (!flSimParameters.variantSubsystemParams.config_filename.empty()) {
      DT_LOG_WARNING(flSimParameters.logLevel, "No variants configuration is provided.");
      // Additional variants settings may be allowed but *must* be compatible
      // with selected variants config and optional variants profile.
    }

    if (flSimParameters.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flSimParameters.logLevel, "No services configuration is provided.");
    }

    // Print:
    if (datatools::logger::is_debug(flSimParameters.logLevel)) {
      flSimParameters.print(std::cerr);
    }

    DT_LOG_TRACE_EXITING(flSimParameters.logLevel);
    return;
  }

  void FLSimulateArgs::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLSimulate setup parameters: " << std::endl;
    out_ << tag << "logLevel                   = " << datatools::logger::get_priority_label(this->logLevel) << std::endl;
    out_ << tag << "userProfile                = " << userProfile << std::endl;
    out_ << tag << "numberOfEvents             = " << numberOfEvents << std::endl;
    out_ << tag << "doSimulation               = " << std::boolalpha << doSimulation << std::endl;
    out_ << tag << "doDigitization             = " << std::boolalpha << doDigitization << std::endl;
    out_ << tag << "experimentalSetupUrn       = " << experimentalSetupUrn << std::endl;
    out_ << tag << "simulationSetupUrn         = " << simulationSetupUrn << std::endl;
    out_ << tag << "simulationSetupConfig      = " << simulationManagerParams.manager_config_filename << std::endl;
    out_ << tag << "digitizationSetupUrn       = " << digitizationSetupUrn << std::endl;
    out_ << tag << "variantConfigUrn           = " << variantConfigUrn << std::endl;
    out_ << tag << "variantProfileUrn          = " << variantProfileUrn << std::endl;
    out_ << tag << "variantSubsystemParams     = " << variantSubsystemParams.config_filename << std::endl;
    out_ << tag << "servicesSubsystemConfigUrn = " << servicesSubsystemConfigUrn << std::endl;
    out_ << tag << "servicesSubsystemConfig    = " << servicesSubsystemConfig << std::endl;
    out_ << tag << "outputMetadataFile         = " << outputMetadataFile << std::endl;
    out_ << tag << "embeddedMetadata           = " << std::boolalpha << embeddedMetadata << std::endl;
    out_ << last_tag << "outputFile                 = " << outputFile << std::endl;
    return;
  }

} // namespace FLSimulate
