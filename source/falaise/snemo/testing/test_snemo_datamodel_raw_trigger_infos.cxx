// -*- mode: c++ ; -*-
// test_snemo_datamodel_raw_trigger_infos.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>

// This project:
#include <falaise/snemo/datamodels/raw_trigger_infos.h>

int main (/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::raw_trigger_infos'!" << std::endl;

    namespace sdm = snemo::datamodel;

    // Trigger informations:
    sdm::raw_trigger_infos RTI;
    RTI.set_trigger_number(1234567);
    RTI.grab_vtzw(0)[1] = true;
    RTI.grab_ctzw(1)[3] = true;
    RTI.grab_ctzw(1)[4] = true;
    RTI.grab_ttp(1, 45)[8] = true;
    RTI.grab_ttp(1, 45)[7] = true;
    RTI.grab_ttp(1, 45)[6] = true;
    RTI.grab_ttp(1, 44)[5] = true;
    RTI.grab_ttp(1, 43)[5] = true;
    RTI.grab_ttp(1, 43)[4] = true;
    RTI.grab_ttp(1, 43)[3] = true;
    RTI.grab_ttp(1, 42)[2] = true;
    RTI.grab_ttp(1, 41)[1] = true;
    RTI.grab_ttp(1, 40)[1] = true;
    RTI.grab_ttp(1, 40)[0] = true;
    RTI.grab_ttp(0, 23)[5] = true;
    RTI.grab_ttp(0, 25)[4] = true;
    RTI.grab_ttp(0, 26)[4] = true;
    RTI.grab_ttp(0, 62)[2] = true;
    RTI.tree_dump(std::clog, "Raw trigger informations: ");

    datatools::data_writer writer("test_snemo_datamodel_raw_trigger_infos.xml");
    writer.store(RTI);

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
