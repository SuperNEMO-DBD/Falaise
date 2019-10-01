// -*- mode: c++ ; -*-
// snemo_non_gui_browser.cxx

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <snvisualization/detector/detector_manager.h>

#include <snvisualization/view/options_manager.h>
#include <snvisualization/view/snemo_draw_manager.h>
#include <snvisualization/view/tiny_viewer.h>

#include <snvisualization/io/event_server.h>
#include <snvisualization/utils/root_utilities.h>

#include <datatools/utils/ioutils.h>
#include <datatools/utils/library_loader.h>

using namespace snemo::visualization::detector;
using namespace snemo::visualization::view;
using namespace snemo::visualization::io;
using namespace snemo::visualization::utils;
using namespace std;

int main(int argc_, char** argv_) {
  const string appname_prefix = "snemo_non_gui_browser: ";

  int error_code = EXIT_SUCCESS;
  try {
    // Setting the options manager
    options_manager& options_mgr = options_manager::get_instance();
    if (!options_mgr.parse_command_line(argc_, argv_)) {
      return EXIT_SUCCESS;
    }

    // Additional options to be parsed only by this program
    int nevents = -1;
    size_t canvas_width = 500;
    size_t canvas_height = 500;
    string scratch_dir = "/tmp";
    vector<string> views;

    namespace po = boost::program_options;
    po::options_description additional_opts("Additional options");
    additional_opts.add_options()("nevents", po::value<int>(&nevents),
                                  "set the maximal number of events to draw")(
        "canvas-width", po::value<size_t>(&canvas_width), "set the canvas width")(
        "canvas-height", po::value<size_t>(&canvas_height), "set the canvas height")(
        "scratch-dir", po::value<string>(&scratch_dir), "set the directory to store data")(
        "views", po::value<vector<string> >(&views), "set 'view' list (3d/top/front/side)");

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc_, argv_).options(additional_opts).allow_unregistered().run(),
        vm);
    po::notify(vm);

    const vector<string>& archive_files = options_mgr.get_input_files();
    if (archive_files.empty()) {
      ostringstream message;
      message << appname_prefix << "No input file given!";
      throw logic_error(message.str());
    }
    // Remove dummy files
    vector<string> existing_files;
    for (vector<string>::const_iterator it_file = archive_files.begin();
         it_file != archive_files.end(); ++it_file) {
      const string& a_file = *it_file;
      if (!boost::filesystem::exists(a_file)) continue;
      existing_files.push_back(a_file);
    }

    // Library loader :
    uint32_t flags = datatools::utils::library_loader::allow_unregistered;
    datatools::utils::library_loader my_library_loader(flags);
    BOOST_FOREACH (const string& dll_name, options_mgr.get_libraries()) {
      clog << datatools::utils::io::notice << appname_prefix << "Loading DLL '" << dll_name << "'."
           << endl;
      my_library_loader.load(dll_name);
    }

    if (views.empty()) {
      views.push_back("3d");
      views.push_back("top");
      views.push_back("front");
      views.push_back("side");
    }

    const bool verbose = options_mgr.is_verbose_mode();
    const bool debug = options_mgr.is_debug_mode();

    // Setting detector manager
    detector_manager& my_detector_manager = detector_manager::get_instance();
    my_detector_manager.set_debug(debug);
    my_detector_manager.set_verbose(verbose);
    my_detector_manager.initialize();
    my_detector_manager.construct();

    // setting the event server
    event_server my_event_server(verbose, debug);

    // Open files
    if (!my_event_server.open(existing_files)) {
      ostringstream message;
      message << appname_prefix << "Archive files can not be opened !";
      throw runtime_error(message.str());
    }

    // setting drawer manager
    snemo_draw_manager my_drawer_manager(&my_event_server);

    if (debug) {
      // Dumping information
      my_event_server.dump();
      my_detector_manager.dump();
    }

    tiny_viewer my_tiny_viewer_2d("Non 2D GUI viewer", canvas_width, canvas_height,
                                  i_embedded_viewer::VIEW_2D);
    my_tiny_viewer_2d.update_detector();

    tiny_viewer my_tiny_viewer_3d("Non 3D GUI viewer", canvas_width, canvas_height,
                                  i_embedded_viewer::VIEW_3D);
    my_tiny_viewer_3d.update_detector();

    while (my_event_server.next_event()) {
      if (nevents > 0 && my_event_server.get_current_event_number() >= nevents) break;

      clog << datatools::utils::io::notice << appname_prefix << "Event #"
           << my_event_server.get_current_event_number() << " loaded" << endl;

      my_drawer_manager.update();

      // Update views (first 2D view to not show the legend)
      my_tiny_viewer_2d.update_scene(&my_drawer_manager);
      my_tiny_viewer_3d.update_scene(&my_drawer_manager);

      BOOST_FOREACH (const string& view_label, views) {
        TCanvas* a_canvas = my_tiny_viewer_2d.get_canvas();
        if (view_label == "3d")
          a_canvas = my_tiny_viewer_3d.get_canvas();
        else if (view_label == "top")
          my_tiny_viewer_2d.set_view_type(TOP_VIEW);
        else if (view_label == "front")
          my_tiny_viewer_2d.set_view_type(FRONT_VIEW);
        else if (view_label == "side")
          my_tiny_viewer_2d.set_view_type(SIDE_VIEW);
        else {
          ostringstream message;
          message << "View label '" << view_label << "' is not available!";
          throw logic_error(message.str());
        }

        // save view
        ostringstream filename;
        filename << scratch_dir << "/" << view_label << "_view_event_" << setw(3) << setfill('0')
                 << my_event_server.get_current_event_number() << ".eps";

        if (!root_utilities::save_view_as(a_canvas, filename.str())) {
          cerr << datatools::utils::io::error << appname_prefix << "Can not save '" << view_label
               << "' view!" << endl;
        }
      }

      my_drawer_manager.clear();
    }

    my_event_server.close();

  } catch (exception& x) {
    cerr << datatools::utils::io::error << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << datatools::utils::io::error << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of snemo_non_gui_browser.cxx
