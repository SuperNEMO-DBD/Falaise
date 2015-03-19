//test_calo_ctw.cxx

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
#include <snemo/digitization/calo_ctw.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::calo_ctw' !" << std::endl;
    
    snemo::digitization::calo_ctw my_calo_ctw;
    my_calo_ctw.grab_geom_id().set_type(422);
    my_calo_ctw.grab_geom_id().set_address(3,0,10); // address of geom id : RACK_INDEX, CRATE_INDEX,BOARD_INDEX. 
    my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    my_calo_ctw.grab_auxiliaries().store_flag("mock");
    my_calo_ctw.set_wall(snemo::digitization::calo_ctw::MAIN_WALL);
    my_calo_ctw.set_clocktick_25ns(20);
    my_calo_ctw.set_htm_pc(5); // Argument is multiplicity per crate
    std::bitset<10> zoning_word (std::string("0001110100"));
    my_calo_ctw.set_zoning_word(zoning_word);
    my_calo_ctw.set_lto_pc_bit(1);
    my_calo_ctw.tree_dump(std::clog, "my_calo_CTW : ", "INFO : ");
    my_calo_ctw.lock_ctw();
    
    std::bitset<10> zoning_word_get;
    my_calo_ctw.get_zoning_word(zoning_word);
    std::clog << "Zoning word of this CTW : " << zoning_word << std::endl;

    std::set<int> active_zones; // Active zones for this CTW.
    std::set<int>::iterator it;
    std::clog << "Number of active zones : " << my_calo_ctw.compute_active_zones(active_zones) << std::endl;
    std::clog << "Active zones are :";
    for (it = active_zones.begin(); it != active_zones.end(); it++)
      {
	std::clog << ' ' << *it;
      }
    std::clog << std::endl;

    datatools::data_writer writer ("test_calo_ctw.xml",
				   datatools::using_multi_archives);
    writer.store(my_calo_ctw);
    
    datatools::data_reader reader("test_calo_ctw.xml",
				  datatools::using_multi_archives);

    snemo::digitization::calo_ctw my_calo_ctw2;
    
    reader.load(my_calo_ctw2);
    my_calo_ctw2.tree_dump(std::clog, "my_calo_ctw2 from file test_calo_ctw.xml : ", "INFO : ");

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
