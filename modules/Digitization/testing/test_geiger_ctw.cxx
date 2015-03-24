//test_geiger_ctw.cxx

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
#include <snemo/digitization/geiger_ctw.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::geiger_ctw' !" << std::endl;

    snemo::digitization::geiger_ctw my_geiger_ctw;
    my_geiger_ctw.grab_geom_id().set_type(422);
    my_geiger_ctw.grab_geom_id().set_address(3,0,10); // address of geom id : RACK_INDEX, CRATE_INDEX,BOARD_INDEX. 
    my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
    my_geiger_ctw.grab_auxiliaries().store_flag("mock");
    my_geiger_ctw.set_clocktick_800ns(20); 
    unsigned long board_id = 14;
    std::bitset<100> test_gg_tp_word (std::string ("1111111111111111111111111111111111111111111111111111010110111111111111111111111111111111111111111111"));  
    my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW : ", "INFO : "); 
    my_geiger_ctw.lock_ctw();

    // datatools::data_writer writer ("test_geiger_ctw.xml",
    // 				   datatools::using_multi_archives);
    // writer.store(my_geiger_ctw);
    
    // datatools::data_reader reader("test_geiger_ctw.xml",
    // 				  datatools::using_multi_archives);

    // snemo::digitization::geiger_ctw my_geiger_ctw2;
    
    // reader.load(my_geiger_ctw2);
    // my_geiger_ctw2.tree_dump(std::clog, "my_geiger_ctw2 from file test_geiger_ctw.xml : ", "INFO : ");

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

  FALAISE_FINI();
  return error_code;
}
