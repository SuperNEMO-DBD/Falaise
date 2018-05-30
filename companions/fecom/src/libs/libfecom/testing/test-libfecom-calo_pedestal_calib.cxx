// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_pedestal_calib.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::calo_pedestal_calib ped;
    ped.board_id = 4;
    ped.channel = 1;
    ped.offset_size = 16;
    for (std::size_t icell = 0; icell < ped.offset_size; icell++) {
      ped.set_offset(icell, 23);
    }
    ped.tree_dump(std::clog, "Calo pedestal calib:");
    ped.print_offsets(std::cout);

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
