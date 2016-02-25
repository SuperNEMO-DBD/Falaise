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
  bool is_output_path  = false;
  bool is_run_number   = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;
  int arg_run_number    = -1;
;

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

    std::string pipeline_simulated_data_filename;
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    // Trigger Decision Data "TDD" bank label :
    std::string TDD_bank_label = "TDD";
    
    datatools::fetch_path_with_env(input_filename);
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
      // pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/raw_simulated_data_brio/Se82_0nubb_500000-source_strips_bulk_SD.brio";
      // pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/raw_simulated_data_brio/Bi214_Po214_500000-source_strips_bulk_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    
    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 10;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", event_number);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    if (debug) reader.tree_dump(std::clog, "Simulated data reader module");

    datatools::fetch_path_with_env(output_path);
    if (is_output_path) output_path = output_path;
    else output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    datatools::fetch_path_with_env(output_path);
    
    // Name of SD output files (FT : Fake Trigger & RT: Real Trigger) :

    std::string sd_file_FT_0 = output_path + "sd_file_1" + ".brio";
    std::string sd_file_FT_1 = output_path + "sd_file_2" + ".brio";
    std::string sd_file_prompt_RT_0 = output_path + "sd_file_3" + ".brio";
    std::string sd_file_prompt_RT_1 = output_path + "sd_file_4" + ".brio";
    std::string sd_file_delayed_RT_0 = output_path + "sd_file_5" + ".brio";
    std::string sd_file_delayed_RT_1 = output_path + "sd_file_6" + ".brio";
    
    // Event writer : 
    dpp::output_module writer_1;
    datatools::properties writer_config_1;
    writer_config_1.store ("logging.priority", "debug");
    writer_config_1.store ("files.mode", "single");   
    writer_config_1.store ("files.single.filename", sd_file_FT_0);
    writer_1.initialize_standalone(writer_config_1); 

    // Event writer : 
    dpp::output_module writer_2;
    datatools::properties writer_config_2;
    writer_config_2.store ("logging.priority", "debug");
    writer_config_2.store ("files.mode", "single");   
    writer_config_2.store ("files.single.filename", sd_file_FT_1);
    writer_2.initialize_standalone(writer_config_2); 
 
    // Event writer : 
    dpp::output_module writer_3;
    datatools::properties writer_config_3;
    writer_config_3.store ("logging.priority", "debug");
    writer_config_3.store ("files.mode", "single");   
    writer_config_3.store ("files.single.filename", sd_file_prompt_RT_0);
    writer_3.initialize_standalone(writer_config_3); 

    // Event writer : 
    dpp::output_module writer_4;
    datatools::properties writer_config_4;
    writer_config_4.store ("logging.priority", "debug");
    writer_config_4.store ("files.mode", "single");   
    writer_config_4.store ("files.single.filename", sd_file_prompt_RT_1);
    writer_4.initialize_standalone(writer_config_4);   

    // Event writer : 
    dpp::output_module writer_5;
    datatools::properties writer_config_5;
    writer_config_5.store ("logging.priority", "debug");
    writer_config_5.store ("files.mode", "single");   
    writer_config_5.store ("files.single.filename", sd_file_delayed_RT_0);
    writer_5.initialize_standalone(writer_config_5); 

    // Event writer : 
    dpp::output_module writer_6;
    datatools::properties writer_config_6;
    writer_config_6.store ("logging.priority", "debug");
    writer_config_6.store ("files.mode", "single");   
    writer_config_6.store ("files.single.filename", sd_file_delayed_RT_1);
    writer_6.initialize_standalone(writer_config_6); 

    // Event record :
    datatools::things ER;

    // Output ROOT file : 
    std::string root_filename = output_path + "trigger_validation.root";
    datatools::fetch_path_with_env(root_filename);
    TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");

    TTree* trigger_decision_tree = new TTree("TriggerDecision", "Trigger decision histograms");

    // Variables definitions :
    Int_t event_id    = 0;
    Bool_t raw_trigger_prompt_decision = false;
    Bool_t raw_trigger_delayed_decision = false;
    Bool_t fake_trigger_prompt_decision = false;
    Bool_t fake_trigger_delayed_decision = false;
    Bool_t prompt_trigger_decision_to_fake_trigger = false;
    Bool_t delayed_trigger_decision_to_fake_trigger = false;
    Int_t total_number_of_calo = 0;
    Int_t total_number_of_main_calo = 0;
    Int_t total_number_of_gveto = 0;
    Int_t total_number_of_gg_cells = 0;
    Int_t total_number_of_prompt_gg_cells = 0;
    Int_t total_number_of_delayed_gg_cells = 0;
    
    // Branch definitions : 
    trigger_decision_tree->Branch("event_id", &event_id, "evend_id/I");
    trigger_decision_tree->Branch("raw_trigger_prompt_decision", &raw_trigger_prompt_decision, "raw_trigger_prompt_decision/O");
    trigger_decision_tree->Branch("raw_trigger_delayed_decision", &raw_trigger_delayed_decision, "raw_trigger_delayed_decision/O");
    trigger_decision_tree->Branch("fake_trigger_prompt_decision", &fake_trigger_prompt_decision, "fake_trigger_prompt_decision/O");
    trigger_decision_tree->Branch("fake_trigger_delayed_decision", &fake_trigger_delayed_decision, "fake_trigger_delayed_decision/O");
    trigger_decision_tree->Branch("prompt_trigger_decision_to_fake_trigger", &prompt_trigger_decision_to_fake_trigger, "prompt_trigger_decision_to_fake_trigger/O");
    trigger_decision_tree->Branch("delayed_trigger_decision_to_fake_trigger", &delayed_trigger_decision_to_fake_trigger, "delayed_trigger_decision_to_fake_trigger/O");
    trigger_decision_tree->Branch("total_number_of_calo", &total_number_of_calo, "total_number_of_calo/I");
    trigger_decision_tree->Branch("total_number_of_main_calo", &total_number_of_main_calo, "total_number_of_main_calo/I");
    trigger_decision_tree->Branch("total_number_of_gveto", &total_number_of_gveto, "total_number_of_gveto/I");
    trigger_decision_tree->Branch("total_number_of_gg_cells", &total_number_of_gg_cells, "total_number_of_gg_cells/I");
    trigger_decision_tree->Branch("total_number_of_prompt_gg_cells", &total_number_of_prompt_gg_cells, "total_number_of_prompt_gg_cells/I");
    trigger_decision_tree->Branch("total_number_of_delayed_gg_cells", &total_number_of_delayed_gg_cells, "total_number_of_delayed_gg_cells/I");

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
    int  calorimeter_gate_size = 4;
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
    trigger_config.store("coincidence.calorimeter_gate_size", calorimeter_gate_size);
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
    
    while (!reader.is_terminated())
      {
	event_id = psd_count;
	raw_trigger_prompt_decision = false;
	raw_trigger_delayed_decision = false;
	fake_trigger_prompt_decision = false;
	fake_trigger_delayed_decision = false;
	prompt_trigger_decision_to_fake_trigger = false;
	delayed_trigger_decision_to_fake_trigger = false;
	total_number_of_calo = 0;
	total_number_of_main_calo = 0;
	total_number_of_gveto = 0;
	total_number_of_gg_cells = 0;
	total_number_of_prompt_gg_cells = 0;
	total_number_of_delayed_gg_cells = 0;
 
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

		for(int i = 0; i < signal_data.get_calo_signals().size(); i++)
		  {
		    const snemo::digitization::calo_signal a_calo_signal = signal_data.get_calo_signals()[i].get();
		    if (debug) a_calo_signal.tree_dump(std::clog, "A Calo signal", "INFO : ");
		  }
		
		if (debug) signal_data.tree_dump(std::clog, "*** Signal Data ***", "INFO : ");
		
		// Creation of calo ctw data :
		snemo::digitization::calo_ctw_data my_calo_ctw_data;

		// Creation of geiger ctw data :
		snemo::digitization::geiger_ctw_data my_geiger_ctw_data;

		if (debug) my_clock_manager.tree_dump(std::clog, "Clock utils : ", "INFO : ");

		// Calo signal to calo TP :
		if (signal_data.has_calo_signals())
		  {
		    snemo::digitization::calo_tp_data my_calo_tp_data;
		    // Set calo clockticks :
		    signal_2_calo_tp.set_clocktick_reference(clocktick_25_reference);
		    signal_2_calo_tp.set_clocktick_shift(clocktick_25_shift);
		    // Signal to calo TP process :
		    signal_2_calo_tp.process(signal_data, my_calo_tp_data);
		    if (debug) my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
		    snemo::digitization::calo_tp_data::calo_tp_collection_type calo_tps = my_calo_tp_data.get_calo_tps();

		    // Calo TP to geiger CTW process :
		    calo_tp_2_ctw_0.process(my_calo_tp_data, my_calo_ctw_data);
		    calo_tp_2_ctw_1.process(my_calo_tp_data, my_calo_ctw_data);
		    calo_tp_2_ctw_2.process(my_calo_tp_data, my_calo_ctw_data);
		    if (debug) my_calo_ctw_data.tree_dump(std::clog, "Calorimeter CTW(s) data : ", "INFO : ");

		  } // end of if has calo signal

		if (signal_data.has_geiger_signals())
		  {	    
		    snemo::digitization::geiger_tp_data my_geiger_tp_data;
		    // Set geiger clockticks :
		    signal_2_geiger_tp.set_clocktick_reference(clocktick_800_reference);
		    signal_2_geiger_tp.set_clocktick_shift(clocktick_800_shift); 
		    // Signal to geiger TP process
		    signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);

		    // Geiger TP to geiger CTW process
		    geiger_tp_2_ctw.process(my_geiger_tp_data, my_geiger_ctw_data);
		    if (debug) my_geiger_ctw_data.tree_dump(std::clog, "Geiger CTW(s) data : ", "INFO : ");
		    
		    double time_limit = 5000.; // time in nanosecond

		    total_number_of_gg_cells = signal_data.get_number_of_geiger_signals();
		    total_number_of_prompt_gg_cells = signal_data.get_number_of_prompt_geiger_signals(time_limit);
		    total_number_of_delayed_gg_cells = signal_data.get_number_of_delayed_geiger_signals(time_limit);

		  } // end of if has geiger signal

		// Creation of outputs collection structures for calo and tracker
		std::vector<snemo::digitization::calo_trigger_algorithm::calo_summary_record> calo_collection_records;
		std::vector<snemo::digitization::tracker_trigger_algorithm_test_new_strategy::tracker_record>   tracker_collection_records;
		
		// Reseting trigger display
		my_trigger_display.reset_matrix_pattern();
		
		// Trigger process
		my_trigger_algo.process(my_calo_ctw_data,
					my_geiger_ctw_data);
		  
		// Finale structures :
		calo_collection_records = my_trigger_algo.get_calo_records_vector();
		tracker_collection_records = my_trigger_algo.get_tracker_records_vector();
		
		//if (debug) my_trigger_display.display_calo_trigger_25ns(my_trigger_algo);
	        //if (debug) my_trigger_display.display_calo_trigger_1600ns(my_trigger_algo);
		//if (debug) my_trigger_display.display_tracker_trigger_1600ns(my_trigger_algo);
	        //if (debug) my_trigger_display.display_coincidence_trigger_1600ns(my_trigger_algo);
		
		// for (int iclocktick = 0; iclocktick <= 10; iclocktick++)
		//   {
		//     my_trigger_display.display_coincidence_trigger_1600ns(my_trigger_algo, iclocktick);
		//   }

		
		if (debug) std::clog << "********* Size of Finale structures for one event *********" << std::endl;
		if (debug) std::clog << "Calo collection size    : " << calo_collection_records.size() << std::endl;
		if (debug) std::clog << "Tracker collection size : " << tracker_collection_records.size() << std::endl;
		
	      } // end of if has "calo" || "xcalo" || "gveto" || "gg" step hits
	    
	    std::vector<snemo::digitization::coincidence_trigger_algorithm_new_strategy::coincidence_calo_record> coincidence_collection_calo_records = my_trigger_algo.get_coincidence_calo_records_vector();

	    raw_trigger_prompt_decision = my_trigger_algo.get_finale_decision();
	    raw_trigger_delayed_decision = my_trigger_algo.get_delayed_finale_decision();

	    if (debug) std::clog << "trigger_finale_decision         [" << raw_trigger_prompt_decision << "]" << std::endl;
	    if (debug) std::clog << "delayed trigger_finale_decision [" << raw_trigger_delayed_decision << "]" << std::endl;
	    
	    if (!raw_trigger_prompt_decision && raw_trigger_delayed_decision) std::cout <<  " ******************************* ANORMAL *******************" << std::endl;
	    
	    if (coincidence_collection_calo_records.size() != 0)
	      {
		total_number_of_main_calo = coincidence_collection_calo_records[2].total_multiplicity_side_0.to_ulong() + coincidence_collection_calo_records[2].total_multiplicity_side_1.to_ulong();
		total_number_of_gveto     = coincidence_collection_calo_records[2].total_multiplicity_gveto.to_ulong();
		total_number_of_calo      = total_number_of_main_calo + total_number_of_gveto;
	      }
	    if (debug) std::clog << "********************************************************************" << std::endl;	    
	    if (debug) std::clog << "Total calo             = " << total_number_of_calo             << std::endl;
	    if (debug) std::clog << "Total main calo        = " << total_number_of_main_calo        << std::endl;
	    if (debug) std::clog << "Total gveto calo       = " << total_number_of_gveto            << std::endl;
	    if (debug) std::clog << "Total GG cells         = " << total_number_of_gg_cells         << std::endl;
	    if (debug) std::clog << "Total prompt GG cells  = " << total_number_of_prompt_gg_cells  << std::endl;
	    if (debug) std::clog << "Total delayed GG cells = " << total_number_of_delayed_gg_cells << std::endl;
	    if (debug) std::clog << "********************************************************************" << std::endl;
	    

	    // Fake trigger prompt : 1 PM / 3 prompt cells
	    if (total_number_of_calo >= 1 && total_number_of_prompt_gg_cells >= 3)
	      {
		fake_trigger_prompt_decision = true;
		prompt_trigger_decision_to_fake_trigger = my_trigger_algo.get_finale_decision();
	      }

	    // Fake trigger delayed : 1 delayed cells and a fake prompt event
	    if (fake_trigger_prompt_decision && total_number_of_delayed_gg_cells >= 1)
	      {
		fake_trigger_delayed_decision = true;
		delayed_trigger_decision_to_fake_trigger = my_trigger_algo.get_delayed_finale_decision();
	      }	
	    
	    // Write in several SD files depending of the trigger decision
	    if(!fake_trigger_prompt_decision) writer_1.process(ER);
	    else writer_2.process(ER);
	    if (fake_trigger_prompt_decision && !prompt_trigger_decision_to_fake_trigger) writer_3.process(ER);
	    else if (fake_trigger_prompt_decision && prompt_trigger_decision_to_fake_trigger) writer_4.process(ER);
	    if (fake_trigger_prompt_decision && !delayed_trigger_decision_to_fake_trigger) writer_5.process(ER);
	    else if (fake_trigger_prompt_decision && delayed_trigger_decision_to_fake_trigger) writer_6.process(ER);

	    my_trigger_algo.clear_records();
	      
	  } //end of if has bank label "SD"
	
	trigger_decision_tree->Fill();
	
	ER.clear();
	psd_count++;
	if (debug) std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      } // end of reader is terminated
    
    root_file->Write();
    root_file->Close(); 
     
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
