// Ourselves
#include "FLTagsCommandLine.h"

// Third party:
// - Bayeux:
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/version.h"

// This Project:
#include "FLTagsErrors.h"
#include "FLTagsUtils.h"
#include "falaise/detail/falaise_sys.h"
#include "falaise/version.h"

namespace FLTags {

  namespace bpo = boost::program_options;

  // static
  FLTagsCommandLine FLTagsCommandLine::makeDefault()
  {
    FLTagsCommandLine flClarg;
    flClarg.logLevel = datatools::logger::PRIO_ERROR;
    flClarg.dot_with_vertex_index = false;
    flClarg.dot_without_vertex_category = false;
    flClarg.dot_without_edge_topic = false;
    flClarg.dot_without_checks = false;
    flClarg.dot_outputFile = "";
    return flClarg;
  }

  void do_version(std::ostream & os, bool isVerbose)
  {
    os << "fltags " << falaise::version::get_version() << "\n";
    if (isVerbose) {
      os << "\n"
         << "Copyright (C) 2018 SuperNEMO Collaboration\n\n"
         << "fltags uses the following external libraries:\n"
         << "* Falaise : " << falaise::version::get_version() << "\n"
         << "* Bayeux  : " << bayeux::version::get_version() << "\n"
         << "* Boost   : " << BOOST_VERSION << "\n"
         << "\n\n";
    }
    return;
  }

  void do_help(const bpo::options_description & od)
  {
    std::cout << "fltags (" << falaise::version::get_version()
              << ") : SuperNEMO tag viewer\n";
    std::cout << "Usage:\n"
              << "  fltags [options]\n"
              << od << "\n";
    return;
  }

  void do_cldialog(int argc, char * argv[], FLTagsCommandLine & clArgs)
  {
    // - Default Command Line
    clArgs = FLTagsCommandLine::makeDefault();

    // Bind command line parser to exposed parameters
    std::string verbosityLabel;
    // Application specific options:
    bpo::options_description optDesc("Options");
    optDesc.add_options()
      ("help,h", "print this help message")

      ("version", "print version number")

      ("verbosity,V",
       bpo::value<std::string>(&verbosityLabel)
       ->value_name("level"),
       "set the verbosity level\n"
       "Example: \n"
       "  -V \"debug\" ")

      ("without-vertex-category,c",
       bpo::value<bool>(&clArgs.dot_without_vertex_category)->zero_tokens()
       ->zero_tokens()
       ->default_value(false),
       "flag to inhibit the printing of the category of each URN vertex\n")

      ("without-edge-topic,t",
       bpo::value<bool>(&clArgs.dot_without_edge_topic)->zero_tokens()
       ->zero_tokens()
       ->default_value(false),
       "flag to inhibit the printing of the topic of each URN edge\n")

      ("without-checks,k",
       bpo::value<bool>(&clArgs.dot_without_checks)->zero_tokens()
       ->zero_tokens()
       ->default_value(false),
       "flag to skip some checks on DOT output\n")

      ("with-vertex-index,i",
       bpo::value<bool>(&clArgs.dot_with_vertex_index)
       ->zero_tokens()
       ->default_value(false),
       "flag to print the index of each URN vertex index (expert only)\n")

      ("output-file,o",
       bpo::value<std::string>(&clArgs.dot_outputFile)
       // ->required()
       ->value_name("file")
       ->default_value(FLTags::default_dot_filename()),
        "file in which to export the graph in DOT format\n"
       "Examples:\n"
       "  -o \"fltags.dot\" \n"
       "To print on the standard output:\n"
       "  -o \"-\" ")
      ;

    // - Parse...
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
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
      do_help(optDesc);
      throw FLDialogHelpRequested();
    }

    if (vMap.count("version")) {
      do_version(std::cout, true);
      throw FLDialogHelpRequested();
    }

    if (vMap.count("verbosity")) {
      clArgs.logLevel = datatools::logger::get_priority(verbosityLabel);
      if (clArgs.logLevel == datatools::logger::PRIO_UNDEFINED) {
        throw FLDialogOptionsError();
      }
    }

    return;
  }

}  // namespace FLTags
