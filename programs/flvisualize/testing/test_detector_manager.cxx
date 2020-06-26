// test_detector_manager.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <EventBrowser/detector/detector_manager.h>

int main(int argc_, char** argv_) {
  using namespace snemo::visualization::detector;
  using namespace std;
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_FAILURE;
  try {
    clog << "Test program for class 'detector_manager'!" << endl;

    // Setting detector manager
    detector_manager& my_detector_manager = detector_manager::get_instance();
    {
      const string detector_config_file =
          falaise::get_resource_dir() + "/config/snemo/demonstrator/geometry/3.0/manager.conf";
      my_detector_manager.initialize(detector_config_file);
      my_detector_manager.construct();
      my_detector_manager.dump();
    }

    // Reset the detector manager and load another one
    {
      const std::string detector_config_file =
          falaise::get_resource_dir() +
          "/config/snemo/tracker_commissioning/geometry/1.0/manager.conf";
      if (my_detector_manager.is_initialized()) {
        my_detector_manager.reset();
      }
      my_detector_manager.initialize(detector_config_file);
      my_detector_manager.construct();
      my_detector_manager.dump();
    }
    return EXIT_SUCCESS;
  } catch (exception& x) {
    cerr << x.what() << endl;
  } catch (...) {
    cerr << "unexpected error!" << endl;
  }
  falaise::terminate();
  return error_code;
}

// end of test_detector_manager.cxx
