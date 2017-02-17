// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_hit.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::calo_hit hit;
    geomtools::geom_id electronic_id(fecom::calo_constants::CALO_CHANNEL_TYPE, 0, 11); // EID [Type:board.channel]
    hit.electronic_id = electronic_id;
    hit.hitmode = fecom::base_hit::SIG_CALORIMETER;
    hit.trigger_id = 12;
    hit.waveform_data_size = 16;
    for (std::size_t icell = 0; icell < hit.waveform_data_size; icell++) {
      hit.set_raw_sample(icell, 23);
    }
    hit.tree_dump(std::clog, "Calo hit:");
    hit.print_waveform(std::cout);

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
