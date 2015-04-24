// test_calo_tp.cxx

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
#include <snemo/digitization/mapping.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try { 
    std::clog << "Test program for class 'snemo::digitization::calo_tp' !" << std::endl;

    snemo::digitization::calo_tp my_calo_tp; 
    // Hit_id and geom_id used are mocked id (just choose for the test) because "real id" come from simulated datas.
    my_calo_tp.set_hit_id(42);
    my_calo_tp.grab_geom_id().set_type(25);
    my_calo_tp.grab_geom_id().set_address(3,0,7); // address type(RACK_INDEX, CRATE_INDEX, BOARD_INDEX)
    my_calo_tp.grab_auxiliaries().store("author", "guillaume"); 
    my_calo_tp.grab_auxiliaries().store_flag("mock");
    my_calo_tp.set_clocktick_25ns(20);
    my_calo_tp.set_htm(3); // Argument is the multiplicity. Here 3 caloFEBs passed the high threshold in the column
    my_calo_tp.set_xt_bit(1);
    my_calo_tp.set_lto_bit(1);
    my_calo_tp.tree_dump(std::clog, "my_calo_tp mock : ", "INFO : ");
    my_calo_tp.lock();

    std::clog << "Htm multiplicity is        " << my_calo_tp.get_htm() << std::endl;
    std::clog << "Htm bits  bit pos [1][0] : " << my_calo_tp.get_htm_bits() << std::endl;
    std::clog << "Lto bit   bit pos [2]    : " << my_calo_tp.is_lto() << std::endl;
    std::clog << "Xt bit    bit pos [3]    : " << my_calo_tp.is_xt() << std::endl;
    std::clog << "Spare bit bit pos [4]    : " << my_calo_tp.is_spare() << std::endl;
    
    // Test for data_writer & data_reader
   
    // datatools::data_writer writer ("test_calo_tp.xml",
    // 				   datatools::using_multi_archives);
    // writer.store(my_calo_tp);

    // datatools::data_reader reader("test_calo_tp.xml",
    // 				  datatools::using_multi_archives);

    // Reseting
    my_calo_tp.unlock();
    my_calo_tp.reset();
    my_calo_tp.tree_dump(std::clog, "my_calo_tp reseted : ", "INFO : ");

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

