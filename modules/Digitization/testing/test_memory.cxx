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
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::test_memory' !" << std::endl;
    
    std::string memory_file;
    memory_file = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A4_D2_memory.data";
    datatools::fetch_path_with_env(memory_file);
    std::string tmp_file;
    tmp_file = "/tmp/test.txt";
    snemo::digitization::memory<4,2> my_A4_D2_mem;
    my_A4_D2_mem.load_from_file(memory_file);

    snemo::digitization::memory<4,2> my_A4_D2_mem_test;
    std::bitset<4> address_bitset;
    std::bitset<2> data_bitset = 01;
    for (int i = 0; i < 16; i++)
      {
	address_bitset = i;
	my_A4_D2_mem_test.push(address_bitset, data_bitset);
      }
    // my_A4_D2_mem_test.memory_map_display();
    // my_A4_D2_mem_test.store_to_file(tmp_file, "This the A4D2 memory test");
    
    unsigned int min_multiplicity = 2;
    unsigned int max_gap = 1;
    snemo::digitization::tracker_trigger_mem_maker my_memory_maker;
    my_memory_maker.configure(snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1,
			      snemo::digitization::tracker_trigger_mem_maker::MEM_SIZE_6X1,
			      snemo::digitization::tracker_trigger_mem_maker::MEM_LVL0_LVL1_ALGO_MAX_GAP);
    my_memory_maker.set_max_gap(max_gap);
    my_memory_maker.initialize();
    my_memory_maker.store(tmp_file);
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



   
