// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/run_info_service.cc */

// Ourselves:
#include <falaise/snemo/services/run_info_service.h>

// Standard library:
#include <cstdlib>
#include <string>
#include <algorithm> // copy
#include <iterator>  // ostream_operator
#include <bitset>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <json/json.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/rc/run_phase.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::run_info_service,
                                         "snemo::run_info_service")

namespace snemo {

  run_info_service::run_info_service()
    : datatools::base_service("RunInfoService",
                              "Run info service",
                              "Service publishing run info")
  {
    return;
  }

  run_info_service::~run_info_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool run_info_service::is_initialized() const
  {
    return _initialized_;
  }

  int run_info_service::initialize(const datatools::properties & config_,
                                   datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    this->datatools::base_service::common_initialize(config_);

    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "csv_files") {
        _mode_ = MODE_CSV_FILES;
      } else if (modeLabel == "db") {
        _mode_ = MODE_DB;
      } else {
        DT_THROW(std::logic_error, "Invalid mode label '" << modeLabel << "'!");
      }
    }

    if (_mode_ == MODE_DB) {
      if (config_.has_key("db_label")) {
        _db_label_ = config_.fetch_string("db_label");     
      }
      if (_db_label_.empty()) {
        _db_label_ = service_info::dbServiceName();
      }
      DT_LOG_DEBUG(get_logging_priority(), "default DB label = " << std::quoted(service_info::dbServiceName()));
      DT_LOG_DEBUG(get_logging_priority(), "DB label = " << std::quoted(_db_label_));
      if (datatools::has<snemo::db_service>(services_, _db_label_)) {
        _db_service_ = &datatools::get<snemo::db_service>(services_, _db_label_);
      } 
      datatools::properties dbConfig;
      config_.export_and_rename_starting_with(dbConfig, "db.", "");
      _init_mode_db_(dbConfig);
    }

    if (_mode_ == MODE_FILES) {
      datatools::properties filesConfig;
      config_.export_and_rename_starting_with(filesConfig, "files.", "");
      _init_mode_files_(filesConfig);
    }

    if (_mode_ == MODE_CSV_FILES) {
      datatools::properties csvFilesConfig;
      config_.export_and_rename_starting_with(csvFilesConfig, "csv_files.", "");
      _init_mode_csv_files_(csvFilesConfig);
    }
      
    _initialized_ = true;
    return 0;
  }

  int run_info_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    _runs_.reset();
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    } else if (_mode_ == MODE_CSV_FILES) {
      _terminate_mode_csv_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  run_info_service::mode_type run_info_service::get_mode() const
  {
    return _mode_;
  }

  bool run_info_service::validate_time_point(const time::time_point & time_point_) const
  {
    return _runs_.contains(time_point_);
  }

  rc::run_id_type run_info_service::find_run_id(const time::time_point & time_point_) const
  {
    return _runs_.locate_run(time_point_);
  }
 
  void run_info_service::_init_mode_db_(const datatools::properties & db_config_)
  {
    if (_db_service_ == nullptr) {
      DT_THROW(std::logic_error, "DB service nullptr !!!");
    }
    _init_mode_db_run_phases_(db_config_);
    _init_mode_db_run_infos_(db_config_);
    return;
  }
  
  void run_info_service::_init_mode_db_run_phases_(const datatools::properties & db_config_)
  {
    datatools::logger::priority logging = get_logging_priority();
    // logging = datatools::logger::PRIO_DEBUG;

    std::string tableName = "Run_phases";
    if (db_config_.has_key("run_phases.table_name")) {
      tableName = db_config_.fetch_string("run_phases.table_name");
    }
    DT_THROW_IF(not _db_service_->has_table(tableName), std::logic_error,
		"Database has no table named " << std::quoted(tableName));
        
    snemo::db::table_selection_type tableSel;
    _db_service_->process_select_all_statement(tableName, tableSel);
    DT_LOG_DEBUG(logging, "DB mode: Table selection size = " << tableSel.size());
    DT_LOG_DEBUG(logging, "DB mode: Parsing table selection...");
    namespace snrc = snemo::rc;
    namespace snt  = snemo::time;
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      DT_LOG_DEBUG(logging, "DB mode: Fetching run phases from row #" << iRow << " : ");
      const snemo::db::record_type row = tableSel[iRow];
      DT_LOG_DEBUG(logging, "DB mode:   Number of fields : " << row.size());
      std::int32_t phaseId = 0;
      snt::time_point phaseStartTime = snt::invalid_point();
      snt::time_point phaseStopTime = snt::invalid_point();
      std::uint32_t phaseStatus = 0;
      
      // Parse columns:
      auto colIndex = 0u;

      // phase id:
      phaseId = std::get<std::int32_t>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  phaseId = " << phaseId);

      // start time:
      std::string phaseStartTimeStr = std::get<std::string>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  phaseStartTimeStr = " << std::quoted(phaseStartTimeStr));
      phaseStartTime = snt::time_point_from_string(phaseStartTimeStr);
      DT_LOG_DEBUG(logging, "  phaseStartTime = " << snt::to_string(phaseStartTime));

      // stop time:
      std::string phaseStopTimeStr = std::get<std::string>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  phaseStopTimeStr = " << std::quoted(phaseStopTimeStr));
      if (phaseStopTimeStr.size()) {
	phaseStopTime = snt::time_point_from_string(phaseStopTimeStr);
      }
      DT_LOG_DEBUG(logging, "  phaseStopTime = " << snt::to_string(phaseStopTime));

      // status:
      phaseStatus = (std::uint32_t) std::get<int>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  phaseStatus = " << std::bitset<32>(phaseStatus));
      DT_LOG_DEBUG(logging, "  colIndex = " << colIndex);
      
      snrc::run_phase phaseDesc;
      phaseDesc.set_id(phaseId);
      phaseDesc.set_start_time(phaseStartTime);
      if (snt::is_valid(phaseStopTime)) {
	phaseDesc.set_stop_time(phaseStopTime);
      }
      phaseDesc.set_status(phaseStatus);
      
      _run_phases_.push_back(phaseDesc);
    }
    
    return;
  }

  void run_info_service::_init_mode_db_run_infos_(const datatools::properties & db_config_)
  {
    datatools::logger::priority logging = get_logging_priority();
    // logging = datatools::logger::PRIO_DEBUG;

    std::string tableName = "Run_infos";
    if (db_config_.has_key("run_infos.table_name")) {
      tableName = db_config_.fetch_string("run_infos.table_name");
    }
    DT_THROW_IF(not _db_service_->has_table(tableName), std::logic_error,
		"Database has no table named " << std::quoted(tableName));
    
    DT_LOG_DEBUG(logging, "Number of runs : " << _runs_.size());
    std::uint32_t runMin = 0u;
    std::uint32_t runMax = 0xFFFFFFFF;
    if (db_config_.has_key("run_min")) {
      runMin = db_config_.fetch_positive_integer("run_min");
    }
    if (db_config_.has_key("run_max")) {
      runMax = db_config_.fetch_positive_integer("run_max");
    }

    DT_LOG_DEBUG(logging, "DB mode: run min = " << runMin);
    DT_LOG_DEBUG(logging, "DB mode: run max = " << runMax);
    
    snemo::db::table_selection_type tableSel;
    _db_service_->process_select_all_statement(tableName, tableSel);
    DT_LOG_DEBUG(logging, "DB mode: Table selection size = " << tableSel.size());
    DT_LOG_DEBUG(logging, "DB mode: Parsing table selection...");
    namespace snrc = snemo::rc;
    namespace snt  = snemo::time;
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      DT_LOG_DEBUG(logging, "DB mode: Fetching run infos from row #" << iRow << " : ");
      const snemo::db::record_type row = tableSel[iRow];
      DT_LOG_DEBUG(logging, "DB mode:   Number of fields : " << row.size());
    
      snrc::run_id_type runNumber = 0u;
      snrc::run_category runCat = snrc::run_category::INDETERMINATE;
      snt::time_period runPeriod = snt::invalid_period();
      std::uint32_t runNumberOfEntries = 0u;
      snt::time_duration runDeadtime = snt::from_quantity(0u * CLHEP::second);
      snrc::run_status_type runStatus = snrc::run_status::make_good();
      // datatools::version_id runDaqConfigId; // invalid
      boost::property_tree::ptree runDetails; // empty at start
      // Parse columns:
      auto colIndex = 0u;

      // Extract run number:
      runNumber = std::get<int>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  =====> runNumber = " << runNumber);

      // Extract run category:
      std::uint32_t runCatValue = std::get<int>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  runCatValue = " << runCatValue);
      runCat = snrc::run_category_from_uint(runCatValue);
      DT_LOG_DEBUG(logging, "  runCat = " << runCat);

      // Extract run start time:
      std::string runStartTimeStr = std::get<std::string>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  runStartTimeStr = " << runStartTimeStr);
      snt::time_point runStartTime = snt::time_point_from_string(runStartTimeStr);
      DT_LOG_DEBUG(logging, "  runStartTime = " << snt::to_string(runStartTime));

      // Extract run duration:
      double runDurationSec = 0.0;
      runDurationSec = std::get<double>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  runDurationSec = " << runDurationSec);
      if (runDurationSec < 1.0) {
	runDurationSec = 1.0; // At least 1 second to validate the runPeriod instance
      }
      snt::time_duration runDuration = snt::from_quantity(runDurationSec * CLHEP::second);
      DT_LOG_DEBUG(logging, "  runDuration = " << snt::to_string(runDuration));
 
      // Extract run status:
      std::uint32_t runStatusBits = 0;
      runStatusBits = (std::uint32_t) std::get<int>(row[colIndex++]);
      runStatus = snrc::run_status::make_from_bitset(runStatusBits);
      DT_LOG_DEBUG(logging, "  runStatus = " << std::bitset<32>(runStatus));
      DT_LOG_DEBUG(logging, "  colIndex = " << colIndex);
     
      // Extract run details:
      std::string runDetailsRepr = std::get<std::string>(row[colIndex++]);
      DT_LOG_DEBUG(logging, "  runDetailsRepr = " << std::quoted(runDetailsRepr));

      std::bitset<7> runCratesFlags;
      int numberOfTriggers = -1;
      int numberOfCaloHits = -1;
      int numberOfTrackerHits = -1;
      double safeDurationSec = std::nan("1");
      std::string daqConfig;
      int phaseId = -1;
      
      if (not runDetailsRepr.empty()) {
	DT_LOG_DEBUG(logging, "  parsing details...");
	// Parse details from JSON stuff :
	Json::Value jsonRoot;
	std::istringstream jsonDetails(runDetailsRepr);
	jsonDetails >> jsonRoot;

	// Extract run #entries:
	std::string runCratesFlagsRepr = jsonRoot.get("crates", "0").asString();
	runCratesFlags = std::bitset<7>(runCratesFlagsRepr);
	numberOfTriggers = jsonRoot.get("nb_triggers", -1).asInt();
	numberOfCaloHits = jsonRoot.get("nb_calo_hits", -1).asInt();
	numberOfTrackerHits = jsonRoot.get("nb_tracker_hits", -1).asInt();
	safeDurationSec = jsonRoot.get("safe_duration", std::nan("1")).asDouble();
	daqConfig = jsonRoot.get("daq_config", "").asString();
	phaseId = jsonRoot.get("phase_id", -1).asInt();

	runDetails.put("crates", runCratesFlags);
	if (numberOfTriggers >= 0) {
	  runDetails.put("nb_triggers", (std::uint32_t) numberOfTriggers);
	}
	if (numberOfCaloHits >= 0) {
	  runDetails.put("nb_calo_hits", (std::uint32_t) numberOfCaloHits);
	}
	if (numberOfTrackerHits >= 0) {
	  runDetails.put("nb_tracker_hits", (std::uint32_t) numberOfTrackerHits);
	}
	if (not std::isnan(safeDurationSec)) {
	  runDetails.put("safe_duration", safeDurationSec);
	}
	if (phaseId >= 0) {
	  runDetails.put("phase_id", phaseId);
	}
	if (daqConfig.size() > 0) {
	  runDetails.put("daq_config", daqConfig);
	}
	
	// Extract run #entries if any:
	int runNbOfEntries = jsonRoot.get("number_of_entries", -1).asInt();
	if (runNbOfEntries > 0) {
	  runNumberOfEntries = runNbOfEntries;
	} else {
	  // use the informations in the 'details' dict:
	  if (runCat == snrc::run_category::PRODUCTION
	      or runCat == snrc::run_category::CALIBRATION_BI) {
	    if (numberOfTriggers > 0) {
	      runNumberOfEntries = numberOfTriggers;
	    }
	  } else if (runCat == snrc::run_category::CALIBRATION_LI
		     or runCat == snrc::run_category::CALIBRATION_REFOM) {
	    if (numberOfCaloHits > 0) {
	      runNumberOfEntries = numberOfCaloHits;
	    }
	  }
	}

	if (snrc::run_status::has_truncated_issues(runStatus)) {
	  if (not std::isnan(safeDurationSec)) {
	    DT_LOG_WARNING(logging, "  fix run duration from safe_period=" << safeDurationSec << " for truncated run #" << runNumber);
	    runDuration = snt::from_quantity(safeDurationSec * CLHEP::second);
	    DT_LOG_WARNING(logging, "  runDuration = " << snt::to_string(runDuration));
 	  }
	}
	
	// // Extract DAQ config:
	// std::string runDaqConfigIdStr = jsonRoot.get("daq_config", "").asString();
	// if (not runDaqConfigIdStr.empty()) {
	//   runDaqConfigId = datatools::version_id(runDaqConfigIdStr);
	//   DT_LOG_DEBUG(logging, "  runDaqConfigId = " << runDaqConfigId);
	// }

	// Extract run deadtime (default):
	// double runDeadtimeSec = std::get<double>(row[colIndex++]);
	double runDeadtimeSec = 0.0;
	runDeadtimeSec = jsonRoot.get("deadtime", 0.0).asDouble();
	runDeadtime = snt::from_quantity(runDeadtimeSec * CLHEP::second);
	DT_LOG_DEBUG(logging, "  runDeadtime = " << snt::to_string(runDeadtime));
      } else {
	DT_LOG_DEBUG(logging, "No details are provided from the JSON field");
      }

      // Compute effective run period:
      runPeriod = snt::time_period(runStartTime, runDuration);
      DT_LOG_DEBUG(logging, "  runPeriod = " << snt::to_string(runPeriod));
      snrc::run_description runDesc =
	snrc::run_description::make_unique_slice(runNumber,
						 runCat,
						 //runDaqConfigId,
						 runPeriod,
						 runNumberOfEntries,
						 runDeadtime,
						 runStatus);
      runDesc.details() = runDetails;
      
      if (datatools::logger::is_debug(logging)) {
	boost::property_tree::ptree popts;
	popts.put("title", "Run description");
	popts.put("indent", "[debug] ");
	runDesc.print_tree(std::clog, popts);
      }
      if ((std::uint32_t) runNumber < runMin or (std::uint32_t) runNumber > runMax) {
	continue;
      }
      DT_LOG_DEBUG(logging, " Adding description of run #" << runNumber);
      _runs_.add_run(runDesc);
    }
       
    if (datatools::logger::is_debug(logging)) {
      for (const auto & r : _runs_.runs()) {
        DT_LOG_DEBUG(logging, "RunID=" << r.first);
      }
    }
    return;
  }

  void run_info_service::_terminate_mode_db_()
  {
    // DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }
  
  void run_info_service::_init_mode_files_(const datatools::properties & config_)
  {
    if (config_.has_key("run_lists")) {
      std::vector<std::string> runLists;
      config_.fetch("run_lists", runLists);
      for (const auto & runList:runLists) {
        load_run_list(runList);
      }
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of runs : " << _runs_.size());
    if (datatools::logger::is_debug(get_logging_priority())) {
      for (const auto & r : _runs_.runs()) {
        std::cerr << "[debug] " << "RunID=" << r.first << std::endl;
      }
    }
    return;
  }

  void run_info_service::_terminate_mode_files_()
  {
    return;
  }
  
  void run_info_service::_init_mode_csv_files_(const datatools::properties & config_)
  {
    if (config_.has_key("run_lists")) {
      std::vector<std::string> runLists;
      config_.fetch("run_lists", runLists);
      for (const auto & runList:runLists) {
        load_run_csv_list(runList);
      }
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of runs : " << _runs_.size());
    if (datatools::logger::is_debug(get_logging_priority())) {
      for (const auto & r : _runs_.runs()) {
        std::cerr << "[debug] " << "RunID=" << r.first << std::endl;
      }
    }
    return;
  }

  void run_info_service::_terminate_mode_csv_files_()
  {
    return;
  }

  const rc::run_list & run_info_service::get_run_list() const
  {
    return _runs_;
  }

  rc::run_list & run_info_service::grab_run_list()
  {
    return _runs_;
  }
 
  bool run_info_service::has_run_phase(const rc::run_phase_id_type run_phase_id_) const
  {
    return (run_phase_id_ >= 0) and (run_phase_id_ < (int) _run_phases_.size());
  }

  const rc::run_phase & run_info_service::get_run_phase(const rc::run_phase_id_type run_phase_id_) const
  {
    if (not has_run_phase(run_phase_id_)) {
      DT_THROW(std::logic_error, "Run phase with ID=" << run_phase_id_ << " does not exist!");
    }
    return _run_phases_[run_phase_id_];
  }
 
  bool run_info_service::has_run_info(const rc::run_id_type run_id_) const
  {
    return _runs_.has_run(run_id_) != 0;
  }
   
  const rc::run_description &
  run_info_service::get_run_info(const rc::run_id_type run_id_) const
  {
    // if (_mode_ == MODE_DB) {
    //   DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    // } 
    auto found = _runs_.runs().find(run_id_);
    DT_THROW_IF(found == _runs_.runs().end(), std::logic_error,
                "Cannot find run description for run ID=" << run_id_ << '!');
    return found->second;
  }
   
  rc::run_phase_id_type run_info_service::find_phase_id(const time::time_point & time_point_) const
  {
    rc::run_phase_id_type runPhaseId = rc::run_phase::INVALID_PHASE_ID;
    for (const auto & runPhase : _run_phases_) {
      if (runPhase.contains(time_point_)) {
	runPhaseId = runPhase.id();
	break;
      }
    }
    return runPhaseId;
  }
  
  rc::run_phase_id_type run_info_service::find_phase_id(const rc::run_id_type run_id_) const
  {
    rc::run_phase_id_type runPhaseId = rc::run_phase::INVALID_PHASE_ID;
    const rc::run_description & runInfo = this->get_run_info(run_id_);
    for (const auto & runPhase : _run_phases_) {
      if (runPhase.has_run(runInfo)) {
	runPhaseId = runPhase.id();
	break;
      }
    }
    return runPhaseId;
  }

  void run_info_service::load_run_list(const std::string & infile_)
  {
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open run list file '" << filename << "'!");
    datatools::multi_properties runListConfig("run", "type");
    std::string runListConfigFilename = filename;
    runListConfig.read(runListConfigFilename);
    if (datatools::logger::is_debug(get_logging_priority())) {
      boost::property_tree::ptree options;
      options.put("indent", "[debug] ");
      options.put("title", "Run list config from '" + infile_ + "'");
      runListConfig.print_tree(std::cerr, options);
    }
    _runs_.load(runListConfig);
    return;
  }
 
  void run_info_service::load_run_csv_list(const std::string & infile_)
  {
    namespace snt = snemo::time;
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open run list file '" << filename << "'!");
    datatools::multi_properties runListConfig("run", "type");
    // static boost::escaped_list_separator<char> _separators("", ",;|", "");
    // typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
    static boost::char_separator<char> _separators(";", "", boost::drop_empty_tokens);
    typedef boost::tokenizer< boost::char_separator<char> > Tokenizer;
    std::vector<std::string> vec;
    std::string line;
    auto nLine = 0u;
    while (std::getline(fin,line)) {
      nLine++;
      if (line.empty() or line[0] == '#') {
	continue;
      }
      Tokenizer tok(line, _separators);
      vec.assign(tok.begin(),tok.end());
      std::cerr << "vec = ";
      std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cerr, "|"));
      std::cerr << std::endl;

      DT_THROW_IF(vec.size() < 4, std::runtime_error, "Invalid format for CSV run list file '" << filename << "' (line #" << nLine << ")!");


      auto index = 0;
      // Extract run number:
      std::string runIdRepr = vec[index++];
      boost::algorithm::trim(runIdRepr);
      snemo::rc::run_id_type  run_id  = std::stoi(runIdRepr);

      // Extract run category:
      std::string runCatRepr = vec[index++];
      boost::algorithm::trim(runCatRepr);
      snemo::rc::run_category run_cat = snemo::rc::run_category_from_string(runCatRepr);

      // // Extract DAQ config:
      // std::string runDaqConfigRepr = vec[index++];
      // boost::algorithm::trim(runDaqConfigRepr);
      // datatools::version_id run_daq_config(runDaqConfigRepr);

      // // Extract EB config:
      // std::string runEbConfigRepr = vec[index++];
      // boost::algorithm::trim(runEbConfigRepr);
      // datatools::version_id run_eb_config(runEbConfigRepr);

      // Extract run start time:
      std::string runStartRepr = vec[index++];
      boost::algorithm::trim(runStartRepr);
      snt::time_point runStart = snt::time_point_from_string(runStartRepr);
      // double qRunStartSecEpoch        = std::stod(vec[2]) * CLHEP::second;
      // snt::time_point runStart = snt::time_point_from_epoch_sec(qRunStartSecEpoch);
      
      // Extract run duration (in second):
      std::string runDurationRepr = vec[index++];
      boost::algorithm::trim(runDurationRepr);
      double qRunDurationSec        = std::stod(runDurationRepr) * CLHEP::second;
      snt::time_duration runDuration = snt::time_duration_from_sec(qRunDurationSec);
      
      // Extract run status:
      std::string runStatusRepr = vec[index++];
      boost::algorithm::trim(runStatusRepr);
      std::bitset<16> runStatusBits{runStatusRepr};  
       
      // // Extract number of entries:
      std::uint32_t numberOfEntries = 0;
      // std::string numberOfEntriesRepr = vec[index++];
      // boost::algorithm::trim(numberOfEntriesRepr);
      // numberOfEntries = std::stoi(numberOfEntriesRepr);
     
      snt::time_period runPeriod(runStart, runDuration);
      snt::time_duration runDeadtime(0,0,0,0); // (hours,minutes,seconds,useconds)
      
      // Add a record for this run
      datatools::properties & run_section = runListConfig.add_section(std::to_string(run_id), "snemo::rc::run_description");
      run_section.store_string("category", snemo::rc::to_string(run_cat));
      // run_section.store_string("daq_config_id", run_daq_config.to_string());
      run_section.store_string("period", snt::to_string(runPeriod));
      run_section.store_integer("number_of_entries", numberOfEntries);
      if (runDeadtime.total_microseconds() > 0) {
	run_section.store_string("deadtime", snt::to_string(runDeadtime));
      }
      run_section.store_string("status", runStatusBits.to_string());
    }
      
    if (datatools::logger::is_debug(get_logging_priority())) {
      boost::property_tree::ptree options;
      options.put("indent", "[debug] ");
      options.put("title", "Run list config from '" + infile_ + "'");
      runListConfig.print_tree(std::cerr, options);
    }
    _runs_.load(runListConfig);
    return;
  }

  const std::vector<rc::run_phase> & run_info_service::run_phases() const
  {
    return _run_phases_;
  }
 
  void run_info_service::print_tree(std::ostream & out_,
				    const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    this->base_service::print_tree(out_, base_print_options::force_inheritance(options_));
  
    out_ << popts.indent << i_tree_dumpable::tag
         << "Mode : ";
    if (_mode_ == MODE_FILES) out_ << "'file'";
    else out_ << "'db'";
    out_ << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "DB label : '" << _db_label_ << "'" << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "DB service : " << std::boolalpha << (_db_service_ != nullptr) << std::dec << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "Run phases: " << _run_phases_.size() << std::endl;
    
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
         << "Runs: " << _runs_.size() << std::endl;
    {
      boost::property_tree::ptree popts2;
      std::string inheritLastItemTag = (popts.inherit ? "|   " : "    ");
      popts2.put("indent", popts.indent + inheritLastItemTag);
      _runs_.print_tree(out_, popts2);
    }
   
    
    // {
    //   uint32_t rCount = 0;
    //   for (const auto & r : _runs_.runs()) {
    //     out_ << popts.indent << i_tree_dumpable::inherit_skip_tag(popts.inherit);
    //     bool lastItem = false;
    //     std::string inheritLastItemTag = (popts.inherit ? "|   " : "    ");
    //     if (rCount + 1 == _runs_.size()) {
    //       lastItem = true;
    //     }
    //     if (lastItem) {
    //       out_ << i_tree_dumpable::last_tag;
    //     } else {
    //       out_ << i_tree_dumpable::tag;
    //     } 
    //     out_ << "Description for run " << r.first << " : " << std::endl;
    //     {
    //       boost::property_tree::ptree popts2;
    //       popts2.put("indent", popts.indent + inheritLastItemTag + (lastItem ? "    ": "|   "));
    //       r.second.print_tree(out_, popts2);
    //     }
    //     rCount++;
    //   }
    // }

    return;
  }
  
}  // namespace snemo
