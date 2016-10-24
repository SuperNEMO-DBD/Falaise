//! \file    flsimulatemain_next.cc
//! \brief   Main program for flsimulate command line application
//! \details Configure, setup and run the Geant4 based simulation
//!          of the SuperNEMO experiment. Configuration is performed
//!          in two steps, a default and then user command line input.
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
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
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
#include <vector>

// Third Party
// - Boost
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
namespace bpo = boost::program_options;

// - Bayeux
#include "bayeux/version.h"
#include "bayeux/bayeux.h"
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/things.h"
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/mctools/g4/simulation_module.h"
#include "bayeux/mctools/g4/manager_parameters.h"
#include "bayeux/mygsl/random_utils.h"
#include "bayeux/mygsl/seed_manager.h"
namespace dtc = datatools::configuration;

// This Project
#include "falaise/version.h"
#include "falaise/falaise.h"
#include "falaise/exitcodes.h"
#include "falaise/resource.h"
#include "FLSimulateResources.h"
#include "PropertyReader.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------
//! Exceptions:
//! Exceptions for configuration
class FLConfigDefaultError : public std::exception {};
class FLConfigHelpHandled : public std::exception {};
class FLConfigUserError : public std::runtime_error {using std::runtime_error::runtime_error;};
//! Exceptions for dialog
class FLDialogHelpRequested : public std::exception {};
class FLDialogOptionsError : public std::exception {};



//----------------------------------------------------------------------
//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose) {
  os << "flsimulate " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
       << "Copyright (C) 2013-2016 SuperNEMO Collaboration\n\n"
       << "flsimulate uses the following external libraries:\n"
       << "* Falaise : " << falaise::version::get_version() << "\n"
       << "* Bayeux  : " << bayeux::version::get_version() << "\n"
       << "* Boost   : " << BOOST_VERSION << "\n"
       << "* Geant4  : " << "9.6.4" << "\n"
       << "\n\n";
  }
}

//! Handle printing of help message to screen
void do_help(const bpo::options_description& od) {
  do_version(std::cout, false);
  std::cout << "Usage:\n"
            << "  flsimulate [options]\n"
            << od
            << "\n";
}

//! Command line help on script schema
void do_help_scripting(std::ostream& os) {
  // Eventually, this will autoformat
  os << "Scripting flsimulate\n"
     << "--------------------\n\n"
     << "The following subsystems of flsimulate may be configured using an input\n"
     << "datatools::multi_properties script. The allowed sections and parameters are:\n"
     << std::endl
     << "[section=\"SimulationSubsystem\" description=\"\"]\n"
     << "experimentID : string = \"demonstrator\"        # Name of detector to simulate\n"
     << "numberOfEvents : integer = 1                    # Number of events to simulate\n"
     << "vertexGenerator : string = \"source_pads_bulk\" # Name of vertex point generator\n"
     << "eventGenerator : string = \"Se82.2nubb\"        # Name of event generator\n"
     << "rngSeedFile : string as path = \"seeds.conf\"   # Path to file containing random number seeds\n"
     << "outputProfile : string = \"\"                   # Output profile (hits collections to output)\n"
     << std::endl
     << "[section=\"VariantSubsystem\" description=\"\"]\n"
     << "profile : string as path = \"vprofile.conf\"         # Input variant profile configuration file\n"
     << "settings : string[N] = \"setting1\" ... \"settingN\" # Individual variant settings\n"
     << std::endl
     << "All sections and parameters are optional, and flsimulate will supply sensible\n"
     << "default values when only some are set.\n"
     << std::endl;
}

//! Collect all needed configuration parameters in one data structure
struct FLSimulateArgs {
  // Application specific parameters:
  datatools::logger::priority     logLevel;                //!< Logging priority threshold
  unsigned int                    numberOfEvents;          //!< Number of events to be processed in the pipeline
  unsigned int                    moduloEvents;            //!< Number of events progress modulo
  std::string                     experimentID;            //!< The label of the virtual experimental setup
  std::string                     setupGeometryVersion;    //!< The version number of the virtual geometry setup
  std::string                     setupSimulationVersion;  //!< The version number of the simulation engine setup
  mctools::g4::manager_parameters simulationManagerParams; /** Parameters for the Geant4 simulation manager
                                                            *  embedded in the simulation module
                                                            */
  std::string                     outputFile;              //!< Output data file for the output module
  // Variants support:
  dtc::variant_service::config    variantSubsystemParams;                //!< Variants configuration


  //! Construct and return the default configuration object
  // Equally, could be supplied in a .application file, though note
  // how some parameters are derived (i.e. there's a postprocessing step)
  static FLSimulateArgs makeDefault() {
    FLSimulateArgs params;
    params.logLevel = datatools::logger::PRIO_ERROR;
    params.numberOfEvents = 1;
    params.moduloEvents = 0;
    params.experimentID = "default";
    params.setupGeometryVersion = "4.0";
    params.setupSimulationVersion = "2.0";
    // Simulation
    params.simulationManagerParams.set_defaults();
    params.simulationManagerParams.logging = "error";
    params.simulationManagerParams.manager_config_filename = FLSimulate::getControlFile(params.experimentID);
    params.simulationManagerParams.vg_name = "source_pads_bulk";
    params.simulationManagerParams.eg_name = "Se82.0nubb";
    params.simulationManagerParams.input_prng_seeds_file = "";
    // Seeding is auto (from system) unless explicit file supplied
    params.simulationManagerParams.vg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the vertex generator
    params.simulationManagerParams.eg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the primary event generator
    params.simulationManagerParams.shpf_seed = mygsl::random_utils::SEED_AUTO; // PRNG for the back end true hit processors
    params.simulationManagerParams.mgr_seed  = mygsl::random_utils::SEED_AUTO; // PRNG for the Geant4 engine itself
    params.simulationManagerParams.output_profiles_activation_rule = "";
    // Variants
    params.variantSubsystemParams.config_filename = FLSimulate::getVariantsConfigFile(params.experimentID);
    // Profile loading as below doesn't appear to work... what is the __default__ thing used above?
    // NB, also fails in flsimulate if "__default__" is supplied, so looks like
    // error in variants or the formatting of the default profile.
    //params.variantSubsystemParams.profile_load = FLSimulate::getVariantsDefaultProfile(params.experimentID);

    return params;
  }
};

struct FLSimulateCommandLine {
  // The parameters we can receive from the command line
  // Help and so on are not marked because these are handled by the UI.
  std::string configScript; //!< Path to configuration script
  std::string outputFile;   //!< Path for the output module
};


//! Handle command line argument dialog
void do_cldialog(int argc, char *argv[], FLSimulateCommandLine& params) {

  // Bind command line parser to exposed parameters
  namespace bpo = boost::program_options;

  // Application specific options:
  bpo::options_description optDesc("Options");
  optDesc.add_options()
    ("help,h","print this help message")
    ("help-scripting","print help on input script format and schema")
    ("version","print version number")
    ("config,c",
     bpo::value<std::string>(&params.configScript)->value_name("[file]"),
     "configuration script for simulation")
    // Output *could* go into script, but as the configuration options are
    // pretty limited at present, keep it simple
    ("output-file,o",
     bpo::value<std::string>(&params.outputFile)->required()->value_name("[file]"),
     "file in which to store simulation results")
    ;

  // - Parse...
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    if (!vMap.count("help") && !vMap.count("version") && !vMap.count("help-scripting")) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      throw FLDialogOptionsError();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLDialogOptionsError();
  }

  // Handle any non-bound options
  if (vMap.count("help")) {
    do_help(optDesc);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("version")) {
    do_version(std::cout, true);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("help-scripting")) {
    do_help_scripting(std::cout);
    throw FLDialogHelpRequested();
  }
}

//----------------------------------------------------------------------

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc, char *argv[], FLSimulateArgs& params) {
  // - Default Config
  params = FLSimulateArgs::makeDefault();

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
  params.outputFile = args.outputFile;
  // If a script was supplied, use that to override params
  if(!args.configScript.empty()) {
    datatools::multi_properties flSimConfig("section", "description");
    flSimConfig.read(args.configScript);
    //flSimConfig.tree_dump(std::cout);
    // Now extract and bind values as needed
    // It's this that defines the schema... (note the awkwardness)
    // SimulationSubsystem
    if(flSimConfig.has_section("SimulationSubsystem")) {
       datatools::properties simSubsystem = flSimConfig.get_section("SimulationSubsystem");
       // Bind properties in this section to the relevant ones in params
       params.experimentID = falaise::Properties::getValueOrDefault<std::string>(simSubsystem,"experimentID",params.experimentID);
       // Here we need to validate the config files for the experiment input
       try {
         params.simulationManagerParams.manager_config_filename = FLSimulate::getControlFile(params.experimentID);
         params.variantSubsystemParams.config_filename = FLSimulate::getVariantsConfigFile(params.experimentID);
       }
       catch (FLSimulate::UnknownResourceException& e) {
         throw FLConfigUserError {e.what()};
       }

       params.numberOfEvents = falaise::Properties::getValueOrDefault<int>(simSubsystem,"numberOfEvents",params.numberOfEvents);
       params.moduloEvents = falaise::Properties::getValueOrDefault<int>(simSubsystem,"moduloEvents", params.moduloEvents);
       params.simulationManagerParams.vg_name = falaise::Properties::getValueOrDefault<std::string>(simSubsystem,"vertexGenerator", params.simulationManagerParams.vg_name);
       params.simulationManagerParams.eg_name = falaise::Properties::getValueOrDefault<std::string>(simSubsystem,"eventGenerator",params.simulationManagerParams.eg_name);
       params.simulationManagerParams.input_prng_seeds_file = falaise::Properties::getValueOrDefault<std::string>(simSubsystem,"rngSeedFile",params.simulationManagerParams.input_prng_seeds_file);
       params.simulationManagerParams.output_profiles_activation_rule = falaise::Properties::getValueOrDefault<std::string>(simSubsystem,"outputProfile",params.simulationManagerParams.output_profiles_activation_rule);
    }
    if(flSimConfig.has_section("VariantSubsystem")) {
      datatools::properties variantSubsystem = flSimConfig.get_section("VariantSubsystem");
      // Bind properties to relevant ones on params
      params.variantSubsystemParams.profile_load = falaise::Properties::getValueOrDefault<std::string>(variantSubsystem,"profile",params.variantSubsystemParams.profile_load);
      params.variantSubsystemParams.settings = falaise::Properties::getValueOrDefault<std::vector<std::string> >(variantSubsystem,"settings",params.variantSubsystemParams.settings);
    }
  }
}

//----------------------------------------------------------------------
//! Perform simulation using command line args as given
falaise::exit_code do_flsimulate(int argc, char *argv[])
{
  // - Configure
  FLSimulateArgs flSimParameters;
  try {
    do_configure(argc, argv, flSimParameters);
  } catch (FLConfigDefaultError& e) {
    std::cerr << "Unable to configure core of flsimulate" << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  } catch (FLConfigHelpHandled& e) {
    return falaise::EXIT_OK;
  } catch (FLConfigUserError& e) {
    std::cerr << "user configuration error: " << e.what() << std::endl;
    return falaise::EXIT_USAGE;
  }

  // Variants support:
  dtc::variant_service vserv;
  try {
    if (flSimParameters.variantSubsystemParams.is_active()) {
      vserv.configure(flSimParameters.variantSubsystemParams);
      // Start and lock the variant service:
      vserv.start();
      // From this point, all other services and/or processing modules can
      // benefit of the variant service during their configuration steps.
    }
  } catch (std::exception & e) {
    std::cerr << "flsimulate : Variant service threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  // - Run
  falaise::exit_code code = falaise::EXIT_OK;
  try {
    // Analyse the simulation manager configuration:
    datatools::multi_properties flSimProperties("name", "");
    flSimProperties.read(flSimParameters.simulationManagerParams.manager_config_filename);

    // Have to setup geometry (could this be made part of the simulation
    // module?)
    // datatools::properties flSimGeoManagerProperties;
    std::string geoManagerFile = flSimProperties.get_section("geometry").fetch_path("manager.config");
    geomtools::manager geoManager;
    datatools::properties geoManagerProperties;
    datatools::properties::read_config(geoManagerFile, geoManagerProperties);
    geoManager.initialize(geoManagerProperties);

    // Simulation module:
    mctools::g4::simulation_module flSimModule;
    flSimModule.set_name("G4SimulationModule");
    flSimModule.set_sd_label("SD");
    flSimModule.set_geometry_manager(geoManager);
    flSimModule.set_geant4_parameters(flSimParameters.simulationManagerParams);
    flSimModule.initialize_simple();

    // Output module:
    dpp::output_module simOutput;
    simOutput.set_name("FLSimulateOutput");
    simOutput.set_single_output_file(flSimParameters.outputFile);
    // Metadata management:
    datatools::multi_properties & metadataStore = simOutput.grab_metadata_store();
    metadataStore = flSimProperties;
    simOutput.initialize_simple();

    // Manual Event loop....
    datatools::things workItem;
    dpp::base_module::process_status status;

    for (unsigned int i(0); i < flSimParameters.numberOfEvents; ++i) {
      workItem.clear();

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

      if (code != falaise::EXIT_OK) {
        break;
      }
    }
  } catch (std::exception& e) {
    std::cerr << "flsimulate : Setup/run of simulation threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    code = falaise::EXIT_UNAVAILABLE;
  }

  // Terminate the variant service:
  vserv.stop();

  return code;
}


//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // - Needed, but nasty
  falaise::initialize();

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  falaise::exit_code ret = do_flsimulate(argc, argv);

  // - Needed, but nasty
  falaise::terminate();
  return ret;
}
