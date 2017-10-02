// -*- mode: c++ ; -*-
// test_boost_serial_access.cxx

// Standard library
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>

// This plugin
#include <falaise/snemo/io/boost_access.h>
#include <falaise/snemo/io/event_server.h>
#include <falaise/snemo/view/options_manager.h>

using namespace std;

int main(int argc_, char** argv_) {
  int error_code = EXIT_FAILURE;
  try {
    clog << "Test program for class 'boost_serial_access'!" << endl;

    vector<string> filenames;
    filenames.push_back("test_boost_access.xml");
    datatools::data_writer writer(filenames.back());
    // datatools::things bag;
    // bag.add<datatools::properties>("fake_prop");
    datatools::properties fake;
    writer.store(fake);

    namespace svi = snemo::visualization::io;
    snemo::visualization::view::options_manager::get_instance().set_logging_priority(
        datatools::logger::PRIO_TRACE);

    // boost_serial access instance
    svi::boost_access my_boost_access;

    // Validate file
    DT_THROW_IF(!my_boost_access.is_valid(filenames), std::logic_error,
                "Files are not boost archive !");

    // After validating, open file and check readibility
    DT_THROW_IF(!my_boost_access.open(filenames), std::logic_error, "Files are not readable !");

    my_boost_access.close();

    return EXIT_SUCCESS;
  } catch (exception& x) {
    cerr << x.what() << endl;
  } catch (...) {
    cerr << "Unexpected error!" << endl;
  }
  return error_code;
}

// end of test_boost_serial_access.cxx
