// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

// This project:
#include <fecom/mock_hc2cd_module.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

// Falaise:
#include <falaise/falaise.h>

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
		<< "-d    [ --display ]        display things for debug" << std::endl << std::endl;
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

    std::string output_filename = output_path + "output_hc2cd.xml"; //data.bz2";
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);

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

    std::string manager_config_file;
    manager_config_file = "~/software/Falaise/Falaise-install/share/Falaise-3.0.0/resources/config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
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

    std::size_t event_counter = 0;
    int event_number = 10;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", event_number);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", input_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump(std::clog, "Half Commissiong Raw Data reader module");

    // Event writer :
    dpp::output_module writer;
    datatools::properties writer_config;
    writer_config.store ("logging.priority", "debug");
    writer_config.store ("files.mode", "single");
    writer_config.store ("files.single.filename", output_filename);
    writer.initialize_standalone(writer_config);

    // Event record :
    datatools::things ER;

    // Simulated Data "SD" bank label :
    std::string HCRD_bank_label = "HCRD";
    // Calibrated Data "CD" bank label :
    std::string CD_bank_label = "CD";

    fecom::mock_hc2cd_module hc2cd_module;
    hc2cd_module.set_channel_mapping(my_channel_mapping);
    hc2cd_module.set_geom_manager(my_manager);
    hc2cd_module.initialize_simple();

    DT_LOG_INFORMATION(logging, "Read and store raw hits in object...");
    DT_LOG_INFORMATION(logging, "...");

    while (!reader.is_terminated())
      {
	reader.process(ER);
	DT_LOG_DEBUG(logging, "Event counter = " << event_counter);

	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(HCRD_bank_label) && ER.is_a<fecom::commissioning_event>(HCRD_bank_label))
	  {
	    DT_LOG_DEBUG(logging, "Has HCRD bank label");
	    hc2cd_module.process(ER);

	    writer.process(ER);
	  }
	event_counter++;
      }

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
