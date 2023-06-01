// test_snemo_services_run_info_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/services/run_info_service.h>
#include <falaise/falaise.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::run_info_service'!" << std::endl;
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
  falaise::terminate();
  return (error_code);
}

void test1()
{
  std::clog << "\nTest 1:\n";
 
   
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "files");
  std::vector<std::string> runInfoMapFiles;
  runInfoMapFiles = { \
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list1.conf",
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list2.conf" \
  };
  runInfoServiceConfig.store("files.run_lists", runInfoMapFiles);
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize_standalone(runInfoServiceConfig);
  runInfoService.print_tree(std::clog);
  runInfoService.reset();
  return;
}
