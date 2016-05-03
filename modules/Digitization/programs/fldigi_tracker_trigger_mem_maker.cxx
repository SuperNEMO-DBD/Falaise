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
    std::clog << "TEST : testing program tracker mem maker " << std::endl;
    std::string mem1_file;
    mem1_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem1.conf";
    datatools::fetch_path_with_env(mem1_file);

    std::string mem2_file;
    mem2_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem2.conf";
    datatools::fetch_path_with_env(mem2_file);

    std::string mem3_file;
    mem3_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem3.conf";
    datatools::fetch_path_with_env(mem3_file);

    std::string mem4_file;
    mem4_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem4.conf";
    datatools::fetch_path_with_env(mem4_file);

    std::string mem5_file;
    mem5_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem5.conf";
    datatools::fetch_path_with_env(mem5_file);

    snemo::digitization::tracker_trigger_mem_maker my_memory_maker;
    
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM1);
    my_memory_maker.initialize();
    my_memory_maker.store_to_file(mem1_file, "Default mem1.conf : Sliding Zone Vertical memory");
    
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM2);
    my_memory_maker.initialize();
    my_memory_maker.store_to_file(mem2_file, "Default mem2.conf : Sliding Zone Horizontal memory");
    
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM3);
    my_memory_maker.initialize();
    my_memory_maker.store_to_file(mem3_file, "Default mem3.conf : Zone Vertical memory");
    
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM4);
    my_memory_maker.initialize();
    my_memory_maker.store_to_file(mem4_file, "Default mem4.conf : Zone Horizontal memory");

    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM5);
    my_memory_maker.initialize();
    my_memory_maker.store_to_file(mem5_file, "Default mem5.conf : Zone Vertical for horizontal memory");
    
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
