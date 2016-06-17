// hc_rate_analysis.cxx
// Standard libraries :
#include <iostream>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/id_mgr.h>
#include <bayeux/geomtools/id_selector.h>

// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// Root : 
#include "TFile.h"
#include "TTree.h"

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
  bool is_display      = false;
  bool is_help         = false;  
  
  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;

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
		<< "$ BuildProducts/bin/half_commissioning-hc_rate_analysis [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output path ]    set a path where all files are stored" << std::endl
		<< "-d  [ --display ]        display things for debug" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "Example : " << std::endl << std::endl;
	return 0;
    }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;   

    std::clog << "Program for half commissioning rate analysis" << std::endl;
    
    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_geom_manager;
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_geom_manager.initialize (manager_config);

    std::string pipeline_simulated_data_filename;
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    datatools::fetch_path_with_env(input_filename);
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Co60-100_row_0_column_0_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number) event_number = arg_event_number;
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

    // Name of SD output files
    std::string HC_writer_1 = output_path + "HC_writer" + ".brio";

    // Event writer : 
    dpp::output_module writer_1;
    datatools::properties writer_config_1;
    writer_config_1.store ("logging.priority", "debug");
    writer_config_1.store ("files.mode", "single");   
    writer_config_1.store ("files.single.filename", HC_writer_1);
    writer_1.initialize_standalone(writer_config_1); 

    // Event record :
    datatools::things ER;
    
    // Vertex in the filename :
    std::string vtx_filename = pipeline_simulated_data_filename;
    std::size_t found = 0;
    found = vtx_filename.find_last_of("/");
    vtx_filename.erase(0, found+1);
    found = vtx_filename.find(".brio");
    vtx_filename.erase(found, vtx_filename.size());
    
    // Output ROOT file : 
    std::string root_filename = output_path + "HC_" + vtx_filename + "_analysis.root";
    datatools::fetch_path_with_env(root_filename);
    TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");

    TTree* hc_analysis_tree = new TTree("HC_analysis_tree", "Half Commissioning analysis tree");
    
    // Internal counters
    int psd_count = 0;         // Event counter

    geomtools::id_selector my_hc_id_selector(my_geom_manager.get_id_mgr());
    std::string hc_half_zone_rules = "category='drift_cell_core' module={0} side={1} layer={1} row={*}";
    my_hc_id_selector.initialize(hc_half_zone_rules);

    if (is_display) my_hc_id_selector.dump(std::clog, "ID selector: ");
    
    while (!reader.is_terminated())
      {
	if(is_display) std::clog <<  "********************************************************************************" << std::endl;
	if(is_display) std::clog <<  "****************************** EVENT #" << psd_count << " **************************************" << std::endl;
	
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	    
	    if (SD.has_step_hits("gg"))
	      {
		const size_t number_of_hits = SD.get_number_of_step_hits("gg");

		// New sd bank
		mctools::simulated_data flaged_sd = SD;
	
		// We have to flag the gg cells already hit before (maybe take into account the dead time of a GG cell)
		for (size_t ihit = 0; ihit < number_of_hits; ihit++)
		  {
		    mctools::base_step_hit & geiger_hit = flaged_sd.grab_step_hit("gg", ihit);	    
		    for (size_t jhit = ihit + 1; jhit < number_of_hits; jhit++)
		      {
			mctools::base_step_hit & other_geiger_hit = flaged_sd.grab_step_hit("gg", jhit);
			if (geiger_hit.get_geom_id() == other_geiger_hit.get_geom_id())
			  {
			    const double gg_hit_time       = geiger_hit.get_time_start();
			    const double other_gg_hit_time = other_geiger_hit.get_time_start();
			    if (gg_hit_time > other_gg_hit_time) 
			      {
				bool geiger_already_hit = true;
				if (!geiger_hit.get_auxiliaries().has_flag("geiger_already_hit")) geiger_hit.grab_auxiliaries().store("geiger_already_hit", geiger_already_hit);
			      }
			    else 
			      {
				bool geiger_already_hit = true;
				if (!other_geiger_hit.get_auxiliaries().has_flag("geiger_already_hit")) other_geiger_hit.grab_auxiliaries().store("geiger_already_hit", geiger_already_hit);
			      }
			  } // end of if get_geom_id
		      } // end of jhit
		  } // end of ihit
		mctools::simulated_data::hit_handle_collection_type BSHC = flaged_sd.get_step_hits("gg");
		if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		int count = 0;
		for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
		     i != BSHC.end();
		     i++) 
		  {
		    const mctools::base_step_hit & BSH = i->get();
		    if (is_display) BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
		    if (BSH.get_auxiliaries().has_flag("geiger_already_hit") || BSH.get_auxiliaries().has_flag("other_geiger_already_hit")) {}
		    else
		      {
			// extract the corresponding geom ID:
			const geomtools::geom_id & geiger_gid = BSH.get_geom_id();

			if (my_hc_id_selector.match(geiger_gid))
			  {
			    if (is_display) std::clog << "ID=" << geiger_gid << " matches the selector rules !" << std::endl;
			  }
			else
			  {
			    if (is_display) std::clog << "ID=" << geiger_gid << " does not match the selector rules !" << std::endl;
			  }
			
			std::clog << "geiger gid = " << geiger_gid << std::endl;
			int hit_id = count;
			double time_start = BSH.get_time_start();
			geomtools::vector_3d position_start_vector = BSH.get_position_start();
			geomtools::vector_3d position_stop_vector  = BSH.get_position_stop();
			geomtools::vector_3d momentum_start_vector = BSH.get_momentum_start();
			
			count++;

		      }
		  } // end of for 
	      } // end of if has step hits "gg"

	  } // end of ER
	ER.clear();
	psd_count++;
	if (is_display) std::clog << "DEBUG : psd count " << psd_count << std::endl;
      } // end of while reader
    

    std::clog << "The end." << std::endl;
  } // end of try

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
