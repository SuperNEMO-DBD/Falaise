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
      bool bad_calo_TP = false;
      {
	int iarg = 1;
	while (iarg < argc_) 
	  {
	    std::string token = argv_[iarg];
	    if (token[0] == '-') 
	      {
		std::string option = token;
		if ((option == "-B") || (option == "--bad-calo-TP")) 
		  {
		    bad_calo_TP = true;
		    std::clog << "warning: option '" << option << "' is activated !" << std::endl;	
		  }
		else 
		  {
		    std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
		  }
	      }
	    else 
	      {
		std::string argument = token;
		{
		  std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
		}
	      }
	    iarg++;
	  }
      }
    
    snemo::digitization::calo_tp_data my_calo_tp_data;

    {
    for (int i = 0; i < 15; i++)
      {
	{
	  snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
	  ctp.set_hit_id(27);
	  ctp.grab_geom_id().set_type(242);
	  ctp.grab_geom_id().set_address(0,1,14,6);
	  ctp.grab_auxiliaries().store("author", "guillaume");
	  ctp.grab_auxiliaries().store_flag("fake");
	  ctp.set_clocktick_25ns(15 + i);
	  ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
	}
      }
    }
    {
    snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
    ctp.set_hit_id(27);
    ctp.grab_geom_id().set_type(24682);
    ctp.grab_geom_id().set_address(1,0,17,6);
    ctp.grab_auxiliaries().store("author", "guillaume");
    ctp.grab_auxiliaries().store_flag("fake");
    ctp.set_clocktick_25ns(20);
    ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
    snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
    ctp.set_hit_id(27);
    ctp.grab_geom_id().set_type(14242);
    ctp.grab_geom_id().set_address(0,1,12,7);
    ctp.grab_auxiliaries().store("author", "guillaume");
    ctp.grab_auxiliaries().store_flag("fake");
    ctp.set_clocktick_25ns(20);
    ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
    {
    snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
    ctp.set_hit_id(27);
    ctp.grab_geom_id().set_type(25);
    ctp.grab_geom_id().set_address(1,1,5,6);
    ctp.grab_auxiliaries().store("author", "guillaume");
    ctp.grab_auxiliaries().store_flag("fake");
    ctp.set_clocktick_25ns(20);
    ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
    }
  
    {
      if (bad_calo_TP == true)
	{
	  snemo::digitization::calo_tp & ctp = my_calo_tp_data.add();
	  ctp.set_hit_id(27);
	  ctp.grab_geom_id().set_type(242);
	  ctp.grab_geom_id().set_address(0,1,14,6);
	  ctp.grab_auxiliaries().store("author", "guillaume");
	  ctp.grab_auxiliaries().store_flag("fake");
	  ctp.set_clocktick_25ns(20);
	  ctp.tree_dump(std::clog, "CTP 1 : ", "INFO : ");
	}
    }
    my_calo_tp_data.lock_tps();

    std::clog << "DEBUG : MAIN : clocktick min = " << my_calo_tp_data.get_clocktick_min() << std::endl;
    std::clog << "DEBUG : MAIN : clocktick max = " << my_calo_tp_data.get_clocktick_max() << std::endl;
    std::clog << "DEBUG : MAIN : clocktick range = " << my_calo_tp_data.get_clocktick_range() << std::endl;

    std::vector<datatools::handle<snemo::digitization::calo_tp> > my_list_of_calo_tp;
    my_calo_tp_data.get_list_of_tp_per_clocktick(20,my_list_of_calo_tp);

    std::clog << "DEBUG : MAIN : " << my_list_of_calo_tp[0].get().get_clocktick_25ns() << std::endl;

    for (int i = 0; i < my_list_of_calo_tp.size(); i++)
      {
	my_list_of_calo_tp[i].get().tree_dump(std::clog, "My list of calo TP clocktick = 20 ns ", "INFO : ");
      }
    
    my_calo_tp_data.tree_dump(std::clog, "my_calo_TP_data : ", "INFO : ");
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

