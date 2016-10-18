// test_tracker_trigger_algorithm_load_from_file.cxx
// Standard libraries :
#include <iostream>
#include <cstdlib>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
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
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;
  bool is_output_path  = false;
  bool is_display      = false;
  bool is_help         = false;

  std::string input_filename;
  std::string output_path;

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
		<< "$ BuildProducts/bin/falaisedigitizationplugin-test_tracker_trigger_algorithm_load_from_file --input ${FALAISE_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data -op ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default"
		<< " --number 5" << std::endl << std::endl
		<< "If no options are set, programs have default values :" << std::endl << std::endl
		<< "input file           = ${FALAISE_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data" << std::endl
		<< "output path          = ${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/" << std::endl
		<< "number of events     = 10" << std::endl << std::endl;
      return 0;
    }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;
    if (debug) std::clog << "Debug mode" << std::endl;
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
      data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/100_events_tracker_matrix_output.data";
    }
    datatools::fetch_path_with_env(data_filename);
    std::clog << "data_filename  = " << data_filename << std::endl << std::endl;


    if (!is_output_path){
      output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
      datatools::fetch_path_with_env(data_filename);
    }
    std::string output_file = output_path + "Tracker_matrix_response_from_loaded_file.log";
    std::clog << "Output file = " << output_file << std::endl;

    // Electronic mapping :
    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.add_preconstructed_type(snemo::digitization::mapping::GEIGER_CATEGORY_TYPE);
    my_e_mapping.initialize();

    // Loading memory from external files :
    std::string mem1 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem1.conf";
    std::string mem2 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem2.conf";
    std::string mem3 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem3.conf";
    std::string mem4 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem4.conf";
    std::string mem5 = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/mem5.conf";

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
    // Open an output
    bool tmp_file_delete = true;
    std::string path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default";
    std::string prefix = "temp_";
    my_tracker_trigger_algo.set_tmp_file(path, prefix, tmp_file_delete);
    my_tracker_trigger_algo.initialize(tracker_config);

    // Internal counters :
    int psd_count = 0; // Event counter

    bool geiger_matrix[snemo::digitization::trigger_info::NSIDES][snemo::digitization::trigger_info::NLAYERS][snemo::digitization::trigger_info::NROWS];

    for (unsigned int iside = 0; iside < snemo::digitization::trigger_info::NSIDES; iside++)
      {
	for (unsigned int jlayer = 0; jlayer < snemo::digitization::trigger_info::NLAYERS; jlayer++)
	  {
	    for (unsigned int krow = 0; krow < snemo::digitization::trigger_info::NROWS; krow++)
	      {
		geiger_matrix[iside][jlayer][krow] = false;
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
    	// if (psd_count == 0)
    	//   {
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

	for (unsigned int i = 0 ; i < line.length(); i++)
	  {
	    char my_value = line[i];
	    if (my_value == '0') geiger_matrix[side][layer][i] = false;
	    if (my_value == '1') geiger_matrix[side][layer][i] = true;
	  }

	if (line.empty())
	  {
	    for (unsigned int i = 0; i < snemo::digitization::trigger_info::NSIDES; i++)
	      {
		if (i == 0)
		  {
		    for (unsigned int j = snemo::digitization::trigger_info::NLAYERS - 1; j != (unsigned)0-1; j--) // Value GEIGER_LAYER_SIZE = 9
		      {
			for (unsigned int k = 0; k < snemo::digitization::trigger_info::NROWS; k++)
			  {
			    if (geiger_matrix[i][j][k] ) std::clog << "1";
			    if(!geiger_matrix[i][j][k])  std::clog << "0";
			  } // end of row loop
			std::clog<<std::endl;

		      } // end of layer loop

		  } // end of if == 0

		if (i == 1)
		  {
		    for (unsigned int j = 0; j < snemo::digitization::trigger_info::NLAYERS; j++)
		      {
			for (unsigned int k = 0; k < snemo::digitization::trigger_info::NROWS; k++)
			  {
			    if (geiger_matrix[i][j][k]) std::clog << "1";
			    if(!geiger_matrix[i][j][k])  std::clog << "0";
			  } // end of row loop
			std::clog<<std::endl;

		      } // end of layer loop

		  } // end of if i==1

	      } // end of side loop
	    std::clog << std::endl;

	    // process here
	    snemo::digitization::geiger_ctw_data my_geiger_ctw_data;
	    snemo::digitization::geiger_ctw & my_geiger_ctw_0 = my_geiger_ctw_data.add();
	    snemo::digitization::geiger_ctw & my_geiger_ctw_1 = my_geiger_ctw_data.add();
	    snemo::digitization::geiger_ctw & my_geiger_ctw_2 = my_geiger_ctw_data.add();
	    // Take care of the CT 800, only even clocktick are processed
	    uint32_t clocktick_800ns = 4; // Just one clocktick for geiger ctw example value : 4
	    int hit_id = 42;
	    geomtools::geom_id temporary_elec_id;
	    temporary_elec_id.set_type(snemo::digitization::mapping::TRACKER_CONTROL_BOARD_TYPE);
	    temporary_elec_id.set_depth(snemo::digitization::mapping::BOARD_DEPTH);
	    temporary_elec_id.set(snemo::digitization::mapping::RACK_INDEX, snemo::digitization::mapping::GEIGER_RACK_ID);
	    temporary_elec_id.set(snemo::digitization::mapping::CRATE_INDEX, 0);
	    temporary_elec_id.set(snemo::digitization::mapping::BOARD_INDEX, snemo::digitization::mapping::CONTROL_BOARD_ID);

	    my_geiger_ctw_0.set_header(hit_id,
				       temporary_elec_id,
				       clocktick_800ns);

	    temporary_elec_id.set(snemo::digitization::mapping::CRATE_INDEX, 1);
	    my_geiger_ctw_1.set_header(hit_id,
				       temporary_elec_id,
				       clocktick_800ns);

	    temporary_elec_id.set(snemo::digitization::mapping::CRATE_INDEX, 2);
	    my_geiger_ctw_2.set_header(hit_id,
				       temporary_elec_id,
				       clocktick_800ns);

	    for (unsigned int iblock = 0; iblock < 19; iblock++)
	      {
		std::bitset<snemo::digitization::geiger::tp::TP_SIZE> bitset_ctw_0;
		std::bitset<snemo::digitization::geiger::tp::TP_SIZE> bitset_ctw_1;
		std::bitset<snemo::digitization::geiger::tp::TP_SIZE> bitset_ctw_2;

		std::bitset<9> ctw0_side_0_row_0_bitset;
		std::bitset<9> ctw0_side_0_row_1_bitset;
		std::bitset<9> ctw0_side_1_row_0_bitset;
		std::bitset<9> ctw0_side_1_row_1_bitset;

		std::bitset<9> ctw1_side_0_row_0_bitset;
		std::bitset<9> ctw1_side_0_row_1_bitset;
		std::bitset<9> ctw1_side_1_row_0_bitset;
		std::bitset<9> ctw1_side_1_row_1_bitset;

		std::bitset<9> ctw2_side_0_row_0_bitset;
		std::bitset<9> ctw2_side_0_row_1_bitset;
		std::bitset<9> ctw2_side_1_row_0_bitset;
		std::bitset<9> ctw2_side_1_row_1_bitset;

		for (unsigned int jlayer = 0; jlayer < snemo::digitization::trigger_info::NLAYERS; jlayer++)
		  {
		    // CTW 0 :
		    if (geiger_matrix[0][jlayer][iblock * 2] == true)
		      {
			ctw0_side_0_row_0_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[0][jlayer][(iblock * 2) + 1] == true)
		      {
			ctw0_side_0_row_1_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[1][jlayer][iblock * 2] == true)
		      {
			ctw0_side_1_row_0_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[1][jlayer][(iblock * 2) + 1] == true)
		      {
			ctw0_side_1_row_1_bitset.set(jlayer, true);
		      }

		    // CTW 1 :
		    if (iblock < 9)
		      {
			if (geiger_matrix[0][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_0_row_0_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[0][jlayer][38 + (iblock * 2) + 1] == true)
			  {
			    ctw1_side_0_row_1_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[1][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_1_row_0_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[1][jlayer][38 + (iblock * 2) + 1] == true)
			  {
			    ctw1_side_1_row_1_bitset.set(jlayer, true);
			  }
		      }

		    else if (iblock == 9)
		      {
			if (geiger_matrix[0][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_0_row_0_bitset.set(jlayer, true);
			  }

			ctw1_side_0_row_1_bitset.set(jlayer, false);

			if (geiger_matrix[1][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_1_row_0_bitset.set(jlayer, true);
			  }

			ctw1_side_1_row_1_bitset.set(jlayer, false);
		      }

		    else if (iblock > 9)
		      {
			if (geiger_matrix[0][jlayer][38 + (iblock * 2) - 1] == true)
			  {
			    ctw1_side_0_row_0_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[0][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_0_row_1_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[1][jlayer][38 + (iblock * 2) - 1] == true)
			  {
			    ctw1_side_1_row_0_bitset.set(jlayer, true);
			  }
			if (geiger_matrix[1][jlayer][38 + iblock * 2] == true)
			  {
			    ctw1_side_1_row_1_bitset.set(jlayer, true);
			  }
		      }

		    // CTW 2 :

		    if (geiger_matrix[0][jlayer][75 + iblock * 2] == true)
		      {
			ctw2_side_0_row_0_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[0][jlayer][75 + (iblock * 2) + 1] == true)
		      {
			ctw2_side_0_row_1_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[1][jlayer][75 + iblock * 2] == true)
		      {
			ctw2_side_1_row_0_bitset.set(jlayer, true);
		      }
		    if (geiger_matrix[1][jlayer][75 + (iblock * 2) + 1] == true)
		      {
			ctw2_side_1_row_1_bitset.set(jlayer, true);
		      }

		  }// end of jlayer

		for (unsigned int i = 0; i < 9; i++)
		  {
		    if (ctw0_side_0_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_0.set(i, true);
		      }
		    if (ctw0_side_0_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_0.set(i + 9, true);
		      }
		    if (ctw0_side_1_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_0.set(i + 18, true);
		      }
		    if (ctw0_side_1_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_0.set(i + 27, true);
		      }

		    if (ctw1_side_0_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_1.set(i, true);
		      }
		    if (ctw1_side_0_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_1.set(i + 9, true);
		      }
		    if (ctw1_side_1_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_1.set(i + 18, true);
		      }
		    if (ctw1_side_1_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_1.set(i + 27, true);
		      }

		    if (ctw2_side_0_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_2.set(i, true);
		      }
		    if (ctw2_side_0_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_2.set(i + 9, true);
		      }
		    if (ctw2_side_1_row_0_bitset.test(i) == true)
		      {
			bitset_ctw_2.set(i + 18, true);
		      }
		    if (ctw2_side_1_row_1_bitset.test(i) == true)
		      {
			bitset_ctw_2.set(i + 27, true);
		      }
		  }
		// std::clog << "******************************************************"<< std::endl;
		// std::clog << "iblock = " << iblock << std::endl;
		// std::clog << "CTW0:S0:R0 " << ctw0_side_0_row_0_bitset << std::endl;
		// std::clog << "CTW0:S0:R0 " << ctw0_side_0_row_0_bitset << std::endl;
		// std::clog << "CTW0:S0:R1 " << ctw0_side_0_row_1_bitset << std::endl;
		// std::clog << "CTW0:S1:R0 " << ctw0_side_1_row_0_bitset << std::endl;
		// std::clog << bitset_ctw_0 << std::endl << std::endl;

		// std::clog << "CTW1:S1:R1 " << ctw1_side_1_row_1_bitset << std::endl;
		// std::clog << "CTW1:S0:R1 " << ctw1_side_0_row_1_bitset << std::endl;
		// std::clog << "CTW1:S1:R0 " << ctw1_side_1_row_0_bitset << std::endl;
		// std::clog << "CTW1:S1:R1 " << ctw1_side_1_row_1_bitset << std::endl;
		// std::clog << bitset_ctw_1 << std::endl << std::endl;

		// std::clog << "CTW2:S1:R1 " << ctw2_side_1_row_1_bitset << std::endl;
		// std::clog << "CTW2:S0:R1 " << ctw2_side_0_row_1_bitset << std::endl;
		// std::clog << "CTW2:S1:R0 " << ctw2_side_1_row_0_bitset << std::endl;
		// std::clog << "CTW2:S1:R1 " << ctw2_side_1_row_1_bitset << std::endl;
		// std::clog << bitset_ctw_2 << std::endl << std::endl;

		my_geiger_ctw_0.set_55_bits_in_ctw_word(iblock, bitset_ctw_0);
		my_geiger_ctw_1.set_55_bits_in_ctw_word(iblock, bitset_ctw_1);
		my_geiger_ctw_2.set_55_bits_in_ctw_word(iblock, bitset_ctw_2);

	      } // end of iblock

	    my_geiger_ctw_0.tree_dump(std::clog, "My GG CTW 0 : ", "INFO : ");
	    my_geiger_ctw_1.tree_dump(std::clog, "My GG CTW 1 : ", "INFO : ");
	    my_geiger_ctw_2.tree_dump(std::clog, "My GG CTW 2 : ", "INFO : ");
	    //std::clog << "GG CTW DATA SIZE = " << my_geiger_ctw_data.get_geiger_ctws().size() << std::endl;

	    std::vector<snemo::digitization::tracker_trigger_algorithm::tracker_record> my_tracker_records;
	    my_tracker_trigger_algo.grab_tmp_file().out() << "Event = " << psd_count << std::endl;
	    my_tracker_trigger_algo.process(my_geiger_ctw_data,
					    my_tracker_records);

	    std::clog << "Tracker records size = " << my_tracker_records.size() << std::endl;
	    my_tracker_records[0].display();

	    std::cout << "next event ? [enter for yes]" << std::endl;
	    std::cin.get();

	    // reset line number and side number :
	    side = 0;
	    line_number = 0;
	    psd_count++;

	  } // end of line.empty()
	    //std::clog << psd_count << std::endl;
      	//} // end of if psd count == 0

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

  falaise::terminate();
  return error_code;
}
