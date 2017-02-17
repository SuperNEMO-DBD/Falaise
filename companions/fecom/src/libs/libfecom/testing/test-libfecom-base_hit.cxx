// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/base_hit.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {
    fecom::base_hit hit;
    hit.hitmode = fecom::base_hit::SIG_CALORIMETER;
    hit.tree_dump(std::clog, "Base hit:");

    fecom::base_hit hit_2;
    hit.hitmode = fecom::base_hit::SIG_TRACKER;
    hit.hit_id = 49;
    hit.tree_dump(std::clog, "Base hit 2:");


  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
