// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/channel_mapping.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::set`` template class :
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>

struct hit_list {

  std::list<std::shared_ptr<fecom::base_hit>> hits;

  fecom::calo_hit * chit_ptr = nullptr;
  fecom::tracker_channel_hit * tchit_ptr = nullptr;

  void make_new_calo_hit()
  {
    std::shared_ptr<fecom::base_hit> sh(new fecom::calo_hit);
    hits.push_back(sh);
    chit_ptr = dynamic_cast<fecom::calo_hit *>(hits.back().get());
    chit_ptr->tdc_cell_step_ns = 0.390625;
  }

  void make_new_tracker_channel_hit()
  {
    std::shared_ptr<fecom::base_hit> sh(new fecom::tracker_channel_hit);
    hits.push_back(sh);
    tchit_ptr = dynamic_cast<fecom::tracker_channel_hit *>(hits.back().get());
  }

  fecom::calo_hit & chit()
  {
    return *chit_ptr;
  }
  fecom::tracker_channel_hit & tchit()
  {
    return *tchit_ptr;
  }

  static bool comparetimestamp(const std::shared_ptr<fecom::base_hit> & a,
			       const std::shared_ptr<fecom::base_hit> & b)
  {
    const fecom::base_hit * hit_1 = a.get();
    const fecom::base_hit * hit_2 = b.get();

    return hit_1->get_timestamp() < hit_2->get_timestamp();
  }

  void sort()
  {
    hits.sort(comparetimestamp);
  }

};

int main(int argc_, char ** argv_)
{
  // Parsing arguments
  int iarg = 1;
  std::string input_filename = "";
  std::string output_path = "";
  bool is_display  = false;
  bool is_help     = false;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];

    if (arg == "-i" || arg == "--input") {
      input_filename = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "-d" || arg == "--display") {
      is_display = true;
    }

    else if (arg =="-h" || arg == "--help") {
      is_help = true;
    }

    else {
      std::cerr << "Unrecognised argument" << std::endl;
      is_help = true;
    }

    iarg++;
  }

  if (is_help)
    {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/fecom_programs/fecom_main_decoder_serializer [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h         [ --help ]        produce help message" << std::endl
		<< "-i         [ --input ]       set an input file" << std::endl
		<< "-op        [ --output path ] set a path where all files are store" << std::endl
		<< "-d         [ --display ]     display things for debug" << std::endl << std::endl;
      return 0;
    }

  datatools::logger::priority logging;
  if (is_display) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering main_decoder_serializer.cxx...");

    // Set the input file from Jihanne :
    if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/calo_fake_tracker_hits_1.dat";
    datatools::fetch_path_with_env(input_filename);
    std::string input_path = input_filename;
    std::size_t found = input_path.find_last_of("/");
    input_path.erase(found+1, input_path.size());

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);
    DT_LOG_INFORMATION(logging, "Input path          : " + input_path);

    // Default output path in input path :
    if (output_path.empty())
      {
	output_path = input_path;
	DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
      }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    std::string output_filename = output_path + "output_decode_sort.xml"; //data.bz2";
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);
    datatools::data_writer serializer(output_filename,
				      datatools::using_multiple_archives);

    // Input file (from Jihanne) hits reader :
    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_FATAL);
    reader.set_input_filename(input_filename);
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    DT_LOG_INFORMATION(logging, "File [" << input_filename << "]   header : ");
    header.tree_dump(std::clog, "Run header:");

    // Build all tracker hit after the build of all commissioning event :
    std::string input_tracker_mapping_file = input_path + "mapping_tracker.csv";
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    std::string input_calo_mapping_file = input_path + "mapping_calo.csv";
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    std::size_t hit_counter = 0;
    //    std::size_t event_serialized = 0;
    hit_list hl;
    hl.make_new_calo_hit();
    hl.make_new_tracker_channel_hit();
    // List -> [!C][!T]
    //           ^   ^

    DT_LOG_INFORMATION(logging, "Read and store raw hits in object...");
    DT_LOG_INFORMATION(logging, "...");

    while(reader.has_next_hit()) {
      DT_LOG_DEBUG(logging, "Hit counter = " << hit_counter);
      reader.load_next_hit(hl.chit(), hl.tchit());

      std::string valid = "none";

      if (hl.chit().is_valid()) {
	// hl.chit().tree_dump(std::clog, "Calo hit is valid :");
	valid = "calo";
	hl.make_new_calo_hit();
	// List -> [C][!T][!C]
	//              ^   ^
      }

      // Do not add tracker channel hit with a null value in the register :
      if (hl.tchit().is_valid() && hl.tchit().timestamp_time_ns != 0 && hl.tchit().timestamp_time_ns != 0) {
	// tchit.tree_dump(std::clog, "Tracker channel hit is valid :");
	valid = "tracker";
	hl.make_new_tracker_channel_hit();
        // List -> [!C][T][!T]
	//           ^      ^
      }

      hit_counter++;
    } // end of while reader
    // List -> [C][T][T][T][C][C][T][T][!C][T][!T]

    // Remove unvalid calo / tracker hits
    std::size_t remove_counter = 0;
    for (auto it_list = hl.hits.begin(); it_list != hl.hits.end();) {
      if (!it_list->get()->is_valid() || it_list->get()->get_timestamp() == 0) {
	it_list = hl.hits.erase(it_list);
	remove_counter++;
      }
      else
        ++it_list;
    }
    std::clog << "Removed elements = " << remove_counter << std::endl;
    std::clog << "Size of the list = " << hl.hits.size() << std::endl;

    DT_LOG_INFORMATION(logging, "Sorting...");
    hl.sort();
    DT_LOG_INFORMATION(logging, "Sorting success...");

    DT_LOG_INFORMATION(logging, "Begin hits serialization...");
    std::size_t serial_counter = 0;
    for (auto it_list = hl.hits.begin();
    	 it_list != hl.hits.end();
    	 it_list++)
      {
    	// it_list->get()->tree_dump(std::clog, "Hit number #" + std::to_string(serial_counter));
	// serializer.store(*it_list->get());

	fecom::calo_hit * chit_to_serialize = nullptr;
	fecom::tracker_channel_hit * tchit_to_serialize = nullptr;

	if (it_list->get()->hitmode == fecom::base_hit::SIG_CALORIMETER)
	  {
	    chit_to_serialize = dynamic_cast<fecom::calo_hit *>(it_list->get());
	    serializer.store(*chit_to_serialize);
	  }
	else if (it_list->get()->hitmode == fecom::base_hit::SIG_TRACKER)
	  {
	    tchit_to_serialize = dynamic_cast<fecom::tracker_channel_hit *>(it_list->get());
	    serializer.store(*tchit_to_serialize);
	  }

	serial_counter++;
      }

    DT_LOG_INFORMATION(logging, "Number of commissioning hits serialized : " + std::to_string(serial_counter));

    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();

    DT_LOG_INFORMATION(logging, "Exiting main_decoder_serializer.cxx...");
    DT_LOG_INFORMATION(logging, "EXIT_STATUS : SUCCESS");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
