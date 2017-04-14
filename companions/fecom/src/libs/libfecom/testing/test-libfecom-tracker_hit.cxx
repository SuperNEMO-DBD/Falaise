// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/tracker_hit.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::tracker_hit hit;

    hit.set_anodic_timestamps(50,
			      61,
			      82,
			      150,
			      189);

    hit.set_cathodic_timestamps(1352,
				1801);


    hit.tree_dump(std::clog, "Tracker hit:");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
