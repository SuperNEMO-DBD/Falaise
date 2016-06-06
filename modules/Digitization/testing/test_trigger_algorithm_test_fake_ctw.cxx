// trigger_algorithm_efficiency_validation.cxx
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
// Root : 
#include "TFile.h"
#include "TTree.h"

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/calo_ctw_constants.h>
#include <snemo/digitization/mapping.h>

#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

#include <snemo/digitization/trigger_algorithm.h>

int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;
  bool is_event_number = false;
  bool is_output_path  = false;
  bool is_run_number   = false;
  bool is_display      = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;
  int arg_run_number    = -1;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") 
      {
	is_input_file = true;
	input_filename = argv_[++iarg];
      } 

    else if (arg == "-op" || arg == "--output-path")
      {
	is_output_path = true;
	output_path = argv_[++iarg];	
      }
    
    else if (arg == "-n" || arg == "--number")
      {
        is_event_number = true;
	arg_event_number    = atoi(argv_[++iarg]);
      }

    else if (arg == "-d" || arg == "--display")
      {
	is_display = true;
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
		<< "$ BuildProducts/bin/falaisedigitizationplugin-trigger_algorithm_efficiency_validation [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output path ]    set a path where all files are stored" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "Example : " << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-trigger_algorithm_efficiency_validation --input ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio -op ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default" 
		<< " --number 5" << std::endl << std::endl
		<< "If no options are set, programs have default values :" << std::endl << std::endl
		<< "input file           = ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio" << std::endl
		<< "output path          = ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl
		<< "number of events     = 10" << std::endl << std::endl;
      return 0;
    }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;

    std::clog << "Test program for class 'snemo::digitization::trigger_algorithm_efficiency_validation' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
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
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";

    // Trigger Decision Data "TDD" bank label :
    std::string TDD_bank_label = "TDD";
    
    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 10;
  
    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
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
    int  coincidence_calorimeter_gate_size = 8; // Number of CT 1600 coincidence gate for calorimeter
    int previous_event_buffer_depth = 10; // Maximum number of PER record (with an internal counter of 1 ms)
    bool activate_coincidence = true;
    
    trigger_config.store("calo.circular_buffer_depth", calo_circular_buffer_depth);
    trigger_config.store("calo.total_multiplicity_threshold", calo_threshold);
    trigger_config.store("calo.inhibit_both_side",  inhibit_both_side_coinc);
    trigger_config.store("calo.inhibit_single_side",  inhibit_single_side_coinc);
    trigger_config.store("tracker.mem1_file", mem1);
    trigger_config.store("tracker.mem2_file", mem2); 
    trigger_config.store("tracker.mem3_file", mem3);
    trigger_config.store("tracker.mem4_file", mem4);
    trigger_config.store("tracker.mem5_file", mem5);
    trigger_config.store("coincidence.calorimeter_gate_size", coincidence_calorimeter_gate_size);
    trigger_config.store("coincidence.previous_event_buffer_depth", previous_event_buffer_depth);
    trigger_config.store("activate_coincidence", activate_coincidence);

    // Creation of trigger display manager :
    snemo::digitization::trigger_display_manager my_trigger_display;
    datatools::properties trigger_display_config;
    bool calo_25ns      = true;
    bool calo_1600ns    = true;
    bool tracker_1600ns = true;
    bool coinc_1600ns   = true;
    trigger_display_config.store("calo_25ns", calo_25ns);
    trigger_display_config.store("calo_1600ns", calo_1600ns);
    trigger_display_config.store("tracker_1600ns", tracker_1600ns);
    trigger_display_config.store("coinc_1600ns", coinc_1600ns);
    my_trigger_display.initialize(trigger_display_config);
    
    // Creation and initialization of trigger algorithm :
    snemo::digitization::trigger_algorithm my_trigger_algo;
    my_trigger_algo.set_electronic_mapping(my_e_mapping);
    //my_trigger_algo.set_trigger_display_manager(my_trigger_display);
    my_trigger_algo.initialize(trigger_config);

    // Internal counters
    int psd_count = 0;         // Event counter
    
    my_clock_manager.compute_clockticks_ref(random_generator);

    if (debug) my_clock_manager.tree_dump(std::clog, "Clock utils : ", "INFO : ");

    // Creation of calo ctw data :
    snemo::digitization::calo_ctw_data my_calo_ctw_data;
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 0, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(42,
			     my_ctw_gid,
			     12); // hit, gid, clocktick
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 4;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [0] : ", "INFO : ");
    }    
    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 0, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(43,
    			     my_ctw_gid,
    			     15); // hit, gid, clocktick
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 9;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [4] : ", "INFO : ");
    }     
    // {
    //   snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
    //   my_calo_ctw.set_header(43,
    // 			     my_ctw_gid,
    // 			     13); // hit, gid, clocktick
    //   my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_calo_ctw.grab_auxiliaries().store_flag("mock");
    //   my_calo_ctw.set_htm_main_wall(1);
    //   int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 5;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [4] : ", "INFO : ");
    // } 

    // {
    //   snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
    //   my_calo_ctw.set_header(57,
    // 			     my_ctw_gid,
    // 			     1500); // hit, gid, clocktick
    //   my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_calo_ctw.grab_auxiliaries().store_flag("mock");
    //   my_calo_ctw.set_htm_main_wall(1);
    //   int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 8;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [1] : ", "INFO : ");
    // }
    // {
    //   snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
    //   my_calo_ctw.set_header(58,
    // 			     my_ctw_gid,
    // 			     1503); // hit, gid, clocktick
    //   my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_calo_ctw.grab_auxiliaries().store_flag("mock");
    //   my_calo_ctw.set_htm_main_wall(1);
    //   int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 2;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [2] : ", "INFO : ");
    // }
    // {
    //   snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
    //   my_calo_ctw.set_header(58,
    // 			     my_ctw_gid,
    // 			     1504); // hit, gid, clocktick
    //   my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_calo_ctw.grab_auxiliaries().store_flag("mock");
    //   my_calo_ctw.set_htm_main_wall(1);
    //   int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 7;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [2] : ", "INFO : ");
    // }
    // {
    //   snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 0, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
    //   my_calo_ctw.set_header(58,
    // 			     my_ctw_gid,
    // 			     1804); // hit, gid, clocktick
    //   my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_calo_ctw.grab_auxiliaries().store_flag("mock");
    //   my_calo_ctw.set_htm_main_wall(2);
    //   int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 7;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 1;
    //   my_calo_ctw.set_zoning_bit(zone_touched, true);
    //   if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [2] : ", "INFO : ");
    // }

    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(47,
    			     my_ctw_gid,
    			     155); // hit, gid, clocktick
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 0;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [4] : ", "INFO : ");
    }  

    {
      snemo::digitization::calo_ctw & my_calo_ctw = my_calo_ctw_data.add();
      geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::CALORIMETER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::CALO_RACK_ID, 1, 10); // GID : [type]:[RACK],[CRATE],[BOARD]
      my_calo_ctw.set_header(47,
    			     my_ctw_gid,
    			     157); // hit, gid, clocktick
      my_calo_ctw.grab_auxiliaries().store("author", "guillaume");
      my_calo_ctw.grab_auxiliaries().store_flag("mock");
      my_calo_ctw.set_htm_main_wall(1);
      int zone_touched = snemo::digitization::calo::ctw::W_ZW_BIT0 + 5;
      my_calo_ctw.set_zoning_bit(zone_touched, true);
      my_calo_ctw.set_lto_main_wall_bit(true);
      if (debug) my_calo_ctw.tree_dump(std::clog, "My_calo_CTW [4] : ", "INFO : ");
    }
    
    // snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
    // {
    //   snemo::digitization::geiger_ctw & my_geiger_ctw = my_geiger_ctw_data.add();
    //   geomtools::geom_id my_ctw_gid(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE, snemo::digitization::mapping::GEIGER_RACK_ID, 1, 10);
    //   my_geiger_ctw.set_header(86,
    // 			       my_ctw_gid
    // 			       );
    //   my_geiger_ctw.grab_auxiliaries().store("author", "guillaume");
    //   my_geiger_ctw.grab_auxiliaries().store_flag("mock");
    // }

    //clock_utils::CALO_CB_SHIFT_CLOCKTICK_NUMBER
      
    if (debug) my_calo_ctw_data.tree_dump(std::clog, "Calorimeter CTW(s) data : ", "INFO : ");
		   
    // Creation of geiger ctw data :
    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;

    if (debug) my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");

    // Creation of outputs collection structures for calo and tracker
    std::vector<snemo::digitization::calo_trigger_algorithm::calo_summary_record> calo_collection_records;
    std::vector<snemo::digitization::tracker_trigger_algorithm::tracker_record>   tracker_collection_records;
    
    // Reseting trigger display
    my_trigger_display.reset_matrix_pattern();

    // Trigger process
    my_trigger_algo.process(my_calo_ctw_data,
			    my_geiger_ctw_data);
    
    // Finale structures :
    calo_collection_records = my_trigger_algo.get_calo_records_vector();
    tracker_collection_records = my_trigger_algo.get_tracker_records_vector();
 
    // if (debug) my_trigger_display.display_calo_trigger_25ns(my_trigger_algo);
    // if (debug) my_trigger_display.display_calo_trigger_1600ns(my_trigger_algo);

    std::vector<snemo::digitization::coincidence_trigger_algorithm::coincidence_calo_record> coincidence_collection_calo_records = my_trigger_algo.get_coincidence_calo_records_vector();

    if (debug) std::clog << "Calo collection record size @ 25ns   = " << calo_collection_records.size() << std::endl;
    if (debug) std::clog << "Calo collection record size @ 1600ns = " << coincidence_collection_calo_records.size() << std::endl;

    bool raw_trigger_prompt_decision = my_trigger_algo.get_finale_decision();
    bool raw_trigger_delayed_decision = my_trigger_algo.get_delayed_finale_decision();

    if (debug) std::clog << "trigger_finale_decision         [" << raw_trigger_prompt_decision << "]" << std::endl;
    if (debug) std::clog << "delayed trigger_finale_decision [" << raw_trigger_delayed_decision << "]" << std::endl;

    my_trigger_algo.clear_records();

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


