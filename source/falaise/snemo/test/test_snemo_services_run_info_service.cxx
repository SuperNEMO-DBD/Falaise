// test_snemo_services_run_info_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/run_info_service.h>
#include <falaise/falaise.h>

void test1();
void test2();
void test3();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::run_info_service'!" << std::endl;
    // test1();
    test2();
    test3();
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

void test2()
{
  std::clog << "\nTest 2:\n";
 
   
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "csv_files");
  std::vector<std::string> runInfoMapCsvFiles;
  runInfoMapCsvFiles = { \
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list_2025.csv"
  };
  runInfoServiceConfig.store("csv_files.run_lists", runInfoMapCsvFiles);
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize_standalone(runInfoServiceConfig);
  runInfoService.print_tree(std::clog);
  runInfoService.reset();
  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";
 
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "db");
  runInfoServiceConfig.store("db_label", "db");
  std::uint32_t runMin = 0u;
  std::uint32_t runMax = 1000000;
  runInfoServiceConfig.store_integer("db.run_min", runMin);
  runInfoServiceConfig.store_integer("db.run_max", runMax);
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize_standalone(runInfoServiceConfig);
  runInfoService.print_tree(std::clog);
  runInfoService.reset();
  return;
}
