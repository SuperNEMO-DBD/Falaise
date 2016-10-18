// test_geiger_ctw_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

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
#include <snemo/digitization/geiger_ctw_data.h>

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::geiger_ctw_data' !" << std::endl;

    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
    {
      snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
      my_geiger_ctw.grab_geom_id().set_type(42);
      my_geiger_ctw.grab_geom_id().set_address(3,0,10);
      my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
      my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      my_geiger_ctw.set_clocktick_800ns(20);
      my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
      my_geiger_ctw.grab_geom_id().set_type(42);
      my_geiger_ctw.grab_geom_id().set_address(3,0,10);
      my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
      my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      my_geiger_ctw.set_clocktick_800ns(35);
      my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
      my_geiger_ctw.grab_geom_id().set_type(42);
      my_geiger_ctw.grab_geom_id().set_address(3,1,10);
      my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
      my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      my_geiger_ctw.set_clocktick_800ns(20);
      my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW_data : ", "INFO : ");
    }  
    {
      snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
      my_geiger_ctw.grab_geom_id().set_type(42);
      my_geiger_ctw.grab_geom_id().set_address(3,2,10);
      my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
      my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      my_geiger_ctw.set_clocktick_800ns(35);
      my_geiger_ctw.tree_dump(std::clog, "my_geiger_CTW_data : ", "INFO : ");
    }  

    std::clog << "Clocktick min = " << my_geiger_ctw_data.get_clocktick_min() << std::endl;
    std::clog << "Clocktick max = " << my_geiger_ctw_data.get_clocktick_max() << std::endl;

    // Creation of a list of CTW per clocktick
    std::vector<datatools::handle<snemo::digitization::geiger_ctw> > my_list_of_gg_ctw_per_clocktick;

    // For this example, 20 is the clocktick
    my_geiger_ctw_data.get_list_of_geiger_ctw_per_clocktick(20,my_list_of_gg_ctw_per_clocktick);
    

    std::clog << "Size of my list of geiger CTW for a clocktick = 20  : " << my_list_of_gg_ctw_per_clocktick.size() << " geiger CTW(s) in the list " << std::endl;

    for (unsigned int i = 0; i < my_list_of_gg_ctw_per_clocktick.size(); i++)
      {
	my_list_of_gg_ctw_per_clocktick[i].get().tree_dump(std::clog, "My CTW(s) in the list with a clocktick = 20 : ", "INFO : ");
      }
    my_geiger_ctw_data.tree_dump(std::clog, "my_geiger_CTW_data : ", "INFO : ");
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
