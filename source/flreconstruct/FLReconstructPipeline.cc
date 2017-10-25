// Ourselves
#include "FLReconstructPipeline.h"

// Standard Library
#include <exception>
#include <memory>

// Third Party
// - Boost
#include <boost/algorithm/string.hpp>
// - Bayeux
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/library_loader.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/i_data_source.h"
#include "bayeux/dpp/input_module.h"
#include "bayeux/dpp/module_manager.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"

// This Project:
#include "FLReconstructImpl.h"
#include "falaise/resource.h"
#include "falaise/snemo/processing/services.h"

namespace FLReconstruct {

//! Configure and run the pipeline
falaise::exit_code do_pipeline(const FLReconstructParams& flRecParameters) {
  DT_LOG_TRACE_ENTERING(flRecParameters.logLevel);

  // Variants support:
  datatools::configuration::variant_service variantService;
  if (!flRecParameters.variantSubsystemParams.logging.empty()) {
    variantService.set_logging(
        datatools::logger::get_priority(flRecParameters.variantSubsystemParams.logging));
  }
  try {
    if (flRecParameters.variantSubsystemParams.is_active()) {
      variantService.configure(flRecParameters.variantSubsystemParams);
      // Start and lock the variant service:
      DT_LOG_DEBUG(flRecParameters.logLevel, "Starting the variants service...");
      variantService.start();
      DT_LOG_DEBUG(flRecParameters.logLevel, "Variants service is started.");
      // From this point, all other services and/or processing modules can benefit
      // of the variant service during their configuration steps.
    }
  } catch (std::exception& e) {
    std::cerr << "flreconstruct : Variant service threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  // - Run:
  falaise::exit_code code = falaise::EXIT_OK;
  try {
    // Load plugins:
    datatools::library_loader libLoader(flRecParameters.userLibConfig);

    // Setup services:
    DT_LOG_DEBUG(flRecParameters.logLevel, "Starting reconstruction services...");
    uint32_t servicesFlags = datatools::service_manager::BLANK;
    servicesFlags |= datatools::service_manager::ALLOW_DYNAMIC_SERVICES;
    datatools::service_manager recServices("flReconstructionServices",
                                           "SuperNEMO Reconstruction Services", servicesFlags);
    if (!flRecParameters.servicesSubsystemConfig.empty()) {
      // Load services:
      std::string services_config_file = flRecParameters.servicesSubsystemConfig;
      datatools::fetch_path_with_env(services_config_file);
      datatools::properties services_config;
      services_config.read_configuration(services_config_file);
      recServices.initialize(services_config);
    } else {
      recServices.initialize();
    }
    DT_LOG_DEBUG(flRecParameters.logLevel, "Reconstruction services are started.");
    if (datatools::logger::is_debug(flRecParameters.logLevel)) {
      recServices.tree_dump(std::cerr, "Reconstruction services: ", "[debug] ");
    }

    // Make sure some core services are setup and started (geometry, electronics, database...):
    falaise::exit_code safeServicesCode = ensure_core_services(flRecParameters, recServices);
    if (safeServicesCode != falaise::EXIT_OK) {
      DT_THROW(std::logic_error, "Cannot start core services!");
      ;
    }

    // - Start up the module manager
    // Dual strategy here
    //  - If they supplied a script, use that, otherwise default to
    //  a single dump module.
    DT_LOG_DEBUG(flRecParameters.logLevel, "Configuring the module manager...");
    std::unique_ptr<dpp::module_manager> moduleManager(new dpp::module_manager);
    moduleManager->set_service_manager(recServices);
    DT_LOG_DEBUG(flRecParameters.logLevel, "Service manager is now plugged in the module manager.");

    // Configure the modules themselves
    if (!flRecParameters.modulesConfig.empty()) {
      moduleManager->load_modules(flRecParameters.modulesConfig);
    } else {
      // Hand configure a dumb dump module
      datatools::properties dumbConfig;
      dumbConfig.store("title", "flreconstruct::default");
      dumbConfig.store("output", "cout");
      moduleManager->load_module(flRecParameters.reconstructionPipelineModule, "dpp::dump_module",
                                 dumbConfig);
    }

    datatools::library_loader altLibLoader;
    // Load a Things2Root module in the manager before initialization
    if (!flRecParameters.outputFile.empty()) {
      DT_LOG_DEBUG(flRecParameters.logLevel, "Configuring the output module...");
      if (boost::algorithm::ends_with(flRecParameters.outputFile, ".root")) {
        std::string pluginPath = falaise::get_plugin_dir();
        altLibLoader.load("Things2Root", pluginPath);
        DT_LOG_DEBUG(flRecParameters.logLevel, "using ROOT format for output");
        datatools::properties t2rConfig;
        t2rConfig.store("output_file", flRecParameters.outputFile);
        moduleManager->load_module("t2rRecOutput", "Things2Root", t2rConfig);
      }
    }

    // Plain initialization:
    moduleManager->initialize_simple();

    // Input module...
    std::unique_ptr<dpp::input_module> recInput(new dpp::input_module);
    DT_LOG_DEBUG(flRecParameters.logLevel, "Configuring the input module...");
    recInput->set_logging_priority(flRecParameters.logLevel);
    recInput->set_single_input_file(flRecParameters.inputFile);
    recInput->initialize_simple();

    DT_LOG_DEBUG(flRecParameters.logLevel,
                 "Number of entries  = " << recInput->get_source().get_number_of_entries());
    DT_LOG_DEBUG(flRecParameters.logLevel,
                 "Number of metadata = " << recInput->get_source().get_number_of_metadata());

    // Output metadata management:
    DT_LOG_DEBUG(flRecParameters.logLevel, "Building output metadata...");
    datatools::multi_properties flRecMetadata("name", "type",
                                              "Metadata associated to a flreconstruct run");
    do_metadata(flRecParameters, flRecMetadata);
    if (datatools::logger::is_debug(flRecParameters.logLevel)) {
      flRecMetadata.tree_dump(std::cerr, "Output metadata: ", "[debug] ");
    }

    // - Pipeline
    dpp::base_module* pipeline = nullptr;
    try {
      pipeline = &(moduleManager->grab(flRecParameters.reconstructionPipelineModule));
    } catch (std::exception& e) {
      DT_LOG_FATAL(flRecParameters.logLevel, "Failed to initialize pipeline : " << e.what());
      return falaise::EXIT_UNAVAILABLE;
    }

    // Output module... only if added in the module manager
    dpp::base_module* recOutputHandle = nullptr;
    std::unique_ptr<dpp::output_module> flRecOutput;
    if (moduleManager->has("t2rRecOutput")) {
      // We instantiate and fetch the t2r module from the manager
      recOutputHandle = &moduleManager->grab("t2rRecOutput");
    } else if (!flRecParameters.outputFile.empty()) {
      // We try to setup an output module
      flRecOutput.reset(new dpp::output_module);
      flRecOutput->set_name("FLReconstructOutput");
      flRecOutput->set_single_output_file(flRecParameters.outputFile);
      // Metadata management:
      // Fetch the metadata to be stored through the output module
      if (flRecParameters.embeddedMetadata) {
        datatools::multi_properties& metadataStore = flRecOutput->grab_metadata_store();
        // Copy metadata from the input module
        metadataStore = flRecMetadata;
      }
      flRecOutput->initialize_simple();
      recOutputHandle = flRecOutput.get();
    }

    if (!flRecParameters.outputMetadataFile.empty()) {
      std::string fMetadata = flRecParameters.outputMetadataFile;
      datatools::fetch_path_with_env(fMetadata);
      flRecMetadata.write(fMetadata);
    }

    // - Now the actual event loop
    DT_LOG_DEBUG(flRecParameters.logLevel, "begin event loop");
    datatools::things workItem;
    std::size_t eventCounter = 0;
    while (true) {
      // Prepare and read work
      workItem.clear();
      if (recInput->is_terminated()) break;
      if (recInput->process(workItem) != dpp::base_module::PROCESS_OK) {
        DT_LOG_FATAL(flRecParameters.logLevel, "Failed to read data record from input source");
        break;
      }

      // Check pre-conditions on event model (requiredInputBanks) ?

      // Feed through pipeline
      dpp::base_module::process_status pStatus = pipeline->process(workItem);
      DT_THROW_IF(pStatus == dpp::base_module::PROCESS_INVALID, std::logic_error,
                  "Bug!!! Module '" << pipeline->get_name()
                                    << "' did not return a valid processing status!");

      // FATAL, ERROR and ERROR_STOP status triggers the abortion of the processing loop.
      // This is a very conservative approach, but it is compatible with the default behaviour of
      // the bxdpp_processing executable.
      if (pStatus == dpp::base_module::PROCESS_FATAL) break;
      if (pStatus == dpp::base_module::PROCESS_ERROR) break;
      if (pStatus == dpp::base_module::PROCESS_ERROR_STOP) break;

      // STOP means the current event should not be processed anymore nor saved
      // but the loop can continue with other items
      if (pStatus == dpp::base_module::PROCESS_STOP) continue;

      // Check post-conditions on event model (expectedOutputBanks) ?

      // Write item
      if (recOutputHandle) {
        pStatus = recOutputHandle->process(workItem);
        if (pStatus != dpp::base_module::PROCESS_OK) {
          DT_LOG_FATAL(flRecParameters.logLevel, "Failed to write data record to output sink");
          break;
        }
      }
      if (flRecParameters.moduloEvents > 0) {
        if (eventCounter % flRecParameters.moduloEvents == 0) {
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Event #" << eventCounter);
        }
      }
      eventCounter++;
      if (flRecParameters.numberOfEvents > 0 && eventCounter > flRecParameters.numberOfEvents) {
        break;
      }
    }
    DT_LOG_DEBUG(flRecParameters.logLevel, "event loop completed");

    // - MUST delete the module manager BEFORE the library loader clears
    // in case the manager is holding resources created from a shared lib
    if (moduleManager.get() != nullptr) {
      if (moduleManager->is_initialized()) {
        moduleManager->reset();
      }
      moduleManager.reset();
    }

    DT_LOG_DEBUG(flRecParameters.logLevel, "Stopping reconstruction services...");
    recServices.reset();
    DT_LOG_DEBUG(flRecParameters.logLevel, "Reconstruction services are stopped");

  } catch (std::exception& e) {
    std::cerr << "flreconstruct : Setup/run of simulation threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    code = falaise::EXIT_UNAVAILABLE;
  }

  if (variantService.is_started()) {
    // Terminate the variant service:
    DT_LOG_DEBUG(flRecParameters.logLevel, "Stopping the variants service...");
    variantService.stop();
    DT_LOG_DEBUG(flRecParameters.logLevel, "Variants service is stopped.");
  }
  DT_LOG_TRACE_EXITING(flRecParameters.logLevel);
  return code;  // falaise::EXIT_OK;
}

falaise::exit_code ensure_core_services(const FLReconstructParams& recParams,
                                        datatools::service_manager& recServices) {
  DT_LOG_TRACE_ENTERING(recParams.logLevel);
  datatools::kernel& dtk = datatools::kernel::instance();
  const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();

  // Geometry is a fundamental service, try to set one if missing:
  std::string geoServiceName = snemo::processing::service_info::default_geometry_service_label();
  if (!recServices.has(geoServiceName) ||
      !recServices.is_a<geomtools::geometry_service>(geoServiceName)) {
    std::string geometrySetupUrn;
    std::string geometrySetupConfig;
    const datatools::urn_info& expSetupUrnInfo =
        dtkUrnQuery.get_urn_info(recParams.experimentalSetupUrn);
    if (expSetupUrnInfo.has_topic("geometry") &&
        expSetupUrnInfo.get_components_by_topic("geometry").size() == 1) {
      geometrySetupUrn = expSetupUrnInfo.get_component("geometry");
      // Resolve geometry file:
      std::string conf_variants_category = "configuration";
      std::string conf_variants_mime;
      std::string conf_variants_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(geometrySetupUrn, conf_variants_category,
                                                   conf_variants_mime, conf_variants_path),
                  std::logic_error, "Cannot resolve URN='" << geometrySetupUrn << "'!");
      geometrySetupConfig = conf_variants_path;
      // No Geometry service was found from the service manager.
      // We try to setup one with the proper configuration.
      datatools::multi_properties geoServiceConfig("name", "type");
      std::string geoServicePath = geometrySetupConfig;
      // "@falaise:config/snemo/demonstrator/geometry/4.0/geometry_service.conf";
      if (!geoServicePath.empty()) {
        datatools::properties& gs =
            geoServiceConfig.add_section("geometry", "geomtools::geometry_service");
        gs.store("manager.configuration_file", geoServicePath);
        recServices.load(geoServiceConfig);
      }
    }
  }

  // Electronics:

  // Database:

  DT_LOG_TRACE_EXITING(recParams.logLevel);
  return falaise::EXIT_OK;
}

}  // namespace FLReconstruct
