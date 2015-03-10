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
#include <snemo/digitization/calo_crate_tw.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {

    snemo::digitization::calo_crate_tw my_calo_crate_tw;
    my_calo_crate_tw.grab_geom_id().set_type(42);
    my_calo_crate_tw.grab_geom_id().set_address(0,1,4,10);
    my_calo_crate_tw.grab_auxiliaries().store("author", "guillaume");
    my_calo_crate_tw.grab_auxiliaries().store_flag("fake");
    my_calo_crate_tw.set_clocktick_25ns(20);
    my_calo_crate_tw.set_htm_pc_info(2);
    my_calo_crate_tw.tree_dump(std::clog, "my_calo_crate_TW : ", "INFO : ");

    std::bitset<10> zoning_word (std::string("1100010010"));
    std::bitset<10> zoning_word2;
    std::set<int> active_zones;
    std::set<int>::iterator it;

    my_calo_crate_tw.set_zoning_word(zoning_word);

    my_calo_crate_tw.tree_dump(std::clog, "my_calo_crate_TW : ", "INFO : ");
    // std::clog << "DEBUG : MAIN " << my_calo_crate_tw.compute_active_zones(active_zones) << std::endl;

    // for(it = active_zones.begin(); it != active_zones.end(); it++)
    //   {
    // 	std::clog << "DEBUG : MAIN " << *it << std::endl;  
    //   }
    my_calo_crate_tw.get_zoning_word(zoning_word2);
    std::clog << "DEBUG : MAIN " << zoning_word2 << std::endl;
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
