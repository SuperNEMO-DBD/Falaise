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
    std::clog << "Test program for class 'snemo::digitization::calo_ctw_data' !" << std::endl;

    snemo::digitization::calo_ctw_data my_calo_ctw_data;
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      my_calo_ctw.grab_geom_id().set_type(42);
      my_calo_ctw.grab_geom_id().set_address(3,0,10);
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_clocktick_25ns(20);
      my_calo_ctw.set_htm_pc(1);
      std::bitset<10> zoning_word (std::string("0001110100"));
      my_calo_ctw.set_zoning_word(zoning_word);
      my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      my_calo_ctw.grab_geom_id().set_type(42);
      my_calo_ctw.grab_geom_id().set_address(3,0,10);
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_clocktick_25ns(35);
      my_calo_ctw.set_htm_pc(7);
      std::bitset<10> zoning_word (std::string("1010101010"));
      my_calo_ctw.set_zoning_word(zoning_word);
      my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      my_calo_ctw.grab_geom_id().set_type(42);
      my_calo_ctw.grab_geom_id().set_address(3,1,10);
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_clocktick_25ns(20);
      my_calo_ctw.set_htm_pc(2);
      std::bitset<10> zoning_word (std::string("0010000100"));
      my_calo_ctw.set_zoning_word(zoning_word);
      my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      my_calo_ctw.grab_geom_id().set_type(42);
      my_calo_ctw.grab_geom_id().set_address(3,2,10);
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_clocktick_25ns(35);
      my_calo_ctw.set_htm_pc(12);
      std::bitset<10> zoning_word (std::string("1111110100"));
      my_calo_ctw.set_zoning_word(zoning_word);
      my_calo_ctw.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");
    }  
    my_calo_ctw_data.lock_ctws();

    std::clog << "Clocktick min = " << my_calo_ctw_data.get_clocktick_min() << std::endl;
    std::clog << "Clocktick max = " << my_calo_ctw_data.get_clocktick_max() << std::endl;

    // Creation of a list of CTW per clocktick
    std::vector<datatools::handle<snemo::digitization::calo_ctw> > my_list_of_ctw_per_clocktick;

    // For this example, 20 is the clocktick
    my_calo_ctw_data.get_list_of_calo_ctw_per_clocktick(20,my_list_of_ctw_per_clocktick);
    

    std::clog << "Size of my list of calo CTW for a clocktick = 20  : " << my_list_of_ctw_per_clocktick.size() << " calo CTW(s) in the list " << std::endl;

    for (int i = 0; i < my_list_of_ctw_per_clocktick.size(); i++)
      {
	my_list_of_ctw_per_clocktick[i].get().tree_dump(std::clog, "My CTW(s) in the list with a clocktick = 20 : ", "INFO : ");
      }
    my_calo_ctw_data.tree_dump(std::clog, "my_calo_CTW_data : ", "INFO : ");

    datatools::data_writer writer ("test_calo_ctw_data.xml",
				   datatools::using_multi_archives);
    writer.store(my_calo_ctw_data);
 
    datatools::data_reader reader("test_calo_ctw_data.xml",
				  datatools::using_multi_archives);

    snemo::digitization::calo_ctw_data my_calo_ctw_data2;
    
    reader.load(my_calo_ctw_data2);
    my_calo_ctw_data2.tree_dump(std::clog, "my_calo_ctw_data2 from file test_calo_ctw_data.xml : ", "INFO : ");

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
