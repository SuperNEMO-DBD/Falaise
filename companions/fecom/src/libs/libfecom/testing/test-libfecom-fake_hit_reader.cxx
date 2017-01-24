// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_hit_reader.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {

    fecom::calo_hit_reader creader;
    creader.set_logging(datatools::logger::PRIO_TRACE);
    creader.set_input_filename("${FECOM_RESOURCES_DIR}/data/samples/52fake_run/calo_fake_tracker_hits_1.dat");
    creader.initialize();
    fecom::run_header header;
    creader.load_run_header(header);
    header.tree_dump(std::clog, "Run header:");

    fecom::calo_hit chit;
    std::size_t hit_counter = 0;
    while(creader.has_next_hit()) {
      chit.reset();
      creader.load_next_hit(chit);
      chit.tree_dump(std::clog, "Calo hit:");
      if (hit_counter++ > 3) break;
    }

    creader.reset();

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
