//test_tracker_trigger_algorithm.cxx
// Standard libraries :
#include <iostream>
#include <fstream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>

// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// Root : 
#include "TFile.h"
#include "TTree.h"

int main(int  argc_ , char ** argv_)
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  bool is_input_file   = false;
  std::string input_filename;

  // Process
  try {
    std::clog << "Little test program for analysis of alpha delayed number of geiger cells hit !" << std::endl;

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

    datatools::fetch_path_with_env(input_filename);
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
      // pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/trigger/simulated_data_brio/Bi214_Po214_500000-source_strips_bulk_SD.brio";
      //pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/trigger/simulated_data_brio/Se82_0nubb_500000-source_strips_bulk_SD.brio";
      //pipeline_simulated_data_filename = "${DATA_NEMO_PERSO_DIR}/trigger/simulated_data_brio/Bi214_Po214_500000-field_wire_surface_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 5);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    // Peut-etre nom a changé : char* (non string, fetch_path_with_env pas utilisable)
    TFile* root_file = new TFile("~/software/Falaise/Falaise-trunk/modules/Digitization/programs/validation.root", "recreate");
    TTree* tree = new TTree("TrackerTree", "Alpha track analysis");
    Int_t number_of_gg_cells = 0;
    Int_t number_of_delayed_gg_cells = 0;
    Int_t number_of_not_delayed_gg_cells = 0;
    Double_t time_start = 0;
    Int_t hit_id = 0; 
    Int_t event_id = 0;

    tree->Branch("number_of_gg_cells", &number_of_gg_cells, "number_of_gg_cells/I");
    tree->Branch("number_of_delayed_gg_cells", &number_of_delayed_gg_cells, "number_of_delayed_gg_cells/I");
    tree->Branch("number_of_not_delayed_gg_cells", &number_of_not_delayed_gg_cells, "number_of_not_delayed_gg_cells/I");
    tree->Branch("time_start", &time_start, "time_start/D");
    tree->Branch("hit_id", &hit_id, "hit_id/I");
    tree->Branch("event_id", &event_id, "event_id/I");

    // Branch a 3d vector geomtools ? 

    // Event record :
    datatools::things ER;
    //std::ofstream file(producted_file.c_str());
    int psd_count = 0;         // Event counter
    while (!reader.is_terminated())
      {
	event_id = psd_count;
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	  
	    if (SD.has_step_hits("gg"))
	      {
		SD.tree_dump(std::clog, "Simulated Data : ", "INFO : ");
		mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("gg");
		std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		number_of_gg_cells =  BSHC.size();
		int count = 0;
		for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
		     i != BSHC.end();
		     i++) 
		  {
		    const mctools::base_step_hit & BSH = i->get();
		    //BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
		    // std::string particle_name = BSH.get_particle_name();
		    time_start = BSH.get_time_start();
		    hit_id = count;
		    if (time_start > 10) number_of_delayed_gg_cells++; // time in ns
		    else{
		      number_of_not_delayed_gg_cells++;
		     }
		    std::clog << " Count (aka hit id ) = " << count << std::endl;
		    count++;
		    tree->Fill();
		  }
		//std::clog << "Number of delayed alpha cells = " << delayed_alpha_cells << std::endl;
	      }
	  }
	//tree->Fill();

	number_of_delayed_gg_cells = 0;
	number_of_gg_cells = 0;
	number_of_not_delayed_gg_cells = 0;
	time_start = 0;
	hit_id = 0; 

	ER.clear();
	psd_count++;

	//std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    

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
