//test_tracker_trigger_algorithm.cxx
// Standard libraries :
#include <iostream>
#include <fstream>

// GSL:
#include <bayeux/mygsl/rng.h>
#include <bayeux/mygsl/histogram.h>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>
// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>
#include <snemo/digitization/tracker_trigger_algorithm.h>

int main(int  argc_ , char ** argv_)
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

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input")
      {
	is_input_file  = true;
	input_filename = argv_[++iarg];
      }

    else if (arg == "-op" || arg == "--output-path")
      {
	is_output_path = true;
	output_path = argv_[++iarg];	
      }
    
    else if (arg == "-rn" || arg == "--run-number")
      {
	is_run_number = true;
	arg_run_number = atoi(argv_[++iarg]);	
      }

    else if (arg == "-n" || arg == "--number")
      {
        is_event_number = true;
	arg_event_number    = atoi(argv_[++iarg]);
      }
    
    else if (arg =="-h" || arg == "--help")
      {
	is_help = true;
      }
    
    else if (arg == "-m" || arg == "--mult")
      {
	is_mult  = true;
	arg_min_mult = atoi(argv_[++iarg]);
      }

    else if (arg == "-g" || arg == "--gap")
      {
	is_gap  = true;
	arg_max_gap = atoi(argv_[++iarg]);
      }
    iarg++;
  }
  // Configuration for each argument
  
  if (is_help) 
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_tracker_trigger_algorithm [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output-path ]    set the path for output files" << std::endl
		<< "-rn [ --run-number ]     set the output run number for output files (ex : -rn 10 ::  output -> tracker_trigger_algorithm_10.brio)" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "-m  [ --mult ]           set a minimum multiplicity for trigger subzone calculation" << std::endl
		<< "-g  [ --gap ]            set a maximum gap for trigger subzone calculation" << std::endl 
		<< "Example : " << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_tracker_trigger_algorithm --input ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio" 
		<< " --output-path ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/"
		<< " --run-number 4242"
		<< " --number 5"
		<< " --gap 2" << std::endl << std::endl
		<< "If no options are set, programs have default values :" << std::endl << std::endl
		<< "input file           = ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio" << std::endl
		<< "ouput path files     = ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl
		<< "run number           = 999" << std::endl
		<< "number of events     = 10" << std::endl
		<< "Calcul option = mult = 2" << std::endl << std::endl;
      return 0;
    }

  // Process
  try {
    std::clog << "Test program for class 'snemo::digitization::tracker_trigger_algorithm' !" << std::endl;
    // Random generator and seed
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    // Geometry manager
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

    // Default input file :
    datatools::fetch_path_with_env(input_filename);
    if (is_input_file) pipeline_simulated_data_filename = input_filename;
    else              pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio"; 
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);
    
    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 10;
  
    // Set output name files :
    std::string output_tracker_decision_00;
    std::string output_tracker_decision_01;
    std::string output_tracker_decision_11;

    // Set output histo name : 
    std::string output_tracker_histo_0;

    // Set output path and name for files :
    if (is_output_path && is_run_number)
      {
	std::ostringstream oss0;
	oss0 << output_path <<"tracker_trigger_decision_"<< arg_run_number <<"_00.brio";
	output_tracker_decision_00 = oss0.str();
	datatools::fetch_path_with_env(output_tracker_decision_00);
        
	std::ostringstream oss1;
	oss1 <<  output_path << "tracker_trigger_decision_" << arg_run_number <<"_01.brio";
	output_tracker_decision_01 = oss1.str();
	datatools::fetch_path_with_env(output_tracker_decision_01);

	std::ostringstream oss2;
	oss2 << "tracker_trigger_decision_"<< arg_run_number <<"_11.brio";
	output_tracker_decision_11 = output_path + oss2.str();
	datatools::fetch_path_with_env(output_tracker_decision_11);

	std::ostringstream oss3;
	oss3 << "tracker_trigger_" << arg_run_number << "_decision.hist";
	output_tracker_histo_0 = output_path + oss3.str();
	datatools::fetch_path_with_env(output_tracker_histo_0);
      }
    else
      {
	// Default testing :
	std::string default_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
	std::string default_run_number   = "999";

	output_tracker_decision_00 = default_path + "tracker_trigger_decision_" + default_run_number + "_00.brio";
	datatools::fetch_path_with_env(output_tracker_decision_00);
	output_tracker_decision_01 = default_path + "tracker_trigger_decision_" + default_run_number + "_01.brio";
	datatools::fetch_path_with_env(output_tracker_decision_01);
	output_tracker_decision_11 = default_path + "tracker_trigger_decision_" + default_run_number + "_11.brio";
	datatools::fetch_path_with_env(output_tracker_decision_11);
        output_tracker_histo_0 = default_path + "tracker_trigger_histo_" + default_run_number + "_decision.hist";
	datatools::fetch_path_with_env(output_tracker_histo_0);
      }

    // Histogram parameters :
    double weight = 1;
    std::ofstream ofhist(output_tracker_histo_0.c_str());
    mygsl::histogram best_final_decision_histogram(3, 0, 3);

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

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", event_number);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    // Event record :
    datatools::things ER;
    
    // Writers for files : 
    dpp::output_module writer_decision_00;
    writer_decision_00.set_single_output_file(output_tracker_decision_00);
    writer_decision_00.initialize_simple();
   
    dpp::output_module writer_decision_01;
    writer_decision_01.set_single_output_file(output_tracker_decision_01);
    writer_decision_01.initialize_simple();

    dpp::output_module writer_decision_11;
    writer_decision_11.set_single_output_file(output_tracker_decision_11);
    writer_decision_11.initialize_simple();

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.initialize();

    // Clock manager :
    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();    
    
    // Internal counters
    int psd_count = 0;         // Event counter
    int decision_00_count = 0; // Tracker decision 00 counter 
    int decision_01_count = 0; // Tracker decision 01 counter 
    int decision_11_count = 0; // Tracker decision 11 counter 

    while (!reader.is_terminated())
      {
	reader.process(ER);
	//datatools::properties & TDD = ER.add<datatools::properties>(TDD_bank_label);

	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	    
	    // Default value for the tracker trigger decision
	    std::bitset<2> best_tracker_trigger_decision (std::string("00"));

	    //SD.tree_dump(std::clog, "my_SD", "SD :");
	    if(SD.has_step_hits("gg"))
	      {
		// Creation of a signal data object to store geiger signals :
		snemo::digitization::signal_data signal_data;

		// Initializing and processing SD to Geiger signal :
		snemo::digitization::sd_to_geiger_signal_algo sd_2_geiger_signal(my_manager);
		sd_2_geiger_signal.initialize();
		sd_2_geiger_signal.process(SD, signal_data);

		std::vector<snemo::digitization::tracker_trigger_algorithm::tracker_record> tracker_collection_records;
		if( signal_data.has_geiger_signals())
		  {
		    // Random clocktick references and shifts 
		    my_clock_manager.compute_clockticks_ref(random_generator);
		    int32_t clocktick_25_reference  = my_clock_manager.get_clocktick_25_ref();
		    double  clocktick_25_shift      = my_clock_manager.get_shift_25();
		    int32_t clocktick_800_reference = my_clock_manager.get_clocktick_800_ref();
		    double  clocktick_800_shift     = my_clock_manager.get_shift_800();
		    
		    std::clog << "CT 25 Ref : " << clocktick_25_reference
			      << " CT 25 shift : " << clocktick_25_shift << std::endl;
		    
		    std::clog << " CT 800 Ref : " << clocktick_800_reference
			      << " CT 800 shift : " << clocktick_800_shift << std::endl;
		    
		    // Creation of a geiger TP data object to store geiger TP :
		    snemo::digitization::geiger_tp_data my_geiger_tp_data;

		    // Initializing and processing Geiger signal to Geiger TP :
		    snemo::digitization::signal_to_geiger_tp_algo signal_2_geiger_tp;
		    signal_2_geiger_tp.initialize(my_e_mapping);
		    signal_2_geiger_tp.set_clocktick_reference(clocktick_800_reference);
		    signal_2_geiger_tp.set_clocktick_shift(clocktick_800_shift);	    
		    signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);
		    // my_geiger_tp_data.tree_dump(std::clog, "Geiger TP(s) data : ", "INFO : ");
		    
		    // Creation of a geiger CTW data object to store geiger CTW :
		    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
	    
		    // Initializing and processing Geiger TP to Geiger CTW :
		    snemo::digitization::geiger_tp_to_ctw_algo geiger_tp_2_ctw;
		    geiger_tp_2_ctw.initialize();
		    geiger_tp_2_ctw.process(my_geiger_tp_data, my_geiger_ctw_data);
		    my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");
		    
		    // Initializing and processing tracker trigger algorithm to make a decision :
		    snemo::digitization::tracker_trigger_algorithm my_tracker_algo;
		    my_tracker_algo.set_electronic_mapping(my_e_mapping);
		    my_tracker_algo.fill_all_layer_memory(memory_layer);
		    my_tracker_algo.fill_all_row_memory(memory_row);
		    my_tracker_algo.fill_all_a4_d2_memory(memory_a4_d2);
		    my_tracker_algo.initialize();
		    snemo::digitization::tracker_trigger_algorithm::tracker_record tracker_level_one_finale_decison;

		    my_tracker_algo.process(my_geiger_ctw_data, tracker_collection_records);			    
		    tracker_level_one_finale_decison = my_tracker_algo.get_tracker_record_finale_decision_structure();

		  } // end of if has geiger signals data
	      } // end of if has "gg" step hits	 

	    unsigned long tracker_trigger_decision_per_event = best_tracker_trigger_decision.to_ulong();
	    // Convert the bitset response [11] (3 in int) to 2 for histogram :
	    if (tracker_trigger_decision_per_event == 3)
	      {
		tracker_trigger_decision_per_event = 2;
	      }
   
	    // Filling the best final decision histogram :
	    best_final_decision_histogram.fill((double)tracker_trigger_decision_per_event, weight);

	    // Store the trigger tracker decision into the trigger decision data "TDD" bank
	    //TDD.store_real("tracker_decision", (double)tracker_trigger_decision_per_event, "Tracker trigger decision for an event");

	    // Writing in the good files :
	    if ((int)tracker_trigger_decision_per_event == 0)
	      {
		writer_decision_00.process(ER);
		decision_00_count++;
	      }
	    else if ((int)tracker_trigger_decision_per_event == 1)
	      {
		writer_decision_01.process(ER);
		decision_01_count++;
	      }
	    else if ((int)tracker_trigger_decision_per_event == 2)
	      {
		writer_decision_11.process(ER);
		decision_11_count++;
	      }
	    else 
	      {
		DT_THROW(std::logic_error, "Tracker trigger decision value [" << tracker_trigger_decision_per_event << "] is not defined ! ");
	      }
	  } // end of if has bank label 
	ER.clear();

	psd_count++;
	//std::clog << "DEBUG : psd count " << psd_count << std::endl;
	//std::clog << "\r" << "DEBUG : psd count " << psd_count << std::flush;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);

      } // end of while

    // ************************************************************************************** \\
    // Histrogram not working, because best finale decision has been deleted from the process \\
    // Changes incoming for outputs                                                           \\
    // ************************************************************************************** \\

    best_final_decision_histogram.print(ofhist);
    ofhist.close();
    
    std::clog << std::endl;
    // std::clog << "DEBUG : [00] count = #" << decision_00_count << std::endl;
    // std::clog << "DEBUG : [01] count = #" << decision_01_count << std::endl;
    // std::clog << "DEBUG : [11] count = #" << decision_11_count << std::endl;
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
