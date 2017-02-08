// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {



  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
