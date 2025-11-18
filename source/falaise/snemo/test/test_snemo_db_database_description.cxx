// test_snemo_db_database_description.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/db/database_description.hpp>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::db::database_description'!" << std::endl;
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

  namespace sndb = snemo::db;

  sndb::database_description runDbDesc1("RunDbDesc1");
  std::clog << "Name = " << std::quoted(runDbDesc1.name()) << '\n';

  sndb::table_description runInfoDesc1("RunInfoDesc1");
  sndb::table_description runStatusDesc1("RunStatusDesc1");
  sndb::table_description runFilesDesc1("RunFilesDesc1");

  runDbDesc1.add_table("run_info",   "Run information",  runInfoDesc1);
  runDbDesc1.add_table("run_status", "Run status",  runStatusDesc1);
  runDbDesc1.add_table("run_file",   "Run files",  runFilesDesc1);
  
  sndb::database_entry runDb1("rundb_1", "SuperNEMO run official database", runDbDesc1);
  std::clog << "Name = " << std::quoted(runDb1.name()) << '\n';
  std::clog << "Text = " << std::quoted(runDb1.text()) << '\n';
  std::clog << "Description = " << std::quoted(runDb1.description().name()) << '\n';
  std::clog << "Number of tables = " << runDb1.description().tables().size() << '\n';
 
  return;
}
