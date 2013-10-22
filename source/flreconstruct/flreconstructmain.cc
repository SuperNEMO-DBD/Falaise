//! \file    flsimulatemain.cc
//! \brief   Main program for flreconstruct command line application
//! \details Configure, setup and run the bayeux::dpp based pipeline
//!          reconstruction of SuperNEMO data.
//!
//!          We need to:
//!          - Construct an input and output module (fixed)
//!          - Input should select itself based on input file type
//!          - Output is a trashcan (devnull) - Null output_module
//!            should handle this
//!          - Only module for now is a module that cats the output
//!            to screen - dump module should handle this.
//!          - Probably have to hard code some things in...
//!
//!          Need to write our own version of dpp_driver to make
//!          interface usable.
//
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
#include "bayeux/version.h"
#include "bayeux/dpp/module_manager.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/dpp/input_module.h"

// - Boost
#include "boost/program_options.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/version.hpp"


// This Project
#include "falaise/falaise.h"
#include "falaise/version.h"
#include "falaise/exitcodes.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------
namespace {
  namespace bpo = boost::program_options;
}

struct FLReconstructArgs {
  std::string inputFile;
  std::string outputFile;
  std::string pipelineScript;
};

enum FLDialogState {
  DIALOG_OK,
  DIALOG_QUERY,
  DIALOG_ERROR,
};

//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose) {
  os << "flreconstruct " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
        << "Copyright (C) 2013 SuperNEMO Collaboration\n\n"
        << "flreconstruct uses the following external libraries:\n"
        << "* Falaise : " << falaise::version::get_version() << "\n"
        << "* Bayeux  : " << bayeux::version::get_version() << "\n"
        << "* Boost   : " << BOOST_VERSION << "\n"
        << "\n\n";
  }
}

//! Handle printing of help message to given ostream
void do_help(std::ostream& os, const bpo::options_description& od) {
  do_version(os, false);
  os << "Usage:\n"
     << "  flreconstruct [options]\n"
     << "Options\n"
     << od
     << "\n";
}

void do_error(std::ostream& os, const char* err) {
  os << "flreconstruct : " << err << "\n";
  os << "Try `flreconstruct --help` for more information\n";
}

FLDialogState do_cldialog(int argc, char *argv[], FLReconstructArgs& args) {
  // Bind command line parser to exposed parameters
  namespace bpo = boost::program_options;
  bpo::options_description optDesc;
  optDesc.add_options()
      ("help,h","print this help message")
      ("version","print version number")
      ("verbose,v","increase verbosity of logging")
      ("input-file,i",
       bpo::value<std::string>(&args.inputFile)->required()->value_name("[file]"),
       "file from which to read data")
      ("output-file,o",
       bpo::value<std::string>(&args.outputFile)->value_name("[file]"),
       "file to which to write data")
      ("pipeline,p",
       bpo::value<std::string>(&args.pipelineScript)->value_name("[file]"),
       "run pipeline script")
      ;
  bpo::positional_options_description posOptDesc;
  posOptDesc.add("input-file",-1);

  // - Store first, handling parse errors
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::command_line_parser(argc,argv)
               .options(optDesc)
               .positional(posOptDesc)
               .run(),
               vMap);
  } catch (boost::program_options::error& e) {
    do_error(std::cerr, e.what());
    return DIALOG_ERROR;
  }

  // Handle messaging if requested
  if (vMap.count("help")) {
    do_help(std::cout, optDesc);
    return DIALOG_QUERY;
  }

  if (vMap.count("version")) {
    do_version(std::cout, true);
    return DIALOG_QUERY;
  }

  // Notify, handling validation errors
  try {
    bpo::notify(vMap);
  } catch (boost::program_options::error& e) {
    do_error(std::cerr, e.what());
    return DIALOG_ERROR;
  }

  return DIALOG_OK;
}

//! Configure and run the pipeline
falaise::exit_code do_pipeline(const FLReconstructArgs& clArgs) {
  // Dual strategy here
  //  - If they supplied a script, use that, otherwise default to
  //  a single dump module.
  //  - An ass to deal with because of use of properties...
  boost::scoped_ptr<dpp::module_manager> moduleManager_(new dpp::module_manager);
  datatools::properties moduleManagerConfig;

  if (!clArgs.pipelineScript.empty()) {
    std::vector<std::string> moduleFiles;
    moduleFiles.push_back(clArgs.pipelineScript);
    moduleManagerConfig.store("modules.configuration_files", moduleFiles);
  } else {
    // Hand configure a dumb dump module
    datatools::properties dumbConfig;
    dumbConfig.store("title", "flreconstruct::default");
    dumbConfig.store("output","cout");
    moduleManager_->load_module("pipeline", "dpp::dump_module", dumbConfig);
  }

  moduleManager_->initialize(moduleManagerConfig);
  moduleManager_->tree_dump();

  // - Can't configure these until we've handled the manager
  const dpp::base_module& pipeline_ = moduleManager_->get("pipeline");

  boost::scoped_ptr<dpp::input_module> input_(new dpp::input_module);
  boost::scoped_ptr<dpp::output_module> output_(new dpp::output_module);


  return falaise::EXIT_OK;
}

//----------------------------------------------------------------------
//! Run reconstruction using command line args as given
falaise::exit_code do_flreconstruct(int argc, char *argv[]) {
  // - Configure
  // Command line args first because we don't have a strategy for
  // what to do without these
  FLReconstructArgs clArgs;
  FLDialogState d = do_cldialog(argc, argv, clArgs);
  switch (d) {
    case DIALOG_QUERY:
      return falaise::EXIT_OK;
      ;;
    case DIALOG_ERROR:
      return falaise::EXIT_USAGE;
      ;;
    default:
      ;;
  }

  falaise::exit_code ret = do_pipeline(clArgs);
  return ret;
}

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // - Needed...
  FALAISE_INIT();

  // - Do the reconstruction
  // Ideally, exceptions should not propagate out of this - the error
  // code should be enough, but might want a catch all...
  falaise::exit_code ret = do_flreconstruct(argc, argv);

  // - Needed...
  FALAISE_FINI();
  return ret;
}

