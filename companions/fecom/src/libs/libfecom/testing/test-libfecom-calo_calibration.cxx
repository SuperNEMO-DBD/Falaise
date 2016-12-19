// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_calibration.hpp>

int main(int argc_, char ** argv_)
{
  try {
    fecom::calo_calibration ccalib;
    ccalib.logging = datatools::logger::PRIO_TRACE;

    std::string cpedfile
      = "${FECOM_RESOURCES_DIR}/data/samples/run_0/Calo_Pedestal_Files/calo_pedestals_slot_0.txt";
    uint16_t board_id = 0;
    ccalib.load_pedestals(board_id, cpedfile);
    // board_id = 1;
    // ccalib.load_pedestals(board_id, cpedfile);
    ccalib.tree_dump(std::clog, "Calorimeter calibration:");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
