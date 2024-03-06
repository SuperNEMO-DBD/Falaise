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
#include "FLpCDToCpCDAlgo.h"
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

      // Output module... only if added in the module manager
      dpp::base_module * recOutputHandle = nullptr;
      std::unique_ptr<dpp::output_module> flAppOutput;

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

      FLpCDToCpCDAlgorithm pcd2cpcdAlgo;
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Verbosity = " << datatools::logger::get_priority_label(flAppParameters_.logLevel));
      pcd2cpcdAlgo.set_verbosity(flAppParameters_.logLevel);
      
      // - Now the actual event loop
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Begin event loop");
      datatools::things inputDataEvent;
      std::size_t inputEventCounter = 0;
      std::size_t outputClusteredEventCounter = 0;
      std::size_t outputUnclusteredEventCounter = 0;
      while (true) {
        // Prepare and read work
        inputDataEvent.clear();
        if (recInput->is_terminated()) {
          // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Input module is terminated");
          break;
        }
        if (recInput->process(inputDataEvent) != dpp::base_module::PROCESS_OK) {
          DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to read data event from input source");
          code = falaise::EXIT_UNAVAILABLE;
          break;
        }

	FLpCDToCpCDAlgorithm::data_records_col outputDataEvents;
        pcd2cpcdAlgo.process(inputDataEvent, outputDataEvents);

        // Write item
        if (recOutputHandle != nullptr) {
	  if (outputDataEvents.size()) {
	    for (auto & outputDataEventHandle : outputDataEvents) {
	      auto & outputDataEvent = outputDataEventHandle.grab();
	      // if (outputDataEvent.has(pcd2cpcdAlgo.cpcd_tag())) {
	      // }
	      auto pStatus = recOutputHandle->process(outputDataEvent);
	      outputClusteredEventCounter++;
	      if (pStatus != dpp::base_module::PROCESS_OK) {
		DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to write event to output sink");
		code = falaise::EXIT_UNAVAILABLE;
		break;
	      }
	    }
	  } else {
	    if (flAppParameters_.preserveUnclusteredEvents) {
	      // No found clusters : the event data record is saved as is:
	      auto pStatus = recOutputHandle->process(inputDataEvent);
	      outputUnclusteredEventCounter++;
	      if (pStatus != dpp::base_module::PROCESS_OK) {
		DT_LOG_FATAL(flAppParameters_.logLevel, "Failed to write unclustered event to output sink");
		code = falaise::EXIT_UNAVAILABLE;
		break;
	      }
	    }
	  }
        }
        if (flAppParameters_.moduloEvents > 0) {
          if (inputEventCounter % flAppParameters_.moduloEvents == 0) {
            DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Input event #" << inputEventCounter);
          }
        }
        inputEventCounter++;
        if (flAppParameters_.numberOfEvents > 0 && inputEventCounter > flAppParameters_.numberOfEvents) {
          break;
        }
      }
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Event loop completed");
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Number of processed input events  : " << inputEventCounter);
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Number of generated clustered output events : " << outputClusteredEventCounter);
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Number of generated unclustered output events : " << outputUnclusteredEventCounter);
      DT_LOG_DEBUG(flAppParameters_.logLevel, "Stopping pcd2cpcd reconstruction services...");
      recServices.reset();
      DT_LOG_DEBUG(flAppParameters_.logLevel, "pcd2cpcd services are stopped");
    } catch (std::exception & e) {
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
