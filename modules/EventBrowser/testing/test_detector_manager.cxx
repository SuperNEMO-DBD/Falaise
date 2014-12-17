// -*- mode: c++ ; -*-
// test_detector_manager.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <falaise/snemo/detector/detector_manager.h>

using namespace snemo::visualization::detector;
using namespace std;

int main()
{
  FALAISE_INIT();
  int error_code = EXIT_FAILURE;
  try {
    clog << "Test program for class 'detector_manager'!" << endl;

    // Setting detector manager
    detector_manager & my_detector_manager = detector_manager::get_instance();
    {
      const string detector_config_file = falaise::get_resource_dir(true) +
        "/config/snemo/demonstrator/geometry/3.0/manager.conf";
      my_detector_manager.initialize(detector_config_file);
      my_detector_manager.construct();
      my_detector_manager.dump();
    }

    // Reset the detector manager and load another one
    {
      const std::string detector_config_file = falaise::get_resource_dir(true) +
        "/config/snemo/tracker_commissioning/geometry/1.0/manager.conf";
      my_detector_manager.reset();
      my_detector_manager.initialize(detector_config_file);
      my_detector_manager.construct();
      my_detector_manager.dump();
    }
    return EXIT_SUCCESS;
  } catch (exception & x) {
    cerr << x.what () << endl;
  } catch (...) {
    cerr << "unexpected error!" << endl;
  }
  FALAISE_FINI();
  return error_code;
}

// end of test_detector_manager.cxx
