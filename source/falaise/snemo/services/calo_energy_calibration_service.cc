// -*- mode: c++ ; -*-
// \file falaise/snemo/services/calo_energy_calibration_service.cc 

// Standard library:
#include <cstdlib>
#include <filesystem>
#include <iomanip>

// Ourselves:
#include <falaise/snemo/services/calo_energy_calibration_service.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/processing/event_time_locator.h>

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
    
    if (_run_info_service_ == nullptr) {
    
      if (config_.has_key("run_info_label")) {
	_run_info_label_ = config_.fetch_string("run_info_label");     
      }
      if (_run_info_label_.empty()) {
	_run_info_label_ = service_info::runInfoServiceName();
      }
      DT_THROW_IF(not datatools::has<snemo::run_info_service>(services_, _run_info_label_),
		  std::logic_error,
		  "No run info service");
      _run_info_service_ = &datatools::get<snemo::run_info_service>(services_,
								  _run_info_label_);
    }
    
    if (config_.has_key("calibrator_type")) {
      std::string calibratorTypeRepr = config_.fetch_string("calibrator_type");
      std::set<std::string> supportedCalibIds
	= processing::calo_energy_calibrator_factory::supported_calibrator_type_ids();
      DT_THROW_IF(supportedCalibIds.count(calibratorTypeRepr) == 0, std::logic_error,
		  "unsupported energy calibrator type " << std::quoted(calibratorTypeRepr));
      if (calibratorTypeRepr == "scaled_amp") {
	_calibrator_type_ = calibrator_type::scaled_amplitude;
      } else if (calibratorTypeRepr == "charge_pol1") {
	_calibrator_type_ = calibrator_type::charge_polynomial_1;
      } else if (calibratorTypeRepr == "charge_pol2") {
	_calibrator_type_ = calibrator_type::charge_polynomial_2;
      }
    }
 
    if (config_.has_key("data_access_mode")) {
      std::string dataAccessModeRepr = config_.fetch_string("data_access_mode");
      if (dataAccessModeRepr == "files") {
	_data_access_ = data_access_mode::files;
      } else if (dataAccessModeRepr == "database") {
	_data_access_ = data_access_mode::database;
      } else {
	DT_THROW(std::logic_error, "unsupported data access mode " << std::quoted(dataAccessModeRepr));
      }
    }
    
    DT_THROW_IF(_data_access_ == data_access_mode::undefined, std::logic_error,
		"energy calibration data access mode is not set");
    
    if (_data_access_ == data_access_mode::database) {
      DT_THROW(std::logic_error, "database mode is not implemented yet");     
    } 

    if (_data_access_ == data_access_mode::files) {
      if (config_.has_key("data_access_path")) {
	std::string dataAccessPath = config_.fetch_string("data_access_path");
	_files_data_path_ = dataAccessPath;
      }
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
    }

    DT_LOG_DEBUG(get_logging_priority(), "Calorimeter energy calibration service data access file path : " << std::quoted(_files_data_path_));
     if (_data_access_ == data_access_mode::files) {
       _prepare_from_files_();
     } else if (_data_access_ == data_access_mode::database) {
       _prepare_from_database_();
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
    
  const snemo::processing::calo_energy_calibrator &
  calo_energy_calibration_service::get_calibrator(const time::time_point & time_point_,
						  const int om_num_) const
  {
    DT_THROW_IF(om_num_ < 0 or om_num_ > (int) processing::calo_energy_calibration_map::NB_OMS_SNDEMO,
		std::range_error, "invalid OM number #" << om_num_);
    static const snemo::processing::scaled_amplitude_calo_energy_calibrator _fallbackCalibrator(_fallback_scale_);
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
    const processing::CaloEnergyCalibrator & energyCalibrator
      = _current_calib_map_handle_->get_calibrator(om_num_);
    return energyCalibrator;
  }

  // DT_THROW_IF(not use_fallback_calibrator(), std::logic_error,
  // 		  "No energy calibrator for OM #" << std::to_string(om_num_));
  // return _fallbackCalibrator;

  void calo_energy_calibration_service::_prepare_from_database_()
  {
    DT_THROW(std::logic_error, "not implemented yet");
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
    if (_current_calib_map_handle_) {
      // should we cache it!
    }
    _current_calib_map_handle_.reset();
    if (_data_access_ == data_access_mode::files) {
      DT_LOG_DEBUG(get_logging_priority(), "Load a new calorimeter energy calibration data map from file...");
      _load_calib_map_from_files_(run_id_);
    } else {
      DT_THROW(std::logic_error, "calibration data access mode is not supported");
    }
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
	 << "Data access : ";
    if (_data_access_ == data_access_mode::files) {
      out_ << std::quoted("files");
    } else if (_data_access_ == data_access_mode::database) {
      out_ << std::quoted("database");
    } else {
      out_ << "undefined";
    }
    out_ << std::endl;
    
    out_ << popts.indent << i_tree_dumpable::tag
	 << "File data path : "
	 << std::quoted(_files_data_path_) << std::endl;
                                     
    out_ << popts.indent << i_tree_dumpable::tag
	 << "Use fallback calibrator  : "
	 << std::boolalpha << _use_fallback_calibrator_ << std::endl;

    if (_use_fallback_calibrator_) {
      out_ << popts.indent << i_tree_dumpable::tag
	   << "Fallback scale amplitude factor : "
	   << std::boolalpha << _use_fallback_calibrator_ << std::endl;
    }
                                     
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
                                  
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
	 << "Current energy calibration map : "
	 << std::boolalpha << (_current_calib_map_handle_.get() != nullptr ? "yes" : "none") << std::endl;
   
    return;
  }
  
} // namespace snemo
