//test_feb_data.cxx


// Standard libraries :
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>



// Falaise:
#include <falaise/falaise.h>


// This project :
#include <snemo/digitization/tracker_feb_data.h>
#include <snemo/digitization/calo_feb_data.h>


int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  try {
    std::clog << "Test program for class 'snemo::digitization::tracker_feb_data' !" << std::endl;

    bool debug = false;

    snemo::digitization::tracker_feb_data my_tracker_feb_data;


    if(debug) std::clog<<"tracker feb data is initialized : "<<std::boolalpha <<my_tracker_feb_data.is_initialized() <<std::endl;
    if(debug) my_tracker_feb_data.tree_dump(std::clog, "tracker FEB : ", "INFO : ");
    
    geomtools::geom_id tracker_feb_id;
    tracker_feb_id.set_type(2017);
    tracker_feb_id.set_address(0,1,2,0,7);

    
    my_tracker_feb_data.set_header(42,tracker_feb_id);
    my_tracker_feb_data.set_data(11,13,17,19,23,29,31);
    bool NEMO3_mode = true;
    my_tracker_feb_data.initialize(NEMO3_mode);
    
    std::clog<<"tracker feb data is initialized : "<<std::boolalpha <<my_tracker_feb_data.is_initialized() <<std::endl;


	
    my_tracker_feb_data.tree_dump(std::clog, "Tracker FEB : ", "INFO : ");


    snemo::digitization::calo_feb_data my_calo_feb_data;
    if(debug) std::clog<<"calo feb data is initialized : "<<std::boolalpha <<my_calo_feb_data.is_initialized() <<std::endl;
    if(debug) my_calo_feb_data.tree_dump(std::clog, "calo FEB : ", "INFO : ");
    
    
    geomtools::geom_id calo_feb_id;
    calo_feb_id.set_type(2018);
    calo_feb_id.set_address(0,2,5,6);
    uint64_t calo_date = 123456789123456789;
    
    int tmp_NoS = 20;
    my_calo_feb_data.initialize(true, tmp_NoS , 16);

    std::vector<uint16_t> waveform[2];
    waveform[0].reserve(tmp_NoS);
    waveform[1].reserve(tmp_NoS);

 
    double baseline[2]    = {0};
    double charge[2]      = {0};
    double max[2]         = {0};
    uint16_t time_max[2]  = {0};
    double falling[2]     = {0};
    double rising[2]      = {0};


    for(int j =0;j<2;j++){    
      for(int i =0;i<tmp_NoS;i++){
	waveform[j].push_back(2*i+j);
      }
      baseline[j]    = 0.666*j;
      charge[j]      = 42-j;
      max[j]         = j*.666;
      time_max[j]    = 12;
      falling[j]     = 3.14+j;
      rising[j]      = 6.022+2*j;
    }
    

    my_calo_feb_data.set_header(42,calo_feb_id, calo_date);
    
    my_calo_feb_data.set_data(waveform);
    
    my_calo_feb_data.set_detected_treshold_per_channel(true , false , 0);
    my_calo_feb_data.set_detected_treshold_per_channel(true , true  , 1);
    
    my_calo_feb_data.set_metadata(baseline,charge,max,time_max,falling,rising);

    my_calo_feb_data.tree_dump(std::clog, "Calo FEB : ", "INFO : ");
    

    

    

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
