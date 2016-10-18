// Standard library:
#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/simulated_data_reader.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  int iarg = 1;
  bool is_input_file = false;
  std::string input_filename;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename=argv_[++iarg];
    } else if (arg == "-f" || arg == "--filename") {
      input_filename=argv_[++iarg];
    }
    iarg++;
  }
  


  try {
    std::clog << "Test program for read calibrated datas !" << std::endl;
    std::string pipeline_simulated_data_filename;
    std::string SD_bank_label = "SD";
    
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
    }
    
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    // reader_config.store ("max_files", 100);
    reader_config.store ("max_record_total", 10000);
    // reader_config.store ("max_record_per_file", 10000);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    datatools::things ER;

    int psd_count = 0;
    while (!reader.is_terminated()) {
      reader.process(ER);
      // A plain `mctools::simulated_data' object is stored here :
      if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
        // Access to the "SD" bank with a stored `mctools::simulated_data' :
	const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	SD.tree_dump(std::cerr);
	std::clog << "DEBUG : has SD_BANK" << std::endl;
	std::clog << "DEBUG : has data " << SD.has_data() << std::endl; 
	std::clog << "DEBUG : has data " << SD.get_vertex() << std::endl; 
	std::clog << "DEBUG : has step_hits " << SD.has_step_hits("gg") << std::endl; 
	if( SD.has_step_hits("gg")){
	  size_t number = SD.get_number_of_step_hits("gg");
	  for (unsigned int i = 0; i < number; i++)
	    {
	      const mctools::base_step_hit & gg_hit = SD.get_step_hit("gg", i);
	      std::clog << "DEBUG : hit id = " << gg_hit.get_hit_id() << ": GID = " << gg_hit.get_geom_id() << std::endl;
	    }
	}
      }     
      // CF README.RST pour display graphique avec loader de manager.conf
      // -> /home/guillaume/data/Bayeux/Bayeux-trunk/source/bxmctools/examples/ex00
      ER.clear();

      psd_count++;
      std::clog << "DEBUG : psd count " << psd_count << std::endl;
      DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
    }
    std::clog << "Number of processed simulated data records: " << psd_count << std::endl;

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
