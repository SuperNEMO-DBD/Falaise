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
#include "FLTagsCommandLine.h"
#include "FLTagsErrors.h"
#include "FLTagsUtils.h"
#include "falaise/property_reader.h"

namespace FLTags {

  void do_postprocess(FLTagsArgs& flTagParameters);

  // static
  FLTagsArgs FLTagsArgs::makeDefault()
  {
    FLTagsArgs params;

    // Application specific parameters:
    params.logLevel = datatools::logger::PRIO_ERROR;
    params.dot_with_vertex_index = false;
    params.dot_with_vertex_category = false;
    params.dot_with_edge_topic = false;
    params.dot_without_checks = false;
    params.dot_outputFile = "";

    return params;
  }

  void do_configure(int argc, char * argv[], FLTagsArgs & flTagsParameters)
  {
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
    flTagsParameters.dot_with_vertex_index    = args.dot_with_vertex_index;
    flTagsParameters.dot_with_vertex_category = args.dot_with_vertex_category;
    flTagsParameters.dot_with_edge_topic      = args.dot_with_edge_topic;
    flTagsParameters.dot_without_checks       = args.dot_without_checks;
    flTagsParameters.dot_outputFile           = args.dot_outputFile;

    do_postprocess(flTagsParameters);
    return;
  }

  void do_postprocess(FLTagsArgs & flTagsParameters)
  {
    DT_LOG_TRACE_ENTERING(flTagsParameters.logLevel);

    if (!flTagsParameters.dot_outputFile.empty() && flTagsParameters.dot_outputFile != "-") {
      if (!flTagsParameters.dot_without_checks &&
          ! boost::ends_with(flTagsParameters.dot_outputFile, ".dot")) {
        DT_THROW(FLConfigUserError, "Invalid file extension '" << flTagsParameters.dot_outputFile << "' (expected '.dot')");
      }
    }

    // Print:
    if (datatools::logger::is_debug(flTagsParameters.logLevel)) {
      flTagsParameters.print(std::cerr);
    }

    DT_LOG_TRACE_EXITING(flTagsParameters.logLevel);
    return;
  }

  void FLTagsArgs::print(std::ostream & out_) const
  {
    static const std::string tag("|-- ");
    static const std::string last_tag("`-- ");
    out_ << "FLTags setup parameters: " << std::endl;
    out_ << tag
         << "logLevel                 = '" << datatools::logger::get_priority_label(this->logLevel)
         << "'" << std::endl;
    out_ << tag
         << "DOT with wertex index    = " << std::boolalpha << dot_with_vertex_index << std::endl;
    out_ << tag
         << "DOT with wertex category = " << std::boolalpha << dot_with_vertex_category << std::endl;
    out_ << tag
         << "DOT with edge topic      = " << std::boolalpha << dot_with_edge_topic << std::endl;
    out_ << last_tag
         << "outputFile               = '" << dot_outputFile << "'" << std::endl;
    return;
  }

}  // namespace FLTags
