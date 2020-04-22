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
const std::string& FLSimulateArgs::default_file_for_seeds() {
  static const std::string _f("__flsimulate-seeds.log");
  return _f;
}

void do_postprocess(FLSimulateArgs& flSimParameters);

// static
FLSimulateArgs FLSimulateArgs::makeDefault() {
  FLSimulateArgs params;

  // Application specific parameters:
  params.logLevel = datatools::logger::PRIO_ERROR;
  params.userProfile = "normal";
  params.numberOfEvents = 1;
  params.doSimulation = true;
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
  params.rngSeeding = "";

  // Variants support:
  params.variantConfigUrn = "";
  params.variantProfileUrn = "";
  params.variantSubsystemParams.config_filename = "";
  params.saveVariantSettings = true;

  // Service support:
  params.servicesSubsystemConfigUrn = "";
  params.servicesSubsystemConfig = "";

  // I/O control:
  params.outputFile = "";

  return params;
}

void do_configure(int argc, char* argv[], FLSimulateArgs& flSimParameters) {
  // - Default Config
  flSimParameters = FLSimulateArgs::makeDefault();

  // - CL Dialog Config
  FLSimulateCommandLine args;
  try {
    do_cldialog(argc, argv, args);
  } catch (FLDialogHelpRequested& e) {
    throw FLConfigHelpHandled();
  } catch (FLDialogOptionsError& e) {
    throw FLConfigUserError{"bad command line input"};
  }

  // Feed input from command line to params
  flSimParameters.logLevel = args.logLevel;
  flSimParameters.userProfile = args.userProfile;
  flSimParameters.outputFile = args.outputFile;
  flSimParameters.mountPoints = args.mountPoints;

  if (static_cast<unsigned int>(!flSimParameters.mountPoints.empty()) != 0u) {
    // Apply mount points as soon as possible, because manually set file path below
    // may use this mechanism to locate files:
    datatools::kernel& dtk = datatools::kernel::instance();
    datatools::library_info& dtklLibInfo = dtk.grab_library_info_register();
    for (const std::string& mountDirective : flSimParameters.mountPoints) {
      std::string theLibname;
      std::string theTopic;
      std::string thePath;
      std::string errMsg;
      bool parsed = datatools::library_info::parse_path_registration_directive(
          mountDirective, theLibname, theTopic, thePath, errMsg);
      DT_THROW_IF(!parsed, FLConfigUserError,
                  "Cannot parse directory mount directive '" << mountDirective << "' : " << errMsg);
      if (theTopic.empty()) {
        theTopic = datatools::library_info::default_topic_label();
        DT_LOG_DEBUG(flSimParameters.logLevel,
                     "Using default path registration topic: " << theTopic);
      }
      DT_LOG_DEBUG(flSimParameters.logLevel, "Path registration: " << mountDirective);
      DT_LOG_DEBUG(flSimParameters.logLevel, "  Library name : " << theLibname);
      DT_LOG_DEBUG(flSimParameters.logLevel, "  Topic        : " << theTopic);
      DT_LOG_DEBUG(flSimParameters.logLevel, "  Path         : " << thePath);
      try {
        dtklLibInfo.path_registration(theLibname, theTopic, thePath, false);
      } catch (std::exception& error) {
        DT_THROW(FLConfigUserError, "Cannot apply directory mount directive '"
                                        << mountDirective << "': " << error.what());
      }
    }
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
    if (flSimConfig.has_key_with_meta("flsimulate", "flsimulate::section")) {
      falaise::property_set baseSystem{flSimConfig.get_section("flsimulate")};

      // Number of simulated events:
      flSimParameters.numberOfEvents =
          baseSystem.get<int>("numberOfEvents", flSimParameters.numberOfEvents);

      // Printing rate for events:
      flSimParameters.simulationManagerParams.number_of_events_modulo = baseSystem.get<int>(
          "moduloEvents", flSimParameters.simulationManagerParams.number_of_events_modulo);

      // Do simulation:
      flSimParameters.doSimulation =
          baseSystem.get<bool>("doSimulation", flSimParameters.doSimulation);
    }

    // Simulation subsystem:
    if (flSimConfig.has_key_with_meta("flsimulate.simulation", "flsimulate::section")) {
      falaise::property_set simSubsystem{flSimConfig.get_section("flsimulate.simulation")};
      // Bind properties in this section to the relevant ones in params:

      // Simulation setup URN:
      flSimParameters.simulationSetupUrn =
          simSubsystem.get<std::string>("simulationSetupUrn", flSimParameters.simulationSetupUrn);

      DT_LOG_DEBUG(flSimParameters.logLevel,
                   "flSimParameters.simulationSetupUrn=" << flSimParameters.simulationSetupUrn);

      // Simulation manager main configuration file:
      if (flSimParameters.userProfile == "production" &&
          simSubsystem.has_key("simulationSetupConfig")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "does not allow to use the '"
                                                     << "simulationSetupConfig"
                                                     << "' simulation configuration parameter!");
      }
      flSimParameters.simulationManagerParams.manager_config_filename =
          simSubsystem.get<std::string>(
              "simulationSetupConfig",
              flSimParameters.simulationManagerParams.manager_config_filename);

      // File for loading internal PRNG's seeds:
      if (flSimParameters.userProfile == "production" && !simSubsystem.has_key("rngSeedFile")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "must use the '"
                                                     << "rngSeedFile"
                                                     << "' simulation configuration parameter!");
      }
      flSimParameters.simulationManagerParams.input_prng_seeds_file =
          simSubsystem.get<falaise::path>(
              "rngSeedFile", flSimParameters.simulationManagerParams.input_prng_seeds_file);

      if (flSimParameters.simulationManagerParams.input_prng_seeds_file.empty()) {
        if (flSimParameters.userProfile == "production") {
          DT_THROW_IF(simSubsystem.has_key("rngEventGeneratorSeed"), FLConfigUserError,
                      "User profile '" << flSimParameters.userProfile << "' "
                                       << "does not allow to use the '"
                                       << "rngEventGeneratorSeed"
                                       << "' simulation configuration parameter!");
          DT_THROW_IF(simSubsystem.has_key("rngVertexGeneratorSeed"), FLConfigUserError,
                      "User profile '" << flSimParameters.userProfile << "' "
                                       << "does not allow to use the '"
                                       << "rngVertexGeneratorSeed"
                                       << "' simulation configuration parameter!");
          DT_THROW_IF(simSubsystem.has_key("rngHitPostprocessingGeneratorSeed"), FLConfigUserError,
                      "User profile '" << flSimParameters.userProfile << "' "
                                       << "does not allow to use the '"
                                       << "rngHitProcessingGeneratorSeed"
                                       << "' simulation configuration parameter!");
          DT_THROW_IF(simSubsystem.has_key("rngGeant4GeneratorSeed"), FLConfigUserError,
                      "User profile '" << flSimParameters.userProfile << "' "
                                       << "does not allow to use the '"
                                       << "rngGeant4GeneratorSeed"
                                       << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.eg_seed = simSubsystem.get<int>(
            "rngEventGeneratorSeed", flSimParameters.simulationManagerParams.eg_seed);

        flSimParameters.simulationManagerParams.vg_seed = simSubsystem.get<int>(
            "rngVertexGeneratorSeed", flSimParameters.simulationManagerParams.vg_seed);

        flSimParameters.simulationManagerParams.shpf_seed = simSubsystem.get<int>(
            "rngHitProcessingGeneratorSeed", flSimParameters.simulationManagerParams.shpf_seed);

        flSimParameters.simulationManagerParams.mgr_seed = simSubsystem.get<int>(
            "rngGeant4GeneratorSeed", flSimParameters.simulationManagerParams.mgr_seed);
      }

      flSimParameters.simulationManagerParams.output_prng_seeds_file =
          simSubsystem.get<std::string>(
              "rngSeedFileSave", flSimParameters.simulationManagerParams.output_prng_seeds_file);

      // File for loading internal PRNG's states:
      if (flSimParameters.userProfile != "expert" && simSubsystem.has_key("inputRngStateFile")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "does not allow to use the '"
                                                     << "inputRngStateFile"
                                                     << "' simulation configuration parameter!");
      }
      flSimParameters.simulationManagerParams.input_prng_states_file =
          simSubsystem.get<std::string>(
              "inputRngStateFile", flSimParameters.simulationManagerParams.input_prng_states_file);

      // File for saving internal PRNG's states:
      flSimParameters.simulationManagerParams.output_prng_states_file =
          simSubsystem.get<std::string>(
              "outputRngStateFile",
              flSimParameters.simulationManagerParams.output_prng_states_file);

      // Saving rate for internal PRNG's states:
      flSimParameters.simulationManagerParams.prng_states_save_modulo = simSubsystem.get<int>(
          "rngStateModuloEvents", flSimParameters.simulationManagerParams.prng_states_save_modulo);
    }

    // Variants subsystem:
    if (flSimConfig.has_key_with_meta("flsimulate.variantService", "flsimulate::section")) {
      falaise::property_set variantSubsystem{flSimConfig.get_section("flsimulate.variantService")};
      // Bind properties to relevant ones on params

      // Variant configuration URN:
      flSimParameters.variantConfigUrn =
          variantSubsystem.get<std::string>("configUrn", flSimParameters.variantConfigUrn);

      // Variant configuration:
      if (flSimParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "does not allow to use the '"
                                                     << "config"
                                                     << "' variants configuration parameter!");
      }
      flSimParameters.variantSubsystemParams.config_filename = variantSubsystem.get<std::string>(
          "config", flSimParameters.variantSubsystemParams.config_filename);

      // Variant profile URN:
      flSimParameters.variantProfileUrn =
          variantSubsystem.get<std::string>("profileUrn", flSimParameters.variantProfileUrn);

      // Variant profile:
      flSimParameters.variantSubsystemParams.profile_load = variantSubsystem.get<falaise::path>(
          "profile", flSimParameters.variantSubsystemParams.profile_load);

      // Variant settings:
      if (flSimParameters.userProfile == "production" && variantSubsystem.has_key("settings")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "does not allow to use the '"
                                                     << "settings"
                                                     << "' variants configuration parameter!");
      }
      flSimParameters.variantSubsystemParams.settings =
          variantSubsystem.get<std::vector<std::string>>(
              "settings", flSimParameters.variantSubsystemParams.settings);
    }

    // Services subsystem:
    if (flSimConfig.has_key_with_meta("flsimulate.services", "flsimulate::section")) {
      falaise::property_set servicesSubsystem{flSimConfig.get_section("flsimulate.services")};

      // Services manager configuration URN:
      flSimParameters.servicesSubsystemConfigUrn = servicesSubsystem.get<std::string>(
          "configUrn", flSimParameters.servicesSubsystemConfigUrn);

      // Services manager main configuration file:
      if (flSimParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
        DT_THROW(FLConfigUserError, "User profile '" << flSimParameters.userProfile << "' "
                                                     << "does not allow to use the '"
                                                     << "config"
                                                     << "' services configuration parameter!");
      }
      flSimParameters.servicesSubsystemConfig =
          servicesSubsystem.get<std::string>("config", flSimParameters.servicesSubsystemConfig);
    }

  }  // !args.configScript.empty()

  do_postprocess(flSimParameters);
}

void do_postprocess(FLSimulateArgs& flSimParameters) {
  DT_LOG_TRACE_ENTERING(flSimParameters.logLevel);
  datatools::kernel& dtk = datatools::kernel::instance();
  const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

  // Propagate verbosity to variant service:
  flSimParameters.variantSubsystemParams.logging =
      datatools::logger::get_priority_label(flSimParameters.logLevel);

  if (flSimParameters.simulationSetupUrn.empty()) {
    // Check for hardcoded path to the main simulation setup configuration file:
    if (!flSimParameters.simulationManagerParams.manager_config_filename.empty()) {
      // Only for 'expert' of 'normal' user profiles.

      // Variant configuration:
      if (flSimParameters.variantSubsystemParams.config_filename.empty()) {
        DT_LOG_WARNING(flSimParameters.logLevel, "No variant configuration file is provided!");
      }

      // Services configuration:
      if (flSimParameters.servicesSubsystemConfig.empty()) {
        DT_LOG_WARNING(flSimParameters.logLevel, "No services configuration file is provided!");
      }

    } else {
      DT_THROW(std::logic_error, "Missing simulation setup configuration file!");
    }
  }

  datatools::urn_info simSetupUrnInfo;
  datatools::urn_info variantConfigUrnInfo;
  if (!flSimParameters.simulationSetupUrn.empty()) {
    // Check URN registration from the system URN query service:
    {
      DT_THROW_IF(
          !dtkUrnQuery.check_urn_info(flSimParameters.simulationSetupUrn,
                                      falaise::tags::simulation_setup_category()),
          std::logic_error,
          "Cannot query simulation setup URN='" << flSimParameters.simulationSetupUrn << "'!");
    }
    simSetupUrnInfo = dtkUrnQuery.get_urn_info(flSimParameters.simulationSetupUrn);

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
      DT_THROW_IF(
          !dtkUrnQuery.resolve_urn_to_path(flSimParameters.simulationSetupUrn, conf_simu_category,
                                           conf_simu_mime, conf_simu_path),
          std::logic_error, "Cannot resolve URN='" << flSimParameters.simulationSetupUrn << "'!");
      flSimParameters.simulationManagerParams.manager_config_filename = conf_simu_path;
    }

    // Variant setup:
    if (flSimParameters.variantConfigUrn.empty()) {
      // Automatically determine the variants configuration component:
      if (simSetupUrnInfo.has_topic("variants") &&
          simSetupUrnInfo.get_components_by_topic("variants").size() == 1) {
        flSimParameters.variantConfigUrn = simSetupUrnInfo.get_component("variants");
      }
    }
    if (!flSimParameters.variantConfigUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters.variantConfigUrn,
                                                falaise::tags::variant_service_category()),
                    std::logic_error,
                    "Cannot query variant setup URN='" << flSimParameters.variantConfigUrn << "'!");
      }
      variantConfigUrnInfo = dtkUrnQuery.get_urn_info(flSimParameters.variantConfigUrn);

      // Resolve variant configuration file:
      std::string conf_variants_category = "configuration";
      std::string conf_variants_mime;
      std::string conf_variants_path;
      DT_THROW_IF(
          !dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantConfigUrn, conf_variants_category,
                                           conf_variants_mime, conf_variants_path),
          std::logic_error, "Cannot resolve URN='" << flSimParameters.variantConfigUrn << "'!");
      flSimParameters.variantSubsystemParams.config_filename = conf_variants_path;
    }

    // Services:
    if (!flSimParameters.servicesSubsystemConfig.empty()) {
      // Force the services configuration path:
      DT_THROW_IF(!flSimParameters.servicesSubsystemConfigUrn.empty(), std::logic_error,
                  "Service configuration URN='" << flSimParameters.servicesSubsystemConfigUrn
                                                << "' conflicts with services configuration path='"
                                                << flSimParameters.servicesSubsystemConfig << "'!");
    } else {
      // Try to set the services setup from a blessed services configuration URN:
      if (flSimParameters.servicesSubsystemConfigUrn.empty()) {
        if (simSetupUrnInfo.has_topic("services") &&
            simSetupUrnInfo.get_components_by_topic("services").size() == 1) {
          // If the simulation setup URN implies a "services" component, fetch it!
          flSimParameters.servicesSubsystemConfigUrn = simSetupUrnInfo.get_component("services");
        }
      }
      if (!flSimParameters.servicesSubsystemConfigUrn.empty()) {
        // Resolve services file:
        std::string conf_services_category = "configuration";
        std::string conf_services_mime;
        std::string conf_services_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.servicesSubsystemConfigUrn,
                                                     conf_services_category, conf_services_mime,
                                                     conf_services_path),
                    std::logic_error,
                    "Cannot resolve URN='" << flSimParameters.servicesSubsystemConfigUrn << "'!");
        flSimParameters.servicesSubsystemConfig = conf_services_path;
      }
    }
  }  // !flSimParameters.simulationSetupUrn.empty()

  if (!flSimParameters.variantSubsystemParams.profile_load.empty()) {
    // Check the variant config path:
    DT_THROW_IF(flSimParameters.variantSubsystemParams.config_filename.empty(), std::logic_error,
                "Variant configuration file='"
                    << flSimParameters.variantSubsystemParams.config_filename << "' is missing!");
    // Force the variant profile path:
    DT_THROW_IF(!flSimParameters.variantProfileUrn.empty(), std::logic_error,
                "Variant profile URN='" << flSimParameters.variantProfileUrn
                                        << "' conflicts with variant profile path='"
                                        << flSimParameters.variantSubsystemParams.profile_load
                                        << "'!");
  } else {
    DT_LOG_DEBUG(flSimParameters.logLevel, "No variant profile is set.");
    if (flSimParameters.variantProfileUrn.empty()) {
      DT_LOG_DEBUG(flSimParameters.logLevel, "No variant profile URN is set.");
      // No variant profile URN is set:
      if (simSetupUrnInfo.is_valid()) {
        DT_LOG_DEBUG(flSimParameters.logLevel,
                     "Trying to find a default one from the current simulation setup...");
        // Try to find a default one from the current variant setup:
        if (simSetupUrnInfo.has_topic("defvarprofile") &&
            simSetupUrnInfo.get_components_by_topic("defvarprofile").size() == 1) {
          // If the simulation setup URN implies a "services" component, fetch it!
          flSimParameters.variantProfileUrn = simSetupUrnInfo.get_component("defvarprofile");
          DT_LOG_DEBUG(flSimParameters.logLevel, "Using the default variant profile '"
                                                     << flSimParameters.variantProfileUrn << "'"
                                                     << " associated to simulation setup '"
                                                     << simSetupUrnInfo.get_urn() << "'.");
        }
      }
    }
    if (!flSimParameters.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(
          !dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantProfileUrn,
                                           conf_variantsProfile_category, conf_variantsProfile_mime,
                                           conf_variantsProfile_path),
          std::logic_error,
          "Cannot resolve variant profile URN='" << flSimParameters.variantProfileUrn << "'!");
      flSimParameters.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }
  }

  if (flSimParameters.variantSubsystemParams.config_filename.empty()) {
    DT_LOG_WARNING(flSimParameters.logLevel, "No variant configuration is provided.");
  } else {
    if (flSimParameters.variantSubsystemParams.profile_load.empty()) {
      DT_LOG_WARNING(flSimParameters.logLevel, "No variant profile is provided.");
    } else {
      // Additional variants settings may be allowed but *must* be compatible
      // with selected variants config and optional variants profile.
    }
  }

  if (flSimParameters.servicesSubsystemConfig.empty()) {
    DT_LOG_WARNING(flSimParameters.logLevel, "No services configuration is provided.");
  }

  // Print:
  if (datatools::logger::is_debug(flSimParameters.logLevel)) {
    flSimParameters.print(std::cerr);
  }
}

void FLSimulateArgs::print(std::ostream& out_) const {
  static const std::string tag("|-- ");
  static const std::string last_tag("`-- ");
  out_ << "FLSimulate setup parameters: " << std::endl;
  out_ << tag
       << "logLevel                   = " << datatools::logger::get_priority_label(this->logLevel)
       << std::endl;
  out_ << tag << "userProfile                = " << userProfile << std::endl;
  out_ << tag << "numberOfEvents             = " << numberOfEvents << std::endl;
  out_ << tag << "doSimulation               = " << std::boolalpha << doSimulation << std::endl;
  out_ << tag << "experimentalSetupUrn       = " << experimentalSetupUrn << std::endl;
  out_ << tag << "simulationSetupUrn         = " << simulationSetupUrn << std::endl;
  out_ << tag << "simulationSetupConfig      = " << simulationManagerParams.manager_config_filename
       << std::endl;
  out_ << tag << "rngSeeding                 = " << rngSeeding << std::endl;
  out_ << tag << "variantConfigUrn           = " << variantConfigUrn << std::endl;
  out_ << tag << "variantProfileUrn          = " << variantProfileUrn << std::endl;
  out_ << tag << "variantSubsystemParams     = " << variantSubsystemParams.config_filename
       << std::endl;
  out_ << tag << "saveVariantSettings        = " << std::boolalpha << saveVariantSettings
       << std::endl;
  out_ << tag << "servicesSubsystemConfigUrn = " << servicesSubsystemConfigUrn << std::endl;
  out_ << tag << "servicesSubsystemConfig    = " << servicesSubsystemConfig << std::endl;
  out_ << last_tag << "outputFile                 = " << outputFile << std::endl;
}

}  // namespace FLSimulate
