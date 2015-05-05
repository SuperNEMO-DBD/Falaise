//test_trigger_algorithm.cxx
// Standard libraries :
#include <iostream>

// GSL:
#include <bayeux/mygsl/rng.h>

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
#include <snemo/digitization/geiger_tp_constants.h>
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>
#include <snemo/digitization/tracker_trigger_algorithm.h>


int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::tracker_trigger_algorithm' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
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

    datatools::things ER;
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.add_preconstructed_type(snemo::digitization::mapping::GEIGER_CATEGORY_TYPE);
    my_e_mapping.initialize();

 
    snemo::digitization::geiger_tp_data my_geiger_tp_data;
    // Creation of few geiger tps


    // for (int i = 0; i < 20; i++)
    //   {
    // 	{
    // 	  snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
    // 	  geomtools::geom_id gg_electronic_id;
    // 	  gg_electronic_id.set_type(666);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 0);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, i);
    // 	  std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000000000000000000010000000000000000001"));
    // 	  gg_tp.set_header(27 + i, // hit id 
    // 			   gg_electronic_id,
    // 			   20, // clocktickk
    // 			   snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    // 			   snemo::digitization::mapping::SIDE_MODE,
    // 			   snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
    // 	  gg_tp.set_data(gg_tp_word);
    // 	  gg_tp.grab_auxiliaries().store("author", "guillaume");
    // 	  gg_tp.grab_auxiliaries().store_flag("fake");
    // 	  gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    // 	}
    //   } //end of loop i
    // for (int i = 0; i < 20; i++)
    //   {
    // 	{
    // 	  snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
    // 	  geomtools::geom_id gg_electronic_id;
    // 	  gg_electronic_id.set_type(666);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 1);
    // 	  gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, i);
    // 	  std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000000000000010000000000000000000010000"));
    // 	  gg_tp.set_header(27 + i, // hit id 
    // 			   gg_electronic_id,
    // 			   20, // clocktickk
    // 			   snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    // 			   snemo::digitization::mapping::SIDE_MODE,
    // 			   snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
    // 	  gg_tp.set_data(gg_tp_word);
    // 	  gg_tp.grab_auxiliaries().store("author", "guillaume");
    // 	  gg_tp.grab_auxiliaries().store_flag("fake");
    // 	  gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    // 	}
    //   } //end of loop i
    



    // {
    //   snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
    //   geomtools::geom_id gg_electronic_id;
    //   gg_electronic_id.set_type(666);
    //   gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
    //   gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 0);
    //   gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, 1);
    //   std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000111111111111111111111111111111111111"));
    //   gg_tp.set_header(27, // hit id 
    // 		       gg_electronic_id,
    // 		       20, // clocktickk
    // 		       snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    // 		       snemo::digitization::mapping::SIDE_MODE,
    // 		       snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
    //   gg_tp.set_data(gg_tp_word);
    //   gg_tp.grab_auxiliaries().store("author", "guillaume");
    //   gg_tp.grab_auxiliaries().store_flag("fake");
    //   gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    // }
    // {
    //   snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
    //   geomtools::geom_id gg_electronic_id;
    //   gg_electronic_id.set_type(666);
    //   gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
    //   gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 0);
    //   gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, 2);
    //   std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000000000011000000011000000111000000111"));
    //   gg_tp.set_header(27, // hit id 
    // 		       gg_electronic_id,
    // 		       20, // clocktickk
    // 		       snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    // 		       snemo::digitization::mapping::SIDE_MODE,
    // 		       snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
    //   gg_tp.set_data(gg_tp_word);
    //   gg_tp.grab_auxiliaries().store("author", "guillaume");
    //   gg_tp.grab_auxiliaries().store_flag("fake");
    //   gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    // }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      geomtools::geom_id gg_electronic_id;
      gg_electronic_id.set_type(666);
      gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
      gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 1);
      gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, 9);
      std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000000000000000011110000000000000011110"));
      gg_tp.set_header(27, // hit id 
    		       gg_electronic_id,
    		       20, // clocktickk
    		       snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    		       snemo::digitization::mapping::SIDE_MODE,
    		       snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
      gg_tp.set_data(gg_tp_word);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    }
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data.add();
      geomtools::geom_id gg_electronic_id;
      gg_electronic_id.set_type(666);
      gg_electronic_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
      gg_electronic_id.set(snemo::digitization::mapping::CRATE_INDEX, 1);
      gg_electronic_id.set(snemo::digitization::mapping::BOARD_INDEX, 11);
      std::bitset<snemo::digitization::geiger::tp::TP_SIZE> gg_tp_word(std::string("0000000000000000000111111111111111111111111111111111111"));
      gg_tp.set_header(27, // hit id 
    		       gg_electronic_id,
    		       20, // clocktickk
    		       snemo::digitization::mapping::THREE_WIRES_TRACKER_MODE,
    		       snemo::digitization::mapping::SIDE_MODE,
    		       snemo::digitization::mapping::NUMBER_OF_CONNECTED_ROWS);
      gg_tp.set_data(gg_tp_word);
      gg_tp.grab_auxiliaries().store("author", "guillaume");
      gg_tp.grab_auxiliaries().store_flag("fake");
      gg_tp.tree_dump(std::clog, "GG_TP 1 : ", "INFO : ");
    }

    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;	    
    snemo::digitization::geiger_tp_to_ctw_algo geiger_tp_2_ctw;
    geiger_tp_2_ctw.initialize();

    geiger_tp_2_ctw.process(my_geiger_tp_data, my_geiger_ctw_data);
    my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");

    snemo::digitization::tracker_trigger_algorithm my_tracker_algo;
    my_tracker_algo.initialize(my_e_mapping);
    my_tracker_algo.process(my_geiger_ctw_data);
    
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
