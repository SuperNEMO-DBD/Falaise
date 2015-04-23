//test_signal_to_tp_process.cxx

// Standard libraries :
#include <iostream>

// GSL:
#include <bayeux/mygsl/rng.h>
// BOOST : 
#include <boost/bimap.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>

int main( int  argc_ , char ** argv_  )
{
 FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::test_bimap' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);

    std::string manager_config_file;
    
    manager_config_file = "~/data/my_falaise/config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
   
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_manager.initialize (manager_config);

 
  
    snemo::digitization::electronic_mapping my_e_mapping;
    
 

    my_e_mapping.set_geo_manager(my_manager);

 

    my_e_mapping.set_module_number(0);
    my_e_mapping.initialize();

    const geomtools::geom_id GID1(1204, 0, 0, 3, 97);
    const geomtools::geom_id GID2(1204, 0, 0, 6, 98);
    const geomtools::geom_id GID3(1204, 0, 0, 5, 99);
    const geomtools::geom_id GID4(1204, 0, 0, 5, 100);
    const geomtools::geom_id GID5(1204, 0, 0, 5, 101);

    geomtools::geom_id EID1;
    geomtools::geom_id EID2;
    geomtools::geom_id EID3;
    geomtools::geom_id EID4;
    geomtools::geom_id EID5;

    
    my_e_mapping.convert_GID_to_EID(snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE, GID1, EID1);
    std::cout <<"GID1 : "<<  GID1;
    std::cout <<" ---> EID1 : "<<  EID1<<std::endl;

    my_e_mapping.convert_GID_to_EID(snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE, GID2, EID2);
    std::cout <<"GID2 : "<<  GID2;
    std::cout <<" ---> EID2 : "<<  EID2<<std::endl;

    my_e_mapping.convert_GID_to_EID(snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE, GID3, EID3);
    std::cout <<"GID3 : "<<  GID3;
    std::cout <<" ---> EID3 : "<<  EID3<<std::endl;

    my_e_mapping.convert_GID_to_EID(snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE, GID4, EID4);
    std::cout <<"GID4 : "<<  GID4;
    std::cout <<" ---> EID4 : "<<  EID4<<std::endl;
    
    my_e_mapping.convert_GID_to_EID(snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE, GID5, EID5);
    std::cout <<"GID5 : "<<  GID5;
    std::cout <<" ---> EID5 : "<<  EID5<<std::endl;

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





   
