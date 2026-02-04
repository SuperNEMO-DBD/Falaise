// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/tracker_cell_status_service.cc */

// Ourselves:
#include <falaise/snemo/services/tracker_cell_status_service.h>

// Standard library;
#include <bitset>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/geomtools/geometry_service.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/datamodels/geomid_utils.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::tracker_cell_status_service,
                                         "snemo::tracker_cell_status_service")

namespace snemo {

  tracker_cell_status_service::tracker_cell_status_service()
    : datatools::base_service("TrackerCellStatusService",
                              "Tracker cell status service",
                              "Service publishing tracker cell status")
  {
    return;
  }

  tracker_cell_status_service::~tracker_cell_status_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool tracker_cell_status_service::is_initialized() const
  {
    return _initialized_;
  }

  int tracker_cell_status_service::initialize(const datatools::properties & config_,
                                              datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
   this->datatools::base_service::common_initialize(config_);

    if (config_.has_key("geometry_label")) {
      _geometry_label_ = config_.fetch_string("geometry_label");     
    }

    if (_geometry_label_.empty()) {
      _geometry_label_ = service_info::geometryServiceName();
    }

    if (datatools::has<geomtools::geometry_service>(services_, _geometry_label_)) {
      _geomgr_ = &datatools::get<geomtools::geometry_service>(services_, _geometry_label_).get_geom_manager();
    }
    DT_THROW_IF(_geomgr_ == nullptr, std::logic_error, "Missing geometry manager!");
    
    const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
    _cell_type_ = idMgr.categories_by_name().find("drift_cell")->second.get_type(); 
    _cell_core_type_ = idMgr.categories_by_name().find("drift_cell_core")->second.get_type(); 

    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
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
      
    _initialized_ = true;
    return 0;
  }

  int tracker_cell_status_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  tracker_cell_status_service::mode_type tracker_cell_status_service::get_mode() const
  {
    return _mode_;
  }

  bool tracker_cell_status_service::has_geometry_manager() const
  {
    return _geomgr_ != nullptr;
  }

  void tracker_cell_status_service::set_geometry_manager(const geomtools::manager & geomgr_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    DT_THROW_IF(not geomgr_.is_initialized(), std::logic_error, "Geometry manager is not initialized!");
    _geomgr_ = &geomgr_;
    return;
  }

  void tracker_cell_status_service::_init_mode_db_(const datatools::properties & db_config_)
  {
    // DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    datatools::logger::priority logging = get_logging_priority();
    logging = datatools::logger::PRIO_DEBUG;

    if (_db_service_ == nullptr) {
      DT_THROW(std::logic_error, "DB service nullptr !!!");
    }

    std::string tableName = "GG_Status_Change_Events";
    if (db_config_.has_key("table_name")) {
      tableName = db_config_.fetch_string("table_name");
    }
    DT_THROW_IF(not _db_service_->has_table(tableName), std::logic_error,
		"Database has no table named " << std::quoted(tableName));
    
    snemo::db::table_selection_type tableSel;
    _db_service_->process_select_all_statement(tableName, tableSel);
    DT_LOG_DEBUG(logging, "DB mode: Table selection size = " << tableSel.size());

    if (tableSel.size()) {
      DT_LOG_DEBUG(logging, "DB mode: Parsing table selection...");
    }
    namespace snrc = snemo::rc;
    namespace snt  = snemo::time;
    snrc::tracker_cell_status_change_event_list changeEventLists[2034];
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      const snemo::db::record_type row = tableSel[iRow];
      std::int32_t eventId = -1;
      eventId = (std::uint32_t) std::get<int>(row[0]);
      DT_LOG_DEBUG(logging, "GG status change event #" << std::to_string(eventId));
 
      std::int32_t ggNum = -1;
      ggNum = (std::uint32_t) std::get<int>(row[1]);
      DT_THROW_IF(not snemo::datamodel::gg_num_is_valid(ggNum), std::logic_error, "Invalid GG cell number");

      // bool validGid = false;
      // validGid = true;
      // DT_THROW_IF(not validGid, std::logic_error,
      // 		  "Not a valid (x)calorimeter/gveto GG number '" << ggNum  << "'!");
      
      std::string eventTimestampStr = std::get<std::string>(row[2]);
      DT_LOG_DEBUG(logging, "  eventTimestampStr = " << eventTimestampStr);
      snt::time_point eventTimestamp = snt::time_point_from_string(eventTimestampStr);
      DT_LOG_DEBUG(logging, "  eventTimestamp = " << snt::to_string(eventTimestamp));
       
      std::string eventTypeStr = std::get<std::string>(row[3]);
      boost::algorithm::trim(eventTypeStr);
      DT_LOG_DEBUG(logging, "  eventTypeStr = " << eventTypeStr);
      snrc::tracker_cell_status_change_event::event_type eventType
	= snrc::tracker_cell_status_change_event::no_change;
      if (eventTypeStr == "reset_bits") {
	eventType = snrc::tracker_cell_status_change_event::reset_bits;
      } else if (eventTypeStr == "set_bit") {
	eventType = snrc::tracker_cell_status_change_event::set_bit;
      } else if (eventTypeStr == "unset_bit") {
	eventType = snrc::tracker_cell_status_change_event::unset_bit;
      } else {
	DT_THROW(std::logic_error,
		 "Invalid calorimeter staus change event type '" << eventTypeStr  << "'!");
      }

      std::string statusBitStr = std::get<std::string>(row[4]);
      // snrc::calorimeter_gg_status::status_bit statusBit;
      std::uint32_t ggStatus = snrc::tracker_cell_status::CELL_GOOD;
      std::uint32_t statusToStringOptions = snrc::tracker_cell_status::ONLY_ONE_BIT;
      ggStatus = snrc::tracker_cell_status::status_from_string(statusBitStr, statusToStringOptions);
      if (ggStatus == snrc::tracker_cell_status::CELL_GOOD) {
	DT_THROW(std::logic_error, "Invalid status bit to be set/unset");
      }

      snemo::rc::tracker_cell_status_change_event changeEvent;
      if (eventType == snrc::tracker_cell_status_change_event::reset_bits) {
	changeEvent = snrc::tracker_cell_status_change_event::make_reset(eventTimestamp);
      } else if (eventType == snrc::tracker_cell_status_change_event::set_bit) {
	snrc::tracker_cell_status::status_bit statusBit = static_cast<snrc::tracker_cell_status::status_bit>(ggStatus);
	changeEvent = snrc::tracker_cell_status_change_event::make_set_bit(eventTimestamp, statusBit);
      } else if (eventType == snrc::tracker_cell_status_change_event::unset_bit) {
	snrc::tracker_cell_status::status_bit statusBit = static_cast<snrc::tracker_cell_status::status_bit>(ggStatus);
	changeEvent = snrc::tracker_cell_status_change_event::make_unset_bit(eventTimestamp, statusBit);
      }
      
      changeEventLists[ggNum].add_event(changeEvent);
      DT_LOG_DEBUG(logging, "Add GG status change event for GG num " << ggNum << " : " << changeEvent);
    }
    
    // Build GG status histories from event lists:
    for (auto ggNum = 0u; ggNum < snemo::datamodel::number_of_ggs(); ggNum++) {
      const auto & eventList = changeEventLists[ggNum];
      if (eventList.size() > 0) {
	geomtools::geom_id ggGid = snemo::datamodel::gg_gid(ggNum);
	snrc::tracker_cell_status_history & ggHistory = this->grab_cell_history(ggGid);
	snrc::build_tracker_cell_status_history_from_event_list(eventList, ggHistory);
      }
    }
    DT_LOG_DEBUG(logging, "#GG with status history: " << _histories_.size());
    return;
  }

  void tracker_cell_status_service::_terminate_mode_db_()
  {
    // DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }
  
  void tracker_cell_status_service::_init_mode_files_(const datatools::properties & config_)
  {
    std::vector<std::string> cellMaps;
    if (config_.has_key("cell_maps")) {
      config_.fetch("cell_maps", cellMaps);
    }
    if (config_.has_key("map_format")) {
      std::string fileMapFormat = config_.fetch_string("map_format");
      if (fileMapFormat == "csv-1" or fileMapFormat == "csv-2") {
	_file_format_ = fileMapFormat;
      } else {
	DT_THROW(std::logic_error, "Invalid map format " << std::quoted(fileMapFormat));
      }
    }
    for (const auto & cellMap: cellMaps) {
      DT_LOG_DEBUG(get_logging_priority(), "Loading cell map : " << cellMap);
      load_cell_status_map(cellMap);
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of histories : " << _histories_.size());
    if (datatools::logger::is_debug(get_logging_priority())) {
      for (const auto & h : _histories_) {
	std::clog << "[debug] " << "GID=" << h.first
		  << " has " << h.second.records().size() << " status records"
		  << '\n';
      }
    }
    return;
  }

  void tracker_cell_status_service::_terminate_mode_files_()
  {
    _histories_.clear();
    return;
  }

  const tracker_cell_status_service::history_type &
  tracker_cell_status_service::get_histories() const
  {
    return _histories_;
  }
 
  bool tracker_cell_status_service::has_cell_history(const geomtools::geom_id & gid_) const
  {
    return _histories_.count(gid_) != 0;
  }

  snemo::rc::tracker_cell_status_history &
  tracker_cell_status_service::grab_cell_history(const geomtools::geom_id & gid_)
  {
    if (not has_cell_history(gid_)) {
      snemo::rc::tracker_cell_status_history emptyHistory;
      _histories_[gid_] = emptyHistory;
    }
    history_type::iterator found = _histories_.find(gid_);
    return found->second;
  }

  const snemo::rc::tracker_cell_status_history &
  tracker_cell_status_service::get_cell_history(const geomtools::geom_id & gid_) const
  {
    history_type::const_iterator found = _histories_.find(gid_);
    DT_THROW_IF(found == _histories_.end(), std::logic_error, "No history for cell '" << gid_ << "'!");
    return found->second;
  }
  
  std::uint32_t tracker_cell_status_service::get_cell_status(const geomtools::geom_id & gid_,
                                                             const time::time_point & time_) const
  {
    std::uint32_t status = snemo::rc::tracker_cell_status::CELL_GOOD;
    if (_mode_ == MODE_DB) {
      // DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
      history_type::const_iterator found = _histories_.find(gid_);
      if (found != _histories_.end()) {
        DT_LOG_DEBUG(get_logging_priority(), "Found history for cell " << gid_);
	if (datatools::logger::is_debug(get_logging_priority())) {
	  found->second.print(std::cerr, "[debug] ");
	}
        status = found->second.get_status(time_);
      } else {
        // DT_LOG_DEBUG(get_logging_priority(), "No history for cell " << gid_);
      }
    } else if (_mode_ == MODE_FILES) {
      history_type::const_iterator found = _histories_.find(gid_);
      if (found != _histories_.end()) {
        DT_LOG_DEBUG(get_logging_priority(), "Found history for cell " << gid_);
        status = found->second.get_status(time_);
      } else {
        // DT_LOG_DEBUG(get_logging_priority(), "No history for cell " << gid_);
      }
    }
    return status;
  }

  void tracker_cell_status_service::load_cell_status_map(const std::string & infile_)
  {
    if (_file_format_ == "csv-1") {
      load_cell_status_map_1(infile_);
    } else if (_file_format_ == "csv-2") {
      load_cell_status_map_2(infile_);
    }

    return;
  }

  void tracker_cell_status_service::load_cell_status_map_2(const std::string & infile_)
  {
    const snemo::geometry::locator_plugin & locators
      = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
    const snemo::geometry::gg_locator & ggLocator = locators.geigerLocator();
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open tracker cell status file '" << filename << "'!");
    while (fin and not fin.eof()) {
      std::string line;
      std::getline(fin, line);
      boost::trim(line);
      DT_LOG_DEBUG(get_logging_priority(), "Line='" << line << "'");
      {
	if (line.size() == 0) {
          continue;
        } else if (line[0] == '#') {
	  continue;
	}
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> sep{";"};
	tokenizer tokens{line, sep};
	std::string idRepr;
	std::string periodStartRepr; 
	std::string periodStopRepr; 
	std::string statusRepr;
	int tkCount = 0;
	for (std::string tk : tokens) {
	  DT_LOG_DEBUG(get_logging_priority(), "tk " << std::quoted(tk));
	  if (idRepr.empty()) {
	    DT_THROW_IF(tkCount != 0, std::logic_error, "Invalid formatted tracker cell status record!");
	    idRepr = tk;
	  } else if (periodStartRepr.empty()) {
	    DT_THROW_IF(tkCount != 1, std::logic_error, "Invalid formatted tracker cell status record!");
	    periodStartRepr = tk;
	  } else if (periodStopRepr.empty()) {
	    DT_THROW_IF(tkCount != 2, std::logic_error, "Invalid formatted tracker cell status record!");
	    periodStopRepr = tk;
	  } else if (statusRepr.empty()) {
	    DT_THROW_IF(tkCount != 3, std::logic_error, "Invalid formatted tracker cell status record!");
	    statusRepr = tk;
	  } else {
	    DT_THROW(std::logic_error, "Invalid token '" << tk << "'!");
	  }
	  tkCount++;
	  if (tkCount == 4) break;
	}
	DT_LOG_DEBUG(get_logging_priority(), "Using fields : " << idRepr
		     << ';' << periodStartRepr
		     << ';' << periodStopRepr
		     << ';' << statusRepr);

	// Cell number:
	geomtools::geom_id cellGid;
	boost::trim(idRepr);
	DT_THROW_IF(idRepr.empty(), std::logic_error, "Missing tracker cell ID!");
	if (idRepr[0] == '[') {
	  std::istringstream gidss(idRepr);
	  gidss >> cellGid;
	  DT_THROW_IF(!gidss, std::logic_error, "Cannot decode missing tracker cell geom ID!");
	  DT_THROW_IF(not cellGid.is_valid(), std::logic_error,
		      "Invalid tracker cell geom ID pattern parsed from '" << idRepr  << "'!");
	  DT_THROW_IF(cellGid.get_type() != _cell_core_type_, std::logic_error,
		      "Invalid type for tracker cell geom ID pattern '" << cellGid
		      << "' with expected cell type=" << _cell_type_ << "'!");
	  DT_THROW_IF(not ggLocator.isGeigerCell(cellGid), std::logic_error,
		      "Token '" << idRepr << "' is not a valid tracker cell geom ID pattern!");
	} else {
	  int cellId = std::stoi(idRepr);
	  // Extract cell GID:
	  cellGid = datamodel::gg_gid(cellId);
	}
	
	// Period start:
	boost::trim(periodStartRepr);
	DT_THROW_IF(periodStartRepr.empty(), std::logic_error, "Missing period start time point!");
	time::time_point periodStartPoint = time::invalid_point();
	periodStartPoint = time::time_point_from_string(periodStartRepr);
        DT_THROW_IF(not time::is_valid(periodStartPoint), std::logic_error,
                    "Invalid period start time point parsed from '" << periodStartRepr  << "'!");

	// Period stop:
	boost::trim(periodStopRepr);
	DT_THROW_IF(periodStopRepr.empty(), std::logic_error, "Missing period stop time point!");
	time::time_point periodStopPoint = time::invalid_point();
	periodStopPoint = time::time_point_from_string(periodStopRepr);
        DT_THROW_IF(not time::is_valid(periodStopPoint), std::logic_error,
                    "Invalid period stop time point parsed from '" << periodStopRepr  << "'!");
	time::time_period period(periodStartPoint, periodStopPoint);

	// Status:
	boost::trim(statusRepr);
	std::bitset<16> statusBits(statusRepr);
	std::uint32_t status = statusBits.to_ulong();
	if (status != snemo::rc::tracker_cell_status::CELL_GOOD) {
	  snemo::rc::tracker_cell_status_history & cellHistory = grab_cell_history(cellGid);
	  DT_LOG_DEBUG(get_logging_priority(), "gid=" << cellGid << " period=" << time::to_string(period) << " status=" << status);
	  cellHistory.add(period, status);
	} else {
	  DT_LOG_WARNING(get_logging_priority(), "Ignoring good status for cell with GID pattern=" << cellGid);
	}
      }
    }
    return;
  }
  
  void tracker_cell_status_service::load_cell_status_map_1(const std::string & infile_)
  {
    const snemo::geometry::locator_plugin & locators
      = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
    const snemo::geometry::gg_locator & ggLocator = locators.geigerLocator();
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open tracker cell status file '" << filename << "'!");
    std::map<std::string, time::time_period> predefinedPeriods;
    std::string currentSelectedPeriod;
    while (fin and not fin.eof()) {
      std::string line;
      std::getline(fin, line);
      boost::trim(line);
      DT_LOG_DEBUG(get_logging_priority(), "Line='" << line << "'");
      {
        std::istringstream checkss(line);
        std::string word;
        checkss >> word;
        if (word.size() == 0) {
          continue;
        } else if (word[0] == '#') {
          if (word.size() > 2) {
            if (word[1] == '@') {
              // Parse special directives:
              // #@period= A : [start date / stop date)
              if (line.substr(0, 9) == "#@period=") {
                DT_LOG_DEBUG(get_logging_priority(), "Found period directive");
                auto eqPos = line.find(':');
                DT_THROW_IF(eqPos == line.npos, std::logic_error,
                            "Invalid formatted predefined period directive '" << word  << "'!"); 
                std::string periodName = line.substr(9, eqPos - 9);
                DT_LOG_DEBUG(get_logging_priority(), "periodName='" << periodName << "'");
                boost::trim(periodName);
                std::string periodRepr = line.substr(eqPos + 1);
                DT_LOG_DEBUG(get_logging_priority(), "periodRepr='" << periodRepr << "'");
                boost::trim(periodRepr);
                DT_THROW_IF(periodRepr.empty(), std::logic_error, "Missing predefined period!");
                time::time_period period = time::time_period_from_string(periodRepr);
                DT_THROW_IF(not time::is_valid(period), std::logic_error,
                            "Invalid period parsed from '" << periodRepr  << "'!");
                predefinedPeriods.emplace(periodName, period);
              } else if (line.substr(0, 15) == "#@selectPeriod=") {
                DT_LOG_DEBUG(get_logging_priority(), "Found select period directive");
                std::string selectedPeriodName = line.substr(15);
                boost::trim(selectedPeriodName);
                DT_THROW_IF(selectedPeriodName.empty(), std::logic_error, "Missing a selected period name!");
                DT_THROW_IF(predefinedPeriods.find(selectedPeriodName) == predefinedPeriods.end(),
                            std::logic_error,
                            "Unknown selected period name '" << selectedPeriodName << "'!");
                currentSelectedPeriod = selectedPeriodName;
              } else if (line.substr(0, 16) == "#@unselectPeriod") {
                currentSelectedPeriod.clear();
              }
            }
          }
          continue;
        }
      }
      DT_LOG_DEBUG(get_logging_priority(), "Predefined periods=" << predefinedPeriods.size());
      typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
      boost::char_separator<char> sep{";"};
      tokenizer tokens{line, sep};
      std::string geomIdRepr;
      std::string periodRepr; 
      std::string statusRepr;
      int tkCount = 0;
      for (std::string tk : tokens) {
        if (geomIdRepr.empty()) {
          DT_THROW_IF(tkCount != 0, std::logic_error, "Invalid formatted tracker cell status record!");
          geomIdRepr = tk;
        } else if (periodRepr.empty()) {
          DT_THROW_IF(tkCount != 1, std::logic_error, "Invalid formatted tracker cell status record!");
          periodRepr = tk;
        } else if (statusRepr.empty()) {
          DT_THROW_IF(tkCount != 2, std::logic_error, "Invalid formatted tracker cell status record!");
          statusRepr = tk;
        } else {
          DT_THROW(std::logic_error, "Invalid token '" << tk << "'!");
        }
        tkCount++;
        if (tkCount == 3) break;
      }
      geomtools::geom_id gidPattern;
      boost::trim(geomIdRepr);
      DT_THROW_IF(geomIdRepr.empty(), std::logic_error, "Missing tracker cell geom ID pattern!");
      {
        std::istringstream gidss(geomIdRepr);
        gidss >> gidPattern;
        DT_THROW_IF(!gidss, std::logic_error, "Cannot decode missing tracker cell geom ID pattern!");
        DT_THROW_IF(not gidPattern.is_valid(), std::logic_error,
                    "Invalid tracker cell geom ID pattern parsed from '" << geomIdRepr  << "'!");
        DT_THROW_IF(gidPattern.get_type() != _cell_core_type_, std::logic_error,
                    "Invalid type for tracker cell geom ID pattern '" << gidPattern  << "' with expected cell type=" << _cell_type_ << "'!");
        DT_THROW_IF(not ggLocator.matchGeigerCell(gidPattern), std::logic_error,
                    "Token '" << geomIdRepr << "' is not a valid tracker cell geom ID pattern!");
      }
      boost::trim(periodRepr);
      time::time_period period = time::invalid_period();
      if (periodRepr.empty()) {
        DT_THROW_IF(currentSelectedPeriod.empty(), std::logic_error, "Missing period or current preselected period!");
        periodRepr = currentSelectedPeriod;
      }
      if (predefinedPeriods.find(periodRepr) != predefinedPeriods.end()) {
        period = predefinedPeriods.find(periodRepr)->second;
      } else {
        period = time::time_period_from_string(periodRepr);
        DT_THROW_IF(not time::is_valid(period), std::logic_error,
                    "Invalid period parsed from '" << periodRepr  << "'!");
      }
      boost::trim(statusRepr);
      std::uint32_t status = snemo::rc::tracker_cell_status::status_from_string(statusRepr);
      if (status != snemo::rc::tracker_cell_status::CELL_GOOD) {
	std::set<geomtools::geom_id> gids;
	auto sz = ggLocator.buildGeigerCells(gidPattern, gids);
	if (sz > 0) {
	  for (const auto & gid : gids) {
	    snemo::rc::tracker_cell_status_history & cellHistory = grab_cell_history(gid);
	    DT_LOG_DEBUG(get_logging_priority(), "gid=" << gid << " period=" << time::to_string(period) << " status=" << status);
	    cellHistory.add(period, status);
	  }
	}
      } else {
        DT_LOG_WARNING(get_logging_priority(), "Ignoring good status for cell with GID pattern=" << gidPattern);
      }
    }   
    fin.close();
    return;
  }

  void tracker_cell_status_service::make_cell_status_map(std::ostream & out_, const time::time_point & t_) const
  {
    const snemo::geometry::locator_plugin & locators
      = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
    const snemo::geometry::gg_locator & ggLocator = locators.geigerLocator();
    unsigned int moduleId = 0;
    unsigned int iSide = 0;
    for (unsigned int iLayer = 0; iLayer < ggLocator.numberOfLayers(iSide); iLayer++) {
      for (unsigned int iRow = 0; iRow < ggLocator.numberOfRows(iSide); iRow++) {
        geomtools::geom_id gid(_cell_type_, moduleId, iSide, iLayer, iRow);
        std::uint32_t status = get_cell_status(gid, t_);
        geomtools::vector_3d cellPos = ggLocator.getCellPosition(iSide, iLayer, iRow);
        out_ << cellPos.y() << ' ' << cellPos.x() << ' ' << status << '\n';
      }
      out_ << '\n';
    }
    out_ << '\n';
    iSide = 1;
    for (unsigned int iLayer = 0; iLayer < ggLocator.numberOfLayers(iSide); iLayer++) {
      for (unsigned int iRow = 0; iRow < ggLocator.numberOfRows(iSide); iRow++) {
        geomtools::geom_id gid(_cell_type_, moduleId, iSide, iLayer, iRow);
        std::uint32_t status = get_cell_status(gid, t_);
        geomtools::vector_3d cellPos = ggLocator.getCellPosition(iSide, iLayer, iRow);
        out_ << cellPos.y() << ' ' << cellPos.x() << ' ' << status << '\n';
      }
      out_ << '\n';
    } 
    out_ << '\n';
    return;
  }
  
  void tracker_cell_status_service::print_tree(std::ostream & out_,
                                               const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    this->base_service::print_tree(out_, base_print_options::force_inheritance(options_));
  
    out_ << popts.indent << i_tree_dumpable::tag
         << "Mode : ";
    if (_mode_ == MODE_FILES) out_ << "'files'";
    else out_ << "'db'";
    out_ << std::endl;
    if (_mode_ == MODE_FILES) {
      out_ << popts.indent << i_tree_dumpable::tag
	   << "File format : " << std::quoted(_file_format_) << std::endl;
    }
  
    out_ << popts.indent << i_tree_dumpable::tag
         << "Geometry label : '" << _geometry_label_ << "'" << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "DB label : '" << _db_label_ << "'" << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "Geometry manager : " << std::boolalpha << (_geomgr_ != nullptr) << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "Cell type : " << _cell_type_ << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "Cell core type : " << _cell_core_type_ << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "DB service : " << std::boolalpha << (_db_service_ != nullptr) << std::endl;
    
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
         << "Histories: " << _histories_.size() << std::endl;

    {
      uint32_t hCount = 0;
      for (const auto & h : _histories_) {
        out_ << popts.indent << i_tree_dumpable::inherit_skip_tag(popts.inherit);
        if (hCount + 1 == _histories_.size()) {
          out_ <<  i_tree_dumpable::last_tag;
        } else {
          out_ <<  i_tree_dumpable::tag;
        } 
        out_ << "History for cell " << h.first << " : #entries=" << h.second.records().size() << std::endl;
        hCount++;
      }
    }

    return;
  }
  
}  // namespace snemo
