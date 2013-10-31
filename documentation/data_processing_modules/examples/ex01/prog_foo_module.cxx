// Standard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/exception.h>

// This project
#include "foo_module.h"

void test_foo_module_simple()
{
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                "Running test_foo_module_simple...");
  {
    // Instantiate a 'foo_module' object:
    falaise::example::foo_module fm;
    fm.set_foo_bank_key("Bar");
    fm.set_foo_flag_key("Dummy");

    // Print it:
    fm.tree_dump(std::cout, "Foo module: ", "", true);
    std::cout << "|-- Foo bank key: '" << fm.get_foo_bank_key() << "'" << std::endl;
    std::cout << "`-- Foo flag key: '" << fm.get_foo_flag_key() << "'" << std::endl;

    // Initialization of the module with minimal resources
    // (no service manager, non external dictionary of modules,
    // no configuration parameters).
    fm.initialize_simple();

    // Instantiate a event record object:
    datatools::things event_record;
    // Print the event record:
    event_record.tree_dump(std::cout, "Event record: ");

    // Module processing the event record:
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Processing the event record...");
    fm.process(event_record);

    // Print the event record:
    event_record.tree_dump(std::cout, "Event record: ");
    const datatools::properties & aBar = event_record.get<datatools::properties>("Bar");
    aBar.tree_dump(std::cout, "The 'Bar' from the event record: ");
  }

  return;
}

void test_foo_module_standalone()
{
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                "Running test_foo_module_standalone...");
  {
    // Instantiate a 'foo_module' object:
    falaise::example::foo_module fm;

    datatools::properties fm_config;
    std::string foo_config_file = "config/dummy.conf";
    datatools::fetch_path_with_env(foo_config_file);
    datatools::properties::read_config(foo_config_file, fm_config);
    // Initialization of the module in standalone mode
    // (no service manager, non external dictionary of modules).
    fm.initialize_standalone(fm_config);

    // Print it:
    fm.tree_dump(std::cout, "Foo module: ", "", true);
    std::cout << "|-- Foo bank key: '" << fm.get_foo_bank_key() << "'" << std::endl;
    std::cout << "`-- Foo flag key: '" << fm.get_foo_flag_key() << "'" << std::endl;

    // Instantiate a event record object:
    datatools::things event_record;
    // Print the event record:
    event_record.tree_dump(std::cout, "Event record: ");

    // Module processing the event record:
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                  "Processing the event record...");
    fm.process(event_record);

    // Print the event record:
    event_record.tree_dump(std::cout, "Event record: ");
    const datatools::properties & aBar = event_record.get<datatools::properties>("Bar");
    aBar.tree_dump(std::cout, "The 'Bar' from the event record: ");
  }

  return;
}


int main(int argc_, char* argv_[]) {
  int error_code = EXIT_SUCCESS;
  try {
    test_foo_module_simple();
    test_foo_module_standalone();
  }
  catch (std::exception & error) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
