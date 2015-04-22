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
    my_e_mapping.initialize("mcalo");

    // snemo::digitization::ID_convertor my_convertor;
    // my_convertor.set_geo_manager(my_manager);
    // my_convertor.set_module_number(0);
    // my_convertor.initialize();

    /*
    typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;
    typedef ID_bimap::value_type ID_doublet;
    ID_bimap test_mon_bimap;
    */
    const geomtools::geom_id GID1(1204, 0, 0, 3, 106);
    const geomtools::geom_id GID2(1204, 0, 0, 6, 95);
    const geomtools::geom_id GID3(1204, 0, 0, 5, 57);
    const geomtools::geom_id GID4(1204, 0, 0, 5, 57);

 

    geomtools::geom_id EID1;
    geomtools::geom_id EID2;
    geomtools::geom_id EID3;
   
    
    EID1 = my_e_mapping.convert_GID_to_EID(GID1);
    std::cout <<"GID1 : "<<  GID1;
    std::cout <<" ---> EID1 : "<<  EID1<<std::endl;

    EID2 = my_e_mapping.convert_GID_to_EID(GID2);
    std::cout <<"GID2 : "<<  GID2;
    std::cout <<" ---> EID2 : "<<  EID2<<std::endl;

    EID3 = my_e_mapping.convert_GID_to_EID(GID3);
    std::cout <<"GID3 : "<<  GID3;
    std::cout <<" ---> EID3 : "<<  EID3<<std::endl;

    EID1 = my_e_mapping.convert_GID_to_EID(GID4);

    std::cout <<"GID4 : "<<  GID4;
    std::cout <<" ---> EID4 : "<<  EID1<<std::endl;
    
    geomtools::geom_id EID42(666,5,1,12,7);
    geomtools::geom_id GID42;
    
    GID42 = my_e_mapping.convert_EID_to_GID(EID42);

    std::cout <<"GID42 : "<<  GID42;
    std::cout <<" ---> EID42 : "<<  EID42<<std::endl;


    geomtools::geom_id GID_CALO(1302, 0, 0, 12, 5);
    geomtools::geom_id EID_CALO;
    EID_CALO = my_e_mapping.convert_GID_to_EID(GID_CALO);
    std::cout <<"GID_CALO : "<<  GID_CALO;
    std::cout <<" ---> EID_CALO : "<<  EID_CALO<<std::endl;


    GID_CALO = my_e_mapping.convert_EID_to_GID(EID_CALO);
    std::cout <<"EID_CALO : "<<  EID_CALO;
    std::cout <<" --->  GID_CALO : "<<  GID_CALO<<std::endl;



    // test_mon_bimap.insert( ID_doublet(GID1 , EID1) );
    // test_mon_bimap.insert( ID_doublet(GID2 , EID2) );
    // test_mon_bimap.insert( ID_doublet(GID3 , EID3) );
   
    
    // for( ID_bimap::const_iterator iter = test_mon_bimap.begin(), iend = test_mon_bimap.end();  iter != iend; ++iter )
    //   {
    // 	std::cout << iter->left << " <--> " << iter->right << std::endl;
    //   }

    // ID_bimap::right_const_iterator right_iter = test_mon_bimap.right.find(EID3);
    // if (right_iter != test_mon_bimap.right.end() ){
    //   std::cout <<"EID : "<<  right_iter->first<<std::endl;
    //   std::cout <<"GID : "<<  right_iter->second<<std::endl;
    // }else{
    //   std::cout<<"EID not found ! "<< std::endl;
    // }
    
    
    
    // ID_bimap::left_const_iterator left_iter = test_mon_bimap.left.find(GID1);
    // if (left_iter != test_mon_bimap.left.end() ){
    //   std::cout <<"GID : "<<  left_iter->first<<std::endl;
    //   std::cout <<"EID : "<<  left_iter->second<<std::endl;
    // }else{
    //   std::cout<<"GID not found ! "<< std::endl;
    // }
    
    // left_iter = test_mon_bimap.left.find(GID4);
    // if (left_iter != test_mon_bimap.left.end() ){
    //   std::cout <<"GID : "<<  left_iter->first<<std::endl;
    //   std::cout <<"EID : "<<  left_iter->second<<std::endl;
    // }else{
    //   std::cout<<"GID not found ! "<< std::endl;
    // }
    


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





   
