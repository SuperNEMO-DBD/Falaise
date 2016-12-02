// test_version.cxx

// Standard libraries :
#include <iostream>
#include <string>

// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// Falaise :
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/version.h>

int main( int  argc_ , char ** argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::version' !" << std::endl;

    std::clog << "Version major: "    << snemo::digitization::version::get_major() << std::endl;
    std::clog << "Version minor: "    << snemo::digitization::version::get_minor() << std::endl;
    std::clog << "Version patch: "    << snemo::digitization::version::get_patch() << std::endl;
    std::clog << "Version revision: " << snemo::digitization::version::get_revision() << std::endl;
    std::clog << "Version : '"        << snemo::digitization::version::get_version() << "'" << std::endl;

  } catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
