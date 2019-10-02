// Ourselves
#include "FLTagsArgs.h"

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/library_info.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/utils.h>

// This Project:
#include "FLTags.h"
#include "FLTagsCommandLine.h"
#include "FLTagsErrors.h"
#include "FLTagsUtils.h"
#include "falaise/property_set.h"

namespace FLTags {

void do_postprocess(FLTagsArgs& flTagsParameters);

// static
FLTagsArgs FLTagsArgs::makeDefault() {
  FLTagsArgs params;

  // Application specific parameters:
  params.logLevel = datatools::logger::PRIO_FATAL;
  params.action = "";
  params.list_with_tree = false;
  params.dot_with_vertex_index = false;
  params.dot_without_vertex_category = false;
  params.dot_without_edge_topic = false;
  params.dot_without_checks = false;
  params.outputFile = "";

  return params;
}

void do_configure(int argc, char* argv[], FLTagsArgs& flTagsParameters) {
  DT_LOG_TRACE_ENTERING(flTagsParameters.logLevel);
  // - Default Config
  flTagsParameters = FLTagsArgs::makeDefault();

  // - CL Dialog Config
  FLTagsCommandLine args;
  try {
    do_cldialog(argc, argv, args);
  } catch (FLDialogHelpRequested& e) {
    throw FLConfigHelpHandled();
  } catch (FLDialogOptionsError& e) {
    throw FLConfigUserError{"bad command line input"};
  }

  // Feed input from command line to params
  flTagsParameters.logLevel = args.logLevel;
  flTagsParameters.action = args.action;
  flTagsParameters.list_with_tree = args.list_with_tree;
  flTagsParameters.dot_with_vertex_index = args.dot_with_vertex_index;
  flTagsParameters.dot_without_vertex_category = args.dot_without_vertex_category;
  flTagsParameters.dot_without_edge_topic = args.dot_without_edge_topic;
  flTagsParameters.dot_without_checks = args.dot_without_checks;
  flTagsParameters.outputFile = args.outputFile;

  do_postprocess(flTagsParameters);
}

void do_postprocess(FLTagsArgs& flTagsParameters) {
  DT_LOG_TRACE_ENTERING(flTagsParameters.logLevel);

  if (flTagsParameters.action.empty()) {
    flTagsParameters.action = fltags::default_action();
  }

  if (!flTagsParameters.outputFile.empty()) {
    if (flTagsParameters.outputFile != "-" && flTagsParameters.action == "graph") {
      if (!flTagsParameters.dot_without_checks &&
          !boost::ends_with(flTagsParameters.outputFile, ".dot")) {
        DT_THROW(FLConfigUserError, "Invalid file extension '" << flTagsParameters.outputFile
                                                               << "' (expected '.dot')");
      }
    }
  } else {
    if (flTagsParameters.action == "list") {
      flTagsParameters.outputFile = FLTags::default_list_filename();
    } else if (flTagsParameters.action == "graph") {
      flTagsParameters.outputFile = FLTags::default_dot_filename();
    }
  }

  // Print:
  if (datatools::logger::is_debug(flTagsParameters.logLevel)) {
    flTagsParameters.print(std::cerr);
  }

  DT_LOG_TRACE_EXITING(flTagsParameters.logLevel);
}

void FLTagsArgs::print(std::ostream& out_) const {
  static const std::string tag("|-- ");
  static const std::string last_tag("`-- ");
  out_ << "FLTags setup parameters: " << std::endl;
  out_ << tag << "logLevel                    = '"
       << datatools::logger::get_priority_label(this->logLevel) << "'" << std::endl;
  out_ << tag << "action                      = '" << action << "'" << std::endl;
  if (action == "list") {
    out_ << tag << "Print list as a tree        = " << std::boolalpha << list_with_tree
         << std::endl;
  }
  if (action == "graph") {
    out_ << tag << "DOT with wertex index       = " << std::boolalpha << dot_with_vertex_index
         << std::endl;
    out_ << tag << "DOT without wertex category = " << std::boolalpha << dot_without_vertex_category
         << std::endl;
    out_ << tag << "DOT without edge topic      = " << std::boolalpha << dot_without_edge_topic
         << std::endl;
  }
  out_ << last_tag << "outputFile                  = '" << outputFile << "'" << std::endl;
}

}  // namespace FLTags
