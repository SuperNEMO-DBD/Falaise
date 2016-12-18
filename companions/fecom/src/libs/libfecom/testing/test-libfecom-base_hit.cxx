// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/base_hit.hpp>

int main(int argc_, char ** argv_)
{
  try {
    fecom::base_hit hit;
    hit.hitmode = fecom::base_hit::SIG_CALORIMETER;
    hit.tree_dump(std::clog, "Base hit:");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
