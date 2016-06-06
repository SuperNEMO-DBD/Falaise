// trigger_algorithm_efficiency_analysis.cxx
// Standard libraries :
#include <iostream>
#include <fstream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// Root : 
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

int main(int  argc_ , char ** argv_)
{  
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Process
  try {    
    bool debug = false;
    
    if (debug) std::clog << "Test program for analysis of trigger algorithm efficiency (Real Trigger) !" << std::endl;

    // Parsing arguments
    int iarg = 1;
    bool is_input_path   = false;
    bool is_output_path  = false;
    bool is_help         = false;

    std::string input_path = "";
    std::string output_path = "";
    
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      
      if (arg == "-ip" || arg == "--input-path")
	{
	  is_input_path = true;
	  input_path = argv_[++iarg];	
	}
      
      else if (arg == "-op" || arg == "--output-path")
	{
	  is_output_path = true;
	  output_path = argv_[++iarg];	
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
		<< "-ip [ --input-path ]     set an input path where the file is. Filename is 'trigger_validation.root' " << std::endl
		<< "-op [ --output-path ]    set a path where files will be stored" << std::endl << std::endl;
      return 0;
    }
      
    std::string input_filename = "trigger_validation.root";
    
    if(is_input_path){
      input_path = input_path;
    }else{
      input_path ="${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    }
    datatools::fetch_path_with_env(input_path);
    std::string input_file = input_path + input_filename;
    datatools::fetch_path_with_env(input_file);
   
    // Creation and openning of ROOT file , and get the infile tree :
    TFile * input_root_file = new TFile(input_file.c_str() , "READ");
    TTree * input_tree = (TTree *)input_root_file->Get("TriggerDecision");

    // Variables definitions :
    Int_t event_id    = 0;
    Bool_t raw_trigger_prompt_decision = false;
    Bool_t raw_trigger_delayed_decision = false;
    Int_t total_number_of_calo = 0;
    Int_t total_number_of_main_calo = 0;
    Int_t total_number_of_gveto = 0;
    Int_t total_number_of_gg_cells = 0;
    Int_t total_number_of_prompt_gg_cells = 0;
    Int_t total_number_of_delayed_gg_cells = 0;

    // Input tree Set all Branch Address :
    input_tree->SetBranchAddress("event_id",                         &event_id);
    input_tree->SetBranchAddress("raw_trigger_prompt_decision",      &raw_trigger_prompt_decision);
    input_tree->SetBranchAddress("raw_trigger_delayed_decision",     &raw_trigger_delayed_decision);
    input_tree->SetBranchAddress("total_number_of_calo",             &total_number_of_calo);
    input_tree->SetBranchAddress("total_number_of_main_calo",        &total_number_of_main_calo);
    input_tree->SetBranchAddress("total_number_of_gveto",            &total_number_of_gveto);
    input_tree->SetBranchAddress("total_number_of_gg_cells",         &total_number_of_gg_cells);
    input_tree->SetBranchAddress("total_number_of_prompt_gg_cells",  &total_number_of_prompt_gg_cells);
    input_tree->SetBranchAddress("total_number_of_delayed_gg_cells", &total_number_of_delayed_gg_cells);
    
   // Begining of the analysis
    Int_t n_entry = input_tree->GetEntries();
    if (debug) std::clog << "Number of Entries in TTree = " <<  n_entry << std::endl; 
   
    int nbr_of_prompt_triggered_event = 0;
    int nbr_of_delayed_triggered_event = 0;
    double mean_number_of_calo = 0;
    double mean_number_of_main_calo = 0;
    double mean_number_of_gveto = 0;
    double mean_number_of_gg_cells = 0;
    double mean_number_of_prompt_gg_cells = 0;
    double mean_number_of_delayed_gg_cells = 0;
    
    for( int i = 0 ; i < n_entry; i++ )
      {
	input_tree->GetEntry(i);
	
	if (raw_trigger_prompt_decision) 
	  {
	    nbr_of_prompt_triggered_event++;
	    mean_number_of_calo += total_number_of_calo;
	    mean_number_of_main_calo += total_number_of_main_calo;
	    mean_number_of_gveto += total_number_of_gveto;
	    mean_number_of_prompt_gg_cells += total_number_of_prompt_gg_cells;
	    mean_number_of_gg_cells += total_number_of_prompt_gg_cells;
	  }

	if (raw_trigger_delayed_decision)
	  {
	    nbr_of_delayed_triggered_event++;
	    mean_number_of_delayed_gg_cells += total_number_of_delayed_gg_cells;
	    mean_number_of_gg_cells += total_number_of_delayed_gg_cells;
	  }
      }
    
    double TOTAL_NUMBER_OF_EVENTS = 100000.;

    mean_number_of_calo = mean_number_of_calo / nbr_of_prompt_triggered_event;
    mean_number_of_main_calo = mean_number_of_main_calo / nbr_of_prompt_triggered_event;
    mean_number_of_gveto = mean_number_of_gveto / nbr_of_prompt_triggered_event;
    mean_number_of_prompt_gg_cells = mean_number_of_prompt_gg_cells / nbr_of_prompt_triggered_event;
    mean_number_of_delayed_gg_cells = mean_number_of_delayed_gg_cells / nbr_of_delayed_triggered_event;
    mean_number_of_gg_cells = mean_number_of_gg_cells / (nbr_of_prompt_triggered_event + nbr_of_delayed_triggered_event);
    
    double fake_trigger_efficiency = (static_cast<double>(n_entry) /  TOTAL_NUMBER_OF_EVENTS) * 100;
    
    double percentage_of_prompt_triggered_event_to_total = (static_cast<double>(nbr_of_prompt_triggered_event) / TOTAL_NUMBER_OF_EVENTS) * 100;
    double percentage_of_prompt_triggered_event_to_FT = (static_cast<double>(nbr_of_prompt_triggered_event) / static_cast<double>(n_entry)) * 100;
        
    double percentage_of_delayed_triggered_event_to_total = (static_cast<double>(nbr_of_delayed_triggered_event) / TOTAL_NUMBER_OF_EVENTS) * 100;
    double percentage_of_delayed_triggered_event_to_FT = (static_cast<double>(nbr_of_delayed_triggered_event) / static_cast<double>(n_entry)) * 100;

    if (debug) std::clog << "% of prompt triggered event to total  = " << percentage_of_prompt_triggered_event_to_total << "%" << std::endl;
    if (debug) std::clog << "% of prompt triggered event to FT     = " << percentage_of_prompt_triggered_event_to_FT << "%" <<  std::endl;
    if (debug) std::clog << "% of delayed triggered event to total = " << percentage_of_delayed_triggered_event_to_total << "%" <<  std::endl;
    if (debug) std::clog << "% of delayed triggered event to FT    = " << percentage_of_delayed_triggered_event_to_FT << "%" <<  std::endl;

    if (debug) std::clog << "Number of prompt event triggered      = " << nbr_of_prompt_triggered_event << std::endl;
    if (debug) std::clog << "Number of delayed event triggered     = " << nbr_of_delayed_triggered_event << std::endl;
    if (debug) std::clog << "Mean number of calo                   = " << mean_number_of_calo << std::endl;
    if (debug) std::clog << "Mean number of main calo              = " << mean_number_of_main_calo << std::endl;
    if (debug) std::clog << "Mean number of gveto                  = " << mean_number_of_gveto << std::endl;
    if (debug) std::clog << "Mean number of gg cells               = " << mean_number_of_gg_cells << std::endl;
    if (debug) std::clog << "Mean number of prompt gg cells        = " << mean_number_of_prompt_gg_cells << std::endl;
    if (debug) std::clog << "Mean number of delayed gg cells       = " << mean_number_of_delayed_gg_cells << std::endl;
        
    std::string output_filename = "trigger_validation.log";
    
    if (is_output_path) output_path = output_path;
    else output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
    datatools::fetch_path_with_env(output_path);    
    
    std::string output_file = output_path + output_filename;
    datatools::fetch_path_with_env(output_file); 
     
    std::ofstream file(output_file.c_str(), std::ios::out | std::ios::trunc);
    if(file) 
      {
	file << "& Output path = " << output_path << std::endl;
	file << "Number of Entries in TTree = " << n_entry << std::endl; 
	file << "& Fake Trigger efficiency = " << fake_trigger_efficiency << std::endl;
     	file << "& % of prompt triggered event to total = " << percentage_of_prompt_triggered_event_to_total  << std::endl;  
	file << "& % of prompt triggered event to FT = " << percentage_of_prompt_triggered_event_to_FT <<  std::endl;
	file << "& % of delayed triggered event to total = " << percentage_of_delayed_triggered_event_to_total << std::endl;
	file << "Number of prompt event triggered = " << nbr_of_prompt_triggered_event << std::endl;
	file << "Number of delayed event triggered = " << nbr_of_delayed_triggered_event << std::endl;
	file << "& Mean number of calo = " << mean_number_of_calo << std::endl;
	file << "& Mean number of main calo = " << mean_number_of_main_calo << std::endl;
	file << "& Mean number of gveto = " << mean_number_of_gveto << std::endl;
	file << "Mean number of gg cells = " << mean_number_of_gg_cells << std::endl;
	file << "& Mean number of prompt gg cells = " << mean_number_of_prompt_gg_cells << std::endl;
	file << "& Mean number of delayed gg cells = " << mean_number_of_delayed_gg_cells << std::endl;
	file.close(); 
      }
    else
      {
	std::cerr << "Erreur à l'ouverture !" << std::endl;
      }
    
    if (debug) std::clog << "The end." << std::endl;
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
