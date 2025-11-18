// test_snemo_db_table_description.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/db/table_description.hpp>

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

  namespace sndb = snemo::db;

  sndb::field_description runNumberDesc1("runNumberDesc1", sndb::data_type::INT, 11, true);
  std::clog << "\nName = " << std::quoted(runNumberDesc1.name()) << '\n';
  std::clog << "  Type = " << runNumberDesc1.type() << '\n';
  std::clog << "  Max length = " << runNumberDesc1.max_length() << '\n';
  std::clog << "  Unsigned = " << std::boolalpha << runNumberDesc1.is_unsigned() << '\n';

  sndb::field_description runStartTimeDesc1("runStartTimeDesc1", sndb::data_type::DATETIME, 20);
  std::clog << "\nName = " << std::quoted(runStartTimeDesc1.name()) << '\n';
  std::clog << "  Type = " << runStartTimeDesc1.type() << '\n';
  std::clog << "  Max length = " << runStartTimeDesc1.max_length() << '\n';

  sndb::field_description runDurationDesc1("runDurationDesc1", sndb::data_type::INTEGER, 20, true);
  std::clog << "\nName = " << std::quoted(runDurationDesc1.name()) << '\n';
  std::clog << "  Type = " << runDurationDesc1.type() << '\n';
  std::clog << "  Max length = " << runDurationDesc1.max_length() << '\n';
  std::clog << "  Unsigned = " << std::boolalpha << runNumberDesc1.is_unsigned() << '\n';

  sndb::field_description runShifterDesc1("runShifterDesc1", sndb::data_type::VAR_STR, 20);
  std::clog << "\nName = " << std::quoted(runShifterDesc1.name()) << '\n';
  std::clog << "  Type = " << runShifterDesc1.type() << '\n';
  std::clog << "  Max length = " << runShifterDesc1.max_length() << '\n';

  sndb::table_description runInfoDesc1("RunInfoDesc1");
  runInfoDesc1.add_field("run_num", "Run number", runNumberDesc1);
  runInfoDesc1.add_field("run_start_time", "Run start time (UTC)", runStartTimeDesc1);
  runInfoDesc1.add_field("run_duration", "Run duration (s)", runDurationDesc1);
  runInfoDesc1.add_field("run_shifter", "Run shifter", runShifterDesc1);
  std::clog << "\nName = " << std::quoted(runInfoDesc1.name()) << '\n';
  std::clog << "  Number of fields = " << runInfoDesc1.fields().size() << '\n';

  sndb::table_entry runInfoTab1("run_info", "SuperNEMO run info official table", runInfoDesc1);
  std::clog << "\nName = " << std::quoted(runInfoTab1.name()) << '\n';
  std::clog << "  Text = " << std::quoted(runInfoTab1.text()) << '\n';
  std::clog << "  Description = " << std::quoted(runInfoTab1.description().name()) << '\n';
 
  return;
}
