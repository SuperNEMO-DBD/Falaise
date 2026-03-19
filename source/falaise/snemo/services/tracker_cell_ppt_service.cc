
// Ourselves:
#include <falaise/snemo/services/tracker_cell_ppt_service.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/datamodels/geomid_utils.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::tracker_cell_ppt_service,
                                         "snemo::tracker_cell_ppt_service")

namespace snemo {

  tracker_cell_ppt_service::tracker_cell_ppt_service()
    : datatools::base_service("TrackerCellPptService",
                              "Tracker cell PPT service",
                              "Service publishing tracker cell PPT")
  {
    return;
  }

  tracker_cell_ppt_service::~tracker_cell_ppt_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  int tracker_cell_ppt_service::initialize(const datatools::properties & config_,
                                              datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    this->datatools::base_service::common_initialize(config_);

    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "db") {
        _mode_ = MODE_DB;
      } else if (modeLabel == "fallback") {
        _mode_ = MODE_FALLBACK;
      } else {
        DT_THROW(std::logic_error, "Invalid mode label '" << modeLabel << "'!");
      }
    }

    // We need the tracker cell status service:
    if (config_.has_key("tracker_cell_status_label")) {
      _tracker_cell_status_label_ = config_.fetch_string("tracker_cell_status_label");     
    }
    if (_tracker_cell_status_label_.empty()) {
      _tracker_cell_status_label_ = service_info::trackerCellStatusServiceName();
    }
    if (datatools::has<snemo::tracker_cell_status_service>(services_, _tracker_cell_status_label_)) {
      _tracker_cell_status_service_ = &datatools::get<snemo::tracker_cell_status_service>(services_, _tracker_cell_status_label_);
    }

    if (_mode_ == MODE_DB) {
      // We need the DB service:
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

  int tracker_cell_ppt_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    } else if (_mode_ == MODE_FALLBACK) {
    }
    _histories_.clear();
    
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  bool tracker_cell_ppt_service::is_initialized() const
  {
    return _initialized_;
  }

  void tracker_cell_ppt_service::load_cell_ppt_map(const std::string & infile_)
  {
    if (_file_format_ == "csv-1") {
      _load_cell_ppt_map_1_(infile_);
    }
    return;
  }

  void tracker_cell_ppt_service::_load_cell_ppt_map_1_(const std::string & infile_)
  {
    // const snemo::geometry::locator_plugin & locators
    //   = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
    // const snemo::geometry::gg_locator & ggLocator = locators.geigerLocator();
    namespace snrc = snemo::rc;
    namespace snt  = snemo::time;
    
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open tracker cell PPT file '" << filename << "'!");
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
	std::string cellIdRepr;
	std::string periodStartRepr; 
	std::string periodStopRepr; 
	std::string pptTypeRepr;
	std::string pptRepr;
	std::string pptErrorRepr;
	int tkCount = 0;
	for (std::string tk : tokens) {
	  DT_LOG_DEBUG(get_logging_priority(), "tk " << std::quoted(tk));
	  if (cellIdRepr.empty()) {
	    DT_THROW_IF(tkCount != 0, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    cellIdRepr = tk;
	  } else if (periodStartRepr.empty()) {
	    DT_THROW_IF(tkCount != 1, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    periodStartRepr = tk;
	  } else if (periodStopRepr.empty()) {
	    DT_THROW_IF(tkCount != 2, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    periodStopRepr = tk;
	  } else if (pptTypeRepr.empty()) {
	    DT_THROW_IF(tkCount != 3, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    pptTypeRepr = tk;
	  } else if (pptRepr.empty()) {
	    DT_THROW_IF(tkCount != 4, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    pptRepr = tk;
	  } else if (pptErrorRepr.empty()) {
	    DT_THROW_IF(tkCount != 5, std::logic_error, "Invalid formatted tracker cell PPT record!");
	    pptErrorRepr = tk;
	  } else {
	    DT_THROW(std::logic_error, "Invalid token '" << tk << "'!");
	  }
	  tkCount++;
	  if (tkCount == 6) break;
	}
	DT_LOG_DEBUG(get_logging_priority(), "Using fields : "
		     << cellIdRepr
		     << ';' << periodStartRepr
		     << ';' << periodStopRepr
		     << ';' << pptTypeRepr
		     << ';' << pptRepr
		     << ';' << pptErrorRepr);

	// Cell number:
	geomtools::geom_id cellGid;
	boost::trim(cellIdRepr);
	DT_THROW_IF(cellIdRepr.empty(), std::logic_error, "Missing tracker cell ID!");
	if (cellIdRepr[0] == '[') {
	  std::istringstream gidss(cellIdRepr);
	  gidss >> cellGid;
	  DT_THROW_IF(!gidss, std::logic_error, "Cannot decode missing tracker cell geom ID!");
	  DT_THROW_IF(not cellGid.is_valid(), std::logic_error,
		      "Invalid tracker cell geom ID pattern parsed from '" << cellIdRepr  << "'!");
	  DT_THROW_IF(cellGid.get_type() != _cell_core_type_, std::logic_error,
		      "Invalid type for tracker cell geom ID pattern '" << cellGid
		      << "' with expected cell type=" << _cell_type_ << "'!");
	  // DT_THROW_IF(not ggLocator.isGeigerCell(cellGid), std::logic_error,
	  // 	      "Token '" << idRepr << "' is not a valid tracker cell geom ID pattern!");
	} else {
	  int cellId = std::stoi(cellIdRepr);
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

	// PPT type:
	boost::trim(pptTypeRepr);
	snrc::tracker_cell_ppt_type pptType = snrc::tracker_cell_ppt_type::inoperative;
	if (pptTypeRepr == "normal") {
	  pptType = snrc::tracker_cell_ppt_type::normal;
	} else if (pptTypeRepr == "missing_bottom") {
	  pptType = snrc::tracker_cell_ppt_type::missing_bottom;
	} else if (pptTypeRepr == "missing_top") {
	  pptType = snrc::tracker_cell_ppt_type::missing_top;
	} else if (pptTypeRepr == "inoperative") {
	  pptType = snrc::tracker_cell_ppt_type::inoperative;
	} else {
	  DT_THROW(std::logic_error,
		   "Invalid tracker cell PPT type '" << pptTypeRepr  << "'!");
	}

	snrc::tracker_cell_ppt_info pptInfo;
	pptInfo.type = pptType;
	if (pptType != snrc::tracker_cell_ppt_type::inoperative) {
	  // Field #5:
	  std::string pptStr = pptRepr;
	  boost::algorithm::trim(pptStr);
	  // Field #6:
	  std::string pptErrorStr = pptErrorRepr;
	  boost::algorithm::trim(pptErrorStr);
	  pptInfo.ppt = std::stof(pptStr) * CLHEP::ns;
	  pptInfo.ppt_error = std::stof(pptErrorStr) * CLHEP::ns;
	}
	
	snrc::tracker_cell_ppt_history & ggPptHistory = this->grab_cell_history(cellGid);
	ggPptHistory.add(period, pptInfo);
	DT_LOG_DEBUG(get_logging_priority(),
		     "gid=" << cellGid << " PPT record=" << ggPptHistory.records().back());
      }
    }
    return;
  }

  const snemo::rc::tracker_cell_ppt_history_map &
  tracker_cell_ppt_service::get_histories() const
  {
    return _histories_;
  }
 
  bool tracker_cell_ppt_service::has_cell_history(const geomtools::geom_id & gid_) const
  {
    return _histories_.count(gid_) != 0;
  }

  snemo::rc::tracker_cell_ppt_history &
  tracker_cell_ppt_service::grab_cell_history(const geomtools::geom_id & gid_)
  {
    if (not has_cell_history(gid_)) {
      snemo::rc::tracker_cell_ppt_history emptyHistory;
      _histories_[gid_] = emptyHistory;
    }
    snemo::rc::tracker_cell_ppt_history_map::iterator found = _histories_.find(gid_);
    return found->second;
  }

  const snemo::rc::tracker_cell_ppt_history &
  tracker_cell_ppt_service::get_cell_history(const geomtools::geom_id & gid_) const
  {
    snemo::rc::tracker_cell_ppt_history_map::const_iterator found = _histories_.find(gid_);
    DT_THROW_IF(found == _histories_.end(), std::logic_error, "No PPT history for cell '" << gid_ << "'!");
    return found->second;
  }
  
  snemo::rc::tracker_cell_ppt_info
  tracker_cell_ppt_service::get_cell_ppt_info(const geomtools::geom_id & gid_,
					      const time::time_point & time_) const
  {
    snemo::rc::tracker_cell_ppt_info pptInfo;
    if (_mode_ == MODE_FALLBACK) {
      pptInfo.type = snemo::rc::tracker_cell_ppt_type::normal;
      pptInfo.ppt = 55.525 * CLHEP::microsecond;
      pptInfo.ppt_error = 3.0 * CLHEP::microsecond;
    } else {
      pptInfo.type = snemo::rc::tracker_cell_ppt_type::inoperative;
      snemo::rc::tracker_cell_ppt_history_map::const_iterator found = _histories_.find(gid_);
      if (found != _histories_.end()) {
	DT_LOG_DEBUG(get_logging_priority(), "Found PPT history for cell " << gid_);
	if (datatools::logger::is_debug(get_logging_priority())) {
	  found->second.print(std::cerr, "[debug] ");
	}
	pptInfo = found->second.get_ppt_info(time_);
      }
    }
    return pptInfo;
  }

  void tracker_cell_ppt_service::_init_mode_db_(const datatools::properties & db_config_)
  {
   datatools::logger::priority logging = get_logging_priority();
    logging = datatools::logger::PRIO_DEBUG;

    if (_db_service_ == nullptr) {
      DT_THROW(std::logic_error, "DB service nullptr !!!");
    }

    std::string tableName = "GG_PPT";
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
    static const auto NBCELLS = 2034u;
    std::vector<snrc::tracker_cell_ppt_record> cellPptRecordsLists[NBCELLS];
    for (auto iCell = 0u; iCell < NBCELLS; iCell++) {
      cellPptRecordsLists[iCell].reserve(10000);
    }
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      const snemo::db::record_type row = tableSel[iRow];

      // Field #0:
      std::int32_t recordId = -1;
      recordId = (std::uint32_t) std::get<int>(row[0]);
      DT_LOG_DEBUG(logging, "GG PPT record #" << std::to_string(recordId));

      // Field #1:
      std::int32_t ggNum = -1;
      ggNum = (std::uint32_t) std::get<int>(row[1]);
      DT_THROW_IF(not snemo::datamodel::gg_num_is_valid(ggNum), std::logic_error, "Invalid GG cell number");
      
      // Field #2:
      std::string startTimestampStr = std::get<std::string>(row[2]);
      DT_LOG_DEBUG(logging, "  startTimestampStr = " << startTimestampStr);
      snt::time_point startTimestamp = snt::time_point_from_string(startTimestampStr);
      DT_LOG_DEBUG(logging, "  startTimestamp = " << snt::to_string(startTimestamp));
     
      // Field #3:
      std::string stopTimestampStr = std::get<std::string>(row[3]);
      DT_LOG_DEBUG(logging, "  stopTimestampStr = " << stopTimestampStr);
      snt::time_point stopTimestamp = snt::time_point_from_string(stopTimestampStr);
      DT_LOG_DEBUG(logging, "  stopTimestamp = " << snt::to_string(stopTimestamp));
       
      // Field #4:
      std::string pptTypeStr = std::get<std::string>(row[4]);
      boost::algorithm::trim(pptTypeStr);
      DT_LOG_DEBUG(logging, "  pptTypeStr = " << pptTypeStr);
      snrc::tracker_cell_ppt_type pptType = snrc::tracker_cell_ppt_type::inoperative;
      if (pptTypeStr == "normal") {
	pptType = snrc::tracker_cell_ppt_type::normal;
      } else if (pptTypeStr == "missing_bottom") {
	pptType = snrc::tracker_cell_ppt_type::missing_bottom;
      } else if (pptTypeStr == "missing_top") {
	pptType = snrc::tracker_cell_ppt_type::missing_top;
      } else if (pptTypeStr == "inoperative") {
	pptType = snrc::tracker_cell_ppt_type::inoperative;
      } else {
	DT_THROW(std::logic_error,
		 "Invalid tracker cell PPT type '" << pptTypeStr  << "'!");
      }

      snrc::tracker_cell_ppt_record pptRecord;
      pptRecord.period = time::time_period(startTimestamp, stopTimestamp);
      pptRecord.info.type = pptType;
      
      if (pptType != snrc::tracker_cell_ppt_type::inoperative) {
	// Field #5:
	std::string pptStr = std::get<std::string>(row[5]);
	boost::algorithm::trim(pptStr);
	// Field #6:
 	std::string pptErrorStr = std::get<std::string>(row[6]);
	boost::algorithm::trim(pptErrorStr);
	pptRecord.info.ppt = std::stof(pptStr) * CLHEP::ns;
	pptRecord.info.ppt_error = std::stof(pptErrorStr) * CLHEP::ns;
      }
      if (cellPptRecordsLists[ggNum].size()) {
	const auto & lastRecord = cellPptRecordsLists[ggNum].back();
	if (pptRecord.period.begin() <= lastRecord.period.last()) {
	  DT_THROW(std::range_error, "Overlapping or PPT time periods mis-ordering for tradker cell #" << ggNum);
	}
      } 
      cellPptRecordsLists[ggNum].push_back(pptRecord);
 	
    }
    
    // Build GG PPT histories from parsed lists:
    for (auto ggNum = 0u; ggNum < snemo::datamodel::number_of_ggs(); ggNum++) {
      const auto & pptRecLists = cellPptRecordsLists[ggNum];
      if (pptRecLists.size() > 0) {
	geomtools::geom_id ggGid = snemo::datamodel::gg_gid(ggNum);
	snrc::tracker_cell_ppt_history & ggPptHistory = this->grab_cell_history(ggGid);
	ggPptHistory.set(pptRecLists);
      }
    }

    DT_LOG_DEBUG(logging, "#GG PPT history: " << _histories_.size());
    return;
  }

  void tracker_cell_ppt_service::_terminate_mode_db_()
  {
    return;
  }

  void tracker_cell_ppt_service::_init_mode_files_(const datatools::properties & config_)
  {
    std::vector<std::string> cellMaps;
    if (config_.has_key("cell_maps")) {
      config_.fetch("cell_maps", cellMaps);
    }
    if (config_.has_key("map_format")) {
      std::string fileMapFormat = config_.fetch_string("map_format");
      if (fileMapFormat == "csv-1") {
	_file_format_ = fileMapFormat;
      } else {
	DT_THROW(std::logic_error, "Invalid map format " << std::quoted(fileMapFormat));
      }
    }
    for (const auto & cellMap: cellMaps) {
      DT_LOG_DEBUG(get_logging_priority(), "Loading cell map : " << cellMap);
      load_cell_ppt_map(cellMap);
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

  void tracker_cell_ppt_service::_terminate_mode_files_()
  {
    return;
  }

  void tracker_cell_ppt_service::print_tree(std::ostream & out_,
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
         << "DB label : '" << _db_label_ << "'" << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "Tracker cell status label : '" << _tracker_cell_status_label_ << "'" << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "Tracker cell status service : " << std::boolalpha << (_tracker_cell_status_service_ != nullptr) << std::endl;
 
    out_ << popts.indent << i_tree_dumpable::tag
         << "Cell type : " << _cell_type_ << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "Cell core type : " << _cell_core_type_ << std::endl;

    out_ << popts.indent << i_tree_dumpable::tag
         << "DB service : " << std::boolalpha << (_db_service_ != nullptr) << std::endl;

    // XXX
    
    return;
  }
  
} // namespace snemo
