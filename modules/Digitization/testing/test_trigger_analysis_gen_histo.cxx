//test_trigger_analysisi_gen_histo.cxx
// Standard libraries :
#include <iostream>
#include <fstream>

// GSL:
#include <bayeux/mygsl/rng.h>
#include <bayeux/mygsl/histogram.h>
#include <bayeux/mygsl/histogram_2d.h>
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
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;  
  bool is_output_path  = false;  
  bool is_event_number = false;
  bool is_run_number   = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;  
  int arg_event_number  = -1;
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
    iarg++;
  }
  
  if (is_help) 
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_trigger_analysis_gen_histo [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output-path ]    set the path for output files" << std::endl
		<< "-rn [ --run-number ]     set the output run number for output files (ex : -rn 10 ::  output -> tracker_trigger_algorithm_10.brio)" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "Example : " << std::endl << std::endl
		<< "--input ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio" 
		<< " --output-path ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl << std::endl;
      return 0;
    }

  // Process
  try {  
    std::clog << "Test program for class 'snemo::digitization::trigger_analysis_gen_histo' !" << std::endl;
   
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

    // Default input file :
    datatools::fetch_path_with_env(input_filename);
    if (is_input_file) pipeline_simulated_data_filename = input_filename;
    else              pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio"; 
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 1000000;

    // Set output histo name : 
    std::string output_tracker_histo_1;
    std::string output_tracker_histo_2;
    std::string output_tracker_histo_3;

    // Set output path and name for files :
    if (is_output_path && is_run_number)
      {

	std::ostringstream oss1;
	oss1 << "tracker_trigger_histo_" << arg_run_number << "_1_mean.hist";
	output_tracker_histo_1 = output_path + oss1.str();
	datatools::fetch_path_with_env(output_tracker_histo_1);

	std::ostringstream oss2;
	oss2 << "tracker_trigger_histo_" << arg_run_number << "_2_xy.hist";
	output_tracker_histo_2 = output_path + oss2.str();
	datatools::fetch_path_with_env(output_tracker_histo_2);

	std::ostringstream oss3;
	oss3 << "tracker_trigger_histo_" << arg_run_number << "_3_yz.hist";
	output_tracker_histo_3 = output_path + oss3.str();
	datatools::fetch_path_with_env(output_tracker_histo_3);
      }
    else
      {
	// Default testing :
	std::string default_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
	std::string default_run_number   = "999";        
	output_tracker_histo_1 = default_path + "tracker_trigger_histo_" + default_run_number + "_1_mean.hist";
	datatools::fetch_path_with_env(output_tracker_histo_1);
	output_tracker_histo_2 = default_path + "tracker_trigger_histo_" + default_run_number + "_2_xy.hist";
	datatools::fetch_path_with_env(output_tracker_histo_2);
	output_tracker_histo_3 = default_path + "tracker_trigger_histo_" + default_run_number + "_3_yz.hist";
	datatools::fetch_path_with_env(output_tracker_histo_3);
      }

    // Histogram parameters :
    double weight = 1;
    std::ofstream ofhist_1(output_tracker_histo_1.c_str());
    mygsl::histogram mean_number_of_geiger_cell(230, 0, 230);

    std::ofstream ofhist_2(output_tracker_histo_2.c_str());
    //mygsl::histogram_2d x_y_position_hist(88, -440, 440, 520, -2600, 2600);
    mygsl::histogram_2d x_y_position_hist(19, -9, 9, 113, 0, 112);

    std::ofstream ofhist_3(output_tracker_histo_3.c_str());
    mygsl::histogram_2d y_z_position_hist(520, -2600, 2600, 320, -1600, 1600);


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

    // Internal counter
    int psd_count = 0;

    while (!reader.is_terminated())
      {
	reader.process(ER);

	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {	   
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	    //SD.tree_dump(std::clog, "my_SD", "SD :");
	    size_t number_of_step_hits = SD.get_number_of_step_hits("gg");
	    if(SD.has_step_hits("gg"))
	      {
		for (unsigned int i = 0; i < number_of_step_hits; i++)
		  {
		    const mctools::base_step_hit & base_step_hit = SD.get_step_hit("gg", i);
	
		    geomtools::vector_3d anode_position_start (0,0,0);
		    const geomtools::vector_3d & anode_position_stop = base_step_hit.get_position_stop();
		    
		    // double x = anode_position_stop.getX();
		    double y = anode_position_stop.getY();
		    double z = anode_position_stop.getZ();

		    if(base_step_hit.get_geom_id().get(1) == 0)x_y_position_hist.fill((int)-base_step_hit.get_geom_id().get(2), (int)base_step_hit.get_geom_id().get(3), weight);
		    if(base_step_hit.get_geom_id().get(1) == 1)x_y_position_hist.fill((int)base_step_hit.get_geom_id().get(2), (int)base_step_hit.get_geom_id().get(3), weight);



		    // x_y_position_hist.fill(x, y, weight);
		    y_z_position_hist.fill(y, z, weight);

		    //std::clog << "DEBUG : X = " << x << " Y = " << y << " Z = " << z << std::endl;
		  }
		
		mean_number_of_geiger_cell.fill((number_of_step_hits + 1e-7), weight);
	      } // end of if has "gg" step hits	 
	    else
	      {
		number_of_step_hits = 0;
		mean_number_of_geiger_cell.fill((number_of_step_hits + 1e-7), weight);
	      }
	    
	  } // end of if has bank label 
	ER.clear();

	psd_count++;
	//std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);

      } // end of while
    mean_number_of_geiger_cell.print(ofhist_1);
    ofhist_1.close();

    x_y_position_hist.print(ofhist_2);
    ofhist_2.close();
    y_z_position_hist.print(ofhist_3);
    ofhist_3.close();
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
