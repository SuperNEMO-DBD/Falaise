// flvisualize.cxx
// Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
// Creation date : 2010-07-03
// Last modified : 2017-03-02
//
// Copyright (C) 2011-2017 Xavier Garrido <garrido@lal.in2p3.fr>
//
//
// \b Description:
//
//  A visualization program that shows SuperNEMO events
//  (simulated/calibrated/...).
//
// \b Example:
//
// @code
// flvisualize -i ${SNCORE_DATA_DIR}/resources/setup/snemo/samples/snemo_SD_CD_0.txt.gz
// @endcode
//
//

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

// Third party:
// - Boost
#include <boost/program_options.hpp>
namespace bpo = boost::program_options;
// - ROOT:
#include <TApplication.h>
#include <TROOT.h>
#include <TSystem.h>
// - Bayeux/datatools:
#include <bayeux/version.h>
#include <bayeux/bayeux.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/library_loader.h>
#include <bayeux/datatools/configuration/variant_service.h>
namespace dtc = datatools::configuration;

// This project:
#include <falaise/falaise.h>
#include <falaise/version.h>
#include <falaise/exitcodes.h>
// This plugin:
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/detector/detector_manager.h>
#include "FLVisualizeResources.h"

//! Exceptions for dialog
class FLDialogHelpRequested : public std::exception {};
class FLDialogOptionsError : public std::exception {};

//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose)
{
  os << "flvisualize " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
       << "Copyright (C) 2013-2016 SuperNEMO Collaboration\n\n"
       << "flvisualize uses the following external libraries:\n"
       << "* Falaise : " << falaise::version::get_version() << "\n"
       << "* Bayeux  : " << bayeux::version::get_version() << "\n"
       << "* Boost   : " << BOOST_VERSION << "\n"
       << "* Geant4  : " << "9.6.4" << "\n"
       << "\n\n";
  }
  return;
}
//! Handle printing of help message to screen
void do_help(const bpo::options_description& od)
{
  namespace sv = snemo::visualization;
  std::cout << "flvisualize (" << falaise::version::get_version() << ") : the SuperNEMO event display application\n\n";
  std::cout << "Usage:\n"
            << "  flvisualize [options]\n"
            << od
            << "\n";
  std::cout << "Examples:\n";
  sv::view::options_manager::get_instance().print_examples(std::cout, "flvisualize", "");
  return;
}

//! Initialization flags for kernel setup
uint32_t app_kernel_init_flags()
{
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= datatools::kernel::init_no_help;
  kernel_init_flags |= datatools::kernel::init_no_splash;
  kernel_init_flags |= datatools::kernel::init_no_inhibit_libinfo;
  kernel_init_flags |= datatools::kernel::init_no_libinfo_logging;
  kernel_init_flags |= datatools::kernel::init_no_variant;
  kernel_init_flags |= datatools::kernel::init_no_inhibit_variant;
  kernel_init_flags |= datatools::kernel::init_no_locale_category;
  kernel_init_flags |= datatools::kernel::init_no_inhibit_qt_gui;
  return kernel_init_flags;
}

//! Collect all needed configuration parameters in one data structure
struct FLVisualizeArgs {
  // Application specific parameters:
  datatools::logger::priority  logLevel;  //!< Logging priority thresh
  std::vector<std::string>     libraries; //!< Libraries to be loaded
  // Variants support:
  dtc::variant_service::config variants;  //!< Variants configuration
};

/*
  std::string experiment_label = "demonstrator";
  // variantsParams.config_filename = FLReconstruct::getVariantsConfigFile(experiment_label);
  // if (variantsParams.profile_load == "__default__") {
  //  variantsParams.profile_load = FLReconstruct::getVariantsDefaultProfile(experiment_label);
  //  if (datatools::logger::is_information(args.logLevel)) {
  //    std::clog << "[information]: " << "Loading default variant profile '"
  //              << variantsParams.profile_load << "'..." << std::endl;
  //  }
  // }
  */

//! Handle command line argument dialog
void do_cldialog(int argc_, char *argv_[], FLVisualizeArgs& params_) {

  // Bind command line parser to exposed parameters
  namespace bpo = boost::program_options;
  namespace sv = snemo::visualization;

  // Variant service options:
  // FLVisualizeArgs appParams;
  params_.logLevel = datatools::logger::PRIO_WARNING;

  // Command line parser :

  // Application specific options:
  bpo::options_description optGeneral("General options");
  optGeneral.add_options()
    ("help,h", "produce help message")
    ("version","print version number")
    ("logging-priority,P",
     bpo::value<std::string>()
     ->default_value("notice")
     ->value_name("level"),
     "set the logging priority threshold")
    ("load-dll,l",
     bpo::value<std::vector<std::string> >(&params_.libraries)
     ->value_name("library"),
     "set a DLL to be loaded.")
    ;

  sv::view::options_manager & options_mgr = sv::view::options_manager::get_instance();
  // Browser options:
  bpo::options_description optBrowser("Browser options");
  // Inhibit some options because they are handled my the application:
  uint32_t browser_flags = 0;
  browser_flags |= sv::view::options_manager::browser_opt_no_logging;
  browser_flags |= sv::view::options_manager::browser_opt_no_dll_load;
  // browser_flags |= sv::view::options_manager::browser_opt_no_detector_config;
  options_mgr.define_browser_options(optBrowser,browser_flags);

  // Browser view options:
  bpo::options_description optView("View options");
  uint32_t view_flags = 0;
  options_mgr.define_view_options(optView, view_flags);

  // Variant service options:
  bpo::options_description optVariants("Variants support");
  uint32_t variant_service_flags = 0;
  variant_service_flags |= dtc::variant_service::NO_LABEL;
  variant_service_flags |= dtc::variant_service::NO_LOGGING;
  // variant_service_flags |= dtc::variant_service::NO_CONFIG_FILENAME;
  variant_service_flags |= dtc::variant_service::NO_TUI;
  dtc::variant_service::init_options(optVariants,
                                     params_.variants,
                                     variant_service_flags);

  // Bayeux/datatools kernel options:
  bpo::options_description optKernel("Kernel options");
  datatools::kernel::param_type paramsKernel;
  datatools::kernel::build_opt_desc(optKernel, paramsKernel, app_kernel_init_flags());

  // All options:
  bpo::options_description optPublic;
  optPublic
    .add(optGeneral)
    .add(optBrowser)
    .add(optView)
    .add(optVariants)
    .add(optKernel);

  // Describe command line arguments :
  bpo::positional_options_description args;
  args.add("input-files", -1);

  // - Store first, handling parse errors
  bpo::variables_map vMap;
  try {
    bpo::parsed_options parsed = bpo::command_line_parser(argc_,argv_)
      .options(optPublic)
      // .allow_unregistered()
      .positional(args)
      .run();
    bpo::store(parsed, vMap);
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

  // Handle any non-bound options
  if (vMap.count("help")) {
    do_help(optPublic);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("version")) {
    do_version(std::cout, true);
    throw FLDialogHelpRequested();
  }

  // Handle logging, which can't be bound yet
  if (vMap.count("logging-priority")) {
    std::string loggingLabel = vMap["logging-priority"].as<std::string>();
    datatools::logger::priority prio = datatools::logger::get_priority(loggingLabel);
    if (prio != datatools::logger::PRIO_UNDEFINED) {
      params_.logLevel = prio;
    } else {
      std::cerr << "[OptionsException] " << "Invalid logging level '" << loggingLabel << "'" << std::endl;
      throw FLDialogOptionsError();
    }
  }

  // Library loader :
  datatools::library_loader my_library_loader;
  for (auto & ilib : params_.libraries) {
    DT_LOG_NOTICE(params_.logLevel, "Loading DLL '" << ilib << "'.");
    DT_THROW_IF(my_library_loader.load(ilib) != EXIT_SUCCESS,
                std::logic_error, "Cannot load DLL '" << ilib << "' !");
  }

  options_mgr.set_logging_priority(params_.logLevel);

  try {
    int ret = options_mgr.apply_options(vMap);
    if (ret < 0) {
      std::cerr << "[OptionsException] " << "Help requested." << std::endl;
      throw FLDialogHelpRequested();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLDialogOptionsError();
  }

  // Handle the experiment:
  std::string experiment_label = "demonstrator";
  try {
    params_.variants.config_filename = FLVisualize::getVariantsConfigFile(experiment_label);
    if (params_.variants.profile_load == "__default__") {
      // 2016-08-26 FM: Not supported yet!!!
      params_.variants.profile_load = FLVisualize::getVariantsDefaultProfile(experiment_label);
      if (datatools::logger::is_information(params_.logLevel)) {
        std::clog << "[information]: " << "Loading default variant profile '"
                  << params_.variants.profile_load << "'..." << std::endl;
      }
    }
  } catch (FLVisualize::UnknownResourceException& e) {
    std::cerr << "[FLVisualize::UnknownResourceException] "
              << e.what()
              << std::endl;
    throw FLDialogOptionsError();
  } catch (std::exception & e) {
    std::cerr << "[FLVisualize::UnknownResourceException] "
              << e.what()
              << std::endl;
    throw FLDialogOptionsError();
  }

  return;
}

//----------------------------------------------------------------------
//! Exceptions for configuration
class FLConfigDefaultError : public std::exception {};
class FLConfigHelpHandled : public std::exception {};
class FLConfigUserError : public std::exception {};

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc_, char *argv_[], FLVisualizeArgs& params_) {
  // - Default Config
  try {
    params_.logLevel = datatools::logger::PRIO_ERROR;
  } catch (std::exception& e) {
    throw FLConfigDefaultError();
  }

  // - CL Dialog Config
  try {
    do_cldialog(argc_, argv_, params_);
  } catch (FLDialogHelpRequested& e) {
    throw FLConfigHelpHandled();
  } catch (FLDialogOptionsError& e) {
    throw FLConfigUserError();
  }
}

//----------------------------------------------------------------------
//! Perform simulation using command line args as given
falaise::exit_code do_flvisualize(int argc_, char *argv_[]) {
  namespace sv = snemo::visualization;
  // - Configure
  FLVisualizeArgs flVisParameters;
  try {
    do_configure(argc_, argv_, flVisParameters);
  } catch (FLConfigDefaultError & e) {
    std::cerr << "Unable to configure core of flvisualize" << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  } catch (FLConfigHelpHandled & e) {
    return falaise::EXIT_OK;
  } catch (FLConfigUserError & e) {
    return falaise::EXIT_USAGE;
  }

  // Variants support:
  dtc::variant_service vserv;
  try {
    if (flVisParameters.variants.is_active()) {
      vserv.configure(flVisParameters.variants);
      // Start and lock the variant service:
      vserv.start();
      // From this point, all other services and/or processing modules can benefit
      // of the variant service during their configuration steps.
      // flVisParameters.variants.print(std::cerr, "flvisualize: variant service configuration:", "DEVEL: ");
    }
  } catch (std::exception & e) {
    std::cerr << "[datatools::configuration::variant_service::variant_exception] "
              << e.what()
              << std::endl;
    throw FLDialogOptionsError();
  }

  // - Run the browser:
  try {

    // Browser runs here...
    DT_LOG_NOTICE(flVisParameters.logLevel, "Configuring browser...");

    // Build detector manager
    sv::detector::detector_manager & detector_mgr = sv::detector::detector_manager::get_instance();
    detector_mgr.initialize();
    detector_mgr.construct();

    // Open a root application
    DT_THROW_IF(gROOT->IsBatch(), std::logic_error, "Can not be run in 'batch' mode");

    int narg = 1;
    TApplication * my_application = new TApplication("ROOT Application", &narg, argv_);

    // Get the screen dimensions
    int position_x, position_y;
    unsigned int screen_width, screen_height;
    gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
                             position_x, position_y,
                             screen_width, screen_height);

    // The window is slightly smaller than the screen
    const double scale_factor = sv::view::options_manager::get_instance().get_scaling_factor();
    const int height = int(scale_factor*screen_height);
    const int width  = int(scale_factor*screen_width);

    // The event_browser* is autodestructive!!! NEVER delete manually!!
    sv::view::event_browser * my_event_browser
      = new sv::view::event_browser(gClient->GetRoot(), width, height);
    my_event_browser->initialize();

    DT_LOG_NOTICE(flVisParameters.logLevel, "Browser runs here...");
    my_application->Run(true);
    DT_LOG_NOTICE(flVisParameters.logLevel, "Browser stopped.");

  } catch (std::exception & e) {
    std::cerr << "flsimulate : setup/run of simulation threw exception" << std::endl;
    std::cerr << e.what() << std::endl;
    return falaise::EXIT_UNAVAILABLE;
  }

  if (vserv.is_started()) {
    // Terminate the variant service:
    vserv.stop();
  }

  return falaise::EXIT_OK;
}

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc_, char *argv_[]) {
  falaise::initialize(argc_, argv_, app_kernel_init_flags());

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  falaise::exit_code ret = do_flvisualize(argc_, argv_);

  falaise::terminate();
  return ret;
}
