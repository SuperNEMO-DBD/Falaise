// test_snemo_services_rc.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/services/rc.h>
#include <falaise/snemo/rc/run_list.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::rc'!" << std::endl;
    test1();
    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test1()
{
  std::clog << "\nTest 1:\n";
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  snemo::rc_svc rcService;
  datatools::properties rcServiceConfig;
  rcService.initialize_standalone(rcServiceConfig);

  return;
}
