// test_vertex_measurement.cxx

// Standard library:
#include <iostream>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

// This project:
#include <falaise/snemo/datamodels/vertex_measurement.h>

int main()
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'vertex_measurement' class." << std::endl;
    // Fake vertex measurement
    snemo::datamodel::vertex_measurement VM;
    geomtools::blur_spot & a_vertex = VM.grab_vertex();
    geomtools::placement::from_string("10 -15 20 (mm)", a_vertex.grab_placement());
    VM.tree_dump(std::cout, "Vertex measurement dump:", "[notice]: ");

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
