// -*- mode: c++ ; -*-
// test_event_server.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

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
    clog << "Test program for class 'event_server'!" << endl;

    bool debug = false;
    bool verbose = false;

    vector<string> archive_files;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else if (option == "--archive-file") {
          archive_files.push_back(argv_[++iarg]);
        } else if (option == "--multiple-archive-files") {
          while (++iarg < argc_) archive_files.push_back(argv_[iarg]);
        } else {
          clog << datatools::io::warning << "Ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        { clog << datatools::io::warning << "Ignoring argument '" << argument << "'!" << endl; }
      }
      iarg++;
    }

    DT_THROW_IF(archive_files.empty(), std::logic_error, "No file given !");

    namespace io = snemo::visualization::io;
    //
    io::event_server my_event_server;

    // Open files
    DT_THROW_IF(!my_event_server.open(archive_files), std::logic_error,
                "Archive files can not be opened !");

    // Dumping information
    my_event_server.dump();

    while (my_event_server.next_event()) {
      std::clog << datatools::io::notice << "test_event_server: "
                << "Event #" << my_event_server.get_current_event_number() << " loaded"
                << std::endl;
      my_event_server.dump_event();
    }

    my_event_server.close();

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

// end of test_event_server.cxx
