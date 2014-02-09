// test_gg_hit.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

// This project:
#include <TrackFit/gg_hit.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'gg_hit'!" << std::endl;
    bool debug = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") ||(option == "--debug")) {
          debug = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // A Geiger hit used as input of the track fit algorithms:
    TrackFit::gg_hit hit;
    hit.set_id(356);
    hit.set_x(10.5 * CLHEP::cm);
    hit.set_y(32.3 * CLHEP::cm);
    hit.set_z(51.7 * CLHEP::cm);
    hit.set_sigma_z(1.2 * CLHEP::cm);
    hit.set_t(750 * CLHEP::ns);
    hit.set_rmax(15. * CLHEP::mm);
    hit.set_max_length(2700. * CLHEP::mm);
    hit.set_r(11.3 * CLHEP::mm);
    hit.set_sigma_r(0.9 * CLHEP::mm);
    hit.tree_dump(std::clog, "A Geiger hit: ");

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

// end of test_gg_hit.cxx
