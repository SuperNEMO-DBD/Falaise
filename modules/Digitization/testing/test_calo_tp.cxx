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
#include <snemo/digitization/calo_tp.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try { 
 
    // snemo::digitization::calo_tp my_calo_tp; 
    // my_calo_tp.set_hit_id(3);
    // my_calo_tp.grab_geom_id().set_type(2742);
    // my_calo_tp.grab_geom_id().set_address(0,1,17,9);
    // my_calo_tp.grab_auxiliaries().store("author", "guillaume");
    // my_calo_tp.grab_auxiliaries().store_flag("fake");
    // my_calo_tp.set_clocktick_25ns(32);
    // my_calo_tp.tree_dump(std::clog, "my_calo_TP : ", "INFO : ");

    // {
    //   datatools::data_writer writer("test_calo_tp.xml",
    // 				    datatools::using_multi_archives);

    //   writer.store(my_calo_tp);
    // }
   
    snemo::digitization::calo_tp my_calo_tp3; 
    my_calo_tp3.set_hit_id(42);
    my_calo_tp3.grab_geom_id().set_type(42);
    my_calo_tp3.grab_geom_id().set_address(0,1,4,10);
    my_calo_tp3.grab_auxiliaries().store("author", "guillaume");
    my_calo_tp3.grab_auxiliaries().store_flag("fake");
    my_calo_tp3.set_clocktick_25ns(20);
    my_calo_tp3.set_htm(5);
    my_calo_tp3.set_xt_bit(1);
    my_calo_tp3.tree_dump(std::clog, "my_calo_TP3 : ", "INFO : ");

    // my_calo_tp.reset();
    // my_calo_tp.tree_dump(std::clog, "my_calo_TP : ", "INFO : ");

    // {
    //   datatools::data_reader reader("test_calo_tp.xml",
    // 				    datatools::using_multi_archives);

    //   reader.load(my_calo_tp);
    //   my_calo_tp.tree_dump(std::clog, "my_calo_TP (loaded) : ", "INFO : ");
    // }

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

