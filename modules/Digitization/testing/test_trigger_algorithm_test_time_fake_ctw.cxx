// test_trigger_algorithm_test_time_fake_ctw.cxx
// Standard libraries :
#include <iostream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// GSL:
#include <bayeux/mygsl/rng.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>

#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

#include <snemo/digitization/trigger_algorithm_test_time.h>

int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_output_path  = false;
  bool is_help         = false;
  std::string output_path = "";
  
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-op" || arg == "--output-path")
      {
	is_output_path = true;
	output_path = argv_[++iarg];	
      }

    else if (arg =="-h" || arg == "--help")
      {
	is_help = true;
      }

    iarg++;
  }

  if (is_help) 
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_trigger_algorithm_test_time_fake_ctw [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-op [ --output path ]    set a path where all files are stored" << std::endl
		<< "Example : " << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_trigger_algorithm_test_time -op ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default" 
		<< "If no options are set, programs have default values :" << std::endl << std::endl
		<< "output path          = ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl << std::endl;
      return 0;
    }

  try {
    // boolean for debugging (display etc)

    std::clog << "Test program for class 'snemo::digitization::test_trigger_algorithm_test_time_fake_ctw' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
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

    datatools::fetch_path_with_env(output_path);
    if (is_output_path) output_path = output_path;
    else output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    datatools::fetch_path_with_env(output_path);

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.add_preconstructed_type(snemo::digitization::mapping::GEIGER_CATEGORY_TYPE);
    my_e_mapping.add_preconstructed_type(snemo::digitization::mapping::CALO_MAIN_WALL_CATEGORY_TYPE);
    my_e_mapping.initialize();
    // Clock manager :
    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();		

    // Loading memory from external files
    std::string mem1 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem1.conf";
    std::string mem2 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem2.conf";
    std::string mem3 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem3.conf";
    std::string mem4 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem4.conf";
    std::string mem5 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem5.conf";

    datatools::fetch_path_with_env(mem1);
    datatools::fetch_path_with_env(mem2);
    datatools::fetch_path_with_env(mem3);
    datatools::fetch_path_with_env(mem4);
    datatools::fetch_path_with_env(mem5);

    // Properties to configure trigger algorithm :
    datatools::properties trigger_config;
    int  calo_circular_buffer_depth = 4;
    int  calo_threshold = 1;
    bool inhibit_both_side_coinc = false;
    bool inhibit_single_side_coinc = false;    
    int coincidence_calorimeter_gate_size = 4;
    int L2_decision_coincidence_gate_size = 5; // Gate for calorimeter / tracker coincidence (5 x 1600 ns) 
    int previous_event_buffer_depth = 10; // Maximum number of PER record (with an internal counter of 1 ms)
    bool activate_any_coincidences = true;
    
    trigger_config.store("calo.circular_buffer_depth", calo_circular_buffer_depth);
    trigger_config.store("calo.total_multiplicity_threshold", calo_threshold);
    trigger_config.store("calo.inhibit_both_side",  inhibit_both_side_coinc);
    trigger_config.store("calo.inhibit_single_side",  inhibit_single_side_coinc);
    trigger_config.store("tracker.mem1_file", mem1);
    trigger_config.store("tracker.mem2_file", mem2); 
    trigger_config.store("tracker.mem3_file", mem3);
    trigger_config.store("tracker.mem4_file", mem4);
    trigger_config.store("tracker.mem5_file", mem5);
    trigger_config.store("coincidence_calorimeter_gate_size", coincidence_calorimeter_gate_size);
    trigger_config.store("L2_decision_coincidence_gate_size", L2_decision_coincidence_gate_size);
    trigger_config.store("previous_event_buffer_depth", previous_event_buffer_depth);
    trigger_config.store("activate_any_coincidences", activate_any_coincidences);
    
    // Creation and initialization of trigger algorithm :
    snemo::digitization::trigger_algorithm_test_time my_trigger_algo;
    my_trigger_algo.set_electronic_mapping(my_e_mapping);
    my_trigger_algo.set_clock_manager(my_clock_manager);
    my_trigger_algo.initialize(trigger_config);
    
    my_clock_manager.compute_clockticks_ref(random_generator);

    // Creation of calo ctw data :
    snemo::digitization::calo_ctw_data my_calo_ctw_data;

    // Creation of geiger ctw data :
    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
    	    
    // Add fake CTW calo and geiger :
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 0, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(155,
			     my_ctw_gid,
			     200); // hit, gid, clocktick25ns
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 5;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      // my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [0] : ", "INFO : ");
    }

    // Creation of a prompt track during 10 CT 1600:
    for (unsigned int ict = 8; ict < 19; ict++)
      {
	{
	  snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
	  geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
	  my_geiger_ctw.set_header(ict+50,
				   my_ctw_gid,
				   ict);
	  my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
	  my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      
	  std::bitset<55> geiger_information;
	  for (unsigned int i = 0; i < 10; i++)
	    {
	      geiger_information.set(i, true);
	    }
	  unsigned int block_index = 6;
	  my_geiger_ctw.set_55_bits_in_ctw_word(block_index, geiger_information);
	  std::bitset<5> hardware_status (std::string("01111"));
	  std::bitset<2> crate_id = 0x1;
	  my_geiger_ctw.set_full_hardware_status(hardware_status);
	  my_geiger_ctw.set_full_crate_id(crate_id);
	  // my_geiger_ctw.tree_dump(std::clog, "My_geiger_CTW [0] : ", "INFO : ");
	}  
      }

    // Test creation of a second event calorimeter / tracker (to see the comportement of PERs)
    // Add fake CTW calo and geiger :
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 0, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(155,
			     my_ctw_gid,
			     16000); // hit, gid, clocktick25ns
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 7;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      // my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [0] : ", "INFO : ");
    }

    {
      for (unsigned int ict = 500; ict < 512; ict++)
	{
	  snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
	  geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
	  my_geiger_ctw.set_header(ict+50,
				   my_ctw_gid,
				   ict); // hit, gid, clocktick800ns
	  my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
	  my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      
	  std::bitset<55> geiger_information;
	  for (unsigned int i = 0; i < 15; i++)
	    {
	      geiger_information.set(i, true);
	    }
	  unsigned int block_index = 18;
	  my_geiger_ctw.set_55_bits_in_ctw_word(block_index, geiger_information);
	  std::bitset<5> hardware_status (std::string("01111"));
	  std::bitset<2> crate_id = 0x1;
	  my_geiger_ctw.set_full_hardware_status(hardware_status);
	  my_geiger_ctw.set_full_crate_id(crate_id);
	  // my_geiger_ctw.tree_dump(std::clog, "My_geiger_CTW [0] : ", "INFO : ");
	}
    }

    // An other delayed tracker event to see with which PER it will be compared:
    {
      // Creation of a delayed track during 3 CT 1600:
      for (unsigned int ict = 900; ict < 906; ict++)
	{
	  snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
	  geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
	  my_geiger_ctw.set_header(ict+50,
				   my_ctw_gid,
				   ict); // hit, gid, clocktick800ns
	  my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
	  my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      
	  std::bitset<55> geiger_information;
	  for (unsigned int i = 0; i < 4; i++)
	    {
	      geiger_information.set(i, true);
	    }
	  unsigned int block_index = 8;
	  my_geiger_ctw.set_55_bits_in_ctw_word(block_index, geiger_information);
	  std::bitset<5> hardware_status (std::string("01111"));
	  std::bitset<2> crate_id = 0x1;
	  my_geiger_ctw.set_full_hardware_status(hardware_status);
	  my_geiger_ctw.set_full_crate_id(crate_id);
	  // my_geiger_ctw.tree_dump(std::clog, "My_geiger_CTW [0] : ", "INFO : ");
	}
    }
    

    // An other far delayed tracker event to see the fall of PER counters:
    {
      for (unsigned int ict = 1600; ict < 1605; ict++)
	{
	  snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
	  geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
	  my_geiger_ctw.set_header(ict+50,
				   my_ctw_gid,
				   ict); // hit, gid, clocktick800ns
	  my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
	  my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      
	  std::bitset<55> geiger_information;
	  for (unsigned int i = 0; i < 4; i++)
	    {
	      geiger_information.set(i, true);
	    }
	  unsigned int block_index = 18;
	  my_geiger_ctw.set_55_bits_in_ctw_word(block_index, geiger_information);
	  std::bitset<5> hardware_status (std::string("01111"));
	  std::bitset<2> crate_id = 0x1;
	  my_geiger_ctw.set_full_hardware_status(hardware_status);
	  my_geiger_ctw.set_full_crate_id(crate_id);
	  // my_geiger_ctw.tree_dump(std::clog, "My_geiger_CTW [0] : ", "INFO : ");
	}
    }


    // An other far far far delayed tracker event to see the fall of all PERs counters:
    {
      for (unsigned int ict = 4000; ict < 4005; ict++)
	{
	  snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
	  geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
	  my_geiger_ctw.set_header(ict+50,
				   my_ctw_gid,
				   ict); // hit, gid, clocktick800ns
	  my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
	  my_geiger_ctw.grab_auxiliaries().store_flag("mock");
      
	  std::bitset<55> geiger_information;
	  for (unsigned int i = 0; i < 6; i++)
	    {
	      geiger_information.set(i, true);
	    }
	  unsigned int block_index = 5;
	  my_geiger_ctw.set_55_bits_in_ctw_word(block_index, geiger_information);
	  std::bitset<5> hardware_status (std::string("01111"));
	  std::bitset<2> crate_id = 0x1;
	  my_geiger_ctw.set_full_hardware_status(hardware_status);
	  my_geiger_ctw.set_full_crate_id(crate_id);
	  // my_geiger_ctw.tree_dump(std::clog, "My_geiger_CTW [0] : ", "INFO : ");
	}
    }

    // Creation of outputs collection structures for calo and tracker
    std::vector<snemo::digitization::trigger_structures::calo_summary_record> calo_collection_records;
    std::vector<snemo::digitization::trigger_structures::coincidence_calo_record> coincidence_collection_calo_records;
    std::vector<snemo::digitization::trigger_structures::tracker_record>   tracker_collection_records;
    std::vector<snemo::digitization::trigger_structures::coincidence_event_record> coincidence_collection_records;
		
    // Trigger process
    my_trigger_algo.process(my_calo_ctw_data,
			    my_geiger_ctw_data);	   
		
    // Finale structures :
    calo_collection_records = my_trigger_algo.get_calo_records_25ns_vector();
    coincidence_collection_calo_records =  my_trigger_algo.get_coincidence_calo_records_1600ns_vector();
    tracker_collection_records = my_trigger_algo.get_tracker_records_vector();
    coincidence_collection_records = my_trigger_algo.get_coincidence_records_vector();

    my_trigger_algo.reset_data();
            
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
