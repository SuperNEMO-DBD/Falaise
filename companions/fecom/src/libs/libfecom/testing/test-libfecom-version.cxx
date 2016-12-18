// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/version.hpp>

int main(int argc_, char ** argv_)
{
  std::clog << "Version: " << FECOM_LIB_VERSION << std::endl;
  return EXIT_SUCCESS;
}
