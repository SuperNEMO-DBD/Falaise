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
#include <snemo/digitization/calo_ctw_data.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {

    snemo::digitization::calo_ctw_data my_calo_ctw_data;
    {
      for (int i = 0; i < 3; i++)
	{
	  {
	    snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
	    my_calo_ctw.grab_geom_id().set_type(42);
	    my_calo_ctw.grab_geom_id().set_address(0,1,4,10);
	    my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
	    my_calo_ctw.grab_auxiliaries().store_flag("fake");
	    my_calo_ctw.set_clocktick_25ns(i+20);
	    my_calo_ctw.set_htm_pc(5);
	    my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
	  }
	}
    }
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      my_calo_ctw.grab_geom_id().set_type(42);
      my_calo_ctw.grab_geom_id().set_address(0,1,4,10);
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("fake");
      my_calo_ctw.set_clocktick_25ns(20);
      my_calo_ctw.set_htm_pc(1);
      my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
    }

    my_calo_ctw_data.lock_ctws();
    std::vector<datatools::handle<snemo::digitization::calo_ctw> > my_list_of_ctw;
    my_calo_ctw_data.get_list_of_calo_ctw_per_clocktick(20,my_list_of_ctw);
    
    for (int i = 0; i < my_list_of_ctw.size(); i++)
      {
	my_list_of_ctw[i].get().tree_dump(std::clog, "My list of calo CTW clocktick = 20 ns ", "INFO : ");
      }
    
    std::clog << "DEBUG : MAIN : clocktick min index = " <<  my_calo_ctw_data.get_clocktick_min_index() << std::endl;

    std::clog << "DEBUG : MAIN : clocktick max index = " <<  my_calo_ctw_data.get_clocktick_max_index() << std::endl;

    std::clog << "DEBUG : MAIN : clocktick min  = " <<  my_calo_ctw_data.get_clocktick_min() << std::endl;

    std::clog << "DEBUG : MAIN : clocktick max  = " <<  my_calo_ctw_data.get_clocktick_max() << std::endl;

    
    std::bitset<10> zoning_word (std::string("1100010010"));
    std::bitset<4> control_word (std::string("0101"));
    std::bitset<10> zoning_word2;
    std::bitset<4> control_word2;
    std::set<int> active_zones;
    const std::set<int>::iterator it;
    
    // my_calo_ctw.set_zoning_word(zoning_word);
    // my_calo_ctw.set_lto_pc_bit(0);
    // my_calo_ctw.set_xt_pc_bit(0);
    // my_calo_ctw.set_control_word(control_word);

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
