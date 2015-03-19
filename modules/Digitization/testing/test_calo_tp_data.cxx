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

int main(int argc_, char **  argv_)
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {   
    std::clog << "Test program for class 'snemo::digitization::calo_tp_data' !" << std::endl;

    snemo::digitization::calo_tp_data my_calo_tp_data;

    // Creation of few calo tp. Calo tp data is a collection of calo tps.
    // geom id for calo tp : (RACK_INDEX, CRATE_INDEX, BOARD_INDEX)
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(12);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,1,7);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("mock");
      ctp.set_clocktick_25ns(15);
      ctp.set_htm(1);
      ctp.set_lto_bit(1);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(17);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,1,15);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("mock");
      ctp.set_clocktick_25ns(15);
      ctp.set_htm(7);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(17);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,1,2);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("mock");
      ctp.set_clocktick_25ns(18);
      ctp.set_htm(5);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(17);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,12);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("mock");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(2);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
      snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
      ctp.set_hit_id(17);
      ctp.grab_geom_id().set_type(42);
      ctp.grab_geom_id().set_address(3,0,18);
      ctp.grab_auxiliaries().store("author", "guillaume");
      ctp.grab_auxiliaries().store_flag("mock");
      ctp.set_clocktick_25ns(20);
      ctp.set_htm(3);
      ctp.set_lto_bit(1);
      ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    my_calo_tp_data.lock_tps();

    std::clog << "Clocktick min = " << my_calo_tp_data.get_clocktick_min() << std::endl;
    std::clog << "Clocktick max = " << my_calo_tp_data.get_clocktick_max() << std::endl;

    // Creation of a list of calo tp per clocktick per crate
    std::vector<datatools::handle<snemo::digitization::calo_tp> > my_list_of_calo_tp_per_clocktick_per_crate;
    
    // For this example, 20 is the clocktick and 0 is the number of crate 
    my_calo_tp_data.get_list_of_tp_per_clocktick_per_crate(20, 0, my_list_of_calo_tp_per_clocktick_per_crate); 
    
    std::clog << "Size of my list of calo tp for a clocktick = 20 and crate number = 0 : " << my_list_of_calo_tp_per_clocktick_per_crate.size() << " calo tp(s) in the list " << std::endl;

    for (int i = 0; i < my_list_of_calo_tp_per_clocktick_per_crate.size(); i++)
      {
	my_list_of_calo_tp_per_clocktick_per_crate[i].get().tree_dump(std::clog, "My list of calo TP per clocktick per crate" , "INFO : ");
      }
    
    my_calo_tp_data.tree_dump(std::clog, "my_calo_TP_data : ", "INFO : ");

    datatools::data_writer writer ("test_calo_tp_data.xml",
				   datatools::using_multi_archives);
    writer.store(my_calo_tp_data);
    
    datatools::data_reader reader("test_calo_tp_data.xml",
				  datatools::using_multi_archives);

    snemo::digitization::calo_tp_data my_calo_tp_data2;
    
    reader.load(my_calo_tp_data2);
    my_calo_tp_data2.tree_dump(std::clog, "my_calo_tp_data2 from file test_calo_tp_data.xml : ", "INFO : ");

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

