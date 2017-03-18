// Ourselves
#include "FLSimulateCommandLine.h"

// Third party:
// - Bayeux:
#include "bayeux/version.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/urn_query_service.h"

// This Project:
#include "falaise/version.h"
#include "falaise/detail/falaise_sys.h"
#include "falaise/common/user_profile.h"
#include "FLSimulateErrors.h"

namespace FLSimulate {

  namespace bpo = boost::program_options;

  // static
  FLSimulateCommandLine FLSimulateCommandLine::makeDefault()
  {
    FLSimulateCommandLine flClarg;
    flClarg.logLevel = datatools::logger::PRIO_ERROR;
    flClarg.configScript = "";
    flClarg.outputMetadataFile = "";
    flClarg.embeddedMetadata = false;
    flClarg.outputFile = "";
    flClarg.userProfile = "normal";
    return flClarg;
  }

  void do_version(std::ostream& os, bool isVerbose)
  {
    os << "flsimulate " << falaise::version::get_version() << "\n";
    if (isVerbose) {
      os << "\n"
         << "Copyright (C) 2013-2017 SuperNEMO Collaboration\n\n"
         << "flsimulate uses the following external libraries:\n"
         << "* Falaise : " << falaise::version::get_version() << "\n"
         << "* Bayeux  : " << bayeux::version::get_version() << "\n"
         << "* Boost   : " << BOOST_VERSION << "\n"
         << "* Geant4  : " << "9.6.4" << "\n"
         << "\n\n";
    }
    return;
  }

  void do_help(const bpo::options_description& od)
  {
    std::cout << "flsimulate (" << falaise::version::get_version() << ") : SuperNEMO simulation program\n";
    std::cout << "Usage:\n"
              << "  flsimulate [options]\n"
              << od
              << "\n";
    return;
  }

  void do_help_scripting(std::ostream& os)
  {
    // Eventually, this will autoformat
    os << "Scripting flsimulate\n"
       << "--------------------\n\n"
       << "The following subsystems of flsimulate may be configured using an input\n"
       << "datatools::multi_properties script. The allowed sections and parameters are:\n"
       << std::endl
       << "[section=\"SimulationSubsystem\" description=\"\"]\n"
       << "simulationUrn : string = \"urn:snemo:demonstrator:simulation:2.1\" \n"
       << "                                                 # URN of simulation setup\n"
       << "numberOfEvents : integer = 1                     # Number of events to simulate\n"
       << "rngSeedFile : string as path = \"seeds.conf\"      # Path to file containing random number seeds\n"
       << "outputProfile : string = \"\"                      # Output profile (hits collections to output)\n"
       << std::endl
       << "[section=\"VariantSubsystem\" description=\"\"]\n"
       << "profile : string as path = \"vprofile.conf\"       # Input variant profile configuration file.\n"
       << "                                                 # (this is the recommended path). \n"
       << "settings : string[N] = \"setting1\" ... \"settingN\" # Individual variant settings\n"
       << "                                                 # (should be reserved to experts). \n"
       << std::endl
       << "[section=\"ServicesSubsystem\" description=\"\"]\n"
       << "configUrn : string as path= \"services.conf\"       # Service manager profile configuration file.\n"
       << "config    : string as path = \"services.conf\"       # Service manager profile configuration file.\n"
       << std::endl
       << "All sections and parameters are optional, and flsimulate will supply sensible\n"
       << "default values when only some are set.\n"
       << std::endl;
    return;
  }

  void do_help_setup(std::ostream& os)
  {
    datatools::logger::priority logging = falaise::detail::falaise_sys::const_instance().get_logging();
    datatools::kernel & dtk = ::datatools::kernel::instance();
    if (dtk.has_urn_query()) {
      const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();
      if (datatools::logger::is_debug(logging)) {
        dtkUrnQuery.tree_dump(std::cerr, "Bayeux/datatools's kernel URN query service:", "[debug] ");
      }
      std::vector<std::string> flsim_urn_infos;
      if (dtkUrnQuery.find_urn_info(flsim_urn_infos,
                                    falaise::detail::falaise_sys::fl_setup_db_name(),
                                    "(urn:snemo:demonstrator:simulation:)([^:]*)",
                                    "configuration"
                                    )) {
        std::clog << "List of supported simulation setups:" << std::endl;
        for (size_t i = 0; i < flsim_urn_infos.size(); i++) {
          const datatools::urn_info & ui = dtkUrnQuery.get_urn_info(flsim_urn_infos[i]);
          os << ui.get_urn() << " : " << ui.get_description() << std::endl;
        }
      } else {
        DT_LOG_WARNING(logging, "Could not find any simulation setup from the global URN query service.");
      }
    }
    return;
  }

  void do_cldialog(int argc, char *argv[], FLSimulateCommandLine & clArgs)
  {
    // - Default Command Line
    clArgs = FLSimulateCommandLine::makeDefault();

    // Bind command line parser to exposed parameters
    std::string verbosityLabel;
    // Application specific options:
    bpo::options_description optDesc("Options");
    optDesc.add_options()
      ("help,h","print this help message")

      ("help-scripting","print help on input script format and schema")

      ("help-setup","print help on simulation setup")

      ("version","print version number")

      ("verbosity,V",
       bpo::value<std::string>(&verbosityLabel)->value_name("level"),
       "set the verbosity level")

      ("user-profile,u",
       bpo::value<std::string>(&clArgs.userProfile)->value_name("name")->default_value("normal"),
       "set the user profile (\"expert\", \"normal\", \"production\")")

      ("config,c",
       bpo::value<std::string>(&clArgs.configScript)->value_name("file"),
       "configuration script for simulation")

      ("output-metadata-file,m",
       bpo::value<std::string>(&clArgs.outputMetadataFile)->value_name("file"),
       "file in which to store metadata")

      ("embedded-metadata,E",
       bpo::value<bool>(&clArgs.embeddedMetadata)->value_name("flag")->default_value(false),
       "flag to (de)activate recording of metadata in the simulation results output file")

      ("output-file,o",
       bpo::value<std::string>(&clArgs.outputFile)->required()->value_name("file"),
       "file in which to store simulation results")

      ;

    // - Parse...
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
      bpo::notify(vMap);
    } catch (const bpo::required_option& e) {
      // We need to handle help/version even if required_option thrown
      if (!vMap.count("help") &&
          !vMap.count("version") &&
          !vMap.count("help-scripting") &&
          !vMap.count("help-setup")) {
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

    if (vMap.count("help-setup")) {
      do_help_setup(std::cout);
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

} // namespace FLSimulate
