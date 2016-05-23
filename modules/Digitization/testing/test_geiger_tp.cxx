// test_geiger_tp.cxx

// Standard libraries :
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/io_factory.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/geiger_tp.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try { 
    std::clog << "Test program for class 'snemo::digitization::geiger_tp' !" << std::endl;

    snemo::digitization::geiger_tp my_geiger_tp;
    
    std::bitset<snemo::digitization::geiger::tp::TP_SIZE> my_tp_bitset(std::string("1111100101000001010100000101010000010101000001010110101"));

    my_geiger_tp.set_gg_tp_bitset(my_tp_bitset);
    my_geiger_tp.set_clocktick_800ns(5);
    my_geiger_tp.set_tracker_row_mode(6);
    my_geiger_tp.set_board_id(12);
    my_geiger_tp.set_crate_id(2);
    my_geiger_tp.lock();
    my_geiger_tp.tree_dump(std::clog, "my_geiger_tp : ", "INFO : ");
    std::clog << "The end." << std::endl;
  }

  catch (std::exception & error) {
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

