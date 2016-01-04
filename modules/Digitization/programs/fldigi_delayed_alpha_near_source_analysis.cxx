//fldigi_delayed_alpha_near_source_analysis.cxx
// Standard libraries :
#include <iostream>
#include <fstream>

// - Bayeux/datatools:
#include <datatools/io_factory.h>

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

  // Process
  try {  
    std::clog << "Little test program for analysis of alpha delayed number of geiger cells hit !" << std::endl;
    
    std::string input_file_name = "${FALAISE_DIGITIZATION_DIR}/programs/validation.root";

    // Creation and openning of ROOT file , and get the infile tree :
    TFile * input_file = new TFile(input_file_name.c_str() , "READ");

    TTree * input_tree = (TTree *)input_file->Get("TrackerTree");
    Int_t number_of_gg_cells = 0;
    Int_t number_of_delayed_gg_cells = 0;
    Int_t number_of_not_delayed_gg_cells = 0;
    double time_start = 0;
    Double_t x_start  = 0;
    Double_t y_start  = 0;
    Double_t z_start  = 0;
    Double_t x_stop   = 0;
    Double_t y_stop   = 0;
    Double_t z_stop   = 0;
    Double_t px_start = 0;
    Double_t py_start = 0;
    Double_t pz_start = 0;
    std::vector<double> * vect_time_start = 0;
    std::vector<Double_t> vect_x_start;
    std::vector<Double_t> vect_y_start;
    std::vector<Double_t> vect_z_start;
    std::vector<Double_t> vect_x_stop;
    std::vector<Double_t> vect_y_stop;
    std::vector<Double_t> vect_z_stop;
    std::vector<Double_t> vect_px_start;
    std::vector<Double_t> vect_py_start;
    std::vector<Double_t> vect_pz_start;
    Int_t hit_id      = 0; 
    Int_t event_id    = 0;
    //input_tree->SetBranchAddress("event_id",   &event_id);
    input_tree->SetBranchAddress("number_of_gg_cells", &number_of_gg_cells);
    input_tree->SetBranchAddress("number_of_delayed_gg_cells", &number_of_delayed_gg_cells);
    input_tree->SetBranchAddress("number_ of_not_delayed_gg_cells", &number_of_not_delayed_gg_cells);
    TBranch *branch_vect_time_start = 0;
    input_tree->SetBranchAddress("time_start_vector", &vect_time_start, &branch_vect_time_start);
    // input_tree->SetBranchAddress("x_start",    &x_start);
    // input_tree->SetBranchAddress("y_start",    &y_start);
    // input_tree->SetBranchAddress("z_start",    &z_start);
    // input_tree->SetBranchAddress("x_stop",     &x_stop);
    // input_tree->SetBranchAddress("y_stop",     &y_stop);
    // input_tree->SetBranchAddress("z_stop",     &z_stop);
    // input_tree->SetBranchAddress("px_start",   &px_start);
    // input_tree->SetBranchAddress("py_start",   &py_start);
    // input_tree->SetBranchAddress("pz_start",   &pz_start);
    // input_tree->SetBranchAddress("hit_id",     &hit_id);

    // Begining of the analysis
    std::clog << "Number of Entries in TTree = " <<  input_tree->GetEntries() << std::endl; 
    Int_t n_entry = input_tree->GetEntries();
    
    for( int i = 0 ; i < input_tree->GetEntries() ; i++ )
      {
    	input_tree->GetEntry(i) ;
    	branch_vect_time_start->GetEntry(i);
    	std::clog << "vect size = " << vect_time_start->size() << std::endl;
    	// for (int j = 0; j < vect_time_start->size(); j++)
    	//   {
    	//     std::clog << "j = " << j << " Time start = " << vect_time_start->at(j) << std::endl;
    	//   }
    	std::clog << "Event ID  = " << event_id << std::endl;
    	//std::clog << "Hit ID  = "   << hit_id << std::endl;
    	//std::clog << "Tstart = "    << time_start << std::endl;
    	std::clog << "Number of gg cells = " << number_of_gg_cells << std::endl;
    	std::clog << "Number of delayed gg cells = " << number_of_delayed_gg_cells << std::endl;
    	std::clog << "Number of not delayed gg cells = " << number_of_not_delayed_gg_cells << std::endl << std::endl;
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
