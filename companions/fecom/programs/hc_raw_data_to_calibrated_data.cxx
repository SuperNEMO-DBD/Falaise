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
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  // Parsing arguments
  int iarg = 1;
  std::string input_filename = "";
  std::string output_filename = "";
  std::string output_path = "";
  std::string input_tracker_mapping_file = "";
  std::string input_calo_mapping_file = "";
  int32_t     first_event_number = -0;
  std::size_t min_nb_calo    = 0;
  std::size_t min_nb_tracker = 0;
  std::size_t max_events = 0;

  bool is_display  = false;
  bool is_help     = false;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];

    if (arg == "-i" || arg == "--input") {
      input_filename = argv_[++iarg];
    }

    else if (arg == "-o" || arg == "--output") {
      output_filename = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "-cm" || arg == "--calo-map") {
      input_calo_mapping_file = argv_[++iarg];
    }

    else if (arg == "-tm" || arg == "--tracker-map") {
      input_tracker_mapping_file = argv_[++iarg];
    }

    else if (arg == "-e" || arg == "--event-number") {
      first_event_number = boost::lexical_cast<std::size_t>(argv_[++iarg]);
    }

    else if (arg == "-C" || arg == "--min-calo-hits") {
      min_nb_calo = boost::lexical_cast<std::size_t>(argv_[++iarg]);
    }

    else if (arg == "-T" || arg == "--min-tracker-hits") {
      min_nb_tracker = boost::lexical_cast<std::size_t>(argv_[++iarg]);
    }

    else if (arg == "-M" || arg == "--max-events") {
      max_events = boost::lexical_cast<std::size_t>(argv_[++iarg]);
    }

    else if (arg == "-d" || arg == "--display") {
      is_display = true;
    }

    else if (arg == "-h" || arg == "--help") {
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
		<< "$ BuildProducts/fecom_programs/hc_raw_data_to_calibrated_data [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h         [ --help ]        produce help message" << std::endl
		<< "-i         [ --input ]       set an input file" << std::endl
		<< "-op        [ --output path ] set a path where all files are store" << std::endl
		<< "-d    [ --display ]        display things for debug" << std::endl << std::endl;
      return EXIT_SUCCESS;
    }


  datatools::logger::priority logging;
  if (is_display) logging = datatools::logger::PRIO_DEBUG;
  else logging = datatools::logger::PRIO_INFORMATION;

  try {
    DT_LOG_INFORMATION(logging, "Entering hc_raw_data_to_calibrated.cxx...");

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

    if (output_filename.empty()) {
      output_filename = output_path + "output_hc2cd.data.bz2";
    }
    datatools::fetch_path_with_env(output_filename);

    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);

    // Build all tracker hit after the build of all commissioning event :
    if (input_tracker_mapping_file.empty()) {
      input_tracker_mapping_file = input_path + "/" + "mapping_tracker.csv";
    }
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    if (input_calo_mapping_file.empty()) {
      input_calo_mapping_file = input_path + "/" + "mapping_calo.csv";
    }
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
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

    uint64_t event_number = first_event_number;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", static_cast<int>(max_events));
    reader_config.store ("files.mode", "single");
    reader_config.store_path("files.single.filename", input_filename);
    reader.initialize_standalone (reader_config);
    // reader.tree_dump(std::clog, "Half Commissiong Raw Data reader module");

    // Event writer :
    dpp::output_module writer;
    datatools::properties writer_config;
    writer_config.store ("logging.priority", "debug");
    writer_config.store ("files.mode", "single");
    writer_config.store ("files.single.filename", output_filename);
    writer.initialize_standalone(writer_config);
    // writer.tree_dump(std::clog, "Half Commissiong Raw Data writer module");

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
	DT_LOG_DEBUG(logging, "Event number = " << event_number);
	reader.process(ER);

	// A plain `fecom::commissioning' object is stored here :
	if (ER.has(HCRD_bank_label) && ER.is_a<fecom::commissioning_event>(HCRD_bank_label))
	  {
	    // DT_LOG_DEBUG(logging, "Has HCRD bank label");
	    // ER.tree_dump(std::clog, "Things tree dump :");
	    // const fecom::commissioning_event & CE = ER.get<fecom::commissioning_event>(HCRD_bank_label);
	    // CE.tree_dump(std::clog, "CE Bank tree dump :");

	    hc2cd_module.process(ER);

	    ER.remove(HCRD_bank_label);
	    // ER.tree_dump(std::clog, "Things after removal :");

	    const snemo::datamodel::calibrated_data & CD = ER.get<snemo::datamodel::calibrated_data>(CD_bank_label);
	    bool save_it = true;
	    if (CD.calibrated_calorimeter_hits().size() < min_nb_calo) {
	      save_it = false;
	    }
	    if (CD.calibrated_tracker_hits().size() < min_nb_tracker) {
	      save_it = false;
	    }
	    if (save_it) {
	      std::clog << "Calo || tracker || calo + tracker event #" << event_number << std::endl;
	      writer.process(ER);
	    }

	  }
	if (max_events > 0 && (event_number >= max_events)) {
          DT_LOG_INFORMATION(logging, "Maximum number of events is reached.");
          break;
        }
	ER.clear();
	event_number++;
      }

    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();

    DT_LOG_INFORMATION(logging, "Exiting main_decoder_serializer.cxx...");
    DT_LOG_INFORMATION(logging, "EXIT_STATUS : SUCCESS");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
