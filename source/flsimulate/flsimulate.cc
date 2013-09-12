// flsimulate - SuperNEMO raw data reconstruction application
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
// - Bayeux
#include <bayeux/version.h>
#include <bayeux/datatools/logger.h>

// - Boost 
#include <boost/version.hpp>
#include <boost/program_options.hpp>

// This Project
#include <falaise/version.h>
#include "DumbSimulation.h"

//----------------------------------------------------------------------
// HACKY DEMO IMPLEMENTATION
//
namespace {
namespace bpo = boost::program_options;

//! Print version information ot an output stream
//! Add detailed information if isVerbose is true
void FLPrintVersion(std::ostream& os, bool isVerbose) {
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

//! Return string containing usage message to be printed when help displayed
std::string FLSimulateUsageMsg() {
  static std::string flUsageMsg;
  if (flUsageMsg.empty()) {
    std::ostringstream os;
    FLPrintVersion(os,false);
    os << "Usage:\n"
       << "  flsimulate [options]\n"
       << "Options";
    flUsageMsg = os.str();
  }
  return flUsageMsg;
}

//! Configure and start simulation
int FLRunSimulation(size_t events, const std::string& sink, bool isVerbose) {
  datatools::logger::priority logPriority = isVerbose ? datatools::logger::PRIO_INFORMATION : datatools::logger::PRIO_WARNING;

  DumbSimulation dummy;
  dummy.doConfigure(sink, events, logPriority);
  int errCode = dummy.doSimulate();
  return errCode;
}

//! Run the command line application logic
int FLSimulateImpl(int argc, char *argv[]) {
  // - Create options description
  bpo::options_description opDesc(FLSimulateUsageMsg());
  opDesc.add_options()
      ("help", "print this message")
      ("version", "print version number")
      ("verbose,v", "increase verbosity of output")
      ("number,n", 
       bpo::value<size_t>()->default_value(1)->value_name("[events]"),
       "number of events to simulate")
      ("output-file,o", 
       bpo::value<std::string>()->required()->value_name("[file]"),
       "path to output file")
      ;

  //--------------------------
  // - Parse command line and handle errors
  bpo::variables_map vMap; 
  try {
    bpo::store(bpo::parse_command_line(argc, argv, opDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    // We leave these for later processing
    if (!vMap.count("help") && !vMap.count("version")) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    return 1;
  }

  //----------------------------
  // - Process options as needed
  // Handle help
  if (vMap.count("help")) {
    std::cout << opDesc << "\n";
    return 0;
  }

  // Check for verbose
  bool isVerbose(vMap.count("verbose"));

  // Handle version
  if (vMap.count("version")) {
    FLPrintVersion(std::cout, isVerbose);
    return 0;
  }

  // Handle event count
  size_t eventsToProcess(vMap["number"].as<size_t>());

  // Handle filenames, if any
  std::string outFile;
  if (vMap.count("output-file")) {
    outFile = vMap["output-file"].as<std::string>();
  }

  int processingCode = FLRunSimulation(eventsToProcess, outFile, isVerbose);
  return processingCode;
}
} // namespace

//----------------------------------------------------------------------
// Here's the main
//
int main(int argc, char *argv[]) {
  int ret_code = FLSimulateImpl(argc, argv);
  return ret_code;
}

