// test_snemo_db_field_description.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/db/field_description.hpp>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::db::field_description'!" << std::endl;
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

  sndb::field_description runNumberDesc1("runNumberDesc1", sndb::data_type::INT, 11, false);
  std::clog << "\nName = " << std::quoted(runNumberDesc1.name()) << '\n';
  std::clog << "  Type = " << std::quoted(sndb::to_string(runNumberDesc1.type())) << '\n';
  std::clog << "  Max length = " << runNumberDesc1.max_length() << '\n';
  std::clog << "  Unsigned = " << std::boolalpha << runNumberDesc1.is_unsigned() << '\n';

  sndb::field_description runGoodDesc1("runGoodDesc1", sndb::data_type::BOOLEAN, 1, false);
  std::clog << "\nName = " << std::quoted(runGoodDesc1.name()) << '\n';
  std::clog << "  Type = " << std::quoted(sndb::to_string(runGoodDesc1.type())) << '\n';
  std::clog << "  Max length = " << runGoodDesc1.max_length() << '\n';
  std::clog << "  Unsigned = " << std::boolalpha << runGoodDesc1.is_unsigned() << '\n';

  sndb::field_description runDetailsDesc1("runDetailsDesc1", sndb::data_type::JSON, 1, false);
  std::clog << "\nName = " << std::quoted(runDetailsDesc1.name()) << '\n';
  std::clog << "  Type = " << std::quoted(sndb::to_string(runDetailsDesc1.type())) << '\n';

  sndb::field_entry runNumberField1("run_num", "SuperNEMO run number official field", runNumberDesc1);
  std::clog << "\nName = " << std::quoted(runNumberField1.name()) << '\n';
  std::clog << "  Text = " << std::quoted(runNumberField1.text()) << '\n';
  std::clog << "  Description = " << std::quoted(runNumberField1.description().name()) << '\n';

  sndb::field_entry runGoodField1("run_good", "SuperNEMO run good flag field", runGoodDesc1);
  std::clog << "\nName = " << std::quoted(runGoodField1.name()) << '\n';
  std::clog << "  Text = " << std::quoted(runGoodField1.text()) << '\n';
  std::clog << "  Description = " << std::quoted(runGoodField1.description().name()) << '\n';

  sndb::field_entry runDetailsField1("run_details", "Details about the run", runDetailsDesc1);
  std::clog << "\nName = " << std::quoted(runDetailsField1.name()) << '\n';
  std::clog << "  Text = " << std::quoted(runDetailsField1.text()) << '\n';
  std::clog << "  Description = " << std::quoted(runDetailsField1.description().name()) << '\n';
 
  return;
}
