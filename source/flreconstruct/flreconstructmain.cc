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
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/library_loader.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/dpp/module_manager.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/dpp/input_module.h"
#include "bayeux/dpp/output_module.h"
#include "bayeux/geomtools/manager.h"

// - Boost
#include "boost/program_options.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/version.hpp"
#include "boost/foreach.hpp"


// This Project
#include "falaise/falaise.h"
#include "falaise/version.h"
#include "falaise/exitcodes.h"
#include "FLReconstructResources.h"

//----------------------------------------------------------------------
// IMPLEMENTATION DETAILS
//----------------------------------------------------------------------
namespace {
  namespace bpo = boost::program_options;
}

struct FLReconstructArgs {
  datatools::logger::priority logLevel;
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
        << "Copyright (C) 2013-2014 SuperNEMO Collaboration\n\n"
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

//! Print list of known module names, one per line, to given stream
void do_module_list(std::ostream& os) {
  typedef std::vector<std::string> ModuleInfo;
  ModuleInfo mods;
  DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(dpp::base_module).list_of_factories(mods);
  BOOST_FOREACH(ModuleInfo::value_type entry, mods) {
    os << entry << std::endl;
  }
}

//! Print OCD help for supplied module name to given ostream
void do_help_module(std::ostream& os, std::string module) {
  // Is module valid?
  typedef std::vector<std::string> ModuleInfo;
  ModuleInfo mods;
  DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(dpp::base_module).list_of_factories(mods);
  std::set<std::string> moduleSet(mods.begin(), mods.end());
  if (moduleSet.find(module) == moduleSet.end()) {
    os << "[error] Argument '" << module
       << "' is not a flreconstruct module\n"
       << "Use '--help-module-list' to see all modules"
       << std::endl;
    return;
  }

  // Is it in OCD?
  const datatools::detail::ocd::ocd_registration& ocd_system_reg
    = datatools::detail::ocd::ocd_registration::get_system_registration();
  if (!ocd_system_reg.has_id(module)) {
    os << "[error] Module '" << module
       << "' is not documented"
       << std::endl;
    return;
  }

  const datatools::object_configuration_description& moduleDoc
      = ocd_system_reg.get(module);
  moduleDoc.print(os);
}

// - Validation of verbosity command line arguments. must exist inside
// the datatools namespace.
// TODO : refactor operator>> into datatools, though can't do this
// for validator (bpo dependency not wanted)
namespace datatools {
std::istream& operator>>(std::istream& in, datatools::logger::priority& p) {
  std::string s;
  in >> s;
  p = datatools::logger::get_priority(s);
  return in;
}

//! validate logging argument
void validate(boost::any& v,
              std::vector<std::string> const& values,
              datatools::logger::priority* /*target_type*/,
              int) {
  // Make sure no previous assignment to v was made
  bpo::validators::check_first_occurrence(v);

  // Extract first string from values, If there is more than one string
  // it's an error and an exception will be thrown
  std::string const& s = bpo::validators::get_single_string(values);
  datatools::logger::priority p = datatools::logger::get_priority(s);
  if(p != datatools::logger::PRIO_UNDEFINED) {
    v = boost::any(p);
  } else {
    throw bpo::validation_error(bpo::validation_error::invalid_option_value);
  }
}
}

FLDialogState do_cldialog(int argc, char *argv[], FLReconstructArgs& args) {
  // Bind command line parser to exposed parameters
  namespace bpo = boost::program_options;
  bpo::options_description optDesc;
  optDesc.add_options()
      ("help,h","print this help message")
      ("help-module-list","list available modules and exit")
      ("help-module", bpo::value<std::string>()->value_name("[mod]"),
       "print help for a single module and exit")
      ("version","print version number")
      ("verbose,v",
       bpo::value<datatools::logger::priority>(&args.logLevel)->default_value(datatools::logger::PRIO_FATAL)->value_name("[level]"),
       "set verbosity level of logging")
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

  if (vMap.count("help-module-list")) {
    do_module_list(std::cout);
    return DIALOG_QUERY;
  }

  if (vMap.count("help-module")) {
    do_help_module(std::cout, vMap["help-module"].as<std::string>());
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

//! Extract metadata from input file
datatools::multi_properties do_get_metadata(const std::string& file) {
  // Metadata is currently available as soon as the input module is
  // initialized.
  boost::scoped_ptr<dpp::input_module> recInput(new dpp::input_module);
  recInput->set_single_input_file(file);
  // Input metadata management:
  const datatools::multi_properties& iMetadataStore = recInput->get_metadata_store();
  recInput->initialize_simple();
  return iMetadataStore;
}

//! Hack services to start using input metadata
void do_start_services(datatools::multi_properties& iData,
                       datatools::service_manager& services) {

  datatools::multi_properties serviceConfig("name","type");

  // Geometry
  datatools::properties& gs = serviceConfig.add_section("geometry", "geomtools::geometry_service");
  gs.store("manager.configuration_file", iData.get_section("geometry").fetch_path("manager.config"));

  // Init
  services.load(serviceConfig);
  services.initialize();
}


//! Configure and run the pipeline
falaise::exit_code do_pipeline(const FLReconstructArgs& clArgs) {
  // Need a service manager
  datatools::service_manager flrServices;

  // - Start up the module manager
  // Dual strategy here
  //  - If they supplied a script, use that, otherwise default to
  //  a single dump module.
  DT_LOG_TRACE(clArgs.logLevel,"configuring module_manager");
  boost::scoped_ptr<dpp::module_manager> moduleManager_(new dpp::module_manager);
  moduleManager_->set_service_manager(flrServices);

  // - Configure the library loader for custom modules
  datatools::multi_properties userLibConfig("name", "filename");
  if(!clArgs.pipelineScript.empty()) {
    datatools::multi_properties userConfig("name", "type");
    userConfig.read(clArgs.pipelineScript);
    try {
      datatools::properties userFLPlugins = userConfig.get_section("flreconstruct.plugins");
      std::vector<std::string> pList;
      userFLPlugins.fetch("plugins", pList);

      BOOST_FOREACH(std::string plugin, pList) {
        datatools::properties& pSection = userLibConfig.add_section(plugin,"");
        userFLPlugins.export_and_rename_starting_with(pSection,plugin+".","");
        pSection.store_flag("autoload");
        // - Default, search plugin DLL in the falaise plugin install directory:
        if (!pSection.has_key("directory")) {
          pSection.store_string("directory", "@falaise.plugins:");
        }
      }
    } catch(std::logic_error& e) {
      // do nothing for now because we can't distinguish errors, and
      // upcoming instantiation of library loader will handle
      // any syntax errors in the properties
    }
    // userConfig.tree_dump(std::cerr, "User config: ", "DEVEL: ");
  }
  // userLibConfig.tree_dump(std::cerr, "User library config: ", "DEVEL: ");

  datatools::library_loader flLibLoader(userLibConfig);

  // Configure the modules themselves
  if (!clArgs.pipelineScript.empty()) {
    moduleManager_->load_modules(clArgs.pipelineScript);
  } else {
    // Hand configure a dumb dump module
    datatools::properties dumbConfig;
    dumbConfig.store("title", "flreconstruct::default");
    dumbConfig.store("output", "cout");
    moduleManager_->load_module("pipeline", "dpp::dump_module", dumbConfig);
  }

  // Plain initialization
  moduleManager_->initialize_simple();

  // Metadata and services - has to be done before we grab the pipeline
  datatools::multi_properties mData = do_get_metadata(clArgs.inputFile);
  do_start_services(mData, flrServices);

  // Input module...
  boost::scoped_ptr<dpp::input_module> recInput(new dpp::input_module);
  DT_LOG_TRACE(clArgs.logLevel,"configuring input module");
  recInput->set_single_input_file(clArgs.inputFile);
  recInput->initialize_simple();
  // mData = recInput->get_metadata_store();

  // - Pipeline
  dpp::base_module* pipeline_;
  try {
    pipeline_ = &(moduleManager_->grab("pipeline"));
  } catch (std::exception& e) {
    DT_LOG_FATAL(clArgs.logLevel, "Failed to initialize pipeline : " << e.what());
    return falaise::EXIT_UNAVAILABLE;
  }

  // Output module... only if file was passed
  boost::scoped_ptr<dpp::output_module> recOutput;
  if (!clArgs.outputFile.empty()) {
    recOutput.reset(new dpp::output_module);
    recOutput->set_name("FLReconstructOutput");
    recOutput->set_single_output_file(clArgs.outputFile);
    // Metadata management:
    // Fetch the metadata to be stored through the output module
    datatools::multi_properties & metadataStore = recOutput->grab_metadata_store();
    // Copy metadata from the input module
    metadataStore = mData;
    // TO DO: add more sections in metadata (from userConfig) ...
    recOutput->initialize_simple();
  }

  // - Now the actual event loop
  DT_LOG_TRACE(clArgs.logLevel,"begin event loop");

  datatools::things workItem;
  while (true) {
    // Prepare and read work
    workItem.clear();
    if(recInput->is_terminated()) break;
    if(recInput->process(workItem) != dpp::base_module::PROCESS_OK) {
      DT_LOG_FATAL(clArgs.logLevel,"Failed to read data record from input source");
      break;
    }

    // Feed through pipeline
    dpp::base_module::process_status pStatus = pipeline_->process(workItem);
    // TO DO:
    // Here the meaning of STOP, ERROR and FATAL (from dpp::base_module::process_status)
    // needs to be clarified.
    // Should be :
    // if(pStatus == dpp::base_module::PROCESS_STOP) break;
    // else if(pStatus == dpp::base_module::PROCESS_ERROR_STOP) break;
    // else if(pStatus == dpp::base_module::PROCESS_ERROR) continue;

    // FATAL means actual processing has failed on current item
    if(pStatus == dpp::base_module::PROCESS_FATAL) continue;

    // INVALID means something very badly wrong, so need to exit whole
    // event loop.
    // Should be :
    // if(pStatus == dpp::base_module::PROCESS_FATAL) break;
    if(pStatus == dpp::base_module::PROCESS_INVALID) break;

    // Write item
    if(recOutput) {
      pStatus = recOutput->process(workItem);
      if(pStatus != dpp::base_module::PROCESS_OK) {
        DT_LOG_FATAL(clArgs.logLevel,"Failed to read data record from input source");
        break;
      }
    }
  }
  DT_LOG_TRACE(clArgs.logLevel,"event loop completed");

  // - MUST delete the module manager BEFORE the library loader clears
  // in case the manager is holding resources created from a shared lib
  moduleManager_.reset();
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
  FLReconstruct::initResources();

  // - Do the reconstruction
  // Ideally, exceptions should not propagate out of this - the error
  // code should be enough, but might want a catch all...
  falaise::exit_code ret = do_flreconstruct(argc, argv);

  // - Needed...
  FALAISE_FINI();
  return ret;
}
