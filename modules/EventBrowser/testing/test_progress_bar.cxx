// -*- mode: c++ ; -*-
// test_progress_bar.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <TApplication.h>
#include <TROOT.h>
#include <TSystem.h>

#include <snvisualization/view/progress_bar.h>

#include <datatools/utils/ioutils.h>

using namespace std;
using namespace snemo::visualization::view;

int main(int argc_, char** argv_) {
  const string appname_prefix = "test_progress_bar: ";

  int error_code = EXIT_SUCCESS;
  try {
    clog << datatools::utils::io::notice << appname_prefix
         << "Test program for class 'progress_bar'!" << endl;

    bool debug = false;

    bool visu_mode = true;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if (option == "--visu-mode") {
          visu_mode = true;
        } else {
          clog << datatools::utils::io::warning << appname_prefix << "Ignoring option '" << option
               << "'!" << endl;
        }
      } else {
        string argument = token;
        /* Here you may add more argument handlers... */
        {
          clog << datatools::utils::io::warning << appname_prefix << "Ignoring argument '"
               << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    // open a root application
    int narg = 1;
    TApplication* my_application = new TApplication("App", &narg, argv_);

    progress_bar* my_progress_bar =
        new progress_bar(gClient->GetRoot(), 300, 100, "Test fictive event", "%.0f events");

    if (!my_progress_bar) {
      ostringstream message;
      message << "Could not start up!" << endl;
      throw runtime_error(message.str());
    }

    my_progress_bar->kill_application(true);

    const unsigned int nmax = 10;
    my_progress_bar->set_range(0, nmax);

    for (unsigned int i_event = 0; i_event < nmax; ++i_event) {
      if (!my_progress_bar->increment(i_event)) break;
      gSystem->Sleep(100);
    }

    delete my_progress_bar;

    // my_application->Run ();
  } catch (exception& x) {
    cerr << datatools::utils::io::error << appname_prefix << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << datatools::utils::io::error << appname_prefix << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_progress_bar.cxx
