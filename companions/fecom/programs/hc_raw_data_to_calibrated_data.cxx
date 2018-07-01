// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

// Third party:
// - Boost:
#include <boost/program_options.hpp>
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

// This project:
#include <fecom/mock_hc2cd_module.hpp>

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging;
  try {

    bool        is_debug       = false;
    std::string input_filename = "";
    std::string output_path = "";
    std::size_t max_events     = 0;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("debug,d", "debug mode")
      ("input,i",
       po::value<std::string>(& input_filename),
       "set an input file")
      ("output,o",
       po::value<std::string>(& output_path),
       "set the output path")
      ("max_events,M",
       po::value<std::size_t>(& max_events)->default_value(10),
       "set the maximum number of events")
      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }

    // Use command ne arguments :
    if (vm.count("debug")) {
      is_debug = true;
    }

    if (is_debug) logging = datatools::logger::PRIO_DEBUG;

    DT_LOG_INFORMATION(logging, "Entering HCRD2CD : hc_raw_data_to_calibrated.cxx...");

    // Set the input file :
    DT_THROW_IF(input_filename.empty(), std::logic_error, "Missing input file!");
    datatools::fetch_path_with_env(input_filename);
    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);

    // Default output path in input path :
    if (output_path.empty()) {
      output_path = "/tmp/";
      DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    std::string output_filename = output_path + "output_hc2cd.brio";
    datatools::fetch_path_with_env(output_filename);
    DT_LOG_INFORMATION(logging, "HC2CD output file :" + output_filename);

    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories")) {
      manager_config.erase ("mapping.excluded_categories");
    }
    my_manager.initialize (manager_config);

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

    // Calo + tracker Events writer :
    std::string output_filename_ct = output_path + "output_hc2cd_calo_tracker_events.brio";
    datatools::fetch_path_with_env(output_filename_ct);
    DT_LOG_INFORMATION(logging, "HC2CD calo + tracker events output file :" + output_filename_ct);
    dpp::output_module ct_writer;
    datatools::properties ct_writer_config;
    ct_writer_config.store ("logging.priority", "debug");
    ct_writer_config.store ("files.mode", "single");
    ct_writer_config.store ("files.single.filename", output_filename_ct);
    ct_writer.initialize_standalone(ct_writer_config);

    // Event record :
    datatools::things ER;

    // Simulated Data "SD" bank label :
    std::string HCRD_bank_label = "HCRD";
    // Calibrated Data "CD" bank label :
    std::string CD_bank_label = "CD";

    fecom::mock_hc2cd_module hc2cd_module;
    // hc2cd_module.set_logging_priority(logging);
    hc2cd_module.set_geom_manager(my_manager);
    hc2cd_module.initialize_simple();

    DT_LOG_INFORMATION(logging, "Read HCRD bank, calibrate it and store in the 'CD' bank...");
    DT_LOG_INFORMATION(logging, "...");

    uint64_t event_number = 0;
    uint64_t modulo = 10000;

    while (!reader.is_terminated())
      {
	if (event_number % modulo == 0) {
	  DT_LOG_DEBUG(logging, "Event number = " << event_number);
	}
	reader.process(ER);

	// A plain `fecom::commissioning' object is stored here :
	if (ER.has(HCRD_bank_label) && ER.is_a<fecom::commissioning_event>(HCRD_bank_label))
	  {
	    hc2cd_module.process(ER);
	    // Remove the bank in order to see the CD bank in flvisualize:
	    ER.remove(HCRD_bank_label);


	    if (ER.has(CD_bank_label)) {
	      const snemo::datamodel::calibrated_data & CD = ER.get<snemo::datamodel::calibrated_data>(CD_bank_label);
	      if (CD.has_calibrated_calorimeter_hits() || CD.has_calibrated_tracker_hits()) {
		// CD.tree_dump(std::clog, "Calibrated data");

		// All events are writed (calo only and calo + tracker)
		writer.process(ER);

		// only calo + tracker events are writed
		if (CD.has_calibrated_calorimeter_hits() && CD.has_calibrated_tracker_hits())
		  {
		    ct_writer.process(ER);
		  }
	      } // end of has calib calo or tracker hits
	    } // has CD bank label
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
    DT_LOG_INFORMATION(logging, "The end...");

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    DT_LOG_FATAL(logging, "EXIT_STATUS : FAILURE");
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}
