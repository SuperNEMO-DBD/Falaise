// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/base_hit.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {

    std::clog << "Base hit is pure virtual now !" << std::endl;


  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
