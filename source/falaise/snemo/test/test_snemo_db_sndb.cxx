// test_snemo_db_sndb.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/db/sndb.hpp>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::db::table_description'!" << std::endl;
    test1();
    std::clog << "\nThe end." << std::endl;
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
 
  snemo::db::sndb sndbDriver;

  std::string sndbDriverConfigPath = "@falaise:snemo/demonstrator/db/sndb/1.0/main.conf";
  datatools::fetch_path_with_env(sndbDriverConfigPath);
  datatools::properties sndbDriverConfig;
  datatools::properties::read_config(sndbDriverConfigPath, sndbDriverConfig);
  sndbDriver.initialize(sndbDriverConfig);
  std::clog << "SNDB:\n";
  sndbDriver.print_tree(std::clog);
  sndbDriver.reset();
  
  return;
}
