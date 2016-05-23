//test_memory.cxx

// Standard libraries :
#include <iostream>
#include <string>

// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// Falaise :
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/memory.h>
#include <snemo/digitization/tracker_trigger_mem_maker.h>

int main( int  argc_ , char ** argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::test_memory' !" << std::endl;
    
    // ******************************* WARNING ******************************
    // TRACKER TRIGGER MEM MAKER HAS CHANGED THE FOLLOWING CODE CAN'T PROCESS :
    // ******************************* WARNING ****************************** 

    // std::string production_file;
    // production_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A6_D1_default_memory.data";
    // datatools::fetch_path_with_env(production_file);
    
    // unsigned int min_multiplicity = 2;
    // unsigned int max_gap = 1;
    // snemo::digitization::tracker_trigger_mem_maker my_memory_maker;
    // my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1,
    // 			      snemo::digitization::tracker_trigger_mem_maker::MEM_SIZE_6X1,
    // 			      snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1_ALGO_MAX_GAP);
    // my_memory_maker.set_max_gap(max_gap);
    // //my_memory_maker.set_min_multiplicity(min_multiplicity);
    // my_memory_maker.initialize();
    // my_memory_maker.store(production_file, "Default A5D1 memory for level 0 to level 1 tracker trigger. Maximum gap = 1 for the projection on rows.");
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



   
