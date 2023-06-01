// Ourselves
#include "FLReconstructCommandLine.h"

// Standard Library
#include <set>
#include <vector>

// Third Party
// - Boost
#include <boost/filesystem.hpp>
#include "boost/program_options.hpp"
#include "boost/version.hpp"
// - Bayeux
#include "bayeux/datatools/detail/ocd_utils.h"
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/library_loader.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/dpp/base_module.h"
#include "bayeux/version.h"

// This project
#include "FLReconstructErrors.h"
#include "falaise/user_level.h"
#include "falaise/detail/falaise_sys.h"
#include "falaise/resource.h"
#include "falaise/version.h"

namespace FLReconstruct {

  namespace bpo = boost::program_options;

  // static
  FLReconstructCommandLine FLReconstructCommandLine::makeDefault()
  {
    FLReconstructCommandLine frArgs;
    frArgs.logLevel = datatools::logger::PRIO_FATAL;
    frArgs.maxNumberOfEvents = 0;
    frArgs.moduloEvents = 0;
    frArgs.userProfile = "normal";
    frArgs.mountPoints.clear();
    frArgs.configScript = "";
    frArgs.inputMetadataFile = "";
    frArgs.outputMetadataFile = "";  
    frArgs.inputFile = "";
    frArgs.outputFile = "";
    return frArgs;
  }

  void FLReconstructCommandLine::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLReconstructCommandLine parameters: " << std::endl;
    out_ << tag
         << "logLevel                     = " << datatools::logger::get_priority_label(this->logLevel)
         << std::endl;
    out_ << tag << "maxNumberOfEvents            = " << maxNumberOfEvents << std::endl;
    out_ << tag << "moduloEvents                 = " << moduloEvents << std::endl;
    out_ << tag << "userProfile                  = '" << userProfile << "'" << std::endl;
    out_ << tag << "mountPoints                  = " << mountPoints.size() << std::endl;
    for (unsigned int i = 0; i < mountPoints.size(); i++) {
      out_ << "|   ";
      if (i + 1 == mountPoints.size()) {
        out_ << "`-- ";
      } else {
        out_ << tag;
      }
      out_ << "Directive : '" << mountPoints[i] << "'" << std::endl;
    }
    out_ << tag << "configScript                 = '" << configScript << "'" << std::endl;
    out_ << tag << "inputMetadataFile            = '" << inputMetadataFile << "'" << std::endl;
    out_ << tag << "inputFile                    = '" << inputFile << "'" << std::endl;
    out_ << tag << "outputMetadataFile           = '" << outputMetadataFile << "'" << std::endl;
    out_ << last_tag << "outputFile                   = '" << outputFile << "'" << std::endl;
 
    return;
  }

  //! Handle printing of version information to given ostream
  void do_version(std::ostream & os_, bool isVerbose_)
  {
    std::string commitInfo{};
    if (!falaise::version::get_commit().empty()) {
      commitInfo = " (" + falaise::version::get_commit();
      commitInfo += falaise::version::is_dirty() ? "-dirty" : "";
      commitInfo += ")";
    }

    os_ << "flreconstruct " << falaise::version::get_version() << commitInfo << "\n";
    if (isVerbose_) {
      os_ << "\n"
          << "Copyright (C) 2013-2022 SuperNEMO Collaboration\n\n"
          << "flreconstruct uses the following external libraries:\n"
          << "* Falaise : " << falaise::version::get_version() << commitInfo << "\n"
          << "* Bayeux  : " << bayeux::version::get_version() << "\n"
          << "* Boost   : " << BOOST_VERSION << "\n"
          << "\n\n";
    }
    return;
  }

  //! Handle printing of help message to given ostream
  void do_help(std::ostream & os_, const bpo::options_description & od_)
  {
    os_ << "flreconstruct (" << falaise::version::get_version()
        << ") : SuperNEMO reconstruction program\n";
    os_ << "Usage:\n"
        << "  flreconstruct [options]\n"
        << od_ << "\n";
    return;
  }

  //! Print list of known module names, one per line, to given stream
  void do_module_list(std::ostream & os_)
  {
    datatools::library_loader libLoader;
    do_load_plugins(libLoader);
    using PluginInfo = std::vector<std::string>;
    PluginInfo mods;
    // Builtin
    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(dpp::base_module).list_of_factories(mods);
    for (const PluginInfo::value_type& entry : mods) {
      os_ << entry << std::endl;
    }
  }

  //! Print OCD help for supplied module name to given ostream
  void do_help_module(std::ostream & os_, const std::string & module_)
  {
    datatools::library_loader libLoader;
    do_load_plugins(libLoader);
    // Is module valid?
    using ModuleInfo = std::vector<std::string>;
    ModuleInfo mods;
    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(dpp::base_module).list_of_factories(mods);
    std::set<std::string> moduleSet(mods.begin(), mods.end());
    if (moduleSet.find(module_) == moduleSet.end()) {
      os_ << "[error] Argument '" << module_ << "' is not a flreconstruct module\n"
          << "Use '--help-module-list' to see all modules" << std::endl;
      return;
    }

    // Is it in OCD?
    const datatools::detail::ocd::ocd_registration& ocd_system_reg =
      datatools::detail::ocd::ocd_registration::get_system_registration();
    if (!ocd_system_reg.has_id(module_)) {
      os_ << "[error] Module '" << module_ << "' is not documented" << std::endl;
      return;
    }

    const datatools::object_configuration_description & moduleDoc = ocd_system_reg.get(module_);
    moduleDoc.print(os_);
  }

  //! Print list of standard pipeline configurations to supplied ostream
  void do_help_pipeline_list(std::ostream & os_)
  {
    datatools::kernel& dtk = ::datatools::kernel::instance();
    if (dtk.has_urn_query()) {
      const datatools::urn_query_service& dtkUrnQuery = dtk.get_urn_query();
      std::vector<std::string> flsim_urn_infos;
      if (dtkUrnQuery.find_urn_info(flsim_urn_infos, falaise::detail::falaise_sys::fl_setup_db_name(),
                                    "(urn:)([^:]*)(:)([^:]*)(:reconstruction:)([^:]*)(:pipeline:)([^:]*)",
                                    "recpipeline")) {
        os_ << "List of supported reconstruction pipeline:" << std::endl;
        for (const auto& flsim_urn_info : flsim_urn_infos) {
          const datatools::urn_info& ui = dtkUrnQuery.get_urn_info(flsim_urn_info);
          os_ << ui.get_urn() << " : " << ui.get_description() << std::endl;
        }
      } else {
        std::cerr << "Could not find any reconstruction setup from the global URN query service." << std::endl;
      }
    }
  }

  //! Load all default plugins
  void do_load_plugins(datatools::library_loader& libLoader)
  {
    std::string pluginPath = falaise::get_plugin_dir();
    // explicitly list for now...
    libLoader.load("Falaise_SimRC", pluginPath);
    libLoader.load("Falaise_CAT", pluginPath);
    libLoader.load("TrackFit", pluginPath);
    libLoader.load("Falaise_TrackFit", pluginPath);
    libLoader.load("Falaise_ChargedParticleTracking", pluginPath);
    libLoader.load("Things2Root", pluginPath);
    libLoader.load("GammaTracking", pluginPath);
    libLoader.load("Falaise_GammaClustering", pluginPath);
  }

  FLDialogState do_cldialog(int argc, char* argv[], FLReconstructCommandLine& clArgs)
  {
    // - Default Command Line
    clArgs = FLReconstructCommandLine::makeDefault();

    // Bind command line parser to exposed parameters
    std::string verbosityLabel;
    // Application specific options:
    // clang-format off
    bpo::options_description optDesc("Options");
    optDesc.add_options()("help,h", "print this help message")
      ("help-module-list", "list available modules and exit")

      ("help-module",
       bpo::value<std::string>()
       ->value_name("name"),
       "print help for a single module and exit")

      ("help-pipeline-list", "list available pipeline configurations and exit")

      ("version", "print version number")

      ("verbosity,V",
       bpo::value<std::string>(&verbosityLabel)
       ->value_name("level"),
       "set the verbosity level")

      ("max-number-events,N",
       bpo::value<uint32_t>(&clArgs.maxNumberOfEvents)
       ->default_value(0)
       ->value_name("N"),
       "maximum number of processed events")

      ("modulo,P",
       bpo::value<uint32_t>(&clArgs.moduloEvents)
       ->default_value(0)
       ->value_name("period"),
       "progress modulo on number of events")

      ("user-profile,u",
       bpo::value<std::string>(&clArgs.userProfile)
       ->value_name("name")
       ->default_value("normal"),
       R"(set the user profile ("expert", "normal", "production"))")

      ("mount-directory,d",
       bpo::value<std::vector<std::string>>(&clArgs.mountPoints)
       ->value_name("rule"),
       "register directories' mount points\n"
       "Example: \n"
       "  -d \"snrs.libraries@/opt/sw/snrs/lib\" \n"
       "  -d \"nemoprod@/etc/nemoprod/config\" \n"
       "  -d \"nemoprod.data@/data/nemoprod/runs\"")

      ("input-metadata-file,M",
       bpo::value<std::string>(&clArgs.inputMetadataFile)
       ->value_name("file"),
       "file from which to load metadata")

      ("output-metadata-file,m",
       bpo::value<std::string>(&clArgs.outputMetadataFile)
       ->value_name("file"),
       "file in which to store metadata")

      ("config,p",
       bpo::value<std::string>(&clArgs.configScript)
       ->value_name("path"),
       "pipeline config script (path)")

      // ("pipeline,p", bpo::value<std::string>(&clArgs.pipelineScript)->value_name("path"),
      //  "pipeline script (path or URN)")

      ("input-file,i",
       bpo::value<std::string>(&clArgs.inputFile)
       ->required()
       ->value_name("file"),
       "file from which to read input data (simulation, real)")

      ("output-file,o",
       bpo::value<std::string>(&clArgs.outputFile)
       ->value_name("file"),
       "file in which to store reconstruction results")
      ;
    // clang-format on

    // - Store first, handling parse errors
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
      bpo::notify(vMap);
    } catch (const bpo::required_option& e) {
      // We need to handle help/version even if required_option thrown
      if ((vMap.count("help") == 0u) && (vMap.count("version") == 0u) &&
          (vMap.count("help-module-list") == 0u) && (vMap.count("help-module") == 0u) &&
          (vMap.count("help-pipeline-list") == 0u)) {
        do_error(std::cerr, e.what());
        return DIALOG_ERROR;
      }
    } catch (const std::exception& e) {
      do_error(std::cerr, e.what());
      return DIALOG_ERROR;
    }

    // Handle any non-bound options
    if (vMap.count("help") != 0u) {
      do_help(std::cout, optDesc);
      return DIALOG_QUERY;
    }

    if (vMap.count("version") != 0u) {
      do_version(std::cout, true);
      return DIALOG_QUERY;
    }

    if (vMap.count("help-module-list") != 0u) {
      do_module_list(std::cout);
      return DIALOG_QUERY;
    }

    if (vMap.count("help-module") != 0u) {
      do_help_module(std::cout, vMap["help-module"].as<std::string>());
      return DIALOG_QUERY;
    }

    if (vMap.count("help-pipeline-list") != 0u) {
      do_help_pipeline_list(std::cout);
      return DIALOG_QUERY;
    }

    if (vMap.count("verbosity") != 0u) {
      clArgs.logLevel = datatools::logger::get_priority(verbosityLabel);
      if (clArgs.logLevel == datatools::logger::PRIO_UNDEFINED) {
        do_error(std::cerr, "Invalid verbosity level '" + verbosityLabel + "'!");
        return DIALOG_ERROR;
      }
    }

    if (falaise::validUserLevels().count(clArgs.userProfile) == 0u) {
      do_error(std::cerr, "Invalid user profile '" + clArgs.userProfile + "'!");
      return DIALOG_ERROR;
    }

    if (vMap.count("mount-directory") != 0u) {
      DT_LOG_DEBUG(clArgs.logLevel, "There are mount directives!");
      std::cerr << "# mount directives = " << clArgs.mountPoints.size() << '\n';
    }

    return DIALOG_OK;
  }

} // namespace FLReconstruct
