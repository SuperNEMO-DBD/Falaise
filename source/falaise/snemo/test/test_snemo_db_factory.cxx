// test_snemo_db_factory.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/db/factory.hpp>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::db::factory'!" << std::endl;
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
 
  snemo::db::factory dbFactory;

  {
    std::string dbFactoryConfigPath = "@falaise:snemo/demonstrator/db/sndb/1.0/rundb/base_field_types.conf";
    
    datatools::multi_properties dbFactoryConfig;
    uint32_t options =					\
      datatools::multi_properties::config::RESOLVE_PATH
      | datatools::multi_properties::config::DONT_CLEAR;
    dbFactoryConfig.read(dbFactoryConfigPath, options);
    dbFactory.build(dbFactoryConfig);
    std::clog << "\ndbFactory=\n";
    dbFactory.print_tree(std::clog);
  }
  
  {
    std::string dbFactoryConfigPath = "@falaise:snemo/demonstrator/db/sndb/1.0/rundb/dataset_tables.conf";
    
    datatools::multi_properties dbFactoryConfig;
    uint32_t options =					\
      datatools::multi_properties::config::RESOLVE_PATH
      | datatools::multi_properties::config::DONT_CLEAR;
    dbFactoryConfig.read(dbFactoryConfigPath, options);
    dbFactory.build(dbFactoryConfig);
    std::clog << "\ndbFactory=\n";
    dbFactory.print_tree(std::clog);
  }
  
  return;
}
