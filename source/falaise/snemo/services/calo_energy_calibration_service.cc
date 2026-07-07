// -*- mode: c++ ; -*-
// \file falaise/snemo/services/calo_energy_calibration_service.cc 

// Standard library:
#include <cstdlib>
#include <filesystem>
#include <iomanip>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/json.h>

// Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>

// Ourselves:
#include <falaise/snemo/services/calo_energy_calibration_service.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/processing/event_time_locator.h>
#include <falaise/snemo/datamodels/geomid_utils.h>
#include <falaise/snemo/processing/calo_energy_calibrator_scamp.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::calo_energy_calibration_service,
                                         "snemo::calo_energy_calibration_service")

namespace snemo {

  calo_energy_calibration_service::calo_energy_calibration_service()
    : datatools::base_service("CaloEnergyCalibrationService",
                              "Calorimeter energy calibration service",
                              "Service publishing calorimeter calibration tools")
  {
    return;
  }

  calo_energy_calibration_service::~calo_energy_calibration_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void calo_energy_calibration_service::set_run_info_service(const run_info_service & run_info_service_)
  {
    DT_THROW_IF(not run_info_service_.is_initialized(), std::logic_error,
		"Run info service is not initialized");
    _run_info_service_ = &run_info_service_;
    return;
  }
    
  bool calo_energy_calibration_service::is_initialized() const
  {
    return _initialized_;
  }
 
  int calo_energy_calibration_service::initialize(const datatools::properties & config_,
						  datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    this->datatools::base_service::common_initialize(config_);

    if (config_.has_flag("use_fallback_calibrator")) {
      _use_fallback_calibrator_ = true;
    }
    
    if (config_.has_key("calibrator_type")) {
      std::string calibratorTypeRepr = config_.fetch_string("calibrator_type");
      std::set<std::string> supportedCalibIds
	= processing::calo_energy_calibrator_factory::supported_calibrator_type_ids();
      if (calibratorTypeRepr == "scaled_amp" or calibratorTypeRepr == "scamp") {
	_calibrator_type_ = calibrator_type::scaled_amplitude;
      } else if (calibratorTypeRepr == "charge_pol1" or calibratorTypeRepr == "pol1") {
	_calibrator_type_ = calibrator_type::charge_polynomial_1;
      } else if (calibratorTypeRepr == "charge_pol2" or calibratorTypeRepr == "pol2") {
	_calibrator_type_ = calibrator_type::charge_polynomial_2;
      } else {
	// DT_THROW_IF(supportedCalibIds.count(calibratorTypeRepr) == 0, std::logic_error,
	// 		  "unsupported energy calibrator type " << std::quoted(calibratorTypeRepr));
	DT_THROW(std::logic_error,
		 "unsupported energy calibrator type " << std::quoted(calibratorTypeRepr));
      }
    }
 
    if (config_.has_key("data_access_mode")) {
      std::string dataAccessModeRepr = config_.fetch_string("data_access_mode");
      if (dataAccessModeRepr == "files") {
	_data_access_ = data_access_mode::files;
      } else if (dataAccessModeRepr == "database" or dataAccessModeRepr == "db") {
	_data_access_ = data_access_mode::database;
      } else {
	DT_THROW(std::logic_error, "unsupported data access mode " << std::quoted(dataAccessModeRepr));
      }
    }
    
    DT_THROW_IF(_data_access_ == data_access_mode::undefined, std::logic_error,
		"energy calibration data access mode is not set");
    
    if (_data_access_ == data_access_mode::database) {
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

    if (_data_access_ == data_access_mode::files) {
    
      if (_run_info_service_ == nullptr) {
    
	if (config_.has_key("run_info_label")) {
	  _run_info_label_ = config_.fetch_string("run_info_label");     
	}
	if (_run_info_label_.empty()) {
	  _run_info_label_ = service_info::runInfoServiceName();
	}
	DT_THROW_IF(not datatools::has<snemo::run_info_service>(services_, _run_info_label_),
		    std::logic_error, "No run info service");
	_run_info_service_ = &datatools::get<snemo::run_info_service>(services_, _run_info_label_);
      }
      
      if (config_.has_key("data_access_path")) {
	std::string dataAccessPath = config_.fetch_string("data_access_path");
	_files_data_path_ = dataAccessPath;
      }

      DT_LOG_DEBUG(get_logging_priority(), "Calorimeter energy calibration service data access file path : " << std::quoted(_files_data_path_));

      if (_files_data_path_.empty()) {
	_files_data_path_ = "@falaise:snemo/demonstrator/data/calibration/calorimeter/0.1/energy";
	datatools::fetch_path_with_env(_files_data_path_);
	{
	  static const char * _envVariable = "SNEMO_CALO_ENERGY_CALIBRATION_DATA_PATH";
	  const char * envPathStr = getenv(_envVariable);
	  if (envPathStr != nullptr) {
	    _files_data_path_ = std::string(envPathStr);
	    DT_LOG_DEBUG(get_logging_priority(), "Calorimeter energy calibration service data access file path has been superseded from env : " << std::quoted(_files_data_path_));
	  } 
	}
      }
      _prepare_from_files_();
    }

    if (_use_fallback_calibrator_) {
      auto scampCalibratorHdl = std::make_shared<snemo::processing::calo_energy_calibrator_scamp>();
      scampCalibratorHdl->set_a_params(_fallback_scale_, _fallback_scale_err_);
      _fallback_calibrator_handle_ = scampCalibratorHdl;
    }
     
    _initialized_ = true;
    return 0;
  }

  int calo_energy_calibration_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    _initialized_ = false;
    _run_info_service_ = nullptr;
    return 0;
  }
 
  bool calo_energy_calibration_service::use_fallback_calibrator() const
  {
    return _use_fallback_calibrator_;
  }

  snemo::processing::CaloEnergyCalibratorHdl 
  calo_energy_calibration_service::get_calibrator_handle(const time::time_point & time_point_,
							 const geomtools::geom_id & om_gid_) const
  {
    int omNum = snemo::datamodel::om_num(om_gid_); ;
    return this->get_calibrator_handle(time_point_, omNum);
  }
  
  snemo::processing::CaloEnergyCalibratorHdl 
  calo_energy_calibration_service::get_calibrator_handle(const time::time_point & time_point_,
							 const int om_num_) const
  {
    DT_THROW_IF(not snemo::datamodel::om_num_is_valid(om_num_), std::logic_error,
		  "Invalid OM number " << om_num_);
    DT_THROW_IF(om_num_ >= (int) datamodel::number_of_oms_demo(),
		std::range_error, "invalid demonstrator OM number #" << om_num_);
     snemo::processing::CaloEnergyCalibratorHdl missingCalibrator;
   
    if (_data_access_ == data_access_mode::files) {
      rc::run_id_type runId = _run_info_service_->find_run_id(time_point_);
      DT_THROW_IF(runId == rc::run_description::INVALID_RUN_ID, std::logic_error,
		  "No run was found to access energy calibrator for OM #" << std::to_string(om_num_)
		  << " at time point " << time::to_string(time_point_));
      DT_LOG_DEBUG(get_logging_priority(), "Run ID = " << runId);
      bool mapIsOk = false;
      if (_runs_with_run_based_map_.count(runId)) {
	DT_LOG_DEBUG(get_logging_priority(), "Run-based map");
	if (_current_calib_map_handle_) {
	  if (not mapIsOk and _current_calib_map_handle_->is_target_run()) {
	    rc::run_id_type mapRunId = _current_calib_map_handle_->get_target_run_id();
	    if (mapRunId == runId) {
	      DT_LOG_DEBUG(get_logging_priority(), "Using current run-based map for run ID = " << runId);
	      mapIsOk = true;	
	    }
	  }
	}
      } else {
	if (_current_calib_map_handle_) {
	  DT_LOG_DEBUG(get_logging_priority(), "Phase-based map");
	  rc::run_phase_id_type phaseId = _run_info_service_->find_phase_id(runId);
	  if (not mapIsOk and _current_calib_map_handle_->is_target_phase()) {
	    rc::run_phase_id_type mapPhaseId = _current_calib_map_handle_->get_target_phase_id();
	    DT_THROW_IF(phaseId == rc::run_phase::INVALID_PHASE_ID, std::logic_error,
			"No run phase was found to access energy calibrator for OM #" << std::to_string(om_num_));
	    if (mapPhaseId == phaseId) {
	      DT_LOG_DEBUG(get_logging_priority(), "Using current phase-based map for run ID = " << runId
			   << " in phase ID = " << phaseId);
	      mapIsOk = true;	
	    }
	  }
	}
      }
      if (not mapIsOk) {
	DT_LOG_DEBUG(get_logging_priority(), "Loading map...");
	auto * mutableThis = const_cast<calo_energy_calibration_service*>(this);
	mutableThis->_load_calib_map_(runId);
      }
      auto calibratorHdl = _current_calib_map_handle_->get_calibrator_handle(om_num_);
      if (calibratorHdl) {
	return calibratorHdl;
      }
    }

    DT_THROW_IF(_data_access_ != data_access_mode::database,
		std::logic_error, "invalid data access");

    if (_data_access_ == data_access_mode::database) {
      geomtools::geom_id omGid = datamodel::om_gid(om_num_, false, false);
      history_map_type::const_iterator found = _histories_.find(omGid);
      if (found != _histories_.end()) {
	DT_LOG_DEBUG(get_logging_priority(), "Found history for OM #" << omGid);	
	DT_LOG_DEBUG(get_logging_priority(), "Searching for calibrator at time " << snemo::time::to_string(time_point_));	
	auto calibratorHdl = found->second.get_calibrator_handle(time_point_);
	if (calibratorHdl) {
	  DT_LOG_DEBUG(get_logging_priority(), "Found calibrator for OM #" << omGid);	
	  return calibratorHdl;
	} else {
	  DT_LOG_DEBUG(get_logging_priority(), "Cannot find calibrator for OM #" << omGid);	
	}
      } else {
	DT_LOG_DEBUG(get_logging_priority(), "====> Cannot find history for OM #" << omGid);
      }
    }

    if (_use_fallback_calibrator_) {
      return _fallback_calibrator_handle_;
    }
    
    return missingCalibrator;
  }
    
  bool calo_energy_calibration_service::has_om_history(const geomtools::geom_id & gid_) const
  {
    return _histories_.count(gid_) != 0;
  }

  const calo_energy_calibration_service::history_map_type &
  calo_energy_calibration_service::get_histories() const
  {
    return _histories_;
  }
  
  void calo_energy_calibration_service::_init_mode_db_(const datatools::properties & db_config_)
  {
    datatools::logger::priority logging = get_logging_priority();

    if (db_config_.has_flag("debug")) {
      logging = datatools::logger::PRIO_DEBUG;
    }

    std::string tableName = "OM_Energy_Calib";
    if (db_config_.has_key("table_name")) {
      tableName = db_config_.fetch_string("table_name");
    }
 
    if (_db_service_ == nullptr) {
      DT_THROW(std::logic_error, "DB service nullptr !!!");
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
    snemo::processing::calo_energy_calibrator_factory calibratorFactory;
    
    for (auto iOm = 0u; iOm < snemo::datamodel::number_of_oms(); iOm++) {
      geomtools::geom_id omGid = snemo::datamodel::om_gid(iOm, false, false);
      snemo::processing::om_energy_calibration_history emptyHistory;
      _histories_[omGid] = emptyHistory;
    }
    
    for (auto iRow = 0u; iRow < tableSel.size(); iRow++) {
      
      const snemo::db::record_type row = tableSel[iRow];
      auto colId = 0u;
      // parse columns:
      std::int32_t recordId = -1;
      recordId = (std::uint32_t) std::get<int>(row[colId++]);
      DT_LOG_DEBUG(logging, "  recordId = " << recordId);

      std::int32_t omNum = -1;
      omNum = (std::uint32_t) std::get<int>(row[colId++]);
      DT_THROW_IF(not snemo::datamodel::om_num_is_valid(omNum), std::logic_error, "Invalid OM number " << omNum);
      
      std::string startTimestampStr = std::get<std::string>(row[colId++]);
      DT_LOG_DEBUG(logging, "  startTimestampStr = " << startTimestampStr);
      snt::time_point startTimestamp = snt::time_point_from_string(startTimestampStr);
      DT_LOG_DEBUG(logging, "  startTimestamp = " << snt::to_string(startTimestamp));
       
      std::string stopTimestampStr = std::get<std::string>(row[colId++]);
      DT_LOG_DEBUG(logging, "  stopTimestampStr = " << stopTimestampStr);
      snt::time_point stopTimestamp = snt::time_point_from_string(stopTimestampStr);
      DT_LOG_DEBUG(logging, "  stopTimestamp = " << snt::to_string(stopTimestamp));
        
      std::string calibMethodType = std::get<std::string>(row[colId++]);
      boost::algorithm::trim(calibMethodType);
      DT_LOG_DEBUG(logging, "  calibMethodType = " << calibMethodType);
      int calibMethodVersion = 1;
      std::string calibMethodParamsRepr;
      snemo::processing::calibrator_parameters calibMethodParams;
      if (not calibMethodType.empty()) {
	calibMethodVersion = std::get<int>(row[colId++]);;
	DT_LOG_DEBUG(logging, "  calibMethodVersion = " << calibMethodVersion);
	calibMethodParamsRepr = std::get<std::string>(row[colId++]);
	DT_LOG_DEBUG(logging, "  calibMethodParamsRepr = " << std::quoted(calibMethodParamsRepr));
	if (not calibMethodParamsRepr.empty()) {
	  DT_LOG_DEBUG(logging, "  calib method parameters...");
	  Json::Value jsonRoot;
	  std::istringstream jsonCalibMethodParams(calibMethodParamsRepr);
	  jsonCalibMethodParams >> jsonRoot;
	  if (calibMethodType == "pol1") {
	    double a = jsonRoot["a"].asDouble();
	    double a_err = jsonRoot["a_err"].asDouble();
	    double aUnit = CLHEP::MeV / CLHEP::volt / CLHEP::nanosecond; 
	    calibMethodParams.put("a", a * aUnit);
	    calibMethodParams.put("a_err", a_err * aUnit);
	  } else if (calibMethodType == "pol2") {
	    double aUnit = CLHEP::MeV / CLHEP::volt / CLHEP::nanosecond; 
	    double bUnit = CLHEP::MeV; 
	    double a = jsonRoot["a"].asDouble();
	    double b = jsonRoot["b"].asDouble();
	    double Var_a = jsonRoot["Var_a"].asDouble();
	    double Var_b = jsonRoot["Var_b"].asDouble();
	    double Cov_ab = jsonRoot["Cov_ab"].asDouble();
	    calibMethodParams.put("a", a * aUnit);
	    calibMethodParams.put("b", b * bUnit);
	    calibMethodParams.put("Var_a", Var_a * aUnit * aUnit);
	    calibMethodParams.put("Var_b", Var_b * bUnit * bUnit);
	    calibMethodParams.put("Cov_ab", Cov_ab * aUnit * bUnit);
	  } else {
	    DT_THROW(std::logic_error, "unsupported energy calibration method " << calibMethodType)
	  }
	}
	
      }

      geomtools::geom_id omGid = snemo::datamodel::om_gid(omNum, false, false);      
      DT_LOG_DEBUG(logging, "Add OM energy calibration record: " << omNum);
      DT_LOG_DEBUG(logging, "  GID = " << omGid);
      DT_LOG_DEBUG(logging, "  startTimestamp = " << snt::to_string(startTimestamp));
      DT_LOG_DEBUG(logging, "  stopTimestamp  = " << snt::to_string(stopTimestamp));
      DT_LOG_DEBUG(logging, "  calibMethodType  = " << std::quoted(calibMethodType));
      if (not calibMethodType.empty()) {
	DT_LOG_DEBUG(logging, "  calibMethodVersion = " << calibMethodVersion);
	DT_LOG_DEBUG(logging, "  calibMethodParams  = ");
	if (datatools::logger::is_debug(logging)) {
	  boost::property_tree::write_json(std::cerr, calibMethodParams);
	}
      }
      if (not calibMethodType.empty()) {
	DT_LOG_DEBUG(logging, "Add a record in a the energy calibration for OM #" << omGid);
	snemo::processing::om_energy_calibration_history & omHistory = _histories_.find(omGid)->second;
	snemo::time::time_period omCalibrationPeriod(startTimestamp, stopTimestamp);
	snemo::processing::CaloEnergyCalibratorHdl calibratorHdl = calibratorFactory(calibMethodType, calibMethodParams);
	omHistory.add(omCalibrationPeriod, calibratorHdl);
      }

    }
    
    return;
  }
  
  void calo_energy_calibration_service::_terminate_mode_db_()
  {
    return;
  }

  void calo_energy_calibration_service::_prepare_from_files_()
  {
    std::filesystem::path filesBaseDirPath(_files_data_path_);
    std::string calibratorTypeId;
    if (_calibrator_type_ == calibrator_type::charge_polynomial_1) {
      calibratorTypeId = "charge_pol1";
    } else if (_calibrator_type_ == calibrator_type::charge_polynomial_2) {
      calibratorTypeId = "charge_pol2";
    } else {
      calibratorTypeId = "scaled_amp";
    }
    for (rc::run_id_type runId = 1000; runId < _max_run_id_; runId++) {
      std::filesystem::path perRunFileDirPath;
      std::string fileName = "calibration_run_" + std::to_string(runId) + ".txt";
      perRunFileDirPath = filesBaseDirPath / calibratorTypeId / "runs" / fileName;
      if (std::filesystem::exists(perRunFileDirPath)) {
	_runs_with_run_based_map_.insert(runId);
      }
    }
    for (rc::run_phase_id_type phaseId = 0; phaseId < _max_phase_id_; phaseId++) {
      std::filesystem::path perPhaseFileDirPath;
      std::string fileName = "calibration_phase_" + std::to_string(phaseId) + ".txt";
      perPhaseFileDirPath = filesBaseDirPath / calibratorTypeId / "phases" / fileName;
      if (std::filesystem::exists(perPhaseFileDirPath)) {
	_phases_with_map_.insert(phaseId);
      }
    }
    DT_LOG_DEBUG(get_logging_priority(), "Found " << _runs_with_run_based_map_.size() << " runs with run-based energy calibration data");
    DT_LOG_DEBUG(get_logging_priority(), "Found " << _phases_with_map_.size() << " phases with energy calibration data");
    return;
  }
  
  void calo_energy_calibration_service::_load_calib_map_(const std::int32_t run_id_)
  {
    DT_THROW_IF(_data_access_ != data_access_mode::files, std::logic_error,
		"not using file data access mode");
    if (_current_calib_map_handle_) {
      // should we cache it!
    }
    _current_calib_map_handle_.reset();
    DT_LOG_DEBUG(get_logging_priority(), "Load a new calorimeter energy calibration data map from file...");
    _load_calib_map_from_files_(run_id_);
    return;
  }

  void calo_energy_calibration_service::_load_calib_map_from_files_(const std::int32_t run_id_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Loading calorimeter energy calibration data file for run #"
		 << run_id_ << "...");
    _current_calib_map_handle_ = std::make_shared<processing::calo_energy_calibration_map>();
    std::filesystem::path filesBaseDirPath(_files_data_path_);
    std::filesystem::path perRunFileDirPath;
    std::filesystem::path energyCalibFilePath;
    std::string calibratorTypeId;
    if (_calibrator_type_ == calibrator_type::charge_polynomial_1) {
      calibratorTypeId = "charge_pol1";
    } else if (_calibrator_type_ == calibrator_type::charge_polynomial_2) {
      calibratorTypeId = "charge_pol2";
    } else {
      calibratorTypeId = "scaled_amp";
    }
    if (_runs_with_run_based_map_.count(run_id_) > 0) {
      std::string fileName = "calibration_run_" + std::to_string(run_id_) + ".txt";
      perRunFileDirPath = filesBaseDirPath / calibratorTypeId / "runs" / fileName;
      DT_THROW_IF(not std::filesystem::exists(perRunFileDirPath), std::runtime_error,
		  "Missing calorimeter energy calibration data file " << perRunFileDirPath);
      energyCalibFilePath = perRunFileDirPath;
      _current_calib_map_handle_->set_target_run_id(run_id_);
    } else {
      // Fetch the phase ID:
      rc::run_phase_id_type phaseId = _run_info_service_->find_phase_id(run_id_);
      if (_phases_with_map_.count(phaseId) > 0) {
	DT_LOG_DEBUG(get_logging_priority(), "Found phase ID=" << phaseId << " for run #" << run_id_);
	std::string fileName = "calibration_phase_" + std::to_string(phaseId) + ".txt";
	std::filesystem::path perPhaseFileDirPath
	  = filesBaseDirPath / calibratorTypeId / "phases" / fileName;
	DT_THROW_IF(not std::filesystem::exists(perPhaseFileDirPath), std::runtime_error,
		    "Missing calorimeter energy calibration data file " << perPhaseFileDirPath);
	DT_LOG_DEBUG(get_logging_priority(), "Found a calorimeter energy calibration data file : "
		     << perPhaseFileDirPath);
	energyCalibFilePath = perPhaseFileDirPath;
	_current_calib_map_handle_->set_target_phase_id(phaseId);
      }
    }
    DT_THROW_IF(energyCalibFilePath.empty(), std::logic_error,
		"No file path was found for energy calibration run " << std::to_string(run_id_));
    DT_LOG_DEBUG(get_logging_priority(), "Loading Calorimeter energy calibration data file : "
		 << energyCalibFilePath << "...");
    processing::calo_energy_calibration_data_parser calibDataParser(calibratorTypeId, get_logging_priority());
    calibDataParser.parse(energyCalibFilePath.string(), *_current_calib_map_handle_);
    DT_LOG_DEBUG(get_logging_priority(), "Calorimeter energy calibration data file for run #"
		 << run_id_ << " has been loaded.");
    return;
  }

  void calo_energy_calibration_service::print_tree(std::ostream & out_,
						   const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);

    this->datatools::base_service::print_tree(out_,
					      base_print_options::force_inheritance(options_));
     
    out_ << popts.indent << i_tree_dumpable::tag
	 << "Data access : ";
    if (_data_access_ == data_access_mode::files) {
      out_ << std::quoted("files");
    } else if (_data_access_ == data_access_mode::database) {
      out_ << std::quoted("database");
    } else {
      out_ << "undefined";
    }
    out_ << std::endl;

    if (_data_access_ == data_access_mode::files) {
                                   
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Calibrator type : ";
      if (_calibrator_type_ == calibrator_type::scaled_amplitude) {
	out_ << std::quoted("scaled_amp");
      } else if (_calibrator_type_ == calibrator_type::charge_polynomial_1) {
	out_ << std::quoted("charge_pol1");
      } else if (_calibrator_type_ == calibrator_type::charge_polynomial_2) {
	out_ << std::quoted("charge_pol2");
      } else {
	out_ << "undefined";
      }
      out_ << std::endl;
      
      out_ << popts.indent << i_tree_dumpable::tag
	   << "File data path : "
	   << std::quoted(_files_data_path_) << std::endl;
                               
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Run info service label  : "
	   << std::quoted(_run_info_label_) << std::endl;
                                     
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Max run ID : "
	   << _max_run_id_ << std::endl;
                                      
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Max phase ID : "
	   << _max_phase_id_ << std::endl;
                                      
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Run info service ID : " << std::boolalpha << (_run_info_service_ != nullptr ? "yes" : "none")
	   << std::endl;
                                      
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Runs with run based map : " << _runs_with_run_based_map_.size() << std::endl;
                                      
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Phases with map : " << _phases_with_map_.size() << std::endl;
                                  
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Current energy calibration map : "
	   << std::boolalpha << (_current_calib_map_handle_.get() != nullptr ? "yes" : "none") << std::endl;

    } else if (_data_access_ == data_access_mode::database) {

      out_ << popts.indent << i_tree_dumpable::tag
	   << "DB label : " << std::quoted(_db_label_) << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
	   << "DB service : " << _db_service_ << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
	   << "OM energy calibration histories : " << std::endl;
      {
	auto historyCount = 0u;
	for (const auto & entry : _histories_) {
	  const auto & omEcalibHistory = entry.second;
	  if (omEcalibHistory.records().size()) {
	    out_ << popts.indent << i_tree_dumpable::skip_tag << "|   "
		 << "History of OM #" << entry.first << " : #records=" << omEcalibHistory.records().size() << std::endl;
	  }
	  historyCount++;
	}
      }

      
    }
 
    if (_use_fallback_calibrator_) {
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Fallback scale amplitude factor : "
	   << _fallback_scale_ << " +/- " << _fallback_scale_err_ << std::endl;
    }
 
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
	   << "Use fallback calibrator  : "
	   << std::boolalpha << _use_fallback_calibrator_ << std::endl;
	
    return;
  }
  
} // namespace snemo
