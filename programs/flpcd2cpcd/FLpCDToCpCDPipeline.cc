// Ourselves
#include "FLpCDToCpCDPipeline.h"

// Standard Library
#include <exception>
#include <memory>

// Third Party
// - Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
// - Bayeux
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include "bayeux/datatools/library_loader.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/i_data_source.h"
#include "bayeux/dpp/input_module.h"
#include "bayeux/dpp/module_manager.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/dpp/context_service.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"

// This Project:
#include "FLpCDToCpCDImpl.h"
#include "falaise/resource.h"
#include "falaise/snemo/services/services.h"

namespace FLpCDToCpCD {

  //! Configure and run the pipeline
  falaise::exit_code do_pipeline(const FLpCDToCpCDParams & flAppParameters_)
  {
    DT_LOG_TRACE_ENTERING(flAppParameters_.logLevel);

    // - Run:
    falaise::exit_code code = falaise::EXIT_OK;
    try {
      // Load plugins:
      datatools::library_loader libLoader(flAppParameters_.userLibConfig);

      // Setup services:
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Starting pcd2cpcd services...");
      uint32_t servicesFlags = datatools::service_manager::BLANK;
      servicesFlags |= datatools::service_manager::ALLOW_DYNAMIC_SERVICES;
      datatools::service_manager recServices("flpCDToCpCDServices",
                                             "SuperNEMO pCDToCpCD Services", servicesFlags);
      if (!flAppParameters_.servicesSubsystemConfig.empty()) {
        // Load services:
        std::string services_config_file = flAppParameters_.servicesSubsystemConfig;
        datatools::fetch_path_with_env(services_config_file);
        datatools::properties services_config;
        services_config.read_configuration(services_config_file);
        recServices.initialize(services_config);
      } else {
        recServices.initialize();
      }
      if (datatools::logger::is_debug(flAppParameters_.logLevel)) {
        recServices.tree_dump(std::cerr, "pCDToCpCD services: ", "[debug] ");
      }

      // Make sure some core services are setup and started (geometry, electronics, database...):
      falaise::exit_code safeServicesCode = ensure_core_services(flAppParameters_, recServices);
      DT_THROW_IF(safeServicesCode != falaise::EXIT_OK, std::logic_error,
                  "Cannot start core services!");

      // - Start up the module manager
      // Dual strategy here
      //  - If they supplied a script, use that, otherwise default to
      //  a single dump module.
      std::unique_ptr<dpp::module_manager> moduleManager(new dpp::module_manager);
      moduleManager->set_service_manager(recServices);

      // Configure the modules themselves
      if (!flAppParameters_.modulesConfig.empty()) {
        DT_LOG_DEBUG(flAppParameters_.logLevel, "Loading modules from modules definition...");
        // << flAppParameters_.modulesConfig << "'...");
        moduleManager->load_modules(flAppParameters_.modulesConfig);
      } else {
        // Hand configure a dumb dump module
        datatools::properties dumbConfig;
        dumbConfig.store("title", "flpcd2cpcd::default");
        dumbConfig.store("output", "cout");
        moduleManager->load_module(flAppParameters_.pcdtocpcdPipelineModule, "dpp::dump_module",
                                   dumbConfig);
      }

      // No Root export:
      // datatools::library_loader altLibLoader;
      // // Load a Things2Root module in the manager before initialization
      // if (!flAppParameters_.outputFile.empty()) {
      //   // if (boost::algorithm::ends_with(flAppParameters_.outputFile, ".root")) {
      //   //   std::string pluginPath = falaise::get_plugin_dir();
      //   //   altLibLoader.load("Things2Root", pluginPath);
      //   //   datatools::properties t2rConfig;
      //   //   t2rConfig.store("output_file", flAppParameters_.outputFile);
      //   //   moduleManager->load_module("t2rRecOutput", "Things2Root", t2rConfig);
      //   // }
      // }

      // Plain initialization:
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Module manager initialization...");
      moduleManager->initialize_simple();
      if (datatools::logger::is_debug(flAppParameters_.logLevel)) {
        moduleManager->tree_dump(std::cerr, "Initialized module manager: ", "[debug] ");
      }

      // Input module...
      std::unique_ptr<dpp::input_module> recInput(new dpp::input_module());
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Configuring the input module...");
      recInput->set_name("FLpCDToCpCDInput");
      recInput->set_logging_priority(flAppParameters_.logLevel);
      std::string inFile(flAppParameters_.inputFile);
      datatools::fetch_path_with_env(inFile);
      if (not boost::filesystem::exists(inFile)) {
        DT_LOG_FATAL(flAppParameters_.logLevel, "Input file '" << inFile << "' does not exist!");
        return falaise::EXIT_UNAVAILABLE;
      }
      recInput->set_single_input_file(inFile);
      recInput->initialize_simple();

      DT_LOG_DEBUG(flAppParameters_.logLevel,
                   "Number of entries  = " << recInput->get_source().get_number_of_entries());
      DT_LOG_DEBUG(flAppParameters_.logLevel,
                   "Number of metadata = " << recInput->get_source().get_number_of_metadata());

      // Output metadata management:
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Building output metadata...");
      datatools::multi_properties flAppMetadata("name", "type",
                                                "Metadata associated to a flpcd2cpcd run");
      do_metadata(flAppParameters_, flAppMetadata);
      if (datatools::logger::is_debug(flAppParameters_.logLevel)) {
        flAppMetadata.tree_dump(std::cerr, "Output metadata: ", "[debug] ");
      }

      // - Pipeline
      dpp::base_module * pipeline = nullptr;
      try {
        pipeline = &(moduleManager->grab(flAppParameters_.pcdtocpcdPipelineModule));
      } catch (std::exception & e) {
        DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to initialize flpcd2cpcd pipeline : " << e.what());
        return falaise::EXIT_UNAVAILABLE;
      }

      // Output module... only if added in the module manager
      dpp::base_module * recOutputHandle = nullptr;
      std::unique_ptr<dpp::output_module> flAppOutput;
      // if (moduleManager->has("t2rRecOutput")) {
      //   // We instantiate and fetch the t2r module from the manager
      //   recOutputHandle = &moduleManager->grab("t2rRecOutput");
      // } else

      if (!flAppParameters_.outputFile.empty()) {
        // We try to setup an output module
        flAppOutput.reset(new dpp::output_module);
        flAppOutput->set_name("FLpCDToCpCDOutput");
        flAppOutput->set_single_output_file(flAppParameters_.outputFile);
        // Metadata management:
        // Fetch the metadata to be stored through the output module
        datatools::multi_properties & metadataStore = flAppOutput->grab_metadata_store();
        // Copy metadata from the input module
        metadataStore = flAppMetadata;
        flAppOutput->initialize_simple();
        recOutputHandle = flAppOutput.get();
      }

      if (!flAppParameters_.outputMetadataFile.empty()) {
        std::string fMetadata = flAppParameters_.outputMetadataFile;
        datatools::fetch_path_with_env(fMetadata);
        flAppMetadata.write(fMetadata);
      }

      // - Now the actual record loop
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Begin record loop");
      datatools::things workItem;
      std::size_t recordCounter = 0;
      while (true) {
        // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "==========> Pipeline loop for record #" << recordCounter);
        // Prepare and read work
        workItem.clear();
        if (recInput->is_terminated()) {
          // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Input module is terminated");
          break;
        }
        if (recInput->process(workItem) != dpp::base_module::PROCESS_OK) {
          DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to read data record from input source");
          code = falaise::EXIT_UNAVAILABLE;
          break;
        }

        // Feed through pipeline
        dpp::base_module::process_status pStatus = pipeline->process(workItem);
        DT_THROW_IF(pStatus == dpp::base_module::PROCESS_INVALID,
                    std::logic_error,
                    "Module '" << pipeline->get_name() << "' did not return a valid processing status!");

        // FATAL, ERROR and ERROR_STOP status triggers the abortion of the processing loop.
        // This is a very conservative approach, but it is compatible with the default behaviour of
        // the bxdpp_processing executable.
        if (pStatus == dpp::base_module::PROCESS_FATAL) {
          code = falaise::EXIT_UNAVAILABLE;
          break;
        }
        if (pStatus == dpp::base_module::PROCESS_ERROR) {
          code = falaise::EXIT_UNAVAILABLE;
          break;
        }
        if (pStatus == dpp::base_module::PROCESS_ERROR_STOP) {
          code = falaise::EXIT_UNAVAILABLE;
          break;
        }

        // STOP means the current record should not be processed anymore nor saved
        // but the loop can continue with other items
        if (pStatus == dpp::base_module::PROCESS_STOP) {
          continue;
        }

        // Check post-conditions on record model (expectedOutputBanks) ?

        // Write item
        if (recOutputHandle != nullptr) {
          pStatus = recOutputHandle->process(workItem);
          if (pStatus != dpp::base_module::PROCESS_OK) {
            DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to write data record to output sink");
            code = falaise::EXIT_UNAVAILABLE;
            break;
          }
        }
        if (flAppParameters_.moduloRecords > 0) {
          if (recordCounter % flAppParameters_.moduloRecords == 0) {
            DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Record #" << recordCounter);
          }
        }
        recordCounter++;
        if (flAppParameters_.numberOfRecords > 0 && recordCounter > flAppParameters_.numberOfRecords) {
          break;
        }
      }
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Record loop completed");

      // - MUST delete the module manager BEFORE the library loader clears
      // in case the manager is holding resources created from a shared lib
      if (moduleManager != nullptr) {
        if (moduleManager->is_initialized()) {
          moduleManager->reset();
        }
        moduleManager.reset();
      }

      DT_LOG_DEBUG(flAppParameters_.logLevel, "Stopping reconstruction services...");
      recServices.reset();
      DT_LOG_DEBUG(flAppParameters_.logLevel, "pcd2cpcd services are stopped");
    } catch (std::exception& e) {
      std::cerr << "flpcd2cpcd : Setup/run of simulation threw exception" << std::endl;
      std::cerr << e.what() << std::endl;
      code = falaise::EXIT_UNAVAILABLE;
    }
    return code; // falaise::EXIT_OK;
  }

  falaise::exit_code ensure_core_services(const FLpCDToCpCDParams & recParams_,
                                          datatools::service_manager & recServices_)
  {
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    // Context is a fundamental service, try to set one if missing:
    std::string contextServiceName = snemo::service_info::contextServiceName();
    if (! recServices_.has(contextServiceName) ||
        ! recServices_.is_a<dpp::context_service>(contextServiceName)) {
      // No context service was found from the service manager.
      datatools::properties contextServiceConfig;
      // store some properties thing in the configuration container ?
      DT_LOG_NOTICE(recParams_.logLevel, "Setting a default 'context' core service...");
      recServices_.load(contextServiceName, "dpp::context_service", contextServiceConfig);
    }
    
    // Geometry is a fundamental service, try to set one if missing:
    std::string geoServiceName = snemo::service_info::geometryServiceName();
    if (! recServices_.has(geoServiceName) ||
        ! recServices_.is_a<geomtools::geometry_service>(geoServiceName)) {
      // No geometry service was found from the service manager.
      // We try to find one from the experimental setup configuration.
      std::string geometrySetupUrn;
      std::string geometrySetupConfig;
      const datatools::urn_info & expSetupUrnInfo =
        dtkUrnQuery.get_urn_info(recParams_.experimentalSetupUrn);
      if (expSetupUrnInfo.has_topic("geometry") &&
          expSetupUrnInfo.get_components_by_topic("geometry").size() == 1) {
        geometrySetupUrn = expSetupUrnInfo.get_component("geometry");
        // Resolve geometry file:
        std::string conf_geometry_category = "configuration";
        std::string conf_geometry_mime;
        std::string conf_geometry_path;
        DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(geometrySetupUrn, conf_geometry_category,
                                                     conf_geometry_mime, conf_geometry_path),
                    std::logic_error, "Cannot resolve URN='" << geometrySetupUrn << "'!");
        geometrySetupConfig = conf_geometry_path;
        // We try to setup one with the proper configuration.
        datatools::multi_properties geoServiceConfig("name", "type");
        std::string geoServicePath = geometrySetupConfig;
        // example: "@falaise:config/snemo/demonstrator/geometry/5.0/GeometryService.conf";
        if (!geoServicePath.empty()) {
          DT_LOG_NOTICE(recParams_.logLevel, "Setting a default 'geometry' core service...");
          datatools::properties & gs =
            geoServiceConfig.add_section(snemo::service_info::geometryServiceName(), "geomtools::geometry_service");
          gs.store("manager.configuration_file", geoServicePath);
          recServices_.load(geoServiceConfig);
        } 
      } else {
        DT_THROW(std::logic_error, "No geometry service is set!");
      }
    }
    
    return falaise::EXIT_OK;
  }

} // namespace FLpCDToCpCD
