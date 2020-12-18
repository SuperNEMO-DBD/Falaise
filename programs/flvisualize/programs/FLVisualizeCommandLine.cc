// Ourselves
#include "FLVisualizeCommandLine.h"

// Third Party
// - Boost
#include <boost/algorithm/string/predicate.hpp>
// - Bayeux
#include <bayeux/datatools/kernel.h>
#include <bayeux/version.h>

// This project:
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/view/options_manager.h>
#include <falaise/falaise.h>
#include <falaise/version.h>

namespace FLVisualize {

namespace bpo = boost::program_options;

// Initialization flags for kernel setup
uint32_t app_kernel_init_flags() {
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= datatools::init_no_help;
  kernel_init_flags |= datatools::init_no_splash;
  kernel_init_flags |= datatools::init_no_inhibit_libinfo;
  kernel_init_flags |= datatools::init_no_libinfo_logging;
  kernel_init_flags |= datatools::init_no_variant;
  kernel_init_flags |= datatools::init_no_inhibit_variant;
  kernel_init_flags |= datatools::init_no_locale_category;
  kernel_init_flags |= datatools::init_no_inhibit_qt_gui;
  kernel_init_flags |= datatools::init_no_inhibit_urnquery;
  return kernel_init_flags;
}

//! Handle printing of version information to given ostream
void do_version(std::ostream& os, bool isVerbose) {
  os << "flvisualize " << falaise::version::get_version() << "\n";
  if (isVerbose) {
    os << "\n"
       << "Copyright (C) 2013-2017 SuperNEMO Collaboration\n\n"
       << "flvisualize uses the following external libraries:\n"
       << "* Falaise : " << falaise::version::get_version() << "\n"
       << "* Bayeux  : " << bayeux::version::get_version() << "\n"
       << "* Boost   : " << BOOST_VERSION << "\n"
       << "\n\n";
  }
}

//! Handle printing of help message to screen
void do_help(const bpo::options_description& od) {
  namespace sv = snemo::visualization;
  std::cout << "flvisualize (" << falaise::version::get_version()
            << ") : the SuperNEMO event display application\n\n";
  std::cout << "Usage:\n"
            << "  flvisualize [options]\n"
            << od << "\n";
  std::cout << "Examples:\n";
  sv::view::options_manager::get_instance().print_examples(std::cout, "flvisualize", "");
}

//! Handle command line argument dialog
void do_cldialog(int argc_, char* argv_[], FLVisualizeArgs& params_) {
  DT_LOG_TRACE_ENTERING(params_.logLevel);
  namespace bpo = boost::program_options;
  namespace dtc = datatools::configuration;
  namespace sv = snemo::visualization;
  // Bind command line parser to exposed parameters

  params_.logLevel = datatools::logger::PRIO_WARNING;

  // Command line parser :

  // Application specific options:
  bpo::options_description optGeneral("General options");
  optGeneral.add_options()("help,h", "produce help message")("version", "print version number")(
      "logging-priority,P", bpo::value<std::string>()->default_value("error")->value_name("level"),
      "set the logging priority threshold")(
      "load-dll,l",
      bpo::value<std::vector<std::string> >(&params_.libraries)->value_name("library"),
      "set a DLL to be loaded.")(
      "experiment-setup,x",
      bpo::value<std::string>(&params_.experimentalSetupUrn)->value_name("tag"),
      "set the experimental setup tag.")("variant-config,v",
                                         bpo::value<std::string>()->value_name("tag"),
                                         "set the variant configuration.")(
      "variant-profile,f", bpo::value<std::string>()->value_name("tag"),
      "set the variant profile.")("services-config,s", bpo::value<std::string>()->value_name("tag"),
                                  "set the services configuration.")(
      "ignore-input-metadata,G",
      bpo::value<bool>(&params_.ignoreInputMetadata)->value_name("flag")->default_value(false),
      "flag to ignore input metadata")(
      "input-metadata-file,M",
      bpo::value<std::string>(&params_.inputMetadataFile)->value_name("file"),
      "file from which to load metadata")("input-file,i",
                                          bpo::value<std::string>(&params_.inputFile)
                                              // ->required()
                                              ->value_name("file"),
                                          "file from which to read input data (simulation, real)");

  sv::view::options_manager& options_mgr = sv::view::options_manager::get_instance();
  // Browser options:
  bpo::options_description optBrowser("Browser options");
  // Inhibit some options because they are handled my the application:
  uint32_t browser_flags = 0;
  browser_flags |= sv::view::options_manager::browser_opt_no_logging;
  browser_flags |= sv::view::options_manager::browser_opt_no_dll_load;
  // browser_flags |= sv::view::options_manager::browser_opt_no_detector_config;
  options_mgr.define_browser_options(optBrowser, browser_flags);

  // Browser view options:
  bpo::options_description optView("View options");
  uint32_t view_flags = 0;
  options_mgr.define_view_options(optView, view_flags);

  // // Variant service options:
  // bpo::options_description optVariants("Variants support");
  // uint32_t variant_service_flags = 0;
  // variant_service_flags |= dtc::variant_service::NO_LABEL;
  // variant_service_flags |= dtc::variant_service::NO_LOGGING;
  // variant_service_flags |= dtc::variant_service::NO_CONFIG_FILENAME;
  // variant_service_flags |= dtc::variant_service::NO_REGISTRY_RULES;
  // variant_service_flags |= dtc::variant_service::NO_REGISTRY_DEPENDENCIES;
  // variant_service_flags |= dtc::variant_service::NO_PROFILE_LOAD;
  // variant_service_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
  // variant_service_flags |= dtc::variant_service::NO_SETTINGS;
  // variant_service_flags |= dtc::variant_service::NO_PROFILE_STORE;
  // variant_service_flags |= dtc::variant_service::NO_REPORTING;
  // variant_service_flags |= dtc::variant_service::NO_GUI;
  // variant_service_flags |= dtc::variant_service::NO_TUI;
  // dtc::variant_service::init_options(optVariants,
  //                                    params_.variants,
  //                                    variant_service_flags);

  // Bayeux/datatools kernel options:
  bpo::options_description optKernel("Kernel options");
  datatools::kernel::param_type paramsKernel;
  datatools::kernel::build_opt_desc(optKernel, paramsKernel, app_kernel_init_flags());

  // All options:
  bpo::options_description optPublic;
  optPublic.add(optGeneral)
      .add(optBrowser)
      .add(optView)
      // .add(optVariants)
      .add(optKernel);

  // Describe command line arguments :
  bpo::positional_options_description args;
  args.add("input-files", -1);

  // - Store first, handling parse errors
  bpo::variables_map vMap;
  try {
    bpo::parsed_options parsed = bpo::command_line_parser(argc_, argv_)
                                     .options(optPublic)
                                     // .allow_unregistered()
                                     .positional(args)
                                     .run();
    bpo::store(parsed, vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help/version even if required_option thrown
    if ((vMap.count("help") == 0u) && (vMap.count("version") == 0u)) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      throw FLDialogOptionsError();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLDialogOptionsError();
  }

  // Handle any non-bound options
  if (vMap.count("help") != 0u) {
    do_help(optPublic);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("version") != 0u) {
    do_version(std::cout, true);
    throw FLDialogHelpRequested();
  }

  if (vMap.count("variant-config") != 0u) {
    std::string vc = vMap["variant-config"].as<std::string>();
    if (boost::algorithm::starts_with(vc, "urn:")) {
      params_.variantConfigUrn = vc;
    } else {
      params_.variants.config_filename = vc;
    }
  }

  if (vMap.count("variant-profile") != 0u) {
    std::string vp = vMap["variant-profile"].as<std::string>();
    if (boost::algorithm::starts_with(vp, "urn:")) {
      params_.variantProfileUrn = vp;
    } else {
      params_.variants.profile_load = vp;
    }
  }

  if (vMap.count("services-config") != 0u) {
    std::string sc = vMap["services-config"].as<std::string>();
    if (boost::algorithm::starts_with(sc, "urn:")) {
      params_.servicesConfigUrn = sc;
    } else {
      params_.servicesConfig = sc;
    }
  }

  // Handle logging, which can't be bound yet
  if (vMap.count("logging-priority") != 0u) {
    std::string loggingLabel = vMap["logging-priority"].as<std::string>();
    datatools::logger::priority prio = datatools::logger::get_priority(loggingLabel);
    if (prio != datatools::logger::PRIO_UNDEFINED) {
      params_.logLevel = prio;
    } else {
      std::cerr << "[OptionsException] "
                << "Invalid logging level '" << loggingLabel << "'" << std::endl;
      throw FLDialogOptionsError();
    }
  }

  // Library loader :
  datatools::library_loader my_library_loader;
  for (auto& ilib : params_.libraries) {
    DT_LOG_NOTICE(params_.logLevel, "Loading DLL '" << ilib << "'.");
    DT_THROW_IF(my_library_loader.load(ilib) != EXIT_SUCCESS, std::logic_error,
                "Cannot load DLL '" << ilib << "' !");
  }

  options_mgr.set_logging_priority(params_.logLevel);

  try {
    int ret = options_mgr.apply_options(vMap);
    if (ret < 0) {
      std::cerr << "[OptionsException] "
                << "Help requested." << std::endl;
      throw FLDialogHelpRequested();
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw FLDialogOptionsError();
  }

  DT_LOG_TRACE_EXITING(params_.logLevel);
}

}  // namespace FLVisualize
