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
    tchit.slot_index = 8;
    tchit.trigger_id = 12;
    tchit.feast_id = 0;
    tchit.channel = 12;
    tchit.channel_type = fecom::tracker_channel_hit::ANODIC_CHANNEL;
    tchit.timestamp_type = "t0";
    tchit.timestamp_value = 42857;

    if (tchit.is_valid()) tchit.tree_dump(std::clog, "Valid tracker channel hit:");
    else std::clog << "Tracker channel hit is not valid ! " << std::endl;

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
