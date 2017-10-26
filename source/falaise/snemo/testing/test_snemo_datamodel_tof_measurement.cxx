// test_tof_measurement.cxx

// Standard library:
#include <iostream>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

// This project:
#include <falaise/snemo/datamodels/tof_measurement.h>

int main()
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'tof_measurement' class." << std::endl;
    // Fake TOF measurement
    snemo::datamodel::tof_measurement TM;
    snemo::datamodel::tof_measurement::probability_type & int_probs
      = TM.grab_internal_probabilities();
    int_probs.push_back(10 * CLHEP::perCent);
    int_probs.push_back(30 * CLHEP::perCent);
    int_probs.push_back(40 * CLHEP::perCent);
    snemo::datamodel::tof_measurement::probability_type & ext_probs
      = TM.grab_external_probabilities();
    ext_probs.push_back(1e-1 * CLHEP::perCent);
    ext_probs.push_back(1e-4 * CLHEP::perCent);
    ext_probs.push_back(1e-5 * CLHEP::perCent);
    TM.tree_dump(std::cout, "TOF measurement dump:", "[notice]: ");

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
