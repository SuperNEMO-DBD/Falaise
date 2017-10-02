//! \file    flsimulatemain.cc
//! \brief   Main program for flsimulate command line application
//! \details Configure, setup and run the Geant4 based simulation
//!          of the SuperNEMO experiment. Configuration is performed
//!          in three steps, a default, then user command line input, then
//!          a configuration script.
//!          Error handling is through exceptions, except at the top
//!          level where standard error codes are used.
//! \todo    Improve error handling, with more logical exceptions.
//!          Exceptions are good, the problem is to condense them from
//!          the various places they can come from into a system
//!          understandable by the user when errors are reported.
//!          In other words, we don't want the user to see a detailed
//!          low level report, rather a clean summary (e.g. "config
//!          failed"), plus any further detail needed for them to report
//!          the issue. This is also why we try to stop exceptions
//!          propgating into main() so that these reports can be generated.
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard Library
#include <string>

// Third Party
// - Boost
#include "boost/filesystem.hpp"
// #include "boost/date_time/posix_time/posix_time.hpp"

// - Bayeux
#include "bayeux/bayeux.h"
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/datatools/things.h"
#include "bayeux/datatools/urn.h"
#include "bayeux/datatools/urn_db_service.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/datatools/urn_to_path_resolver_service.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/mctools/g4/manager_parameters.h"
#include "bayeux/mctools/g4/simulation_module.h"
#include "bayeux/mygsl/random_utils.h"
#include "bayeux/mygsl/seed_manager.h"
#include "bayeux/version.h"

// This Project
#include "falaise/exitcodes.h"
#include "falaise/falaise.h"
#include "falaise/resource.h"
#include "falaise/snemo/datamodels/data_model.h"
#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/processing/services.h"
#include "falaise/version.h"

#include "FLSimulateArgs.h"
// #include "FLSimulateCommandLine.h"
#include "FLSimulateErrors.h"
#include "FLSimulateResources.h"

namespace FLSimulate {

//! Perform simulation using command line args as given
falaise::exit_code do_flsimulate(int argc, char *argv[]);

//! Populate the metadata container with various informations classified in several categories
falaise::exit_code do_metadata(const FLSimulateArgs &, datatools::multi_properties &);

}  // end of namespace FLSimulate

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  falaise::initialize();

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  falaise::exit_code ret = FLSimulate::do_flsimulate(argc, argv);

  falaise::terminate();
  return ret;
}

namespace FLSimulate {

//----------------------------------------------------------------------
falaise::exit_code do_metadata(const FLSimulateArgs &flSimParameters,
                               datatools::multi_properties &flSimMetadata) {
  falaise::exit_code code = falaise::EXIT_OK;

  // System section:
  datatools::properties &system_props =
      flSimMetadata.add_section("flsimulate", "flsimulate::section");
  system_props.set_description("flsimulate basic system informations");

  system_props.store_string("bayeux.version", bayeux::version::get_version(), "Bayeux version");

  system_props.store_string("falaise.version", falaise::version::get_version(), "Falaise version");

  system_props.store_string("application", "flsimulate",
                            "The simulation application used to produce Monte Carlo data");

  system_props.store_string("application.version", falaise::version::get_version(),
                            "The version of the simulation application");

  system_props.store_string("userProfile", flSimParameters.userProfile, "User profile");

  system_props.store_integer("numberOfEvents", flSimParameters.numberOfEvents,
                             "Number of simulated events");

  system_props.store_boolean("doSimulation", flSimParameters.doSimulation, "Activate simulation");

  system_props.store_boolean("doDigitization", flSimParameters.doDigitization,
                             "Activate digitization");

  if (!flSimParameters.experimentalSetupUrn.empty()) {
    system_props.store_string("experimentalSetupUrn", flSimParameters.experimentalSetupUrn,
                              "Experimental setup URN");
  }

  system_props.store_boolean("embeddedMetadata", flSimParameters.embeddedMetadata,
                             "Metadata embedding flag");

  // Remove timestamp from metadata:
  // boost::posix_time::ptime start_run_timestamp =
  // boost::posix_time::second_clock::universal_time(); system_props.store_string("timestamp",
  //                           boost::posix_time::to_iso_string(start_run_timestamp),
  //                           "Run start timestamp");

  if (flSimParameters.doSimulation) {
    // Simulation section:
    datatools::properties &simulation_props =
        flSimMetadata.add_section("flsimulate.simulation", "flsimulate::section");
    simulation_props.set_description("Simulation setup parameters");

    if (!flSimParameters.simulationSetupUrn.empty()) {
      simulation_props.store_string("simulationSetupUrn", flSimParameters.simulationSetupUrn,
                                    "Simulation setup URN");
    } else if (!flSimParameters.simulationManagerParams.manager_config_filename.empty()) {
      simulation_props.store_path("simulationSetupConfig",
                                  flSimParameters.simulationManagerParams.manager_config_filename,
                                  "Simulation manager configuration file");
    }
    if (flSimParameters.saveRngSeeding && !flSimParameters.rngSeeding.empty()) {
      // Saving effective initial seeds for PRNGs:
      simulation_props.store_string("rngSeeding", flSimParameters.rngSeeding, "PRNG initial seeds");
    }
  }

  if (flSimParameters.doDigitization) {
    // Digitization section:
    datatools::properties &digitization_props =
        flSimMetadata.add_section("flsimulate.digitization", "flsimulate::section");
    digitization_props.set_description("Digitization setup parameters");

    // Not implemented yet.

    // if (!flSimParameters.digitizationSetupUrn.empty()) {
    //   digitization_props.store_string("digitizationSetupUrn",
    //                                        flSimParameters.digitizationSetupUrn,
    //                                        "Digitization setup URN");
    // } else if (!flSimParameters.digitizationSetupConfig.empty()) {
    //   digitization_props.store_path("digitizationSetupConfig",
    //                                      flSimParameters.digitizationSetupConfig,
    //                                      "Digitization manager configuration file");
    // }
  }

  // Variants section:
  datatools::properties &variants_props =
      flSimMetadata.add_section("flsimulate.variantService", "flsimulate::section");
  variants_props.set_description("Variant setup");

  if (!flSimParameters.variantConfigUrn.empty()) {
    variants_props.store_string("configUrn", flSimParameters.variantConfigUrn,
                                "Variants setup configuration URN");
  } else if (!flSimParameters.variantSubsystemParams.config_filename.empty()) {
    variants_props.store_path("config", flSimParameters.variantSubsystemParams.config_filename,
                              "Variants setup configuration path");
  }

  if (!flSimParameters.variantProfileUrn.empty()) {
    variants_props.store_string("profileUrn", flSimParameters.variantProfileUrn,
                                "Variants profile URN");
  } else if (!flSimParameters.variantSubsystemParams.profile_load.empty()) {
    variants_props.store_path("profile", flSimParameters.variantSubsystemParams.profile_load,
                              "Variants profile path");
  }

  if (flSimParameters.saveVariantSettings &&
      flSimParameters.variantSubsystemParams.settings.size()) {
    // Saving effective list of variant settings:
    variants_props.store("settings", flSimParameters.variantSubsystemParams.settings,
                         "Effective variants settings");
  }

  // Services section:
  datatools::properties &services_props =
      flSimMetadata.add_section("flsimulate.services", "flsimulate::section");
  services_props.set_description("Services configuration");

  if (!flSimParameters.servicesSubsystemConfigUrn.empty()) {
    services_props.store_string("configUrn", flSimParameters.servicesSubsystemConfigUrn,
                                "Services setup configuration URN");
  } else if (!flSimParameters.servicesSubsystemConfig.empty()) {
    services_props.store_path("config", flSimParameters.servicesSubsystemConfig,
                              "Services setup configuration path");
  }

  return code;
}

//----------------------------------------------------------------------
falaise::exit_code do_flsimulate(int argc, char *argv[]) {
  // - Configure:
  FLSimulateArgs flSimParameters;
  try {
    do_configure(argc, argv, flSimParameters);
  } catch (FLConfigDefaultError &e) {
    std::cerr << "Unable to configure core of flsimulate" << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  } catch (FLConfigHelpHandled &e) {
    return falaise::EXIT_OK;
  } catch (FLConfigUserError &e) {
    std::cerr << "User configuration error: " << e.what() << std::endl;
    return falaise::EXIT_USAGE;
  }

  // - Variants support:
  datatools::configuration::variant_service variantService;
  if (!flSimParameters.variantSubsystemParams.logging.empty()) {
    variantService.set_logging(
        datatools::logger::get_priority(flSimParameters.variantSubsystemParams.logging));
  }
  try {
    if (flSimParameters.variantSubsystemParams.is_active()) {
      variantService.configure(flSimParameters.variantSubsystemParams);
      // Start and lock the variant service:
      variantService.start();
      // From this point, all other services and/or processing modules can
      // benefit of the variant service during their configuration steps.
      if (flSimParameters.variantSubsystemParams.settings.size()) {
        // The Variant service uses explicit settings:
        // Make sure we know the full list of effective variant settings corresponding to user
        // choice:
        datatools::properties dummyVariantProps;
        datatools::configuration::variant_repository::exporter varRepExp(
            dummyVariantProps,
            datatools::configuration::variant_repository::exporter::EXPORT_NOCLEAR);
        varRepExp.process(variantService.get_repository());
        // Update the list of *all* settings, not only those requested by the user:
        flSimParameters.variantSubsystemParams.settings = varRepExp.get_settings();
      }
    }
  } catch (std::exception &e) {
    std::cerr << "flsimulate : Variant service threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  // - Run:
  falaise::exit_code code = falaise::EXIT_OK;
  try {
    // Setup services:
    datatools::service_manager services("flSimulationServices", "SuperNEMO Simulation Services");
    std::string services_config_file = flSimParameters.servicesSubsystemConfig;
    datatools::fetch_path_with_env(services_config_file);
    datatools::properties services_config;
    services_config.read_configuration(services_config_file);
    services.initialize(services_config);

    // Simulation module:
    mctools::g4::simulation_module flSimModule;
    flSimModule.set_name("G4SimulationModule");
    std::string sd_label = snemo::datamodel::data_info::default_simulated_data_label();
    std::string geo_label = snemo::processing::service_info::default_geometry_service_label();
    flSimModule.set_sd_label(sd_label);
    flSimModule.set_geo_label(geo_label);
    flSimModule.set_geant4_parameters(flSimParameters.simulationManagerParams);
    flSimModule.initialize_simple_with_service(services);
    if (flSimModule.is_initialized()) {
      // Fetch effective seeds' value after simulation module initialization
      // because the embedded PRNG seed manager makes the final choice of
      // initial seeds.
      std::ostringstream rngSeedingOut;
      rngSeedingOut << flSimModule.get_seed_manager();
      flSimParameters.rngSeeding = rngSeedingOut.str();
      DT_LOG_DEBUG(flSimParameters.logLevel, "PRNG seeding = " << flSimParameters.rngSeeding);
    }

    // Digitization module:
    if (flSimParameters.doDigitization) {
      DT_THROW(std::logic_error, "Digitization is not supported yet!");
    }

    // Output metadata management:
    datatools::multi_properties flSimMetadata("name", "type",
                                              "Metadata associated to a flsimulate run");
    do_metadata(flSimParameters, flSimMetadata);
    if (datatools::logger::is_debug(flSimParameters.logLevel)) {
      flSimMetadata.tree_dump(std::cerr, "Simulation metadata: ", "[debug]: ");
    }

    if (!flSimParameters.outputMetadataFile.empty()) {
      std::string fMetadata = flSimParameters.outputMetadataFile;
      datatools::fetch_path_with_env(fMetadata);
      flSimMetadata.write(fMetadata);
    }

    // Simulation output module:
    dpp::output_module simOutput;
    simOutput.set_name("FLSimulateOutput");
    simOutput.set_single_output_file(flSimParameters.outputFile);
    // Metadata management:
    if (flSimParameters.embeddedMetadata) {
      // Push the metadata in the metadata store:
      datatools::multi_properties &metadataStore = simOutput.grab_metadata_store();
      metadataStore = flSimMetadata;
    }
    simOutput.initialize_simple();

    // Manual Event loop....
    datatools::things workItem;
    dpp::base_module::process_status status;

    for (unsigned int i(0); i < flSimParameters.numberOfEvents; ++i) {
      workItem.clear();

      // Add the event header bank
      auto &eventHeader = workItem.add<snemo::datamodel::event_header>(
          snemo::datamodel::data_info::default_event_header_label(), "Event Header Bank");
      eventHeader.set_generation(snemo::datamodel::event_header::GENERATION_SIMULATED);
      datatools::event_id eventID{datatools::event_id::ANY_RUN_NUMBER, static_cast<int>(i)};
      eventHeader.set_id(eventID);

      status = flSimModule.process(workItem);
      if (status != dpp::base_module::PROCESS_OK) {
        std::cerr << "flsimulate : Simulation module failed" << std::endl;
        code = falaise::EXIT_UNAVAILABLE;
      }

      status = simOutput.process(workItem);
      if (status != dpp::base_module::PROCESS_OK) {
        std::cerr << "flsimulate : Output module failed" << std::endl;
        code = falaise::EXIT_UNAVAILABLE;
      }

      // Here we will process optional ASB+Digitization+terminal output modules

      if (code != falaise::EXIT_OK) {
        break;
      }
    }
  } catch (std::exception &e) {
    std::cerr << "flsimulate : Setup/run of simulation threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    code = falaise::EXIT_UNAVAILABLE;
  }

  // Terminate the variant service:
  if (variantService.is_started()) {
    variantService.stop();
  }

  return code;
}

}  // end of namespace FLSimulate
