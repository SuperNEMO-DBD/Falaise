// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_calibration.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::calo_calibration ccalib;
    ccalib.logging = datatools::logger::PRIO_TRACE;

    std::string cpedfile
      = "${FECOM_RESOURCES_DIR}/data/samples/run_0/Calo_Pedestal_Files/calo_pedestals_slot_0.txt";
    ccalib.load_pedestals(cpedfile);
    ccalib.tree_dump(std::clog, "Calorimeter calibration:");

    for (auto it_map = ccalib.calo_pedestals.begin();
	 it_map != ccalib.calo_pedestals.end();
	 it_map++)
      {
	std::clog << "Calo_channel_id board " << it_map->first.board_id << " channel " << it_map->first.channel_id << " <=> ";
	it_map->second.tree_dump(std::clog, "A calo pedestal");
	// it_map->second.print_offsets(std::clog);


	std::clog << std::endl;
      }


  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
