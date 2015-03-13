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
#include <snemo/digitization/calo_tp_data.h>
#include <snemo/digitization/calo_ctw.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {

    snemo::digitization::calo_ctw my_calo_ctw;
    my_calo_ctw.grab_geom_id().set_type(42);
    my_calo_ctw.grab_geom_id().set_address(0,1,4,10);
    my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    my_calo_ctw.grab_auxiliaries().store_flag("fake");
    my_calo_ctw.set_clocktick_25ns(20);
    my_calo_ctw.reset_clocktick_25ns();
    my_calo_ctw.set_clocktick_25ns(50);
    my_calo_ctw.tree_dump(std::clog, "my_calo_CTW : ", "INFO : ");

    snemo::digitization::calo_tp_data my_calo_tp_data;
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(25);
      ctp.grab_geom_id().set_address(0,0,0,1);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(2);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }

    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(27);
      ctp.grab_geom_id().set_type(25);
      ctp.grab_geom_id().set_address(0,0,0,18);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("fake");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(1);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    my_calo_tp_data.lock_tps();

    std::vector<datatools::handle<snemo::digitization::calo_tp> > my_list_of_calo_tp;
    my_calo_tp_data.get_list_of_tp_per_clocktick(20, my_list_of_calo_tp);
    
    std::bitset<10> zoning_word;
    my_calo_ctw.get_zoning_word(zoning_word);
    std::clog << "DEBUG : MAIN : " << zoning_word << std::endl;

    my_calo_ctw.tree_dump(std::clog, "my_calo_CTW : ", "INFO : ");
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
