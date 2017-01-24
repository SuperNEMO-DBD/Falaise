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
    hit.hit_id = 42;
    hit.hitmode = fecom::base_hit::SIG_TRACKER;
    hit.slot_index = 4;
    hit.channel = 1;
    hit.event_id = 155;
    hit.set_anodic_times(50,
			 61,
			 82,
			 150,
			 189);

    hit.set_cathodic_times(1352,
			   1801);


    hit.tree_dump(std::clog, "Tracker hit:");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
