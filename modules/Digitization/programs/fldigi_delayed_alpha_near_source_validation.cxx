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
  falaise::initialize(argc_, argv_);
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
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 10);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    std::string root_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/fldigi_delayed_alpha_validation.root";
    datatools::fetch_path_with_env(root_filename);
    TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");

    TTree* gg_cells_tree = new TTree("GGCellsTree", "Alpha track analysis");
    TTree* delayed_cells_tree = new TTree("DelayedCellsTree", "Alpha track analysis");

    // Variables definitions :
    Int_t event_id    = 0;
    std::vector<Int_t> vect_number_of_gg_cells;
    std::vector<Int_t> vect_number_of_delayed_gg_cells;
    std::vector<Int_t> vect_number_of_not_delayed_gg_cells;
    std::vector<Int_t> vect_delayed_time_shift;
    std::vector<Int_t> vect_hit_id;
    std::vector<Double_t> vect_time_start;
    std::vector<Double_t> vect_x_start;
    std::vector<Double_t> vect_y_start;
    std::vector<Double_t> vect_z_start;
    std::vector<Double_t> vect_x_stop;
    std::vector<Double_t> vect_y_stop;
    std::vector<Double_t> vect_z_stop;
    std::vector<Double_t> vect_px_start;
    std::vector<Double_t> vect_py_start;
    std::vector<Double_t> vect_pz_start;
    std::vector<Int_t> * ptr_number_of_gg_cells = & vect_number_of_gg_cells;
    std::vector<Int_t> * ptr_number_of_delayed_gg_cells = & vect_number_of_delayed_gg_cells;
    std::vector<Int_t> * ptr_number_of_not_delayed_gg_cells = & vect_number_of_not_delayed_gg_cells;
    std::vector<Int_t> * ptr_delayed_time_shift = & vect_delayed_time_shift;
    std::vector<Int_t> * ptr_hit_id = & vect_hit_id;
    std::vector<Double_t> * ptr_time_start = & vect_time_start;
    std::vector<Double_t> * ptr_x_start = & vect_x_start;
    std::vector<Double_t> * ptr_y_start = & vect_y_start;
    std::vector<Double_t> * ptr_z_start = & vect_z_start;
    std::vector<Double_t> * ptr_x_stop = & vect_x_stop;
    std::vector<Double_t> * ptr_y_stop = & vect_y_stop;
    std::vector<Double_t> * ptr_z_stop = & vect_z_stop;
    std::vector<Double_t> * ptr_px_start = & vect_px_start;
    std::vector<Double_t> * ptr_py_start = & vect_py_start;
    std::vector<Double_t> * ptr_pz_start = & vect_pz_start;

    // Branch definitions : 
    gg_cells_tree->Branch("event_id", &event_id, "event_id/I");
    delayed_cells_tree->Branch("event_id", &event_id);//, "event_id/I");
    delayed_cells_tree->Branch("number_of_gg_cells", &ptr_number_of_gg_cells);//, "number_of_gg_cells/I");
    delayed_cells_tree->Branch("number_of_delayed_gg_cells", &ptr_number_of_delayed_gg_cells);//, "number_of_delayed_gg_cells/I");
    delayed_cells_tree->Branch("number_of_not_delayed_gg_cells", &ptr_number_of_not_delayed_gg_cells);//, "number_of_not_delayed_gg_cells/I");
    delayed_cells_tree->Branch("delayed_time_shift", &ptr_delayed_time_shift);//, "delayed_time_shift/I");
    gg_cells_tree->Branch("hit_id_vector",     &ptr_hit_id);//, "hit_id/I");
    gg_cells_tree->Branch("time_start_vector", &ptr_time_start);//, "time_start/D");
    gg_cells_tree->Branch("x_start_vector",    &ptr_x_start);//, "x_start/D");
    gg_cells_tree->Branch("y_start_vector",    &ptr_y_start);//, "y_start/D");
    gg_cells_tree->Branch("z_start_vector",    &ptr_z_start);//, "z_start/D");
    gg_cells_tree->Branch("x_stop_vector",     &ptr_x_stop);//, "x_stop/D");
    gg_cells_tree->Branch("y_stop_vector",     &ptr_y_stop);//, "y_stop/D");
    gg_cells_tree->Branch("z_stop_vector",     &ptr_z_stop);//, "z_stop/D");
    gg_cells_tree->Branch("px_start_vector",   &ptr_px_start);//, "px_start/D");
    gg_cells_tree->Branch("py_start_vector",   &ptr_py_start);//, "py_start/D");
    gg_cells_tree->Branch("pz_start_vector",   &ptr_pz_start);//, "pz_start/D");
    
    static const Int_t MAXIMUM_DELAYED_TIME = 50;
    
    bool debug = false;

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
	    ptr_time_start->clear();
	    ptr_x_start-> clear();
	    ptr_y_start->clear();
	    ptr_z_start->clear();
	    ptr_x_stop->clear();
	    ptr_y_stop->clear();
	    ptr_z_stop->clear();
	    ptr_px_start->clear();
	    ptr_py_start->clear();
	    ptr_pz_start->clear();
	    ptr_number_of_gg_cells->clear();
	    ptr_number_of_delayed_gg_cells->clear();
	    ptr_number_of_not_delayed_gg_cells->clear();
	    ptr_delayed_time_shift->clear();  

	    for (Int_t delayed_time_shift = 10; delayed_time_shift <= MAXIMUM_DELAYED_TIME; delayed_time_shift+= 10) // Time for delayed gg cells (in ns)
	      {	 
 
		if (debug) std::clog << "Delayed time shift = " << delayed_time_shift << std::endl;
		Int_t number_of_gg_cells = 0;
		Int_t number_of_delayed_gg_cells = 0;
		Int_t number_of_not_delayed_gg_cells = 0;

		if (SD.has_step_hits("gg"))
		  {
		    if (debug) SD.tree_dump(std::clog, "Simulated Data : ", "INFO : ");
		    mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("gg");
		    if (debug) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		    number_of_gg_cells =  BSHC.size();
		    int count = 0;
		    for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
			 i != BSHC.end();
			 i++) 
		      {
			const mctools::base_step_hit & BSH = i->get();
			if (debug) BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
			// std::string particle_name = BSH.get_particle_name();

			Int_t hit_id = count;
			Double_t time_start = BSH.get_time_start();
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

			// Just to fill one time these informations
			if (delayed_time_shift == 10)
			  {
			    ptr_hit_id->push_back(hit_id);
			    ptr_time_start->push_back(time_start);
			    ptr_x_start->push_back(x_start);
			    ptr_y_start->push_back(y_start);
			    ptr_z_start->push_back(z_start);
			    ptr_x_stop->push_back(x_stop);
			    ptr_y_stop->push_back(y_stop);
			    ptr_z_stop->push_back(z_stop);
			    ptr_px_start->push_back(px_start);
			    ptr_py_start->push_back(py_start);
			    ptr_pz_start->push_back(pz_start);
			  }

			if (time_start > delayed_time_shift) number_of_delayed_gg_cells++; // time in ns
			else number_of_not_delayed_gg_cells++;
			
			count++;
		      }
		  }
		if (debug) std::clog << "GG Cells #" << number_of_gg_cells << " Delayed GG Cells #" << number_of_delayed_gg_cells << " Not Delayed GG Cells #" << number_of_not_delayed_gg_cells << std::endl;
		ptr_delayed_time_shift->push_back(delayed_time_shift);
		ptr_number_of_gg_cells->push_back(number_of_gg_cells);
	        ptr_number_of_delayed_gg_cells->push_back(number_of_delayed_gg_cells);
	        ptr_number_of_not_delayed_gg_cells->push_back(number_of_not_delayed_gg_cells);
	      }
	  }

	gg_cells_tree->Fill();	
	delayed_cells_tree->Fill();
	
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

  falaise::terminate();
  return error_code;
}
