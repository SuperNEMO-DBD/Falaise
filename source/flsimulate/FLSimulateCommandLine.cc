// Ourselves
#include "FLSimulateCommandLine.h"

// Third party:
// - Bayeux:
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/version.h"

// This Project:
#include "FLSimulateErrors.h"
#include "FLSimulateUtils.h"
#include "falaise/common/user_profile.h"
#include "falaise/detail/falaise_sys.h"
#include "falaise/version.h"

namespace FLSimulate {

namespace bpo = boost::program_options;

// static
FLSimulateCommandLine FLSimulateCommandLine::makeDefault() {
  FLSimulateCommandLine flClarg;
  flClarg.logLevel = datatools::logger::PRIO_ERROR;
  flClarg.configScript = "";
  flClarg.outputMetadataFile = "";
  flClarg.embeddedMetadata = true;
  flClarg.outputFile = "";
  flClarg.userProfile = "normal";
  return flClarg;
}

void do_version(std::ostream& os, bool isVerbose) {
  std::string commitInfo{};
  if (falaise::version::get_commit() != "") {
    commitInfo = " (" + falaise::version::get_commit();
    commitInfo += falaise::version::is_dirty() ? "-dirty" : "";
    commitInfo += ")";
  }

  os << "flsimulate " << falaise::version::get_version() << commitInfo << "\n";
  if (isVerbose) {
    os << "\n"
       << "Copyright (C) 2013-2017 SuperNEMO Collaboration\n\n"
       << "flsimulate uses the following external libraries:\n"
       << "* Falaise : " << falaise::version::get_version() << commitInfo << "\n"
       << "* Bayeux  : " << bayeux::version::get_version() << "\n"
       << "* Boost   : " << BOOST_VERSION << "\n"
       << "* Geant4  : "
       << "9.6.4"
       << "\n"
       << "\n\n";
  }
  return;
}

void do_help(const bpo::options_description& od) {
  std::cout << "flsimulate (" << falaise::version::get_version()
            << ") : SuperNEMO simulation program\n";
  std::cout << "Usage:\n"
            << "  flsimulate [options]\n"
            << od << "\n";
  return;
}

void do_help_scripting(std::ostream& os) {
  // Eventually, this will autoformat
  os << "Scripting flsimulate\n"
     << "--------------------\n\n"
     << "The following subsystems of flsimulate may be configured using an input\n"
     << "datatools::multi_properties script. Some allowed sections and parameters are:\n"
     << std::endl
     << "#@description FLSimulate configuration script\n"
     << "#@key_label  \"name\"\n"
     << "#@meta_label \"type\"\n"
     << std::endl
     << "[name=\"flsimulate\" type=\"flsimulate::section\"]\n"
     << "numberOfEvents : integer = 1                     # Number of events to simulate\n"
     << std::endl
     << "[name=\"flsimulate.simulation\" type=\"flsimulate::section\"]\n"
     << "simulationSetupUrn : string = \"" << default_simulation_setup() << "\" \n"
     << "                                                 # URN of simulation setup\n"
     << "rngSeedFile : string as path = \"seeds.conf\"      # Path to file containing random "
        "number seeds\n"
     << std::endl
     << "[name=\"flsimulate.variantService\" type=\"flsimulate::section\"]\n"
     << "profile : string as path = \"vprofile.conf\"       # Input variant profile configuration "
        "file.\n"
     << "                                                 # (this is the recommended path). \n"
     << "settings : string[N] = \"setting1\" ... \"settingN\" # Individual variant settings\n"
     << "                                                 # (should be reserved to experts). \n"
     << std::endl
     << "All sections and parameters are optional, and flsimulate will supply sensible\n"
     << "default values when only some are set.\n"
     << std::endl;
  return;
}

void do_help_simulation_setup(std::ostream& os) {
  std::map<std::string, std::string> m = ::FLSimulate::list_of_simulation_setups();
  std::clog << "List of supported simulation setups: ";
  if (m.empty()) std::clog << "<empty>";
  std::clog << std::endl;
  for (const auto& entry : m) {
    os << entry.first << " : ";
    if (entry.second.empty()) {
      os << "<no available description>";
    } else {
      os << entry.second;
    }
    os << std::endl;
  }
  return;
}

void do_cldialog(int argc, char* argv[], FLSimulateCommandLine& clArgs) {
  // - Default Command Line
  clArgs = FLSimulateCommandLine::makeDefault();

  // Bind command line parser to exposed parameters
  std::string verbosityLabel;
  // Application specific options:
  bpo::options_description optDesc("Options");
  optDesc.add_options()("help,h", "print this help message")

      ("help-scripting", "print help on input script format and schema")

          ("help-simulation-setup", "print help on simulation setup")

              ("version", "print version number")

                  ("verbosity,V", bpo::value<std::string>(&verbosityLabel)->value_name("level"),
                   "set the verbosity level\n"
                   "Example: \n"
                   "  -V \"debug\" ")

                      ("user-profile,u",
                       bpo::value<std::string>(&clArgs.userProfile)
                           ->value_name("name")
                           ->default_value("normal"),
                       "set the user profile (\"expert\", \"normal\", \"production\")")

                          ("mount-directory,d",
                           bpo::value<std::vector<std::string>>(&clArgs.mountPoints)
                               ->value_name("rule"),
                           "register directories' mount points\n"
                           "Example: \n"
                           "  -d \"nemoprod@/etc/nemoprod/config\" \n"
                           "  -d \"nemoprod.data@/data/nemoprod/runs\"")

                              ("config,c",
                               bpo::value<std::string>(&clArgs.configScript)->value_name("file"),
                               "configuration script for simulation\n"
                               "Examples: \n"
                               "  -c \"simu.conf\" \n"
                               "  -c \"${WORKER_DIR}/config/simu1.conf\"")

                                  ("output-metadata-file,m",
                                   bpo::value<std::string>(&clArgs.outputMetadataFile)
                                       ->value_name("file"),
                                   "file in which to store metadata\n"
                                   "Example:\n"
                                   "  -m \"simu.meta\"")

                                      ("embedded-metadata,E",
                                       bpo::value<bool>(&clArgs.embeddedMetadata)
                                           ->value_name("flag")
                                           ->default_value(true),
                                       "flag to (de)activate recording of metadata in the "
                                       "simulation results output file")

                                          ("output-file,o",
                                           bpo::value<std::string>(&clArgs.outputFile)
                                               ->required()
                                               ->value_name("file"),
                                           "file in which to store simulation results\n"
                                           "Examples:\n"
                                           "  -o \"example.brio\" \n"
                                           "  -o \"${WORKER_DIR}/data/run_1.xml\"")

      ;

  // - Parse...
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    if (!vMap.count("help") && !vMap.count("version") && !vMap.count("help-scripting") &&
        !vMap.count("help-simulation-setup")) {
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

  if (vMap.count("help-simulation-setup")) {
    do_help_simulation_setup(std::cout);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("verbosity")) {
    clArgs.logLevel = datatools::logger::get_priority(verbosityLabel);
    if (clArgs.logLevel == datatools::logger::PRIO_UNDEFINED) {
      throw FLDialogOptionsError();
    }
  }

  if (!falaise::common::supported_user_profiles().count(clArgs.userProfile)) {
    DT_THROW(FLDialogOptionsError, "Invalid user profile '" << clArgs.userProfile << "'");
  }

  return;
}

}  // namespace FLSimulate
