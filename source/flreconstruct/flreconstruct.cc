// flreconstruct - SuperNEMO raw data reconstruction application
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

//----------------------------------------------------------------------
// HACKY DEMO IMPLEMENTATION
//
namespace {
  namespace bpo = boost::program_options;
}

//! Print version information ot an output stream
//! Add detailed information if isVerbose is true
void FLPrintVersion(std::ostream& os, bool isVerbose) {
  os << "flreconstruct " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
        << "Copyright (C) 2013 SuperNEMO Collaboration\n\n"
        << "Falaise uses the following external libraries:\n"
        << "* Bayeux : " << bayeux::version::get_version() << "\n"
        << "* Boost  : " << BOOST_VERSION 
        << "\n\n";
  }
}

//! Return string containing usage message to be printed when help displayed
std::string FLReconstructUsageMsg() {
  static std::string flRecoMsg;
  if (flRecoMsg.empty()) {
    std::ostringstream os;
    FLPrintVersion(os,false);
    os << "Usage:\n"
       << "  flreconstruct [options]\n"
       << "Options";
    flRecoMsg = os.str();
  }
  return flRecoMsg;
}

//! Process data from input file to output file
int FLProcessData(const std::string& source, 
                  const std::string& sink,
                  bool isVerbose) {
  datatools::logger::priority logPriority = isVerbose ? datatools::logger::PRIO_INFORMATION : datatools::logger::PRIO_WARNING;

  DT_LOG_INFORMATION(logPriority,"processing startup");

  DT_LOG_INFORMATION(logPriority,"data source : " << source);
  DT_LOG_INFORMATION(logPriority,"data sink   : " << sink);

  DT_LOG_INFORMATION(logPriority,"processing shutdown");
  return 0;
}

//! Run the command line application logic
int FLReconstructImpl(int argc, char *argv[]) {
  // - Create options description
  bpo::options_description opDesc(FLReconstructUsageMsg());
  opDesc.add_options()
      ("help", "print this message")
      ("version", "print version number")
      ("verbose,v", "increase verbosity of output")
      ("input-file,i", bpo::value<std::string>(), "path to input file")
      ("output-file,o", bpo::value<std::string>(),"path to output file")
      ;

  // - Parse and handle errors
  bpo::variables_map vMap; 
  try {
    bpo::store(bpo::parse_command_line(argc, argv, opDesc), vMap);
    bpo::notify(vMap);
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    return 1;
  }

  // - Process options as needed
  // Handle help
  if (vMap.count("help")) {
      std::cout << opDesc << "\n";
      return 1;
  }

  // Check for verbose
  bool isVerbose(vMap.count("verbose"));

  // Handle version
  if (vMap.count("version")) {
    FLPrintVersion(std::cout, isVerbose);
    return 0;
  }

  // Handle filenames, if any
  std::string inFile;
  std::string outFile;
 
  if (vMap.count("input-file")) {
    inFile = vMap["input-file"].as<std::string>();
  }
  if (vMap.count("output-file")) {
    outFile = vMap["output-file"].as<std::string>();
  }

  int processingCode = FLProcessData(inFile, outFile, isVerbose);

  return processingCode;
}


//----------------------------------------------------------------------
// Here's the main
//
int main(int argc, char *argv[]) {
  int ret_code = FLReconstructImpl(argc, argv);
  return ret_code;
}

