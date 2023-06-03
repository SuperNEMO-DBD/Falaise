
// Third Party
// - Boost
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
// - Bayeux
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/library_loader.h>
#include <bayeux/dpp/input_module.h>
#include <bayeux/dpp/module_manager.h>
#include <bayeux/dpp/i_data_source.h>

// This Project
#include <falaise/exitcodes.h>
#include <falaise/resource.h>
#include <falaise/falaise.h>

/// Configuration parameters
struct FlUddScanParams
{
  /// Logging priority threshold (switch '-V debug' in the command line)  
  datatools::logger::priority logLevel = datatools::logger::PRIO_ERROR;
  /// Main configuration file for plugins loader 
  datatools::multi_properties userLibConfig;
  /// Input metadata file
  std::string inputMetadataFile;
  /// Input data file for the input module (switch '-i file' in the command line)  
  std::string inputFile;
  /// Metadata imported from the input
  datatools::multi_properties inputMetadata;
  /// First event number to be processed (switch '-F 1' in the command line)
  std::size_t firstProcessedEventNumber = 1;
  /// Maximum number of events (switch '-N 10' in the command line)
  std::size_t maxNumberOfProcessedEvents = 0;
  /// Flag to print UDD data (switch '-P' in the command line)
  bool printUddData = false;
           
  /// Build a default arguments set:
  static FlUddScanParams makeDefault();
};

falaise::exit_code do_pipeline(const FlUddScanParams & uddScanParams_);

int main(int argc_, char* argv_[])
{
  int exitCode = EXIT_SUCCESS;
  falaise::initialize(argc_, argv_); // Load Falaise resources...
  FlUddScanParams uddScanParams = FlUddScanParams::makeDefault();

  std::string verbosityLabel;
  namespace bpo = boost::program_options;
  bpo::options_description optDesc("Options");
  optDesc.add_options()
    // ("help,h", "print this help message")
    ("verbosity,V",
     bpo::value<std::string>(&verbosityLabel)
     ->value_name("level"),
     "set the verbosity level")
    ("first-event-number,F",
     bpo::value<std::size_t>(&uddScanParams.firstProcessedEventNumber)
     ->default_value(0)
     ->value_name("N"),
     "first event number to be processed")
    ("max-number-events,N",
     bpo::value<std::size_t>(&uddScanParams.maxNumberOfProcessedEvents)
     ->default_value(0)
     ->value_name("N"),
     "maximum number of processed events")
    ("input-file,i",
     bpo::value<std::string>(&uddScanParams.inputFile)
     ->required()
     ->value_name("file"),
     "file from which to read input data (simulation, real)")
    ("print-udd,P",
     bpo::value<bool>(&uddScanParams.printUddData)
     ->default_value(false)
     ->zero_tokens(),
     "print UDD data in the terminal")
    ;
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc_, argv_, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option & e) {
    std::cerr << "[error] Required option : " << e.what() << "!" <<  '\n';
    // // We need to handle help/version even if required_option thrown
    // if ((vMap.count("help") == 0u) && (vMap.count("version") == 0u) &&
    // 	(vMap.count("help-module-list") == 0u) && (vMap.count("help-module") == 0u) &&
    // 	(vMap.count("help-pipeline-list") == 0u)) {
    // }
    exitCode = EXIT_FAILURE;
  } catch (const std::exception& e) {
    std::cerr << "[error] Option error : " << e.what() << "!" <<  '\n';
    exitCode = EXIT_FAILURE;
  }

  if (exitCode == EXIT_SUCCESS) {
    if (vMap.count("verbosity") != 0u) {
      uddScanParams.logLevel = datatools::logger::get_priority(verbosityLabel);
      if (uddScanParams.logLevel == datatools::logger::PRIO_UNDEFINED) {
	std::cerr << "[error] Invalid verbosity level '" << verbosityLabel << "'!" <<  '\n';
	exitCode = EXIT_FAILURE;
      }
    }
  }
  
  if (exitCode == EXIT_SUCCESS) {
    falaise::exit_code code = do_pipeline(uddScanParams);
    if (code != falaise::EXIT_OK) {
      exitCode = EXIT_FAILURE;
    }
  }
  
  falaise::terminate(); // Unload Falaise resources...
  return exitCode;
}

// static
FlUddScanParams FlUddScanParams::makeDefault()
{
  FlUddScanParams params;
  params.logLevel = datatools::logger::PRIO_ERROR;
  params.userLibConfig.set_key_label("name");
  params.userLibConfig.set_meta_label("filename");
  params.firstProcessedEventNumber = 0;
  params.maxNumberOfProcessedEvents = 0;
  params.printUddData = false;
  return params;
}

falaise::exit_code do_pipeline(const FlUddScanParams & uddScanParams_)
{
  falaise::exit_code code = falaise::EXIT_OK;
  datatools::logger::priority logLevel = uddScanParams_.logLevel;
  // logLevel = datatools::logger::PRIO_DEBUG;
  DT_LOG_DEBUG(uddScanParams_.logLevel, "Entering UDD scan pipeline");
  try {

    datatools::library_loader libLoader(uddScanParams_.userLibConfig);
    DT_LOG_DEBUG(logLevel, "test");
    std::string pluginPath = falaise::get_plugin_dir();
    DT_LOG_DEBUG(logLevel, "pluginPath=" << pluginPath);
    libLoader.load("UddAnalysis", pluginPath);
    if (datatools::logger::is_debug(logLevel)) {
      libLoader.print(std::cerr);
    }
    
    // Input module:
    DT_LOG_DEBUG(logLevel, "Input module...");
    dpp::input_module uddInput;
    uddInput.set_name("FlUddInput");
    uddInput.set_logging_priority(uddScanParams_.logLevel);
    std::string inFile(uddScanParams_.inputFile);
    datatools::fetch_path_with_env(inFile);
    if (not boost::filesystem::exists(inFile)) {
      DT_LOG_FATAL(logLevel, "Input file '" << inFile << "' does not exist!");
      return falaise::EXIT_UNAVAILABLE;
    }
    uddInput.set_single_input_file(inFile);
    uddInput.initialize_simple();
    DT_LOG_DEBUG(logLevel,
		 "Number of entries  = " << uddInput.get_source().get_number_of_entries());
    DT_LOG_DEBUG(logLevel,
		 "Number of metadata = " << uddInput.get_source().get_number_of_metadata());

    DT_LOG_DEBUG(logLevel, "Module manager...");
    dpp::module_manager moduleManager;
    moduleManager.set_logging_priority(logLevel);

    DT_LOG_DEBUG(logLevel, "UDD analysis module...");
    std::string uddModuleName = "uddScan";
    datatools::properties uddAnalysisConfig;
    if (uddScanParams_.printUddData) {
      uddAnalysisConfig.store_flag("print_udd_data");
    }
    moduleManager.load_module(uddModuleName,
			      "UddAnalysis",
			      uddAnalysisConfig);
    moduleManager.initialize_simple();

    // Select pipeline
    DT_LOG_DEBUG(logLevel, "Pipeline...");
    dpp::base_module * pipeline = nullptr;
    try {
      pipeline = &(moduleManager.grab(uddModuleName));
      DT_LOG_DEBUG(logLevel, "pipeline=" << pipeline);
      DT_LOG_DEBUG(logLevel, "pipeline.name=" << pipeline->get_name());
    } catch (std::exception & e) {
      DT_LOG_FATAL(logLevel, "Failed to initialize pipeline : " << e.what());
      return falaise::EXIT_UNAVAILABLE;
    }
    
    DT_LOG_DEBUG(logLevel, "Begin event loop");
    datatools::things eventRecord; // Event record to be loaded
    std::size_t inputEventCounter = 0;
    std::size_t processedEventCounter = 0;
    while (true) {
      eventRecord.clear(); // Clear the current event record
      // Check the status of theinput module:
      if (uddInput.is_terminated()) {
	DT_LOG_DEBUG(logLevel, "Input module is terminated");
	break;
      }
      // Load the next event record:
      if (uddInput.process(eventRecord) != dpp::base_module::PROCESS_OK) {
	DT_LOG_FATAL(logLevel, "Failed to read data record from input source");
	code = falaise::EXIT_UNAVAILABLE;
	break;
      }
      inputEventCounter++;
      DT_LOG_DEBUG(logLevel, "Event #" << inputEventCounter);
      DT_LOG_DEBUG(logLevel, "First processed event #" << uddScanParams_.firstProcessedEventNumber);
      if (inputEventCounter < uddScanParams_.firstProcessedEventNumber) {
	// Skip this event record:
	continue;
      }
      // Process the event record through the active module:
      dpp::base_module::process_status pStatus = pipeline->process(eventRecord);
      processedEventCounter++;
      DT_THROW_IF(pStatus == dpp::base_module::PROCESS_INVALID,
		  std::logic_error,
		  "Module '" << pipeline->get_name() << "' did not return a valid processing status!");
      // Some checks:
      if (pStatus == dpp::base_module::PROCESS_FATAL) {
	code = falaise::EXIT_UNAVAILABLE;
	break;
      }
      if (pStatus == dpp::base_module::PROCESS_ERROR) {
	code = falaise::EXIT_UNAVAILABLE;
	break;
      }
      if (pStatus == dpp::base_module::PROCESS_ERROR_STOP) {
	code = falaise::EXIT_UNAVAILABLE;
	break;
      }
      if (pStatus == dpp::base_module::PROCESS_STOP) {
	// STOP means the current event record should not be processed anymore nor saved
	// but the loop can continue with other loaded event records
	continue;
      }
      DT_LOG_DEBUG(logLevel, "Number of processed events = " << processedEventCounter);
      if (uddScanParams_.maxNumberOfProcessedEvents > 0 and processedEventCounter >= uddScanParams_.maxNumberOfProcessedEvents) {
	DT_LOG_DEBUG(logLevel, "processedEventCounter=" << processedEventCounter << " at maximum=" << uddScanParams_.maxNumberOfProcessedEvents);
	break;
      }
    }
    DT_LOG_DEBUG(logLevel, "Event loop completed");

    moduleManager.reset();

  } catch (std::exception & error) {
    std::cerr << "fluddscan : Setup/run of UDD scan threw exception" << std::endl;
    std::cerr << error.what() << std::endl;
    code = falaise::EXIT_UNAVAILABLE;
  }
  
  return code; 
}
