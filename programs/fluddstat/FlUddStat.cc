// Ourselves
#include "FlUddStat.hh"

// Standard Library
#include <cstdlib>
#include <memory>
#include <string>
#include <fstream>
#include <optional>

// Bayeux:
#include "bayeux/dpp/input_module.h"
#include "bayeux/datatools/event_id.h"
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/utils.h"

// This Project
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>
#include <falaise/snemo/datamodels/udd_utils.h>
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/metadata_utils.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/rc/run_description.h>
#include "FlUddScan.hh"
#include "FlUddChannelMonitoring.hh"

namespace FLUddStat {

  const int PICOTOMICRO = 1000000;
  
  FLUddStatApp::FLUddStatApp(const FLUddStatConfig & config_)
    : _config_(config_)
  {
    datatools::fetch_path_with_env(_config_.outputDirPath);
    return;
  }

  int FLUddStatApp::run()
  {
    std::unique_ptr<dpp::input_module> inputMod(new dpp::input_module);
    std::string inputUddFilePath(_config_.inputUddFilePath);
    datatools::fetch_path_with_env(inputUddFilePath);
    inputMod->set_single_input_file(inputUddFilePath);
    // inputMod->set_logging_priority(datatools::logger::PRIO_DEBUG);
    datatools::multi_properties inMetadataStore = inputMod->get_metadata_store();
    inputMod->initialize_simple();

    datatools::properties channelMonConfig;
    channelMonConfig.store("verbosity", (int) _config_.verbosity);
    channelMonConfig.store("out_dir_path", _config_.outputDirPath);
    FlUddChannelMonitoring channelMon;
    channelMon.initialize(channelMonConfig);
    
    int runID = snemo::rc::run_description::INVALID_RUN_ID;
    std::optional<bool> realEvent;
    std::optional<snemo::datamodel::timestamp> startTimestamp;
    std::optional<snemo::datamodel::timestamp> stopTimestamp;
    snemo::datamodel::timestamp previousTimestamp;
    std::uint32_t eventCounter = 0u;
    std::ofstream outputDt("dt.data");
    while (not inputMod->is_terminated()) {
      FLUddScanner scanner;

      datatools::things event;
      inputMod->process(event);
      scanner.process(event);
      channelMon.process(event);
      // event.tree_dump(std::clog, "Event", "[debug] ");
      DT_THROW_IF(not event.has("EH"), std::logic_error, "Missing event header");
      const snemo::datamodel::event_header & EH
	= event.get<snemo::datamodel::event_header>("EH");
      if (runID == snemo::rc::run_description::INVALID_RUN_ID) {
	const datatools::event_id & eventID = EH.get_id();
	runID = eventID.get_run_number();
      }
      if (not realEvent.has_value()) {
	realEvent = EH.is_real();
      }
      const snemo::datamodel::timestamp & ts = EH.get_timestamp();
      if (not startTimestamp.has_value()) {
	startTimestamp = ts;
      }    
      stopTimestamp = ts;
      if (eventCounter > 0) {
 	int64_t t1_s = previousTimestamp.get_seconds();
	int64_t t2_s = stopTimestamp->get_seconds();
	int64_t t1_us = previousTimestamp.get_picoseconds() / PICOTOMICRO;
	int64_t t2_us = stopTimestamp->get_picoseconds() / PICOTOMICRO;
	double dt = (t2_s - t1_s) * CLHEP::second
	  + (t2_us - t1_us) * CLHEP::microsecond; 
	// std::clog << "[log] dt=" << dt / CLHEP::second  << '\n';
	outputDt << dt / CLHEP::second  << '\n';
      }
      eventCounter++;
      if (eventCounter > 0) {
	previousTimestamp = ts;
      }
      if (_config_.maxNbEvents > 0 and eventCounter >= _config_.maxNbEvents) {
	break;
      } 
      event.clear();
    }
    outputDt.close();
    std::clog << "[log] runid=" << runID << '\n';
    std::clog << "[log] #events=" << eventCounter << '\n';
    std::clog << "[log] realEvent=" << std::boolalpha << *realEvent << '\n';
    std::clog << "[log] startTimestamp=" << *startTimestamp << '\n';
    std::clog << "[log] stopTimestamp=" << *stopTimestamp << '\n';
    int64_t runStartTime_s = startTimestamp->get_seconds();
    int64_t runStartTime_us = startTimestamp->get_picoseconds() / PICOTOMICRO;
    int64_t runStopTime_s = stopTimestamp->get_seconds();
    int64_t runStopTime_us = stopTimestamp->get_picoseconds() / PICOTOMICRO;
    std::clog << "[log] runStartTime_s=" << runStartTime_s << '\n';
    std::clog << "[log] runStartTime_us=" << runStartTime_us << '\n';
    std::clog << "[log] runStopTime_s=" << runStopTime_s << '\n';
    std::clog << "[log] runStopTime_us=" << runStopTime_us << '\n';
    int runDuration_s = runStopTime_s - runStartTime_s;
    std::clog << "[log] runDuration_s=" << runDuration_s << '\n';

    double elapsed = runStartTime_s * CLHEP::second + runStartTime_us * CLHEP::microsecond;
    std::clog << "[log] elapsed=" << elapsed / CLHEP::second << " s" << '\n';
    auto runStartTimePoint = snemo::time::time_point_from_epoch_sec(elapsed);
    auto runDuration = snemo::time::time_duration_from_sec(runDuration_s * CLHEP::second); 
    channelMon.terminate();

    std::filesystem::path outputDirPath = _config_.outputDirPath;
    std::filesystem::path outputStatFilePath = outputDirPath / "udd.stat";
    std::string outputStatFilePathS(outputStatFilePath.string());
    datatools::fetch_path_with_env(outputStatFilePathS);
    std::ofstream outputStatFile(outputStatFilePathS);
    outputStatFile << "run_id=" << runID << '\n';
    outputStatFile << "run_start=" << snemo::time::to_string(runStartTimePoint) << '\n';
    outputStatFile << "run_duration=" << snemo::time::to_string(runDuration) << '\n';
    outputStatFile << "nb_events=" << eventCounter << '\n';
    outputStatFile.close();

    
    return EXIT_SUCCESS;
  }
  
} // FLUddStat
