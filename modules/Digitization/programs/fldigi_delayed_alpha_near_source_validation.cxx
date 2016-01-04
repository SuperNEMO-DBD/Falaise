//fldigi_delayed_alpha_near_source_validation.cxx
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
    reader_config.store ("max_record_total", 1);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    std::string root_filename = "${FALAISE_DIGITIZATION_DIR}/programs/validation.root";
    datatools::fetch_path_with_env(root_filename);
    TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");
    TTree* tree = new TTree("TrackerTree", "Alpha track analysis");
    Int_t number_of_gg_cells = 0;
    Int_t number_of_delayed_gg_cells = 0;
    Int_t number_of_not_delayed_gg_cells = 0;
    std::vector<double> vect_time_start;
    std::vector<Double_t> vect_x_start;
    std::vector<Double_t> vect_y_start;
    std::vector<Double_t> vect_z_start;
    std::vector<Double_t> vect_x_stop;
    std::vector<Double_t> vect_y_stop;
    std::vector<Double_t> vect_z_stop;
    std::vector<Double_t> vect_px_start;
    std::vector<Double_t> vect_py_start;
    std::vector<Double_t> vect_pz_start;
    std::vector<double> * ptr_time_start = & vect_time_start;
    std::vector<Double_t> * ptr_x_start = & vect_x_start;
    std::vector<Double_t> * ptr_y_start = & vect_y_start;
    std::vector<Double_t> * ptr_z_start = & vect_z_start;
    std::vector<Double_t> * ptr_x_stop = & vect_x_stop;
    std::vector<Double_t> * ptr_y_stop = & vect_y_stop;
    std::vector<Double_t> * ptr_z_stop = & vect_z_stop;
    std::vector<Double_t> * ptr_px_start = & vect_px_start;
    std::vector<Double_t> * ptr_py_start = & vect_py_start;
    std::vector<Double_t> * ptr_pz_start = & vect_pz_start;

    std::vector<Int_t> vect_hit_id;
    std::vector<Int_t> * ptr_hit_id = & vect_hit_id;
  
    Int_t event_id    = 0;

    tree->Branch("event_id_vector", &event_id, "event_id/I");
    tree->Branch("number_of_gg_cells", &number_of_gg_cells, "number_of_gg_cells/I");
    tree->Branch("number_of_delayed_gg_cells", &number_of_delayed_gg_cells, "number_of_delayed_gg_cells/I");
    tree->Branch("number_ of_not_delayed_gg_cells", &number_of_not_delayed_gg_cells, "number_of_not_delayed_gg_cells/I");
    tree->Branch("hit_id_vector",     &ptr_hit_id, "hit_id/I");
    tree->Branch("time_start_vector", &vect_time_start);//, "time_start/D");
    tree->Branch("x_start_vector",    &ptr_x_start, "x_start/D");
    tree->Branch("y_start_vector",    &ptr_y_start, "y_start/D");
    tree->Branch("z_start_vector",    &ptr_z_start, "z_start/D");
    tree->Branch("x_stop_vector",     &ptr_x_stop, "x_stop/D");
    tree->Branch("y_stop_vector",     &ptr_y_stop, "y_stop/D");
    tree->Branch("z_stop_vector",     &ptr_z_stop, "z_stop/D");
    tree->Branch("px_start_vector",   &ptr_px_start, "px_start/D");
    tree->Branch("py_start_vector",   &ptr_py_start, "py_start/D");
    tree->Branch("pz_start_vector",   &ptr_pz_start, "pz_start/D");

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
	    ptr_hit_id->clear();
	    //ptr_time_start->clear();
	    ptr_x_start-> clear();
	    ptr_y_start->clear();
	    ptr_z_start->clear();
	    ptr_x_stop->clear();
	    ptr_y_stop->clear();
	    ptr_z_stop->clear();
	    ptr_px_start->clear();
	    ptr_py_start->clear();
	    ptr_pz_start->clear();

	    vect_time_start.clear();

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
		    BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
		    // std::string particle_name = BSH.get_particle_name();

		    Int_t hit_id = count;
		    ptr_hit_id->push_back(hit_id);

		    double time_start = BSH.get_time_start();
		    //ptr_time_start->push_back(time_start);
		    vect_time_start.push_back(time_start);

		    geomtools::vector_3d position_start_vector = BSH.get_position_start();
		    geomtools::vector_3d position_stop_vector  = BSH.get_position_stop();
		    geomtools::vector_3d momentum_start_vector = BSH.get_momentum_start();
		    Double_t x_start = position_start_vector.getX();
		    Double_t y_start = position_start_vector.getY();
		    Double_t z_start = position_start_vector.getZ();
		    Double_t x_stop  = position_stop_vector.getX();
		    Double_t y_stop  = position_stop_vector.getY();
		    Double_t z_stop  = position_stop_vector.getZ();
		    Double_t px_start = momentum_start_vector.getX();
		    Double_t py_start = momentum_start_vector.getY();
		    Double_t pz_start = momentum_start_vector.getZ();

		    ptr_x_start->push_back(x_start);
		    ptr_y_start->push_back(y_start);
		    ptr_z_start->push_back(z_start);
		    ptr_x_stop->push_back(x_stop);
		    ptr_y_stop->push_back(y_stop);
		    ptr_z_stop->push_back(z_stop);
		    ptr_px_start->push_back(px_start);
		    ptr_py_start->push_back(py_start);
		    ptr_pz_start->push_back(pz_start);


		    if (time_start > 10) number_of_delayed_gg_cells++; // time in ns
		    else{
		      number_of_not_delayed_gg_cells++;
		     }
		    std::clog << " Count (aka hit id ) = " << count << std::endl;
		    count++;
		  }
		std::clog << "Vect time start size = " << vect_time_start.size() << std::endl;
		tree->Fill();
	      }
	  }

	number_of_delayed_gg_cells = 0;
	number_of_gg_cells = 0;
	number_of_not_delayed_gg_cells = 0;

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
