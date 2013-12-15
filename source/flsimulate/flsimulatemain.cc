//! \file    flsimulatemain.cc
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

// Third Party
// - Boost
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
namespace bpo = boost::program_options;

// - Bayeux
#include "bayeux/version.h"
#include "bayeux/bayeux.h"
#include "bayeux/mctools/g4/manager.h"
#include "bayeux/mctools/g4/manager_parameters.h"

// This Project
#include "falaise/version.h"
#include "falaise/falaise.h"
#include "falaise/exitcodes.h"
#include "FLSimulateResources.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//! Exceptions for dialog
class FLDialogHelpRequested : public std::exception {};
class FLDialogOptionsError : public std::exception {};

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
            << od
            << "\n";
}

//! Handle command line argument dialog
void do_cldialog(int argc, char *argv[], mctools::g4::manager_parameters& params) {
  // Bind command line parser to exposed parameters
  namespace bpo = boost::program_options;
  bpo::options_description optDesc;
  optDesc.add_options()
    ("help,h","print this help message")
    ("version","print version number")
    ("verbose,v","increase verbosity of logging")
    ("number,n",
     bpo::value<uint32_t>(&params.number_of_events)
      ->default_value(1)
      ->value_name("[events]"),
     "number of events to simulate")
    ("vertex-generator,x",
     bpo::value<std::string>(&params.vg_name)
      ->default_value("experimental_hall_roof")
      ->value_name("[name]"),
     "The name of the vertex generator"
     )
    ("event-generator,e",
     bpo::value<std::string>(&params.eg_name)
      ->default_value("muon.cosmic.sea_level.toy")
      ->value_name("[name]"),
     "The name of the event generator"
     )
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
      throw FLDialogOptionsError();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLDialogOptionsError();
  }

  // Explicitely  set   the  seeds   of  the   4  embedded   PRNGs  to
  // 'mygsl::random_utils::SEED_INVALID'   (0)    for   an   automated
  // randomization  of seeds  from the  current time  (and some  other
  // possible entropy  sources).  This is not  recommended because the
  // interface should  enforce the user  to explicitely set  the seeds
  // for a  simulation run.  Note  that the 'XXX_seed'  parameters are
  // initialized  with  the 'mygsl::random_utils::SEED_INVALID'  value
  // (-1) which leads to an exception at PRNG startup.
  params.vg_seed   = mygsl::random_utils::SEED_TIME; // PRNG for the vertex generator
  params.eg_seed   = mygsl::random_utils::SEED_TIME; // PRNG for the primary event generator
  params.shpf_seed = mygsl::random_utils::SEED_TIME; // PRNG for the back end true hit processors
  params.mgr_seed  = mygsl::random_utils::SEED_TIME; // PRNG for the Geant4 engine itself
  // params.vg_seed   = 1;
  // params.eg_seed   = 2;
  // params.shpf_seed = 3;
  // params.mgr_seed  = 4;

  // Handle verbose, which can't be bound yet
  if (vMap.count("verbose")) params.logging = "information";

  // Handle any non-bound options
  if (vMap.count("help")) {
    do_help(optDesc);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("version")) {
    do_version(std::cout, true);
    throw FLDialogHelpRequested();
  }
}

//----------------------------------------------------------------------
//! Exceptions for configuration
class FLConfigDefaultError : public std::exception {};
class FLConfigHelpHandled : public std::exception {};
class FLConfigUserError : public std::exception {};

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc, char *argv[], mctools::g4::manager_parameters& params) {
  // - Default Config
  try {
    FLSimulate::initResources();
    params.set_defaults();
    params.logging = "error";
    params.manager_config_filename = FLSimulate::getResourceDir() + "/resources/config/snemo/tracker_commissioning/simulation/control/1.0/manager.conf";
  } catch (std::exception& e) {
    throw FLConfigDefaultError();
  }

  // - Possible ".Falaise/flsimulate.rc" Config

  // - CL Dialog Config
  try {
    do_cldialog(argc, argv, params);
  } catch (FLDialogHelpRequested& e) {
    throw FLConfigHelpHandled();
  } catch (FLDialogOptionsError& e) {
    throw FLConfigUserError();
  }
}

//----------------------------------------------------------------------
//! Perform simulation using command line args as given
falaise::exit_code do_flsimulate(int argc, char *argv[]) {
  // - Configure
  mctools::g4::manager_parameters flSimParameters;
  try {
    do_configure(argc, argv, flSimParameters);
  } catch (FLConfigDefaultError& e) {
    std::cerr << "Unable to configure core of flsimulate" << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  } catch (FLConfigHelpHandled& e) {
    return falaise::EXIT_OK;
  } catch (FLConfigUserError& e) {
    return falaise::EXIT_USAGE;
  }

  // - Run
  try {
    mctools::g4::manager flSimulation;
    mctools::g4::manager_parameters::setup(flSimParameters, flSimulation);
    flSimulation.run_simulation();
  } catch (std::exception& e) {
    std::cerr << "flsimulate : setup/run of simulation threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  return falaise::EXIT_OK;
}


//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // - Needed, but nasty
  FALAISE_INIT();

  // - Initialize and fix the resource paths
  FLSimulate::initResources();
  FLSimulate::doFixupResourcePaths();

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  falaise::exit_code ret = do_flsimulate(argc, argv);

  // - Needed, but nasty
  FALAISE_FINI();
  return ret;
}
