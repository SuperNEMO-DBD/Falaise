// test_geiger_tp_data.cxx

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
#include <snemo/digitization/geiger_tp_data.h>

int main(int /* argc_ */, char ** /* argv_ */)
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::geiger_tp_data' !" << std::endl;

    snemo::digitization::geiger_tp_data my_geiger_tp_data;

    // Creation of few geiger tps. Geiger tp data is a collection of geiger tps.
    // geom id for geiger tp : (RACK_INDEX, CRATE_INDEX, BOARD_INDEX)
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(12);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,1,7);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("mock");
      gg_tp.set_clocktick_800ns(15);
      gg_tp.tree_dump(std::clog, "GG TP : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(17);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,2,7);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("mock");
      gg_tp.set_clocktick_800ns(15);
      gg_tp.tree_dump(std::clog, "GG TP : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(17);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,2,8);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("mock");
      gg_tp.set_clocktick_800ns(18);
      gg_tp.tree_dump(std::clog, "GG TP : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(17);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,3,8);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("mock");
      gg_tp.set_clocktick_800ns(20);
      gg_tp.tree_dump(std::clog, "GG TP : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      gg_tp.set_hit_id(17);
      gg_tp.grab_geom_id().set_type(42);
      gg_tp.grab_geom_id().set_address(3,4,6);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("mock");
      gg_tp.set_clocktick_800ns(20);
      gg_tp.tree_dump(std::clog, "GG TP : ", "INFO : ");
    }
    my_geiger_tp_data.lock();

    std::clog << "Clocktick min = " << my_geiger_tp_data.get_clocktick_min() << std::endl;
    std::clog << "Clocktick max = " << my_geiger_tp_data.get_clocktick_max() << std::endl;

    // Creation of a list of geiger tp per clocktick per crate
    std::vector<datatools::handle<snemo::digitization::geiger_tp> > my_list_of_gg_tp_per_clocktick_per_crate;

    // For this example, 20 is the clocktick and 0 is the number of crate
    my_geiger_tp_data.get_list_of_gg_tp_per_clocktick_per_crate(20, 0, my_list_of_gg_tp_per_clocktick_per_crate);

    std::clog << "Size of my list of geiger tp for a clocktick = 20 and crate number = 0 : " << my_list_of_gg_tp_per_clocktick_per_crate.size() << " geiger tp(s) in the list " << std::endl;

    for (int i = 0; i < (int) my_list_of_gg_tp_per_clocktick_per_crate.size(); i++)
      {
        my_list_of_gg_tp_per_clocktick_per_crate[i].get().tree_dump(std::clog, "My list of geiger TP per clocktick per crate" , "INFO : ");
      }
    my_geiger_tp_data.tree_dump(std::clog, "my_geiger_TP_data : ", "INFO : ");

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
