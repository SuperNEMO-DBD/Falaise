// -*- mode: c++ ; -*-
// test_brio_access.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include <snvisualization/io/brio_access.h>
#include <snvisualization/io/event_server.h>

#include <datatools/ioutils.h>

// For Boost I/O :
// Some pre-processor guard about Boost I/O usage and linkage :
// #include <datatools/serialization/bio_guard.h>
// #include <geomtools/serialization/bio_guard.h>
// #include <genbb_help/serialization/bio_guard.h>
// #include <sncore/serialization/bio_guard.h>

using namespace std;

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'brio_access'!" << endl;

    bool debug = false;
    bool verbose = false;

    vector<string> brio_files;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else {
          clog << datatools::io::warning << "Ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        brio_files.push_back(argument);
      }
      iarg++;
    }

    DT_THROW_IF(brio_files.empty(), std::logic_error, "No file given !");

    namespace svi = snemo::visualization::io;

    // event server in order to store event
    svi::event_server my_event_server;
    my_event_server.initialize(brio_files);

    // brio access instance
    svi::brio_access my_brio_access;

    // Validate file
    DT_THROW_IF(!my_brio_access.is_valid(brio_files), std::logic_error,
                "Files are not brio archives !");

    // After validating, open file and check readibility
    DT_THROW_IF(!my_brio_access.open(brio_files), std::logic_error, "Files are not readable !");

    // clog << "NOTICE: " << "test_brio_access: "
    //      << "Number of event = " << my_event_server.get_number_of_events ()
    //      << endl;

    size_t i_event = 0;
    while (my_brio_access.retrieve_event(my_event_server.get_event(), i_event++)) {
      std::clog << datatools::io::notice << "test_brio_access: "
                << "Event #" << i_event << " loaded" << std::endl;
    }

    my_brio_access.close();

    return EXIT_SUCCESS;
  } catch (exception& x) {
    cerr << datatools::io::error << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << datatools::io::error << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_brio_access.cxx
