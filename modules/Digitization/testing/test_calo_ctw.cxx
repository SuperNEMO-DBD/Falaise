// snemo/digitization/test_calo_ctw.cxx

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
#include <snemo/digitization/calo_ctw.h>

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::calo_ctw' !" << std::endl;

    snemo::digitization::calo_ctw my_calo_ctw;
    my_calo_ctw.grab_geom_id().set_type(422);
    my_calo_ctw.grab_geom_id().set_address(
        3, 0, 10);  // address of geom id : RACK_INDEX, CRATE_INDEX, BOARD_INDEX.
    my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    my_calo_ctw.grab_auxiliaries().store_flag("mock");
    my_calo_ctw.set_clocktick_25ns(20);
    my_calo_ctw.set_htm_main_wall(5);  // Argument is multiplicity per crate
    std::bitset<10> zoning_word(std::string("0001110100"));
    my_calo_ctw.set_main_zoning_word(zoning_word);
    my_calo_ctw.set_lto_main_wall_bit(1);
    my_calo_ctw.tree_dump(std::clog, "my_calo_CTW : ", "INFO : ");

    std::set<int> active_zones;  // Active zones for this CTW.
    std::set<int>::iterator it;
    std::clog << "Number of active zones : " << my_calo_ctw.compute_active_zones(active_zones)
              << std::endl;
    std::clog << "Active zones are :";
    for (it = active_zones.begin(); it != active_zones.end(); it++) {
      std::clog << ' ' << *it;
    }
    std::clog << std::endl;

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
