// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/tracker_channel_hit.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::tracker_channel_hit tchit;

    tchit.hit_id = 42;
    tchit.hitmode = fecom::base_hit::SIG_TRACKER;
    geomtools::geom_id electronic_id(fecom::tracker_constants::ANODIC_CHANNEL_TYPE, 0, 0, 12); // EID [Type:board.feast.channel]
    tchit.electronic_id = electronic_id;
    tchit.trigger_id = 12;
    tchit.timestamp_type = "t0";
    tchit.timestamp_value = 1205;

    if (tchit.is_valid()) tchit.tree_dump(std::clog, "Valid tracker channel hit:");
    else std::clog << "Tracker channel hit is not valid ! " << std::endl;

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
