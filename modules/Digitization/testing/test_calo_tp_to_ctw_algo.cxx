//test_calo_tp_to_ctw_algo.cxx

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
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::calo_tp_to_ctw_algo' !" << std::endl;

    snemo::digitization::calo_tp_data my_calo_tp_data;
    // Creation of few calo tps
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,14);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(2);
      ctp.set_lto_bit(1);
      ctp.tree_dump(std::clog, "CTP 1 : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,5);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP 2 : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,17);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(35);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP 3 : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,1,5);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(35);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP 4 : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,1,11);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(42);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP 5 : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,15);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP 6 : ", "INFO : ");
    }
    my_calo_tp_data.lock();

    snemo::digitization::calo_ctw_data my_calo_ctw_data;
    snemo::digitization::calo_tp_to_ctw_algo algo;

    // Algo take a list of calo tp to fill a list of calo ctw data

    algo.initialize();
    algo.process(my_calo_tp_data, my_calo_ctw_data);

    my_calo_ctw_data.tree_dump(std::clog, "my_calo_ctw_data : ", "INFO : ");

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
