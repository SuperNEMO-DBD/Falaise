// test_tracker_trigger_algorithm_load_from_file.cxx
// Standard libraries :
#include <iostream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// GSL:
#include <bayeux/mygsl/rng.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>

#include <snemo/digitization/trigger_info.h>
#include <snemo/digitization/geiger_ctw_data.h>

#include <snemo/digitization/tracker_trigger_algorithm.h>

int main( int  argc_ , char **argv_  )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;
  bool is_output_path  = false;
  bool is_event_number = false;
  bool is_display      = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") 
      {
	is_input_file = true;
	input_filename = argv_[++iarg];
      } 

    else if (arg == "-op" || arg == "--output-path")
      {
	is_output_path = true;
	output_path = argv_[++iarg];	
      }

    else if (arg == "-n" || arg == "--number")
      {
        is_event_number = true;
	arg_event_number    = atoi(argv_[++iarg]);
      }

    else if (arg == "-d" || arg == "--display")
      {
	is_display = true;
      }
    
    else if (arg =="-h" || arg == "--help")
      {
	is_help = true;
      }

    iarg++;
  }

  if (is_help) 
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "Description : " << std::endl
		<< "This program take a file with a matrix pattern to convert it into geiger ctw data then run the tracker algorithm " << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_tracker_trigger_algorithm_load_from_file [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output path ]    set a path where all files are stored" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "Example : " << std::endl << std::endl
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_tracker_trigger_algorithm_load_from_file --input ${FALAISEBREW_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data -op ${FALAISEBREW_DIGITIZATION_TESTING_DIR}/output_default" 
		<< " --number 5" << std::endl << std::endl
		<< "If no options are set, programs have default values :" << std::endl << std::endl
		<< "input file           = ${FALAISEBREW_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data" << std::endl
		<< "output path          = ${FALAISEBREW_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl
		<< "number of events     = 10" << std::endl << std::endl;
      return 0;
    }

 try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;
    std::clog << "Test tracker trigger algorithm from a loading file !" << std::endl;
    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    std::clog << "manager_config_file = " << manager_config_file << std::endl;
    datatools::fetch_path_with_env(manager_config_file);
    std::clog << "manager_config_file = " << manager_config_file << std::endl;
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
    
    std::string data_filename; 
   
    if(is_input_file){
      datatools::fetch_path_with_env(input_filename);
      data_filename = input_filename;
    }else{
      data_filename = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data";
    }
    datatools::fetch_path_with_env(data_filename);
    std::clog << "data_filename  = " << data_filename << std::endl;

    
    // Number of events :
    int event_number = -1;
    if (is_event_number)  event_number = arg_event_number;
    else                 event_number = 1;

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.initialize();

   // Loading memory from external files :
    std::string mem1 = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem1.conf";
    std::string mem2 = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem2.conf";
    std::string mem3 = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem3.conf";
    std::string mem4 = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem4.conf";
    std::string mem5 = "${FALAISEBREW_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem5.conf";

    datatools::fetch_path_with_env(mem1);
    datatools::fetch_path_with_env(mem2);
    datatools::fetch_path_with_env(mem3);
    datatools::fetch_path_with_env(mem4);
    datatools::fetch_path_with_env(mem5);

    // Tracker algorithm configuration :
    datatools::properties tracker_config;
    tracker_config.store("mem1_file", mem1);
    tracker_config.store("mem2_file", mem2); 
    tracker_config.store("mem3_file", mem3);
    tracker_config.store("mem4_file", mem4);
    tracker_config.store("mem5_file", mem5);

    // Creation and initialization of trigger algorithm :
    snemo::digitization::tracker_trigger_algorithm my_tracker_trigger_algo;
    my_tracker_trigger_algo.set_electronic_mapping(my_e_mapping);
    my_tracker_trigger_algo.initialize(tracker_config);

    // Internal counters :
    int psd_count = 0; // Event counter   

    bool geiger_matrix[snemo::digitization::trigger_info::NSIDES][snemo::digitization::trigger_info::NLAYERS][snemo::digitization::trigger_info::NROWS];
   
    for (int iside = 0; iside < snemo::digitization::trigger_info::NSIDES; iside++)
      {
	for (int jlayer = 0; jlayer < snemo::digitization::trigger_info::NLAYERS; jlayer++)
	  {
	    for (int krow = 0; krow < snemo::digitization::trigger_info::NROWS; krow++)
	      {
		geiger_matrix[iside][jlayer][krow] == false;
	      } // end of krow
	  } // end of jlayer
      } // end of iside

    int line_number = 0;

    int side = 0;

    std::ifstream fin(data_filename.c_str());
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open file '" << data_filename << "'!");

    std::string line;
    while (std::getline(fin, line))
      {
	// Fill matrix :
	int layer = 0;
	if (line_number < 9) 
	  {
	    side = 0;
	    layer = 8 - line_number;
	  }
	else
	  {
	    side = 1;
	    layer = line_number - 9;
	  }
	
	for (int i=0 ; i < line.length(); i++) 
	  { 
	    char my_value = line[i];
	    if (my_value == '0') geiger_matrix[side][layer][i] = false;;
	    if (my_value == '1') geiger_matrix[side][layer][i] = true;
	    
	   
	  }
	
	if (line.empty())
	  {
	    for (int i = 0; i < snemo::digitization::trigger_info::NSIDES; i++)
	      {
		if (i == 0)
		  {
		    for (int j = snemo::digitization::trigger_info::NLAYERS - 1; j >= 0; j--) // Value GEIGER_LAYER_SIZE = 9
		      {
			//std::clog << ' ';
			for (int k = 0; k < snemo::digitization::trigger_info::NROWS; k++)
			  {
		  
			    if (geiger_matrix[i][j][k] ) std::clog << "1";
		  
			    if(!geiger_matrix[i][j][k])  std::clog << "0";	  

			  } // end of row loop
			std::clog<<std::endl;	

		      } // end of layer loop

		  } // end of if == 0

		if (i == 1)
		  {  
		    for (int j = 0; j < snemo::digitization::trigger_info::NLAYERS; j++)
		      {
			//std::clog << ' ' ;
			for (int k = 0; k < snemo::digitization::trigger_info::NROWS; k++)
			  {
		  
			    if (geiger_matrix[i][j][k] ) std::clog << "1";
		  
			    if(!geiger_matrix[i][j][k])  std::clog << "0";	  


			  } // end of row loop
			std::clog<<std::endl;	    
  
		      } // end of layer loop

		  } // end of if i==1

	      } // end of side loop
	    std::clog << std::endl;



	    // process here




	    // reset line number and side number :
	    side = 0;
	    line_number = 0;
	  }

	//std::clog << side << ' ' << line_number << ' ' << layer << ' ' << line << std::endl;
	if (!line.empty()) line_number++;
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
