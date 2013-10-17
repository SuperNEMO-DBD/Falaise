// flsimulatemain - Command line flsimulate application
//
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

// Third Party
// - Boost
#include "boost/program_options.hpp"
namespace bpo = boost::program_options;

// - Bayeux
#include "bayeux/version.h"
#include "bayeux/datatools/datatools.h"
#include "bayeux/mctools/g4/manager.h"
#include "bayeux/mctools/g4/manager_parameters.h"

// This Project
#include "falaise/version.h"
#include "FLSimulateResources.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------
//! Commonly used exit status codes
//! Based on the definitions in the 4.3BSD <sysexits.h> header file
//! \todo Should check and use sysexits.h if available
enum FLExitCode {
  EXIT_HELP        = -1,  //! Help reported, not considered a failure
  EXIT_OK          = 0,  //! Successful termination
  EXIT_USAGE       = 64, //! Command line usage error
  EXIT_UNAVAILABLE = 69, //! Service unavailable
};

//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose) {
  os << "flsimulate " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
        << "Copyright (C) 2013 SuperNEMO Collaboration\n\n"
        << "flsimulate uses the following external libraries:\n"
        << "* Falaise : " << falaise::version::get_version() << "\n"
        << "* Bayeux  : " << bayeux::version::get_version() << "\n"
        << "* Boost   : " << BOOST_VERSION << "\n"
        << "* Geant4  : " << "10.x (eventually)"
        << "\n\n";
  }
}

//! Handle printing of help message to screen
void do_help(const bpo::options_description& od) {
  do_version(std::cout, false);
  std::cout << "Usage:\n"
            << "  flsimulate [options]\n"
            << "Options\n"
            << od;
}

//! Parse command line arguments to configure the simulation parameters
FLExitCode do_configure(int argc, char *argv[], mctools::g4::manager_parameters& params) {
  try {
    FLSimulate::initResources();
    params.set_defaults();
    params.logging = "error";
    params.manager_config_filename = FLSimulate::getResourceDir() + "/foo.txt";
  } catch (std::exception& e) {
    std::cerr << "[FLSimulate::do_configure] Unable to configure simulation"
              << std::endl
              << e.what() << std::endl;
    return EXIT_UNAVAILABLE;
  }


  // Bind command line parser to exposed
  namespace bpo = boost::program_options;
  bpo::options_description optDesc;
  optDesc.add_options()
      ("help,h","print this help message")
      ("version","print version number")
      ("verbose,v","increase verbosity of logging")
      ("number,n",
       bpo::value<uint32_t>(&params.number_of_events)->default_value(1)->value_name("[events]"),
       "number of events to simulate")
      ("output-file,o",
       bpo::value<std::string>(&params.output_data_file)->required()->value_name("[file]"),
       "file in which to store simulation results")
      ;

  // - Parse...
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    if (!vMap.count("help") && !vMap.count("version")) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      return EXIT_USAGE;
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    return EXIT_USAGE;
  }

  // Handle verbose, which can't be bound yet
  if (vMap.count("verbose")) params.logging = "information";

  // Handle any non-bound options
  if (vMap.count("help")) {
    do_help(optDesc);
    return EXIT_HELP;
  }

  if (vMap.count("version")) {
    do_version(std::cout, true);
    return EXIT_HELP;
  }

  return EXIT_OK;
}

//! Perform simulation using command line args as given
FLExitCode do_flsimulate(int argc, char *argv[]) {
  mctools::g4::manager_parameters flSimParameters;
  FLExitCode confStatus = do_configure(argc, argv, flSimParameters);
  switch (confStatus) {
    case EXIT_HELP:
      return EXIT_OK;
    case EXIT_OK:
      break;
    default:
      return confStatus;
  }

  try {
    mctools::g4::manager flSimulation;
    mctools::g4::manager_parameters::setup(flSimParameters, flSimulation);
    flSimulation.run_simulation();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_UNAVAILABLE;
  }

  return EXIT_OK;
}


//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // - Needed, but nasty, and DON'T let datatools interfere with the
  // command line args
  DATATOOLS_INIT_MAIN(0, 0);

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  FLExitCode ret = do_flsimulate(argc, argv);

  // - Needed, but nasty
  DATATOOLS_FINI()
  return ret;
}







