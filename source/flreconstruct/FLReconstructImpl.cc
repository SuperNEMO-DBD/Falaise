// Ourselves
#include "FLReconstructImpl.h"

// Standard Library
#include <string>
#include <vector>
#include <memory>

// Third Party
// - Boost
#include "boost/date_time/posix_time/posix_time.hpp"
// - Bayeux
#include "bayeux/bayeux.h"
#include "bayeux/version.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/factory_macros.h"
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/input_module.h"

// This Project
#include "falaise/version.h"
#include "falaise/resource.h"
#include "falaise/property_reader.h"
#include "falaise/exitcodes.h"
#include "FLReconstructParams.h"
#include "FLReconstructCommandLine.h"
#include "FLReconstructUtils.h"

namespace FLReconstruct {

  metadata_collector::metadata_collector(const uint32_t /*flags*/) {
  }

  //! Extract metadata from input file
  datatools::multi_properties
  metadata_collector::do_get_metadata_from_data(const std::string & file) const
  {
    // Metadata is currently available as soon as the input module is initialized.
    std::unique_ptr<dpp::input_module> recInput(new dpp::input_module);
    recInput->set_single_input_file(file);
    // Input metadata management:
    const datatools::multi_properties& iMetadataStore = recInput->get_metadata_store();
    recInput->initialize_simple();
    datatools::multi_properties md;
    md.clear_key_label();
    md.clear_meta_label();
    md = iMetadataStore;
    md.set_meta_label("type");
    return md;
  }

  //! Extract metadata from input metadata file
  datatools::multi_properties
  metadata_collector::do_get_metadata_from_file(const std::string & file) const
  {
    std::string mdfile = file;
    datatools::fetch_path_with_env(mdfile);
    datatools::multi_properties md;
    md.clear_key_label();
    md.clear_meta_label();
    md.read(mdfile);
    return md;
  }

  ///! Parse command line arguments to configure the reconstruction parameters
  void do_configure(int argc, char *argv[], FLReconstructParams& flRecParameters)
  {
    // - Default Config
    flRecParameters = FLReconstructParams::makeDefault();

    // - CL Dialog Config
    FLReconstructCommandLine clArgs;
    FLDialogState clDialogRet = DIALOG_OK;
    try {
      clDialogRet = do_cldialog(argc, argv, clArgs);
      //DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "Command line dialog end with exit code=" << clDialogRet);

      if (clDialogRet == DIALOG_ERROR) {
        //DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "Detected a dialog error!");
        throw FLConfigUserError {"bad command line input"};
      } else if (clDialogRet == DIALOG_QUERY) {
        //DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "Detected a dialog query!");
        throw FLConfigHelpHandled();
      }
    } catch (FLConfigHelpHandled& e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogHelpRequested& e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogOptionsError& e) {
      throw FLConfigUserError {"bad command line input"};
    } catch (std::exception & e) {
      throw FLConfigUserError {"bad command line input"};
    }
    DT_LOG_DEBUG(clArgs.logLevel, "Configuring...");

    // Import parameters from the command line:
    flRecParameters.logLevel           = clArgs.logLevel;
    flRecParameters.moduloEvents       = clArgs.moduloEvents;
    flRecParameters.userProfile        = clArgs.userProfile;
    flRecParameters.inputMetadataFile  = clArgs.inputMetadataFile;
    flRecParameters.inputFile          = clArgs.inputFile;
    flRecParameters.outputMetadataFile = clArgs.outputMetadataFile;
    flRecParameters.embeddedMetadata   = clArgs.embeddedMetadata;
    flRecParameters.outputFile         = clArgs.outputFile;

    if (flRecParameters.userProfile.empty()) {
      // Force a default user profile:
      flRecParameters.userProfile = "normal";
    }
    if (flRecParameters.outputMetadataFile.empty()) {
      // Force a default metadata log file:
      flRecParameters.outputMetadataFile = "flreconstruct-metadata.log";
      // Force storage of metadata in the output data file:
      flRecParameters.embeddedMetadata = true;
    }

    // Parse the FLReconstruct pipeline script:
    datatools::multi_properties flRecConfig("name", "type");
    if (!clArgs.pipelineScript.empty()) {
      std::string pipelineScript = clArgs.pipelineScript;
      datatools::fetch_path_with_env(pipelineScript);
      flRecConfig.read(pipelineScript);
    }

    // Fetch basic configuration from the script:
    if (flRecConfig.has_key_with_meta("flreconstruct", "flreconstruct::section")) {

      // Reconstruction basic subsystem:
      datatools::properties basicSystem = flRecConfig.get_section("flreconstruct");
      flRecConfig.remove("flreconstruct");

      // Fetch the experimental setup URN:
      flRecParameters.experimentalSetupUrn
        = falaise::properties::getValueOrDefault<std::string>(basicSystem,
                                                              "experimentalSetupUrn",
                                                              flRecParameters.experimentalSetupUrn);

      // Number of events to be processed:
      flRecParameters.numberOfEvents =
        falaise::properties::getValueOrDefault<int>(basicSystem,
                                                    "numberOfEvents",
                                                    flRecParameters.numberOfEvents);

      // Printing rate for events:
      flRecParameters.moduloEvents =
        falaise::properties::getValueOrDefault<int>(basicSystem,
                                                    "moduloEvents",
                                                    flRecParameters.moduloEvents);

      // // Unused for now:
      // flRecParameters.dataType
      //   = falaise::properties::getValueOrDefault<std::string>(basicSystem,
      //                                                         "dataType",
      //                                                         flRecParameters.dataType);
      // flRecParameters.dataSubtype
      //   = falaise::properties::getValueOrDefault<std::string>(basicSystem,
      //                                                         "dataSubtype",
      //                                                         flRecParameters.dataSubtype);
      // flRecParameters.requiredInputBanks
      //   = falaise::properties::getValueOrDefault<std::vector<std::string> >(basicSystem,
      //                                                                       "requiredInputBanks",
      //                                                                       flRecParameters.requiredInputBanks);
      // flRecParameters.expectedOutputBanks
      //   = falaise::properties::getValueOrDefault<std::vector<std::string> >(basicSystem,
      //                                                                       "expectedOutputBanks",
      //                                                                       flRecParameters.expectedOutputBanks);
    }

    // Fetch variant service configuration:
    if (flRecConfig.has_key_with_meta("flreconstruct.variantService", "flreconstruct::section")) {

      datatools::properties variantSubsystem = flRecConfig.get_section("flreconstruct.variantService");
      flRecConfig.remove("flreconstruct.variantService");

      // Variant configuration URN:
      flRecParameters.variantConfigUrn
        = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                              "configUrn",
                                                              flRecParameters.variantConfigUrn);

      // Variant configuration:
      if (flRecParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
        DT_THROW(FLConfigUserError,
                 "User profile '" << flRecParameters.userProfile << "' "
                 << "does not allow to use the '" << "config" << "' variants configuration parameter!");
      }
      flRecParameters.variantSubsystemParams.config_filename
        = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                              "config",
                                                              flRecParameters.variantSubsystemParams.config_filename);

      // Variant profile URN:
      flRecParameters.variantProfileUrn
        = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                              "profileUrn",
                                                              flRecParameters.variantProfileUrn);


      // Variant profile:
      flRecParameters.variantSubsystemParams.profile_load
        = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                              "profile",
                                                              flRecParameters.variantSubsystemParams.profile_load);

      // Variant settings:
      if (flRecParameters.userProfile != "expert" && variantSubsystem.has_key("settings")) {
        DT_THROW(FLConfigUserError,
                 "User profile '" << flRecParameters.userProfile << "' "
                 << "does not allow to use the '" << "settings" << "' variants configuration parameter!");
      }
      flRecParameters.variantSubsystemParams.settings
        = falaise::properties::getValueOrDefault<std::vector<std::string> >(variantSubsystem,
                                                                            "settings",
                                                                            flRecParameters.variantSubsystemParams.settings);
    }

    // Fetch plugins configuration:
    if (flRecConfig.has_key_with_meta("flreconstruct.plugins", "flreconstruct::section")) {
      try {
        datatools::properties userFLPlugins = flRecConfig.get_section("flreconstruct.plugins");
        flRecConfig.remove("flreconstruct.plugins");
        std::vector<std::string> pList;
        userFLPlugins.fetch("plugins", pList);
        for(std::string plugin_name : pList) {
          static const std::string no_explicit_plugin_file = "";
          datatools::properties& pSection
            = flRecParameters.userLibConfig.add_section(plugin_name, no_explicit_plugin_file);
          userFLPlugins.export_and_rename_starting_with(pSection, plugin_name + ".", "");
          pSection.store_flag("autoload");
          // - Default, search plugin DLL in the falaise plugin install directory:
          if (!pSection.has_key("directory")) {
            pSection.store_string("directory", "@falaise.plugins:");
          }
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

      datatools::properties servicesSubsystem = flRecConfig.get_section("flreconstruct.services");
      flRecConfig.remove("flreconstruct.services");

      // Services manager configuration URN:
      flRecParameters.servicesSubsystemConfigUrn =
        falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                            "configUrn",
                                                            flRecParameters.servicesSubsystemConfigUrn);

      // Services manager main configuration file:
      if (flRecParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
        // User profile 'production' must used official registered services configuration:
        DT_THROW(FLConfigUserError,
                 "User profile '" << flRecParameters.userProfile << "' "
                 << "does not allow to use the '" << "config" << "' services configuration parameter!");
      }
      flRecParameters.servicesSubsystemConfig =
        falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                            "config",
                                                            flRecParameters.servicesSubsystemConfig);
    }

    // Fetch pipeline configuration:
    if (flRecConfig.has_key_with_meta("flreconstruct.pipeline", "flreconstruct::section")) {

      datatools::properties pipelineSubsystem = flRecConfig.get_section("flreconstruct.pipeline");
      flRecConfig.remove("flreconstruct.pipeline");
      if (datatools::logger::is_debug(flRecParameters.logLevel)) {
        pipelineSubsystem.tree_dump(std::cerr, "Pipeline subsystem: ", "[debug] ");
      }

      flRecParameters.reconstructionPipelineUrn =
        falaise::properties::getValueOrDefault<std::string>(pipelineSubsystem,
                                                            "configUrn",
                                                            flRecParameters.reconstructionPipelineUrn);

      flRecParameters.reconstructionPipelineConfig =
        falaise::properties::getValueOrDefault<std::string>(pipelineSubsystem,
                                                            "config",
                                                            flRecParameters.reconstructionPipelineConfig);

      flRecParameters.reconstructionPipelineModule =
        falaise::properties::getValueOrDefault<std::string>(pipelineSubsystem,
                                                            "module",
                                                            flRecParameters.reconstructionPipelineModule);

    }


    {
      // In principle all "flreconstruct::section"(s) of interest should have
      // been processed now.
      // Clean the flRecConfig from unused sections of type "flreconstruct::section":
      std::vector<std::string> section_keys = flRecConfig.keys();
      std::vector<std::string> unused_section_keys;
      for (std::size_t i = 0; i < section_keys.size(); i++) {
        if (flRecConfig.has_key_with_meta(section_keys[i], "flreconstruct::section")) {
          DT_LOG_ERROR(flRecParameters.logLevel, "Found an unused flreconstruct section named '"
                       << section_keys[i] << "'! We will discard it!");
          unused_section_keys.push_back(section_keys[i]);
        }
      }
      for (std::size_t i = 0; i < unused_section_keys.size(); i++) {
        flRecConfig.remove(unused_section_keys[i]);
        DT_LOG_ERROR(flRecParameters.logLevel, "Unused flreconstruct section named '"
                     << section_keys[i] << "' has been removed.");
       }
    }

    // From this point, no other section than the processing pipeline modules definitions
    // should be present in the script. So we fetch modules configuration:
    flRecParameters.modulesConfig = flRecConfig;

    do_postprocess(flRecParameters);
    return;
  }

  void do_postprocess_input_metadata(FLReconstructParams & flRecParameters)
  {
    DT_LOG_TRACE_ENTERING(flRecParameters.logLevel);
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    // Collect input metadata from input files:
    // we first try from some input metadata companion file, if provided,
    // then from the input data file itself, in the hope it contains metadata records.
    uint32_t mcflags = 0;
    metadata_collector metadataCol(mcflags);
    if (!flRecParameters.inputMetadataFile.empty()) {
      // Fetch the metadata from the companion input metadata file, if any:
      DT_LOG_DEBUG(flRecParameters.logLevel, "Fetching input metadata from input metadata companion file...");
      flRecParameters.inputMetadata = metadataCol.do_get_metadata_from_file(flRecParameters.inputMetadataFile);
    } else if (!flRecParameters.inputFile.empty()) {
      // Fetch the metadata from the input data file:
      DT_LOG_DEBUG(flRecParameters.logLevel, "Fetching input metadata from input data file...");
      flRecParameters.inputMetadata = metadataCol.do_get_metadata_from_data(flRecParameters.inputFile);
    } else {
      // No metadata is available. Do nothing.
      // Probably we should handle this path in the future.
    }
    if (datatools::logger::is_debug(flRecParameters.logLevel)) {
      flRecParameters.inputMetadata.tree_dump(std::cerr, "Input metadata: ", "[debug] ");
    }

    // Input metadata of interest:
    std::string inputDataExperimentalSetupUrn = ""; // the experimental setup identifier used to produce input data
    std::string inputDataUserProfile = "";          // the user profile used to produce input data
    // std::size_t inputDataNumberOfEvents = 0;        // the number of event after input data
    // Fetch input metadata:
    if (!flRecParameters.inputMetadata.empty()) {
      // Try to extract informations from the metadata:
      DT_LOG_DEBUG(flRecParameters.logLevel, "Found input metadata");

      if (flRecParameters.inputMetadata.has_key_with_meta("flsimulate", "flsimulate::section")) {
        // Input metadata from FLSimulate
        DT_LOG_DEBUG(flRecParameters.logLevel, "Input metadata were generated by FLSimulate");

        bool inputDataDoSimulation = false;
        bool inputDataDoDigitization = false;

        {
          // System section:
          const datatools::properties & inputDataSystemSection
            = flRecParameters.inputMetadata.get_section("flsimulate");
          if (datatools::logger::is_debug(flRecParameters.logLevel)) {
            inputDataSystemSection.tree_dump(std::cerr, "Input metadata flsimulate basic system: ", "[debug] ");
          }

          // Check simulation flag associated to input data:
          if (inputDataSystemSection.has_key("doSimulation")) {
            inputDataDoSimulation = inputDataSystemSection.fetch_boolean("doSimulation");
          }

          // Check digitization flag associated to input data:
          if (inputDataSystemSection.has_key("doDigitization")) {
            inputDataDoDigitization = inputDataSystemSection.fetch_boolean("doDigitization");
          }

          // Check user profile associated to input data:
          if (inputDataSystemSection.has_key("userProfile")) {
            inputDataUserProfile = inputDataSystemSection.fetch_string("userProfile");
          }

          // First try to fetch the experimental setup identifier/URN:
          if (inputDataSystemSection.has_key("experimentalSetupUrn")) {
            inputDataExperimentalSetupUrn = inputDataSystemSection.fetch_string("experimentalSetupUrn");
          }

          // // Check user profile associated to input data:
          // if (inputDataSystemSection.has_key("numberOfEvents")) {
          //   inputDataNumberOfEvents = inputDataSystemSection.fetch_integer("numberOfEvents");
          // }
        } // System section

        // Simulation section:
        if (inputDataDoSimulation &&
            flRecParameters.inputMetadata.has_key_with_meta("SimulationSubsystem", "flsimulate::section")) {
          const datatools::properties & inputDataSimuSubsystemSection
            = flRecParameters.inputMetadata.get_section("SimulationSubsystem");
          if (datatools::logger::is_debug(flRecParameters.logLevel)) {
            inputDataSimuSubsystemSection.tree_dump(std::cerr, "Input metadata simulation subsystem: ", "[debug] ");
          }

          // If no experimental setup identifier/URN is set:
          if (inputDataExperimentalSetupUrn.empty()) {
            // Then try to fetch the simulation setup identifier/URN:
            if (inputDataSimuSubsystemSection.has_key("simulationSetupUrn")) {
              std::string inputDataSimuSetupUrn = inputDataSimuSubsystemSection.fetch_string("simulationSetupUrn");
              {
                std::string conf_category = "simsetup";
                DT_THROW_IF(!dtkUrnQuery.check_urn_info(inputDataSimuSetupUrn, conf_category),
                            std::logic_error,
                            "Cannot query URN='" << inputDataSimuSetupUrn << "'!");
              }
              // and extract the associated 'experimentalSetupUrn':
              const datatools::urn_info & simuSetupUrnInfo = dtkUrnQuery.get_urn_info(inputDataSimuSetupUrn);
              if (simuSetupUrnInfo.has_topic("expsetup") &&
                  simuSetupUrnInfo.get_components_by_topic("expsetup").size() == 1) {
                inputDataExperimentalSetupUrn = simuSetupUrnInfo.get_component("expsetup");
              }
            }
          }
        } // Simulation section

        // Digitization section:
        if (inputDataDoDigitization &&
            flRecParameters.inputMetadata.has_key_with_meta("DigitizationSubsystem", "flsimulate::section")) {
          const datatools::properties & inputDataDigiSubsystemSection
            = flRecParameters.inputMetadata.get_section("DigitizationSubsystem");
          if (datatools::logger::is_debug(flRecParameters.logLevel)) {
            inputDataDigiSubsystemSection.tree_dump(std::cerr, "Input metadata digitization subsystem: ", "[debug] ");
          }

          // Not implemented yet.

          // // If no experimental setup identifier/URN is set:
          // if (inputDataExperimentalSetupUrn.empty()) {
          //   // Then try to fetch the simulation setup identifier/URN:
          //   if (inputDataDigiSubsystemSection.has_key("digitizationSetupUrn")) {
          //     std::string inputDataDigiSetupUrn = inputDataDigiSubsystemSection.fetch_string("digitizationSetupUrn");
          //     {
          //       std::string conf_category = "setup";
          //       DT_THROW_IF(!dtkUrnQuery.check_urn_info(inputDataDigiSetupUrn, conf_category),
          //                   std::logic_error,
          //                   "Cannot query URN='" << inputDataDigiSetupUrn << "'!");
          //     }
          //     // and extract the associated 'experimentalSetupUrn':
          //     const datatools::urn_info & digiSetupUrnInfo = dtkUrnQuery.get_urn_info(inputDataDigiSetupUrn);
          //     if (digiSetupUrnInfo.has_topic("expsetup") &&
          //         digiSetupUrnInfo.get_components_by_topic("expsetup").size() == 1) {
          //       inputDataExperimentalSetupUrn = digiSetupUrnInfo.get_component("expsetup");
          //     }
          //   }
          // }

        } // Digitization section

      } // Input metadata from FLSimulate

      // Try to extract informations from the metadata:
      if (flRecParameters.inputMetadata.has_key_with_meta("flreconstruct", "flreconstruct::section")) {
        // Input metadata from FLReconstruct
        DT_LOG_DEBUG(flRecParameters.logLevel, "Input metadata were generated by FLReconstruct");
        {
          // System section:
          const datatools::properties & inputDataSystemSection
            = flRecParameters.inputMetadata.get_section("flreconstruct");
          if (datatools::logger::is_debug(flRecParameters.logLevel)) {
            inputDataSystemSection.tree_dump(std::cerr, "Input metadata flreconstruct basic system: ", "[debug] ");
          }

          // Check user profile associated to input data:
          if (inputDataSystemSection.has_key("userProfile")) {
            inputDataUserProfile = inputDataSystemSection.fetch_string("userProfile");
          }

          // First try to fetch the experimental setup identifier/URN:
          if (inputDataSystemSection.has_key("experimentalSetupUrn")) {
            inputDataExperimentalSetupUrn = inputDataSystemSection.fetch_string("experimentalSetupUrn");
          }

          // // Check user profile associated to input data:
          // if (inputDataSystemSection.has_key("numberOfEvents")) {
          //   inputDataNumberOfEvents = inputDataSystemSection.fetch_integer("numberOfEvents");
          // }

        } // System section

      } // Input metadata from FLReconstruct

      if (!inputDataExperimentalSetupUrn.empty()) {
        DT_LOG_NOTICE(flRecParameters.logLevel, "Input metadata from the experimental setup identifier (URN) is '" << inputDataExperimentalSetupUrn << "'.");
      } else {
        DT_LOG_NOTICE(flRecParameters.logLevel, "No experimental setup identifier (URN) is set from input metadata.");
      }

      {
        // Checks:

        // Check the user profile:
        if (flRecParameters.userProfile == "production") {
          DT_THROW_IF(inputDataUserProfile != "production", FLConfigUserError,
                      "User profile '" << flRecParameters.userProfile << "' "
                      << "is not compatible with input metadata production user profile '" << inputDataUserProfile << "'!");
        } else if (flRecParameters.userProfile == "normal") {
          DT_THROW_IF(inputDataUserProfile == "expert", FLConfigUserError,
                      "User profile '" << flRecParameters.userProfile << "' "
                      << "is not compatible with input metadata production user profile '" << inputDataUserProfile << "'!");
        }

        // Check the experimental setup identifier:
        if (! flRecParameters.experimentalSetupUrn.empty()) {
          DT_THROW_IF(!inputDataExperimentalSetupUrn.empty() &&
                      (inputDataExperimentalSetupUrn != flRecParameters.experimentalSetupUrn),
                      std::logic_error,
                      "Experimental setup URN='" << flRecParameters.experimentalSetupUrn << "' conflicts with experimental setup URN='"
                      << inputDataExperimentalSetupUrn << "' from input metadata!");
        }

      } // End of checks.

      {
        // Settings:

        if (flRecParameters.experimentalSetupUrn.empty()) {
          // Force the experimental setup identifier from the value found int the input metadata
          flRecParameters.experimentalSetupUrn = inputDataExperimentalSetupUrn;
          DT_LOG_NOTICE(flRecParameters.logLevel, "The experimental setup identifier (URN) is set to '" << inputDataExperimentalSetupUrn << "' from input metadata.");
        }

        // End of settings.
      }

    } // End of using input metadata

    DT_LOG_TRACE_EXITING(flRecParameters.logLevel);
    return;
  }

  void do_postprocess(FLReconstructParams & flRecParameters)
  {
    DT_LOG_TRACE_ENTERING(flRecParameters.logLevel);
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    // Process input metadata:
    do_postprocess_input_metadata(flRecParameters);

    if (!flRecParameters.reconstructionPipelineUrn.empty()) {
      // Check URN registration from the system URN query service:
      DT_THROW_IF(!dtkUrnQuery.check_urn_info(flRecParameters.reconstructionPipelineUrn, "recsetup"),
                  std::logic_error,
                  "Cannot query reconstruction setup URN='" << flRecParameters.reconstructionPipelineUrn << "'!");


      // Resolve reconstruction config file path:
      std::string conf_rec_category = "configuration";
      std::string conf_rec_mime;
      std::string conf_rec_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flRecParameters.reconstructionPipelineUrn,
                                                   conf_rec_category,
                                                   conf_rec_mime,
                                                   conf_rec_path),
                  std::logic_error,
                  "Cannot resolve URN='" << flRecParameters.reconstructionPipelineUrn << "'!");
      flRecParameters.reconstructionPipelineConfig = conf_rec_path;
    }

    if (!flRecParameters.reconstructionPipelineConfig.empty()) {
      if (!flRecParameters.modulesConfig.empty()) {
        DT_THROW(std::logic_error,
                 "Pipeline module configuration file '" << flRecParameters.reconstructionPipelineConfig << "' conflicts with pipeline inline configuration provided by the script!");

      }
      std::string pipeline_config_filename = flRecParameters.reconstructionPipelineConfig;
      datatools::fetch_path_with_env(pipeline_config_filename);
      flRecParameters.modulesConfig.read(pipeline_config_filename);
      if (datatools::logger::is_debug(flRecParameters.logLevel)) {
        flRecParameters.modulesConfig.tree_dump(std::cerr, "Pipeline configuration: ", "[debug] ");
      }
    } else {
      DT_LOG_NOTICE(flRecParameters.logLevel, "No reconstruction pipeline configuration (URN/path) is set.");
    }

    if (flRecParameters.experimentalSetupUrn.empty()) {
      // If experimental setup URN is not set..
      DT_LOG_NOTICE(flRecParameters.logLevel, "No experimental setup identifier (URN) is set.");

      // Variants service configuration can be hardcoded:
      if (!flRecParameters.variantSubsystemParams.config_filename.empty()) {
        DT_LOG_NOTICE(flRecParameters.logLevel, "Using a manually set variant service configuration file.");
      }

      // Services configuration must be hardcoded (typically with at least the geometry service):
      DT_THROW_IF(flRecParameters.servicesSubsystemConfig.empty(),
                  std::logic_error,
                  "Missing services configuration file!");
    }

    // The experimental setup URN is set, we try to extract automatically the path of the
    // components associated to it (variants, services...)
    if (! flRecParameters.experimentalSetupUrn.empty()) {
      // Check URN registration from the system URN query service:
      {
        std::string conf_category = "expsetup";
        DT_THROW_IF(!dtkUrnQuery.check_urn_info(flRecParameters.experimentalSetupUrn, conf_category),
                    std::logic_error,
                    "Cannot query URN='" << flRecParameters.experimentalSetupUrn << "'!");
      }
      const datatools::urn_info & expSetupUrnInfo =
        dtkUrnQuery.get_urn_info(flRecParameters.experimentalSetupUrn);

      // Variants:
      // Automatically determine the variants configuration component:
      std::string variantConfigUrn;
      if (expSetupUrnInfo.has_topic("variants") &&
          expSetupUrnInfo.get_components_by_topic("variants").size() == 1) {
        variantConfigUrn = expSetupUrnInfo.get_component("variants");
      }
      if (!flRecParameters.variantConfigUrn.empty()) {
        DT_THROW_IF(flRecParameters.variantConfigUrn != variantConfigUrn,
                    std::logic_error,
                    "Variant config URN='" << flRecParameters.variantConfigUrn << "' does not match "
                    << "the automatically resolved URN='" << variantConfigUrn << "' from experimental setup '"
                    << flRecParameters.experimentalSetupUrn << "'!");
      } else {
        flRecParameters.variantConfigUrn = variantConfigUrn;
      }
      if (! flRecParameters.variantConfigUrn.empty()) {
        // Resolve variants file:
        std::string conf_variants_category = "configuration";
        std::string conf_variants_mime;
        std::string conf_variants_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flRecParameters.variantConfigUrn,
                                                     conf_variants_category,
                                                     conf_variants_mime,
                                                     conf_variants_path),
                    std::logic_error,
                    "Cannot resolve URN='" << flRecParameters.variantConfigUrn << "'!");
        flRecParameters.variantSubsystemParams.config_filename = conf_variants_path;
      }

      // Services:
      if (!flRecParameters.servicesSubsystemConfig.empty()) {
        // Force the services config path:
        DT_THROW_IF(!flRecParameters.servicesSubsystemConfigUrn.empty(),
                    std::logic_error,
                    "Service configuration URN='" << flRecParameters.servicesSubsystemConfigUrn << "' "
                    << "conflicts with services configuration path='"
                    << flRecParameters.servicesSubsystemConfig << "'!");
      } else {
        // Try to set the services setup from a blessed services configuration URN:
        std::string servicesSubsystemConfigUrn;
        if (flRecParameters.servicesSubsystemConfigUrn.empty()) {
          if (expSetupUrnInfo.has_topic("services") &&
              expSetupUrnInfo.get_components_by_topic("services").size() == 1) {
            // If the experimental setup URN implies a "services" component, fetch it!
            servicesSubsystemConfigUrn = expSetupUrnInfo.get_component("services");
          }
        }
        if (!flRecParameters.servicesSubsystemConfigUrn.empty()) {
          DT_THROW_IF(flRecParameters.servicesSubsystemConfigUrn != servicesSubsystemConfigUrn,
                      std::logic_error,
                      "Services config URN='" << flRecParameters.servicesSubsystemConfigUrn << "' does not match "
                      << "the automatically resolved URN='" << servicesSubsystemConfigUrn << "' from experimental setup '"
                      << flRecParameters.experimentalSetupUrn << "'!");
        } else {
          flRecParameters.servicesSubsystemConfigUrn = servicesSubsystemConfigUrn;
        }
        if (!flRecParameters.servicesSubsystemConfigUrn.empty()) {
          // Resolve services file:
          std::string conf_services_category = "configuration";
          std::string conf_services_mime;
          std::string conf_services_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flRecParameters.servicesSubsystemConfigUrn,
                                                       conf_services_category,
                                                       conf_services_mime,
                                                       conf_services_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flRecParameters.servicesSubsystemConfigUrn << "'!");
          flRecParameters.servicesSubsystemConfig = conf_services_path;
        }
      }
    }

    // Variants profile:
    if (!flRecParameters.variantSubsystemParams.profile_load.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(!flRecParameters.variantProfileUrn.empty(),
                  std::logic_error,
                  "Required variants profile URN='" << flRecParameters.variantProfileUrn << "' "
                  << "conflicts with required variants profile path='"
                  << flRecParameters.variantSubsystemParams.profile_load << "'!");
    } else if (!flRecParameters.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flRecParameters.variantProfileUrn,
                                                   conf_variantsProfile_category,
                                                   conf_variantsProfile_mime,
                                                   conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << flRecParameters.variantProfileUrn << "'!");
      flRecParameters.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }

    // Warnings:
    if (flRecParameters.variantSubsystemParams.config_filename.empty()) {
      DT_LOG_WARNING(flRecParameters.logLevel, "No variants configuration is provided.");
    } else {
      if (flRecParameters.variantSubsystemParams.profile_load.empty()) {
        DT_LOG_WARNING(flRecParameters.logLevel, "No variants profile is provided.");
      }
    }

    if (flRecParameters.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flRecParameters.logLevel, "No services configuration is provided.");
    }

    // Print:
    if (datatools::logger::is_debug(flRecParameters.logLevel)) {
      flRecParameters.print(std::cerr);
    }

    DT_LOG_TRACE_EXITING(flRecParameters.logLevel);
    return;
  }

  falaise::exit_code do_metadata(const FLReconstructParams & flRecParameters,
                                 datatools::multi_properties & flRecMetadata)
  {
    falaise::exit_code code = falaise::EXIT_OK;

    // System section:
    datatools::properties & system_props
      = flRecMetadata.add_section("flreconstruct", "flreconstruct::section");
    system_props.set_description("flreconstruct basic system informations");

    system_props.store_string("bayeux.version", bayeux::version::get_version(),
                              "Bayeux version");

    system_props.store_string("falaise.version", falaise::version::get_version(),
                              "Falaise version");

    system_props.store_string("application", "flreconstruct",
                              "The flreconstruct application used to produce reconstructed data");

    system_props.store_string("application.version", falaise::version::get_version(),
                              "The version of the reconstruction application");

    system_props.store_string("userProfile", flRecParameters.userProfile,
                              "User profile");

    system_props.store_boolean("embeddedMetadata",
                               flRecParameters.embeddedMetadata,
                               "Metadata embedding flag");

    if (flRecParameters.numberOfEvents > 0) {
      system_props.store_integer("numberOfEvents",
                                 flRecParameters.numberOfEvents,
                                 "Number of reconstructed events");
    }

    if (!flRecParameters.experimentalSetupUrn.empty()) {
      system_props.store_string("experimentalSetupUrn",
                                flRecParameters.experimentalSetupUrn,
                                "Experimental setup URN");
    }

    boost::posix_time::ptime start_run_timestamp = boost::posix_time::second_clock::universal_time();
    system_props.store_string("timestamp",
                              boost::posix_time::to_iso_string(start_run_timestamp),
                              "Run start timestamp");

    // Reconstruction section:
    datatools::properties & reconstruction_props
      = flRecMetadata.add_section("flreconstruct.pipeline", "flreconstruct::section");
    reconstruction_props.set_description("Reconstruction setup parameters");

    if (!flRecParameters.reconstructionPipelineUrn.empty()) {
      reconstruction_props.store_string("reconstructionPipelineUrn",
                                        flRecParameters.reconstructionPipelineUrn,
                                        "Reconstruction setup URN");
    }

    if (!flRecParameters.reconstructionPipelineConfig.empty()) {
      reconstruction_props.store_string("reconstructionPipelineConfig",
                                        flRecParameters.reconstructionPipelineUrn,
                                        "Reconstruction setup main configuration file");
    }

    if (!flRecParameters.reconstructionPipelineModule.empty()) {
      reconstruction_props.store_string("reconstructionPipelineModule",
                                        flRecParameters.reconstructionPipelineUrn,
                                        "Reconstruction pipeline top module");
    }

    // Variants section:
    datatools::properties & variants_props
      = flRecMetadata.add_section("flreconstruct.variantService", "flreconstruct::section");
    variants_props.set_description("Variant setup");

    if (!flRecParameters.variantConfigUrn.empty()) {
      variants_props.store_string("configUrn", flRecParameters.variantConfigUrn,
                                  "Variants setup configuration URN");
    } else if (!flRecParameters.variantSubsystemParams.config_filename.empty()) {
      variants_props.store_path("config", flRecParameters.variantSubsystemParams.config_filename,
                                "Variants setup configuration path");
    }

    if (!flRecParameters.variantProfileUrn.empty()) {
      variants_props.store_string("profileUrn", flRecParameters.variantProfileUrn,
                                  "Variants profile URN");
    } else if (!flRecParameters.variantSubsystemParams.profile_load.empty()) {
      variants_props.store_path("profile", flRecParameters.variantSubsystemParams.profile_load,
                                "Variants profile path");
    }

    if (flRecParameters.variantSubsystemParams.settings.size()) {
      // Not with "production" user profile:
      variants_props.store("settings", flRecParameters.variantSubsystemParams.settings,
                           "Variants settings");
    }

    // Services section:
    datatools::properties & services_props
      = flRecMetadata.add_section("flreconstruct.services", "flreconstruct::section");
    services_props.set_description("Services configuration");

    if (!flRecParameters.servicesSubsystemConfigUrn.empty()) {
      services_props.store_string("configUrn", flRecParameters.servicesSubsystemConfigUrn,
                                  "Services setup configuration URN");
    } else if (!flRecParameters.servicesSubsystemConfig.empty()) {
      services_props.store_path("config", flRecParameters.servicesSubsystemConfig,
                                "Services setup configuration path");
    }

    return code;
  }

} // namespace FLReconstruct
