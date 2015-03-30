//test_sd_to_calo_ctw_process.cxx

// Standard libraries :
#include <iostream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/sd_to_calo_tp_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::sd_to_calo_ctw_process' !" << std::endl;
 
    std::string manager_config_file;
    
    manager_config_file = "~/data/my_falaise/config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
   
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_manager.initialize (manager_config);

    std::string pipeline_simulated_data_filename;
    std::string SD_bank_label = "SD";

    pipeline_simulated_data_filename = "/home/guillaume/data/my_falaise/outputs/Se82_0nubb-source_strips_bulk_SD.brio";
    
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 10);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    datatools::things ER;
    snemo::digitization::ID_convertor my_convertor;
    my_convertor.set_geo_manager(my_manager);
    my_convertor.set_module_number(0);
    my_convertor.initialize();

    int32_t clocktick_reference = 12;
    snemo::digitization::sd_to_calo_tp_algo sd_2_calo_tp;
    sd_2_calo_tp.initialize(clocktick_reference, my_convertor);

    snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw;
    calo_tp_2_ctw.initialize();

    snemo::digitization::calo_tp_data my_calo_tp_data;
    snemo::digitization::calo_ctw_data my_calo_ctw_data;

    int psd_count = 0;
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);

	    if( SD.has_step_hits("calo"))
	      {		  
		sd_2_calo_tp.process(SD, my_calo_tp_data);
		my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
	      }
	  }  
	    my_calo_tp_data.lock_tps();
	    calo_tp_2_ctw.process(my_calo_tp_data, my_calo_ctw_data);
	    my_calo_ctw_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
	    my_calo_tp_data.reset();
   
	// CF README.RST pour display graphique avec loader de manager.conf
	// -> /home/guillaume/data/Bayeux/Bayeux-trunk/source/bxmctools/examples/ex00
	ER.clear();
	//my_calo_tp_data.lock_tps();
	psd_count++;
	std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
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
