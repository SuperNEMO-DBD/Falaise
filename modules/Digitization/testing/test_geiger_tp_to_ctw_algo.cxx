//test_geiger_tp_to_ctw_algo.cxx

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
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::geiger_tp_to_ctw_algo' !" << std::endl;

    snemo::digitization::geiger_tp_data my_geiger_tp_data;
    // Creation of few geiger tps
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(5,0,14);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(20);
      std::bitset<55> my_gg_tp_bitset(std::string("1101010111011111110101011101111111010101110111111101010111011111101110111111101010111011111101111111"));
      gg_tp.set_gg_tp_bitset(my_gg_tp_bitset);
      gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(5,1,2);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(20);
      std::bitset<55> my_gg_tp_bitset(std::string("1101010111011111110101011101111111010101110111111101010111011111101110111111101010111011111101111111"));
      gg_tp.set_gg_tp_bitset(my_gg_tp_bitset);
      gg_tp.tree_dump(std::clog, "GG_TP 2 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,0,17);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(35);
      gg_tp.tree_dump(std::clog, "GG_TP 3 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,1,5);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(35);
      gg_tp.tree_dump(std::clog, "GG_TP 4 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,1,11);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(42);
      gg_tp.tree_dump(std::clog, "GG_TP 5 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(27);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,0,15);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.set_clocktick_800ns(20);
      gg_tp.tree_dump(std::clog, "GG_TP 6 : ", "INFO : ");
    }
    my_geiger_tp_data.lock();

    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
    snemo::digitization::geiger_tp_to_ctw_algo algo;

    // Algo take a list of geiger tp to fill a list of geiger ctw data

    algo.initialize();
    algo.process(my_geiger_tp_data, my_geiger_ctw_data);

    my_geiger_ctw_data.tree_dump(std::clog, "my_geiger_ctw_data : ", "INFO : ");

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
