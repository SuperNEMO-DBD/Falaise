// test_falaise.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <falaise/version.h>

int main(int /* argc_ */, char** /* argv_ */) {
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'Falaise' library." << std::endl;

    std::clog << "Falaise version : " << falaise::version::get_version() << std::endl;
    std::clog << "Falaise resource dir : '" << falaise::get_resource_dir() << "'" << std::endl;
    std::clog << "Falaise resource file : '" << falaise::get_resource("README.rst") << "'"
              << std::endl;

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
