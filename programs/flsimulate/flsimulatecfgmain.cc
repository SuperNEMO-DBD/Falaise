//! \file    flsimulatecfgmain.cc
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
#include <vector>

// Third Party
// - Boost
#include "boost/program_options.hpp"
// - Bayeux
#include <bayeux/version.h>
#include <bayeux/datatools/urn_query_service.h>
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/urn.h"

// This Project
#include "FLSimulateErrors.h"
#include "FLSimulateUtils.h"
#include "falaise/exitcodes.h"
#include "falaise/falaise.h"
#include "falaise/tags.h"
#include "falaise/version.h"

//----------------------------------------------------------------------
// DECLARATIONS
//----------------------------------------------------------------------
namespace FLSimulateConfig {

//! \brief The parameters we can receive from the command line
// Help and so on are not marked because these are handled by the UI.
struct FLSimulateConfigureCommandLine {
  datatools::logger::priority logLevel;  //!< Logging priority threshold
  std::string simulationSetupUrn;
  std::string inputVariantProfileUrn;
  std::string inputVariantProfile;
  std::vector<std::string> inputVariantSettings;
  std::string outputVariantProfile;
#if DATATOOLS_WITH_QT_GUI == 1
  bool variantGui;
#endif  // DATATOOLS_WITH_QT_GUI == 1
#if BAYEUX_VERSION >= 30405 
  bool uiMutableAtStart = false;
  bool uiHideSecondariesAtStart = false;
#endif // BAYEUX_VERSION >= 30405
  static FLSimulateConfigureCommandLine makeDefault();
};

//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose);

//! Handle printing of help message to screen
void do_help(const boost::program_options::options_description& od);

//! Command line help on simulation setup
void do_help_simulation_setup(std::ostream& os);

//! Handle command line argument dialog
void do_cldialog(int argc, char* argv[], FLSimulateConfigureCommandLine& clArgs);

//! \brief Collect all needed configuration parameters in one data structure
struct FLSimulateConfigureParams {
  datatools::logger::priority logLevel; //!< Logging priority threshold
  std::string simulationSetupUrn;
  std::string inputVariantProfileUrn;
  std::string variantConfigUrn;
  datatools::configuration::variant_service::config variantServiceConfig;
  static FLSimulateConfigureParams makeDefault();
};

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc, char* argv[], FLSimulateConfigureParams& params);

//! Perform simulation using command line args as given
falaise::exit_code do_flsimulate_config(int argc, char* argv[]);

}  // end of namespace FLSimulateConfig

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  falaise::initialize(argc, argv);

  // - Do the configuration.
  falaise::exit_code ret = FLSimulateConfig::do_flsimulate_config(argc, argv);

  falaise::terminate();
  return ret;
}

//----------------------------------------------------------------------
// DEFINTIONS
//----------------------------------------------------------------------
namespace FLSimulateConfig {

// static
FLSimulateConfigureCommandLine FLSimulateConfigureCommandLine::makeDefault() {
  FLSimulateConfigureCommandLine flClarg;
  flClarg.logLevel = datatools::logger::PRIO_ERROR;
  flClarg.simulationSetupUrn = "";
  flClarg.inputVariantProfile = "";
  flClarg.inputVariantSettings.clear();
  flClarg.outputVariantProfile = "";
#if DATATOOLS_WITH_QT_GUI == 1
  flClarg.variantGui = true;
#endif  // DATATOOLS_WITH_QT_GUI == 1
  return flClarg;
}

// static
FLSimulateConfigureParams FLSimulateConfigureParams::makeDefault() {
  FLSimulateConfigureParams pars;
  pars.logLevel = datatools::logger::PRIO_FATAL;
  pars.simulationSetupUrn = "";
  pars.variantConfigUrn = "";
  pars.variantServiceConfig.logging = datatools::logger::get_priority_label(pars.logLevel);
  pars.variantServiceConfig.config_filename = "";
  pars.variantServiceConfig.profile_load = "";
  pars.variantServiceConfig.profile_load_dont_ignore_unknown = false;
  pars.variantServiceConfig.settings.clear();
  pars.variantServiceConfig.profile_store = "";
#if DATATOOLS_WITH_QT_GUI == 1
  pars.variantServiceConfig.gui = true;
#endif                                    // DATATOOLS_WITH_QT_GUI == 1
  pars.variantServiceConfig.tui = false;  // Not implemented yet
  return pars;
}

void do_help(const boost::program_options::options_description& od) {
  std::cout << "flsimulate-configure (" << falaise::version::get_version()
            << ") : SuperNEMO simulation profile configuration program\n";
  std::cout << "\n";
  std::cout << "  The flsimulate-configure program provides an interface to build \n";
  std::cout << "  a simulation variant profile on top of a simulation setup.      \n\n";
  std::cout << "Usage:\n"
            << "  flsimulate-configure [options]\n"
            << od << "\n";
  std::cout << "Examples:\n\n";
  std::cout << "  $ flsimulate-configure -o myprofile.conf \n\n";
  std::cout << "  $ flsimulate-configure  \\\n"
            << "      -t \"" << FLSimulate::default_simulation_setup() << "\" \\\n"
            << "      -o myprofile.conf \n\n";
  std::cout << "  $ flsimulate-configure \\\n"
            << "      --no-gui \\\n"
            << "      -t \"" << FLSimulate::default_simulation_setup() << "\" \\\n"
            << "      -i oldprofile.conf \\\n"
            << "      -s \"geometry:layout/if_basic/magnetic_field=false\" \\\n"
            << "      -o myprofile.conf \n\n";
}

void do_help_simulation_setup(std::ostream& os) {
  std::map<std::string, std::string> m = ::FLSimulate::list_of_simulation_setups();
  std::clog << "List of supported simulation setups: ";
  if (m.empty()) {
    std::clog << "<empty>";
  }
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
}

void do_version(std::ostream& os, bool /* isVerbose */) {
  os << "flsimulate-configure " << falaise::version::get_version() << "\n";
}

void do_cldialog(int argc, char* argv[], FLSimulateConfigureCommandLine& clArgs) {
  namespace bpo = boost::program_options;

  // - Default Command Line
  clArgs = FLSimulateConfigureCommandLine::makeDefault();

  // Bind command line parser to exposed parameters
  std::string verbosityLabel;
  // Application specific options:
  // clang-format off
  bpo::options_description optDesc("Options");
  optDesc.add_options()("help,h", "print this help message")

    ("help-simulation-setup", "print help on available simulation setups")

    ("version", "print version number")

    ("verbosity,V", bpo::value<std::string>(&verbosityLabel)->value_name("level"),
    "set the verbosity level")

    ("setup-tag,t",
    bpo::value<std::string>(&clArgs.simulationSetupUrn)
    ->value_name("urn")
    ->default_value(FLSimulate::default_simulation_setup()),
    "tag of the simulation setup on top of which the generated variant profile "
    "should be applied")

    ("input-profile-tag,p",
    bpo::value<std::string>(&clArgs.inputVariantProfileUrn)->value_name("urn"),
    "tag of the input variant profile used as a base for the generated profile")

    ("input-profile,i",
    bpo::value<std::string>(&clArgs.inputVariantProfile)->value_name("file"),
    "file from which to load a variant profile used as a base for the "
    "generated profile")

    ("setting,s",
    bpo::value<std::vector<std::string>>(&clArgs.inputVariantSettings)
    ->value_name("setting"),
    "apply variant setting directives to the new profile           \n"
    "Example:                                                      \n"
    "\t--setting=\"geometry:layout=Basic\"                         \n"
    "\t--setting=\"geometry:layout/if_basic/magnetic_field=false\" \n")

    ("output-profile,o",
    bpo::value<std::string>(&clArgs.outputVariantProfile)
    ->value_name("file"),
    "file in which to store the generated variant profile")

#if DATATOOLS_WITH_QT_GUI == 1
    ("gui", bpo::value<bool>()->zero_tokens(),
    "activate the variant service's GUI from which to browse "
    "and edit the variant profile")

    ("no-gui", bpo::value<bool>()->zero_tokens(),
    "deactivate the variant service's GUI")
    
#if BAYEUX_VERSION >= 30405 
    // New feature (issue #233) with Bayeux 3.4.5
    ("ui-mutable-at-start,M", bpo::value<bool>()->zero_tokens(),
    "start the variant UI in writable mode")

    ("ui-hide-secondaries-at-start,H", bpo::value<bool>()->zero_tokens(),
    "start the variant UI hiding parameters' secondary choices (if applicable)")
#endif //BAYEUX_VERSION >= 30405 

#endif  // DATATOOLS_WITH_QT_GUI == 1
  ;
  // clang-format on

  // - Parse...
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    if (vMap.count("help") == 0u && vMap.count("version") == 0u &&
        vMap.count("help-simulation-setup") == 0u) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      throw FLSimulate::FLDialogOptionsError();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLSimulate::FLDialogOptionsError();
  }

  // Handle any non-bound options
  if (vMap.count("help") != 0u) {
    do_help(optDesc);
    throw FLSimulate::FLDialogHelpRequested();
  }

  if (vMap.count("help-simulation-setup") != 0u) {
    do_help_simulation_setup(std::cout);
    throw FLSimulate::FLDialogHelpRequested();
  }

  if (vMap.count("verbosity") != 0u) {
    clArgs.logLevel = datatools::logger::get_priority(verbosityLabel);
    if (clArgs.logLevel == datatools::logger::PRIO_UNDEFINED) {
      throw FLSimulate::FLDialogOptionsError();
    }
  }

  if (vMap.count("input-profile-tag") != 0u && vMap.count("input-profile") != 0u) {
    std::cerr << "[OptionsException] "
              << "Cannot use simultaneously '--input-profile-tag' and '--input-profile'"
              << std::endl;
    throw FLSimulate::FLDialogOptionsError();
  }

#if DATATOOLS_WITH_QT_GUI == 1
  if (vMap.count("gui") != 0u) {
    clArgs.variantGui = true;
  }

  if (vMap.count("no-gui") != 0u) {
    clArgs.variantGui = false;
  }
  
#endif  // DATATOOLS_WITH_QT_GUI == 1
#if BAYEUX_VERSION >= 30405 
  if (vMap.count("ui-mutable-at-start") != 0u) {
    clArgs.uiMutableAtStart = true;
  }

  if (vMap.count("ui-hide-secondaries-at-start") != 0u) {
    clArgs.uiHideSecondariesAtStart = true;
  }
#endif // BAYEUX_VERSION >= 30405 
}

void do_configure(int argc, char* argv[], FLSimulateConfigureParams& params) {
  // - Default Config
  params = FLSimulateConfigureParams::makeDefault();

  // - CL Dialog Config
  FLSimulateConfigureCommandLine clArgs = FLSimulateConfigureCommandLine::makeDefault();
  try {
    do_cldialog(argc, argv, clArgs);
  } catch (FLSimulate::FLDialogHelpRequested& e) {
    throw FLSimulate::FLConfigHelpHandled();
  } catch (FLSimulate::FLDialogOptionsError& e) {
    throw FLSimulate::FLConfigUserError{"bad command line input"};
  }

  params.logLevel = clArgs.logLevel;
  params.simulationSetupUrn = clArgs.simulationSetupUrn;
  params.inputVariantProfileUrn = clArgs.inputVariantProfileUrn;
  params.variantServiceConfig.profile_load = clArgs.inputVariantProfile;
  params.variantServiceConfig.settings = clArgs.inputVariantSettings;
  params.variantServiceConfig.profile_store = clArgs.outputVariantProfile;
#if DATATOOLS_WITH_QT_GUI == 1
  // Launch the variant GUI browser/editor:
  params.variantServiceConfig.gui = clArgs.variantGui;
  DT_LOG_DEBUG(params.logLevel, "Using GUI : " << clArgs.variantGui);
#else
  DT_LOG_WARNING(params.logLevel, "The current version of Falaise does not support GUI mode!");
#endif  // DATATOOLS_WITH_QT_GUI == 1
  
#if BAYEUX_VERSION >= 30405 
  params.variantServiceConfig.ui_writable_at_start = clArgs.uiMutableAtStart;
  params.variantServiceConfig.ui_inhibit_secondary_choices = clArgs.uiHideSecondariesAtStart;
#endif // BAYEUX_VERSION >= 30405 

  if (params.simulationSetupUrn.empty()) {
    params.simulationSetupUrn = FLSimulate::default_simulation_setup();
  }

  datatools::kernel& dtk = datatools::kernel::instance();
  const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();
  datatools::urn_info simSetupUrnInfo;
  datatools::urn_info variantConfigUrnInfo;
  // Check URN registration from the system URN query service:
  {
    DT_THROW_IF(!dtkUrnQuery.check_urn_info(params.simulationSetupUrn,
                                            falaise::tags::simulation_setup_category()),
                std::logic_error,
                "Cannot query simulation setup URN='" << params.simulationSetupUrn << "'!");
  }
  simSetupUrnInfo = dtkUrnQuery.get_urn_info(params.simulationSetupUrn);

  // Variant setup:
  if (params.variantConfigUrn.empty()) {
    // Automatically determine the variants configuration component:
    if (simSetupUrnInfo.has_topic("variants") &&
        simSetupUrnInfo.get_components_by_topic("variants").size() == 1) {
      params.variantConfigUrn = simSetupUrnInfo.get_component("variants");
    }
  }
  if (!params.variantConfigUrn.empty()) {
    // Check URN registration from the system URN query service:
    {
      DT_THROW_IF(!dtkUrnQuery.check_urn_info(params.variantConfigUrn,
                                              falaise::tags::variant_service_category()),
                  std::logic_error,
                  "Cannot query variant setup URN='" << params.variantConfigUrn << "'!");
    }
    variantConfigUrnInfo = dtkUrnQuery.get_urn_info(params.variantConfigUrn);

    // Resolve variant configuration file:
    std::string conf_variants_category = "configuration";
    std::string conf_variants_mime;
    std::string conf_variants_path;
    DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(params.variantConfigUrn, conf_variants_category,
                                                 conf_variants_mime, conf_variants_path),
                std::logic_error, "Cannot resolve URN='" << params.variantConfigUrn << "'!");
    params.variantServiceConfig.config_filename = conf_variants_path;
  }
  DT_THROW_IF(params.variantServiceConfig.config_filename.empty(), std::logic_error,
              "No variant service configuration path is set!");

  // No variant profile is set:
  if (params.variantServiceConfig.profile_load.empty()) {
    if (params.inputVariantProfileUrn.empty()) {
      DT_LOG_DEBUG(params.logLevel, "No input variant profile URN is set.");
      // No variant profile URN is set:
      if (simSetupUrnInfo.is_valid()) {
        DT_LOG_DEBUG(params.logLevel,
                     "Trying to find a default one from the current simulation setup...");
        // Try to find a default one from the current variant setup:
        if (simSetupUrnInfo.has_topic("defvarprofile") &&
            simSetupUrnInfo.get_components_by_topic("defvarprofile").size() == 1) {
          // If the simulation setup URN implies a "services" component, fetch it!
          params.inputVariantProfileUrn = simSetupUrnInfo.get_component("defvarprofile");
          DT_LOG_DEBUG(params.logLevel, "Using the default variant profile '"
                                            << params.inputVariantProfileUrn << "'"
                                            << " associated to simulation setup '"
                                            << simSetupUrnInfo.get_urn() << "'.");
        }
      }
    }
    if (!params.inputVariantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(
                      params.inputVariantProfileUrn, conf_variantsProfile_category,
                      conf_variantsProfile_mime, conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variant profile URN='" << params.inputVariantProfileUrn << "'!");
      params.variantServiceConfig.profile_load = conf_variantsProfile_path;
    }
  }

  DT_THROW_IF(params.variantServiceConfig.profile_load.empty(), std::logic_error,
              "No variant service input profile path is set!");

  DT_LOG_DEBUG(params.logLevel,
               "Simulation setup tag      : [" << params.simulationSetupUrn << ']');
  DT_LOG_DEBUG(params.logLevel,
               "Variant configuration tag : [" << variantConfigUrnInfo.get_urn() << ']');
  DT_LOG_DEBUG(params.logLevel, "Variant configuration     : '"
                                    << params.variantServiceConfig.config_filename << "'");
  DT_LOG_DEBUG(params.logLevel,
               "Input variant profile tag : [" << params.inputVariantProfileUrn << ']');
  DT_LOG_DEBUG(params.logLevel,
               "Input variant profile     : '" << params.variantServiceConfig.profile_load << "'");
}

falaise::exit_code do_flsimulate_config(int argc, char* argv[]) {
  falaise::exit_code ret = falaise::EXIT_OK;
  // - Configure:
  FLSimulateConfigureParams params;
  try {
    do_configure(argc, argv, params);
  } catch (FLSimulate::FLConfigDefaultError& e) {
    std::cerr << "Unable to configure core of flsimulate-configure" << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  } catch (FLSimulate::FLConfigHelpHandled& e) {
    return falaise::EXIT_OK;
  } catch (FLSimulate::FLConfigUserError& e) {
    std::cerr << "User configuration error: " << e.what() << std::endl;
    return falaise::EXIT_USAGE;
  }

  datatools::configuration::variant_service variantService;
  if (!params.variantServiceConfig.logging.empty()) {
    variantService.set_logging(
        datatools::logger::get_priority(params.variantServiceConfig.logging));
  }
  try {
    if (params.variantServiceConfig.is_active()) {
      variantService.configure(params.variantServiceConfig);
      // Start and lock the variant service:
      variantService.start();
      // From this point, all other services and/or processing modules can
      // benefit of the variant service during their configuration steps.
      if (!params.variantServiceConfig.settings.empty()) {
        // The Variant service uses explicit settings:
        // Make sure we know the full list of effective variant settings corresponding to user
        // choice:
        datatools::properties dummyVariantProps;
        datatools::configuration::variant_repository::exporter varRepExp(
            dummyVariantProps,
            datatools::configuration::variant_repository::exporter::EXPORT_NOCLEAR);
        varRepExp.process(variantService.get_repository());
        // Update the list of *all* settings, not only those requested by the user:
        params.variantServiceConfig.settings = varRepExp.get_settings();
      }
    }
  } catch (std::exception& e) {
    std::cerr << "flsimulate : Variant service threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  // Terminate the variant service:
  if (variantService.is_started()) {
    variantService.stop();
  }

  return ret;
}

}  // end of namespace FLSimulateConfig
