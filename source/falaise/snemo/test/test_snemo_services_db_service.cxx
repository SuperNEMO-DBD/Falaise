// test_snemo_services_run_info_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <cassert>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/db/table_selection.hpp>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::db_service'!" << std::endl;
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

  snemo::db_service dbService;
  datatools::properties dbServiceConfig;
  dbServiceConfig.store("logging.priority", "debug");
  dbServiceConfig.store("name", "RunDb");
  dbServiceConfig.store("display_name", "SuperNEMO run database");
  dbServiceConfig.store("terse_description", "Readonly access service to the SuperNEMO run database");
  dbServiceConfig.store("db_access", "~/.sndb.conf");
  dbServiceConfig.store("sndb_config", "@falaise:snemo/demonstrator/db/sndb/1.0/main.conf");
  dbService.initialize_standalone(dbServiceConfig);
  //return;
  std::clog << "\n\nDB Service:\n";
  dbService.print_tree(std::clog, snemo::db_service::empty_options());

  try {

    assert(dbService.has_table("Run_phases"));
    assert(dbService.has_table("Run_infos"));
    assert(dbService.has_table("Dataset_models"));
    assert(dbService.has_table("DatasetProducer_models"));
    assert(dbService.has_table("DatasetProducer_configs"));
    assert(dbService.has_table("DatasetProducers"));
    assert(dbService.has_table("Datasets"));
    assert(not dbService.has_table("Run_foo"));
        
    {
      std::clog << "\nSelection: " << '\n';
      snemo::db::table_selection_type tabSel;
      dbService.process_select_all_statement("Run_phases", tabSel);
      std::clog << "#records = " << tabSel.size() << '\n';
    }
    
    {
      std::clog << "\nSelection: " << '\n';
      snemo::db::table_selection_type tabSel;
      dbService.process_select_all_statement("Run_infos", tabSel);
      std::clog << "#records = " << tabSel.size() << '\n';
    }

  } catch (std::exception & err) {
    std::cerr << "table selection error: " << err.what() << std::endl;
  }
  dbService.reset();

  return;
}
