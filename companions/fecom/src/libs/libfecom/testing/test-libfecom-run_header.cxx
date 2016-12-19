// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/run_header.hpp>

int main(int argc_, char ** argv_)
{
  try {
    fecom::run_header rh;
    rh.software_version = "V1.2";
    rh.date = "2016.12.16";
    rh.time = "13h.49m.59s.431ms";
    rh.data_type = "RAW";

    rh.tree_dump(std::clog, "Calorimeter calibration:");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
