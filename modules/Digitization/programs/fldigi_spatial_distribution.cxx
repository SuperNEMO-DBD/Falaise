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
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <TImage.h>
#include <TSystem.h>
#include <TPDF.h>
#include <TLegend.h>
#include <TStyle.h>

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
  bool is_verbose      = false;
  bool is_print        = false;
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

    else if (arg == "-v" || arg == "--verbosity")
      {
	is_verbose = true;
      }
    
    else if (arg == "-p" || arg == "--print")
      {
	is_print = true;
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
		<< "$ BuildProducts/bin/fldigi_delayed_alpha_validation [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]        produce help message" << std::endl
		<< "-i  [ --input ]       set an input file" << std::endl
		<< "-op [ --output_path ] set the output path for producted files" << std::endl
		<< "-n  [ --number ]      set the number of events" << std::endl
		<< "-v  [ --verbosity]    add verbosity level" << std::endl
		<< "-p  [ --print ]       print files in PDF/PNG" << std::endl
		<< "Example : " << std::endl << std::endl
		<< "$ BuildProducts/bin/fldigi_spatial_distribution -n 1000 -i ${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio -op ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default -p -v" << std::endl << std::endl;
      return 0;
    }
  
  // Process
  try {
    std::clog << "Little test program for analysis of spatial distribution of geiger cells hit and vertex !" << std::endl;

    bool debug = false;

    bool verbosity = false;
    if (is_verbose) verbosity = true;
        
    // Manager.conf
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
      input_filename = pipeline_simulated_data_filename;
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 10;
    // Output path :
    if (is_output_path) output_path = output_path;
    else output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    datatools::fetch_path_with_env(output_path);

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", event_number);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");
      
    static const int MAXIMUM_DELAYED_TIME = 8000; // in nanosecond, linked with the digi trigger and geiger drift

    // Event record :
    datatools::things ER;

    int psd_count = 0;         // Event counter
    std::string output_filename = output_path + "spatial_distribution.root";
    datatools::fetch_path_with_env(output_filename);
    
    std::string run_number = input_filename;
    std::size_t found = run_number.find("run");
    if (found != std::string::npos)
      {
	run_number.erase(0, found);
	found = run_number.find_first_of("/");
	run_number.erase(found, run_number.size());
      }
    TFile * output_file = new TFile(output_filename.c_str(), "RECREATE");
    
    TH2F * geiger_cells_distribution_TH2F = new TH2F("Prompt Geiger cells spatial distribution",
						     Form("%s : Prompt Geiger cells spatial distribution", run_number.c_str()) ,
						     115, -1.5 , 114 - 0.5,
						     28, -5.5 , 23 - 0.5);

    TH2F * vertex_distribution_TH2F = new TH2F("Vertex spatial distribution TH2F",
					       Form("%s : Vertex spatial distribution; x_vertex_position; y_vertex_position", run_number.c_str()),
					       104, -2700.5 , 2700 - 0.5,
					       120, -600 , 600);
    
    TH1F * vertex_distribution_TH1F = new TH1F("Vertex spatial distribution TH1F",
					       Form("%s : Vertex spatial distribution; y_vertex_position; count", run_number.c_str()),
					       104, -2600.5 , 2600 - 0.5);
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label); 
 
	    const geomtools::vector_3d event_vertex = SD.get_vertex();
	    double x_vertex = event_vertex.x();
	    double y_vertex = event_vertex.y();
	    double z_vertex = event_vertex.z();
	    
	    if(verbosity) std::clog << "Event #" << psd_count << std::endl;
	    if(verbosity) std::clog << "X = " << x_vertex << std::endl;
	    if(verbosity) std::clog << "Y = " << y_vertex << std::endl;
	    if(verbosity) std::clog << "Z = " << z_vertex << std::endl;
	    
	    vertex_distribution_TH2F->Fill(y_vertex, x_vertex);
	    vertex_distribution_TH1F->Fill(y_vertex);

	    int number_of_delayed_gg_cells = 0;
	    int number_of_not_delayed_gg_cells = 0;

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
			  }
		      }
		  }
		
		mctools::simulated_data::hit_handle_collection_type BSHC = flaged_sd.get_step_hits("gg");
	        // if (verbosity) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		int count = 0;
	       
		for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
		     i != BSHC.end();
		     i++) 
		  {
		    const mctools::base_step_hit & BSH = i->get();

		    if (BSH.get_auxiliaries().has_flag("geiger_already_hit") || BSH.get_auxiliaries().has_flag("other_geiger_already_hit")) {}
		    else
		      {
			// extract the corresponding geom ID:
			const geomtools::geom_id & geiger_gid = BSH.get_geom_id();
			
			/*const geomtools::vector_3d cell_position = BSH.get_position_stop();
			double cell_position_x = cell_position.x();
			double cell_position_y = cell_position.y();
			double cell_position_z = cell_position.z();*/
			 
			if(verbosity) std::clog << "GID cell : " << geiger_gid << std::endl;
			int side  = geiger_gid.get(1);
			int layer = geiger_gid.get(2);
			int row   = geiger_gid.get(3);
			  
			if(verbosity) std::clog << "side = " << side << " layer = " << layer << " row = " << row << std::endl;
			  
			int X_histo = row;
			int Y_histo = 0;
			
			if (side == 0)
			  {
			    Y_histo = 9 + layer;
			  }
			else if (side == 1)
			  {
			    Y_histo = 8 - layer;
			  }
			
			if(verbosity) std::clog << "X_HISTO = " << X_histo << " Y_HISTO = " << Y_histo << std::endl << std::endl;
			geiger_cells_distribution_TH2F->Fill(X_histo, Y_histo);
			double time_start = BSH.get_time_start();

			if (time_start > MAXIMUM_DELAYED_TIME) number_of_delayed_gg_cells++; // time in ns
			else number_of_not_delayed_gg_cells++;
			count++;
		      }
		  }	
	      }
	  }
	ER.clear();
	psd_count++;	
	if (debug) std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    output_file->cd();
    vertex_distribution_TH1F->Write();
    vertex_distribution_TH2F->Write();
    geiger_cells_distribution_TH2F->Write();
    output_file->Write();
 
    std::string string_buffer = "";
    
    if (is_print) {
      TCanvas * c = new TCanvas("c","l",960,800);
      gStyle->SetOptStat(0);
      vertex_distribution_TH2F->Draw("colz");
      string_buffer = output_path + "vertex_distribution_th2f.pdf";
      c->Print(string_buffer.c_str());
      gSystem->ProcessEvents();
      TImage * img = TImage::Create();
      img->FromPad(c);
      string_buffer = output_path + "vertex_distribution_th2f.png";
      img->WriteImage(string_buffer.c_str());
      
      c = new TCanvas("c","l",960,800);
      gStyle->SetOptStat(1);
      vertex_distribution_TH1F->Draw();
      string_buffer = output_path + "vertex_distribution_th1f.pdf";
      c->Print(string_buffer.c_str());
      gSystem->ProcessEvents();
      TImage * img2 = TImage::Create();
      img2->FromPad(c);
      string_buffer = output_path + "vertex_distribution_th1f.png";
      img2->WriteImage(string_buffer.c_str());

      c = new TCanvas("c","l",960,800);
      gStyle->SetOptStat(0);
      geiger_cells_distribution_TH2F->Draw("colz");
      string_buffer = output_path + "geiger_cells_distribution.pdf";
      c->Print(string_buffer.c_str());
      gSystem->ProcessEvents();
      TImage * img3 = TImage::Create();
      img3->FromPad(c);
      string_buffer = output_path + "geiger_cells_distribution.png";
      img3->WriteImage(string_buffer.c_str());
      
      delete c;
    }
   
    delete vertex_distribution_TH1F;
    delete vertex_distribution_TH2F;
    delete geiger_cells_distribution_TH2F;
    output_file->Close();
    delete output_file;

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
