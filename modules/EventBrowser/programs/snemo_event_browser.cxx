// snemo_event_browser.cxx
// Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
// Creation date : 2010-07-03
// Last modified : 2014-11-26
//
// Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
// snemo_event_browser -i ${SNCORE_DATA_DIR}/resources/setup/snemo/samples/snemo_SD_CD_0.txt.gz
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
// - ROOT:
#include <TApplication.h>
#include <TROOT.h>
#include <TSystem.h>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/library_loader.h>

// This project:
#include <falaise/falaise.h>
// This plugin:
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/detector/detector_manager.h>

int main(int argc_, char **argv_)
{
  falaise::initialize(argc_, argv_);
  const std::string APPNAME_PREFIX = "snemo_event_browser: ";

  int error_code = EXIT_SUCCESS;
  try {
    namespace sv = snemo::visualization;

    // Command line parser :
    sv::view::options_manager & options_mgr = sv::view::options_manager::get_instance();
    if (!options_mgr.parse_command_line(argc_, argv_)) {
      return EXIT_SUCCESS;
    }

    // Library loader :
    datatools::library_loader my_library_loader;
    for (std::vector<std::string>::const_iterator
           ilib = options_mgr.get_libraries().begin();
         ilib != options_mgr.get_libraries().end(); ++ilib) {
      DT_LOG_NOTICE(options_mgr.get_logging_priority(), "Loading DLL '" << *ilib << "'.");
      DT_THROW_IF(my_library_loader.load(*ilib) != EXIT_SUCCESS,
                  std::logic_error, "Cannot load DLL '" << *ilib << "' !");
    }

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
    const double scale_factor = options_mgr.get_scaling_factor();
    const int height = int(scale_factor*screen_height);
    const int width  = int(scale_factor*screen_width);

    // The event_browser* is autodestructive!!! NEVER delete manually!!
    sv::view::event_browser * my_event_browser
      = new sv::view::event_browser(gClient->GetRoot(), width, height);
    my_event_browser->initialize();

    my_application->Run(true);
  } catch (std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, APPNAME_PREFIX << x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}

// end of snemo_event_browser.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
