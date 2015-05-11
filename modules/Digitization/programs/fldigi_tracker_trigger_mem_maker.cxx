//fldigi_tracker_trigger_mem_maker.cxx

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
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::string production_file;
    production_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_default_min_mult.data";
    
    unsigned int min_multiplicity = 2;
    unsigned int max_gap = 1;
    snemo::digitization::tracker_trigger_mem_maker my_memory_maker;
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1,
			      snemo::digitization::tracker_trigger_mem_maker::MEM_SIZE_5X1,
			      snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1_ALGO_MIN_MULTIPLICITY);
    //my_memory_maker.set_max_gap(max_gap);
    my_memory_maker.set_min_multiplicity(min_multiplicity);
    my_memory_maker.initialize();
    my_memory_maker.store(production_file, "Default A5D1 memory for a minimum multiplicity = 2 for the projection on layers. ");
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



   
