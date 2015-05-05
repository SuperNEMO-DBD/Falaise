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

int main( int  argc_ , char ** argv_  )
{
 FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::test_memory' !" << std::endl;
    
    std::string memory_file;
    memory_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A4_D2_memory.data";
    datatools::fetch_path_with_env(memory_file);
    //memory_file = "/home/guillaume/data/Falaise/Falaise-trunk/modules/Digitization/testing/config/trigger/tracker/A4_D2_memory.data";
    
    snemo::digitization::memory<4,2> my_A4_D2_mem;
    my_A4_D2_mem.load_from_file(memory_file);


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





   
