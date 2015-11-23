// test_trigger_algorithm.cxx
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

#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

#include <snemo/digitization/trigger_algorithm.h>

int main( int  argc_ , char **argv_  )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;
  bool is_event_number = false;
  bool is_mult         = false;
  bool is_gap          = false;
  bool is_output_path  = false;
  bool is_run_number   = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;
  int arg_min_mult      = -1;
  int arg_max_gap       = -1;
  int arg_run_number    = -1;
;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename=argv_[++iarg];
    } else if (arg == "-f" || arg == "--filename") {
      input_filename=argv_[++iarg];
    }
    iarg++;
  }
  

  try {
    std::clog << "Test program for class 'snemo::digitization::trigger_algorithm' !" << std::endl;
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

    std::string pipeline_simulated_data_filename;
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    // Trigger Decision Data "TDD" bank label :
    std::string TDD_bank_label = "TDD";
    
    datatools::fetch_path_with_env(input_filename);
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      // pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
      pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/trigger/simulated_data_brio/Se82_0nubb_500000-source_strips_bulk_SD.brio";
      // pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/trigger/simulated_data_brio/Bi214_Po214_500000-source_strips_bulk_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Loading memory from external files
    std::string memory_layer;
    std::string memory_row;
    std::string memory_a4_d2 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A4_D2_default_memory.data";
    datatools::fetch_path_with_env(memory_a4_d2);

    if (is_mult && !is_gap)
      {
	if (arg_min_mult == 2)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_default_min_mult_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_default_min_mult_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
	else if (arg_min_mult == 3)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_min_mult_3_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_min_mult_3_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
	else if (arg_min_mult == 4)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_min_mult_4_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_min_mult_4_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
      }

    else if (is_gap && !is_mult)
      {
	if (arg_max_gap == 1)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_default_max_gap_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_default_max_gap_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
	else if (arg_max_gap == 2)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_max_gap_2_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_max_gap_2_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
	else if (arg_max_gap == 3)
	  {
	    memory_layer = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_max_gap_3_memory.data";
	    datatools::fetch_path_with_env(memory_layer);
	    memory_row   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_max_gap_3_memory.data";
	    datatools::fetch_path_with_env(memory_row);
	  }
      }
    else
      {
	// Default data if no options :
        memory_layer   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_default_min_mult_memory.data";
	datatools::fetch_path_with_env(memory_layer);
        memory_row     = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_default_min_mult_memory.data";
	datatools::fetch_path_with_env(memory_row);
      }

    
	datatools::fetch_path_with_env(memory_layer);
 	datatools::fetch_path_with_env(memory_row);

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 15);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    // Event record :
    datatools::things ER;

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.initialize();

    // Clock manager :
    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();		
  
    // Initializing SD to calo signal algo :
    snemo::digitization::sd_to_calo_signal_algo sd_2_calo_signal(my_manager);
    sd_2_calo_signal.initialize();

    // Initializing SD to geiger signal algo :
    snemo::digitization::sd_to_geiger_signal_algo sd_2_geiger_signal(my_manager);
    sd_2_geiger_signal.initialize();

    // Initializing signal to calo tp algo :
    snemo::digitization::signal_to_calo_tp_algo signal_2_calo_tp;
    signal_2_calo_tp.initialize(my_e_mapping);

    // Initializing signal to geiger tp algo :
    snemo::digitization::signal_to_geiger_tp_algo signal_2_geiger_tp;
    signal_2_geiger_tp.initialize(my_e_mapping);
		  
    // Initializing calo tp to calo ctw algorithms for each crate :
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_0;
    calo_tp_2_ctw_0.set_crate_number(snemo::digitization::mapping::MAIN_CALO_SIDE_0_CRATE);
    calo_tp_2_ctw_0.initialize();
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_1;
    calo_tp_2_ctw_1.set_crate_number(snemo::digitization::mapping::MAIN_CALO_SIDE_1_CRATE);
    calo_tp_2_ctw_1.initialize();
    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw_2;
    calo_tp_2_ctw_2.set_crate_number(snemo::digitization::mapping::XWALL_GVETO_CALO_CRATE);
    calo_tp_2_ctw_2.initialize();

    // Initializing geiger tp to geiger ctw :
    snemo::digitization::geiger_tp_to_ctw_algo geiger_tp_2_ctw;
    geiger_tp_2_ctw.initialize();

    // Properties to configure trigger algorithm :
    datatools::properties trigger_config;
    int calo_circular_buffer_depth = 4;
    int calo_threshold = 1;
    bool inhibit_both_side_coinc = false;
    bool inhibit_single_side_coinc = false;    
    int calorimeter_gate_size = 4;
    bool activate_coincidence = true;
    
    trigger_config.store("calo.circular_buffer_depth", calo_circular_buffer_depth);
    trigger_config.store("calo.total_multiplicity_threshold", calo_threshold);
    trigger_config.store("calo.inhibit_both_side",  inhibit_both_side_coinc);
    trigger_config.store("calo.inhibit_single_side",  inhibit_single_side_coinc);
    trigger_config.store("tracker.memory_layer_file", memory_layer);
    trigger_config.store("tracker.memory_row_file",   memory_row);
    trigger_config.store("tracker.memory_a4_d2_file", memory_a4_d2);
    trigger_config.store("coincidence.calorimeter_gate_size", calorimeter_gate_size);
    trigger_config.store("activate_coincidence", activate_coincidence);

    // Creation of trigger display manager :
    snemo::digitization::trigger_display_manager my_trigger_display;
    datatools::properties trigger_display_config;
    bool calo_25ns = true;
    bool calo_1600ns = true;
    bool tracker_1600ns = true;
    trigger_display_config.store("calo_25ns", calo_25ns);
    trigger_display_config.store("tracker_1600ns", tracker_1600ns);
    my_trigger_display.initialize(trigger_display_config);

    // Creation and initialization of trigger algorithm :
    snemo::digitization::trigger_algorithm my_trigger_algo;
    my_trigger_algo.set_electronic_mapping(my_e_mapping);
    my_trigger_algo.set_trigger_display_manager(my_trigger_display);
    my_trigger_algo.initialize(trigger_config);

    // Internal counters
    int psd_count = 0;         // Event counter
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);

	    my_clock_manager.compute_clockticks_ref(random_generator);
	    int32_t clocktick_25_reference  = my_clock_manager.get_clocktick_25_ref();
	    double  clocktick_25_shift      = my_clock_manager.get_shift_25();
	    int32_t clocktick_800_reference = my_clock_manager.get_clocktick_800_ref();
	    double  clocktick_800_shift     = my_clock_manager.get_shift_800();

	    if (SD.has_step_hits("calo") || SD.has_step_hits("xcalo") || SD.has_step_hits("gveto") || SD.has_step_hits("gg"))
	      {
		// Creation of a signal data object to store calo & geiger signals :
		snemo::digitization::signal_data signal_data;	
		
		// Processing Calo signal :
		sd_2_calo_signal.process(SD, signal_data);

		// Processing Geiger signal :
		sd_2_geiger_signal.process(SD, signal_data);

		// Creation of calo ctw data :
		snemo::digitization::calo_ctw_data my_calo_ctw_data;

		// Creation of geiger ctw data :
		snemo::digitization::geiger_ctw_data my_geiger_ctw_data;

		my_clock_manager.tree_dump(std::clog, "Clock utils : ", "INFO : ");

		// Calo signal to calo TP :
		if( signal_data.has_calo_signals())
		  {
		    snemo::digitization::calo_tp_data my_calo_tp_data;
		    // Set calo clockticks :
		    signal_2_calo_tp.set_clocktick_reference(clocktick_25_reference);
		    signal_2_calo_tp.set_clocktick_shift(clocktick_25_shift);
		    // Signal to calo TP process :
		    signal_2_calo_tp.process(signal_data, my_calo_tp_data);
		    my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");

		    // Calo TP to geiger CTW process :
		    calo_tp_2_ctw_0.process(my_calo_tp_data, my_calo_ctw_data);
		    calo_tp_2_ctw_1.process(my_calo_tp_data, my_calo_ctw_data);
		    calo_tp_2_ctw_2.process(my_calo_tp_data, my_calo_ctw_data);
		    my_calo_ctw_data.tree_dump(std::clog, "Calorimeter CTW(s) data : ", "INFO : ");

		  } // end of if has calo signal

		if( signal_data.has_geiger_signals())
		  {	    
		    snemo::digitization::geiger_tp_data my_geiger_tp_data;
		    // Set geiger clockticks :
		    signal_2_geiger_tp.set_clocktick_reference(clocktick_800_reference);
		    signal_2_geiger_tp.set_clocktick_shift(clocktick_800_shift); 
		    // Signal to geiger TP process
		    signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);

		    // Geiger TP to geiger CTW process
		    geiger_tp_2_ctw.process(my_geiger_tp_data, my_geiger_ctw_data);
		    my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");
		    
		  } // end of if has geiger signal

		// Creation of outputs collection structures for calo and tracker
		std::vector<snemo::digitization::calo_trigger_algorithm::calo_summary_record> calo_collection_records;
		std::vector<snemo::digitization::tracker_trigger_algorithm::tracker_record>   tracker_collection_records;
		
		// Trigger process
		my_trigger_algo.process(my_calo_ctw_data,
					my_geiger_ctw_data);
		  
		// Finale structures :
		calo_collection_records = my_trigger_algo.get_calo_records_vector();
		tracker_collection_records = my_trigger_algo.get_tracker_records_vector();
		//my_trigger_display.display_calo_trigger_25ns(my_trigger_algo);
		//my_trigger_display.display_calo_trigger_1600ns(my_trigger_algo);
		my_trigger_display.display_tracker_trigger_1600ns(my_trigger_algo);
		std::clog << "********* Size of Finale structures for one event *********" << std::endl;
		std::clog << "Calo collection size    : " << calo_collection_records.size() << std::endl;
		std::clog << "Tracker collection size : " << tracker_collection_records.size() << std::endl;
		
	      } // end of if has "calo" || "xcalo" || "gveto" || "gg" step hits
	    
	  }
	ER.clear();

	psd_count++;
	std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    
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
