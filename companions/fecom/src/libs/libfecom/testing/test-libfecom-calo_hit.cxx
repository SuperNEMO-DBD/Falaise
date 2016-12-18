// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_hit.hpp>

int main(int argc_, char ** argv_)
{
  try {
    fecom::calo_hit hit;
    hit.slot_index = 4;
    hit.channel = 1;
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
