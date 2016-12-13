// test_calo_signal_generator_driver.cxx
// Standard libraries :
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/temporary_files.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <mygsl/parameter_store.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/asb/calo_signal_generator_driver.h>


int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);  
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::signal::signal_shape_builder'!" << std::endl;

    bool draw = false;

    // Parsing arguments
    int iarg = 1;
    bool is_event_number = false;
    int arg_event_number  = -1;

    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      else if (arg == "-n" || arg == "--number")
	{
	  is_event_number = true;
	  arg_event_number = atoi(argv_[++iarg]);
	}
      iarg++;
    }

    std::string SD_filename = "/home/goliviero/software/Falaise/Falaise-trunk/modules/AnalogSignalBuilder/testing/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";

    // Number of events :
    int event_number = -1;
    if (is_event_number) event_number = arg_event_number;
    else                 event_number = 5;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", event_number);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", SD_filename);
    reader.initialize_standalone (reader_config);
  
    // Event record :
    datatools::things ER;
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    
    unsigned int psd_count = 0; // Event counter
    
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	    mctools::signal::signal_data SSD;
	    
	    if (SD.has_step_hits("calo")) // main calo only for the moment  || SD.has_step_hits("xcalo") || SD.has_step_hits("gveto"))
	      {
		snemo::asb::calo_signal_generator_driver csbd(snemo::asb::calo_signal_generator_driver::MODE_TRIANGLE, "calo");
		datatools::properties csbd_config;
		csbd_config.store("signal_category", "calo");
		// add property signal builder .conf (@falaise/modules/ASBmodule/resources/XXX.conf    version 1.0)
		// calo builder config path
		csbd.initialize(csbd_config);
		csbd.process(SD, SSD);  
		
		std::clog << "SSD size = " << SSD.get_number_of_signals("calo") << std::endl;
		
		datatools::temp_file tmp_file;
		tmp_file.set_remove_at_destroy(true);
		tmp_file.create("/tmp", "test_signal_shape_buider_");
	      }
	    
	  } //end of if has bank label "SD"
	
	ER.clear();
	psd_count++;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      } // end of reader is terminated
    
    DT_LOG_NOTICE(logging, "Reader is terminated");
         
    
    
    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test mygsl::signal::signal_shape_builder' ");
      
    
      usleep(200);
    }
    
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1


    std::clog << "The end." << std::endl;
    
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "Unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}

