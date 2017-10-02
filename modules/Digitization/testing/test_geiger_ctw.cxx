// test_geiger_ctw.cxx

// Standard libraries :
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/logger.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/geiger_ctw.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::geiger_ctw' !" << std::endl;

    snemo::digitization::geiger_ctw my_geiger_ctw;
    my_geiger_ctw.grab_geom_id().set_type(422);
    my_geiger_ctw.grab_geom_id().set_address(
        3, 0, 10);  // address of geom id : RACK_INDEX, CRATE_INDEX,BOARD_INDEX.
    my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
    my_geiger_ctw.grab_auxiliaries().store_flag("mock");
    my_geiger_ctw.set_clocktick_800ns(20);
    std::bitset<100> test_gg_tp_word(
        std::string("111111111111111111111111111111111111111111111111111101011011111111111111111111"
                    "1111111111111111111111"));
    my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW : ", "INFO : ");
    std::clog << "The end." << std::endl;
  }

  catch (std::exception& error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  falaise::terminate();
  return error_code;
}
