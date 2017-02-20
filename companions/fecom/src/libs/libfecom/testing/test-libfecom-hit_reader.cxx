// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/calo_hit_reader.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {

    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_TRACE);
    reader.set_input_filename("${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.dat");
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    header.tree_dump(std::clog, "Run header:");

    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;

    std::size_t hit_counter = 0;
    while(reader.has_next_hit()) {
      chit.reset();
      reader.load_next_hit(chit,
			   tchit);
      chit.tree_dump(std::clog, "Calo hit:");
      if (hit_counter++ > 3) break;
    }

   reader.reset();

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
