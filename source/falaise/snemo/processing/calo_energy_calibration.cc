// -*- mode: c++ ; -*-
/// \file falaise/snemo/processing/calo_energy_calibration.cc

// Ourselves:
#include "calo_energy_calibration.h"

// Standard library:
#include <fstream>


// Third party:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/snemo/datamodels/geomid_utils.h>

namespace snemo {

  namespace processing {

    /***** calo_energy *****/
    
    bool calo_energy::has_value() const
    {
      return datatools::is_valid(value);
    }
    
    bool calo_energy::has_sigma() const
    {
      return datatools::is_valid(sigma);
    }
    
    void calo_energy::reset()
    {
      value = datatools::invalid_real();
      sigma = datatools::invalid_real();
      return;
    }

    /***** scaled_amplitude_calo_energy_calibrator *****/

    scaled_amplitude_calo_energy_calibrator::scaled_amplitude_calo_energy_calibrator()
    {
      return;
    }

    scaled_amplitude_calo_energy_calibrator::scaled_amplitude_calo_energy_calibrator(const double scale_)
    {
      set_scale(scale_);
      return;
    }

    void scaled_amplitude_calo_energy_calibrator::configure(const calibrator_parameters & parameters_)
    {
      // double scaleValue = parameters_.get("scale", 5.0 * CLHEP::MeV / CLHEP::volt); 
      double scaleValue = parameters_.get<double>("scale");
      set_scale(scaleValue);
      return;
    }
 
    void scaled_amplitude_calo_energy_calibrator::set_scale(const double scale_)
    {
      DT_THROW_IF(scale_ <= 0.0, std::logic_error, "Invalid amplitude scale factor");
      _scale_ = scale_;
      return;
    }
  
    double scaled_amplitude_calo_energy_calibrator::get_scale() const
    {
      return _scale_;
    }
 
    calo_energy
    scaled_amplitude_calo_energy_calibrator::operator()
      (const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_amplitude()) {
	auto signalAmplitude = pcd_calo_hit_.get_amplitude();
	double hitEnergy = -signalAmplitude * _scale_;
	calibratedEnergy.value = hitEnergy;
	if (pcd_calo_hit_.has_sigma_amplitude()) {
	  auto signalAmplitudeSigma = pcd_calo_hit_.get_sigma_amplitude();
	  double hitEnergySigma = signalAmplitudeSigma * _scale_;
	  calibratedEnergy.sigma = hitEnergySigma;
	}
      }
      return calibratedEnergy;
    }

    /***** charge_pol1_calo_energy_calibrator *****/
   
    charge_pol1_calo_energy_calibrator::charge_pol1_calo_energy_calibrator()
    {
      return;
    }

    charge_pol1_calo_energy_calibrator::charge_pol1_calo_energy_calibrator(const double constant_)
    {
      set_constant(constant_);
      return;
    }

    void charge_pol1_calo_energy_calibrator::set_constant(const double constant_)
    {
      DT_THROW_IF(constant_ <= 0.0, std::logic_error, "Invalid charge constant factor");
      _constant_ = constant_;
      return;
    }

    double charge_pol1_calo_energy_calibrator::get_constant() const
    {
      return _constant_;
    }
 
    void charge_pol1_calo_energy_calibrator::configure(const calibrator_parameters & parameters_)
    {
      double constant = parameters_.get<double>("constant");
      set_constant(constant);
      return;
    }
 
    calo_energy
    charge_pol1_calo_energy_calibrator::operator()
      (const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_charge()) {
	auto signalCharge = pcd_calo_hit_.get_charge();
	double hitEnergy = -signalCharge * _constant_;
	calibratedEnergy.value = hitEnergy;
	if (pcd_calo_hit_.has_sigma_charge()) {
	  auto signalChargeSigma = pcd_calo_hit_.get_sigma_charge();
	  double hitEnergySigma = signalChargeSigma * _constant_;
	  calibratedEnergy.sigma = hitEnergySigma;
	}
      }
      return calibratedEnergy;
    }

    /***** charge_pol2_calo_energy_calibrator *****/
  
    charge_pol2_calo_energy_calibrator::charge_pol2_calo_energy_calibrator()
    {
      return;
    }

    charge_pol2_calo_energy_calibrator::charge_pol2_calo_energy_calibrator(const double constant0_,
									   const double constant1_)
    {
      set_constant0(constant0_);
      set_constant1(constant1_);
      return;
    }
 
    void charge_pol2_calo_energy_calibrator::set_constant1(const double constant1_)
    {
      DT_THROW_IF(constant1_ <= 0.0, std::logic_error, "Invalid charge constant1");
      _constant1_ = constant1_;
      return;
    }
    void charge_pol2_calo_energy_calibrator::set_constant0(const double constant0_)
    {
      _constant0_ = constant0_;
      return;
    }

    double charge_pol2_calo_energy_calibrator::get_constant1() const
    {
      return _constant1_;
    }

    double charge_pol2_calo_energy_calibrator::get_constant0() const
    {
      return _constant0_;
    }
 
   void charge_pol2_calo_energy_calibrator::configure(const calibrator_parameters & parameters_)
    {
      double constant1 = parameters_.get<double>("constant1");
      double constant0 = parameters_.get<double>("constant0");
      set_constant1(constant1);
      set_constant0(constant0);
      return;
    }
 
    calo_energy
    charge_pol2_calo_energy_calibrator::operator()
      (const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_charge()) {
	auto signalCharge = pcd_calo_hit_.get_charge();
	double hitEnergy = -signalCharge * _constant1_ + _constant0_;
	calibratedEnergy.value = hitEnergy;
	if (pcd_calo_hit_.has_sigma_charge()) {
	  auto signalChargeSigma = pcd_calo_hit_.get_sigma_charge();
	  double hitEnergySigma = signalChargeSigma * _constant1_;
	  calibratedEnergy.sigma = hitEnergySigma;
	}
      }
      return calibratedEnergy;
    }

    /***** calo_energy_calibration_map *****/
 
    calo_energy_calibration_map::calo_energy_calibration_map()
    {
      _calibrators_.assign(NB_OMS, nullptr); // fill with empty calibrators...
      return;
    }

    rc::run_id_type calo_energy_calibration_map::get_target_run_id() const
    {
      DT_THROW_IF(not is_target_run(), std::logic_error, "not a run target");
      return _target_run_id_;
    }
  
    rc::run_phase_id_type calo_energy_calibration_map::get_target_phase_id() const
    {
      DT_THROW_IF(not is_target_phase(), std::logic_error, "not a phase target");
      return _target_phase_id_;
    }
      
    void calo_energy_calibration_map::set_target_run_id(const rc::run_id_type target_run_id_)
    {
      _target_run_id_ = target_run_id_;
      _target_phase_id_ = rc::run_phase::INVALID_PHASE_ID;
      return;
    }

    void calo_energy_calibration_map::set_target_phase_id(const rc::run_phase_id_type target_phase_id_)
    {
      _target_run_id_ = rc::run_description::INVALID_RUN_ID;
      _target_phase_id_ = target_phase_id_;
      return;
    }

    bool calo_energy_calibration_map::is_target_run() const
    {
      return _target_run_id_ != rc::run_description::INVALID_RUN_ID;
    }

    bool calo_energy_calibration_map::is_target_phase() const
    {
      return _target_phase_id_ != rc::run_phase::INVALID_PHASE_ID;
    }

    bool calo_energy_calibration_map::has_calibrator(const int om_num_) const
    {
      DT_THROW_IF((om_num_ < 0) or (om_num_ >= (int)_calibrators_.size()),
		  std::range_error, "Invalid OM number");
      return _calibrators_[om_num_].get() != nullptr;
    }
    
    void calo_energy_calibration_map::add_calibrator(const int om_num_,
						     const CaloEnergyCalibratorHdl & calibrator_handle_)
    {
      _calibrators_[om_num_] = calibrator_handle_;
      return;
    }
      
    const CaloEnergyCalibrator &
    calo_energy_calibration_map::get_calibrator(const int om_num_) const
    {
      return *_calibrators_[om_num_];
    }
      
    const CaloEnergyCalibrator &
    calo_energy_calibration_map::get_calibrator(const geomtools::geom_id & calo_gid_) const
    {
      const int caloOmNum = snemo::datamodel::om_num(calo_gid_); 
      return get_calibrator(caloOmNum);
    }

    void calo_energy_calibration_map::reset_calibrators()
    {
      _calibrators_.assign(NB_OMS, nullptr); // fill with empty calibrators...
      return;
    }
 
    /***** calo_energy_calibrator_factory *****/

    // static
    std::set<std::string> calo_energy_calibrator_factory::supported_calibrator_type_ids()
    {
      static const std::set<std::string> _supCalTypeIds = {"scaled_amp", "charge_pol1", "charge_pol2"};
      return _supCalTypeIds;
    }
     
    std::unique_ptr<CaloEnergyCalibrator>
    calo_energy_calibrator_factory::operator()(const std::string & calibrator_type_id_,
					       const boost::property_tree::ptree & parameters_)
    {
      std::unique_ptr<CaloEnergyCalibrator> calibratorHdl;
      if (calibrator_type_id_ == "scaled_amp") {
	calibratorHdl = std::make_unique<scaled_amplitude_calo_energy_calibrator>();
      } else if (calibrator_type_id_ == "charge_pol1") {
	calibratorHdl = std::make_unique<charge_pol1_calo_energy_calibrator>();
      } else if (calibrator_type_id_ == "charge_pol2") {
	calibratorHdl = std::make_unique<charge_pol2_calo_energy_calibrator>();
      } else {
	DT_THROW(std::logic_error," unsupported calorimeter energy calibrator '" << calibrator_type_id_ << "'");
      }
      calibratorHdl->configure(parameters_);
      return calibratorHdl;
    }
      
    /***** calo_energy_calibration_data_parser *****/

    calo_energy_calibration_data_parser::calo_energy_calibration_data_parser(const std::string & calibrator_type_id_,
									     const datatools::logger::priority verbosity_)
      : _calibrator_type_id_(calibrator_type_id_)
      , _verbosity_(verbosity_)
    {
      DT_THROW_IF(not calo_energy_calibrator_factory::supported_calibrator_type_ids().count(calibrator_type_id_),
		  std::logic_error,
		  "unsupported calorimeter energy calibrator type " << std::quoted(calibrator_type_id_));
      return;
    }

    void calo_energy_calibration_data_parser::parse(const std::string & filepath_, calo_energy_calibration_map & map_)
    {
      map_.reset_calibrators();
      // Requested calibrator type ID:
      std::string calibratorTypeId = _calibrator_type_id_;
      std::ifstream ifile(filepath_);
      processing::calo_energy_calibrator_factory caloEnergyCalibratorFactory;
      std::string parsedCalibratorTypeId;
      rc::run_id_type parsedRunId = -1;
      rc::run_phase_id_type parsedPhaseId = -1;
      // Special header tag keys (format "#@key=value"):
      static const std::string calibTypeKey = "calibrator-type";
      static const std::string nbColumnsKey = "number-of-columns";
      static const std::string phaseIdKey = "phase-id";
      static const std::string runIdKey = "run-id";
      std::size_t nbColumns = 0;
      bool parsingData = false;
      int lineCounter = 0;
      std::string rawLine;
      while (std::getline(ifile, rawLine)) {
	lineCounter++;
	std::string line = boost::trim_copy(rawLine);
	if (line.empty()) { // skip empty lines
	  continue;
	}
	if (line[0] == '#') {
	  if (line.size() > 2 and line.substr(0,2) == "#@") {
	    DT_THROW_IF(parsingData, std::logic_error, "no header tag is allowed while parsing calibration data");
	    // Processing special header tags with format : "#@key=value"
	    std::string keyValueLine = line.substr(2);
	    std::vector<std::string> keyValueTokens;
	    boost::algorithm::split(keyValueTokens, keyValueLine, boost::is_any_of("="), boost::token_compress_on);
	    if (keyValueTokens.size() != 2) {
	      DT_THROW(std::logic_error, "unsupported tag format in " << std::quoted(line) << " at line " << lineCounter);
	    }
	    std::string keyToken = keyValueTokens[0];
	    std::string valueToken = keyValueTokens[1];
	    DT_LOG_DEBUG(_verbosity_, "processing special header tag " << std::quoted(keyToken) << " at line " << lineCounter);
	    if (keyToken == calibTypeKey) {
	      DT_THROW_IF(not parsedCalibratorTypeId.empty(), std::logic_error,
			  "tag " << std::quoted(calibTypeKey) << " is already processed at line " << lineCounter);
	      parsedCalibratorTypeId = valueToken;
	      DT_THROW_IF(parsedCalibratorTypeId != calibratorTypeId, std::logic_error,
			  "tag " << std::quoted(calibTypeKey) << "=" << std::quoted(parsedCalibratorTypeId)
			  << " does not match requested calibration type " << std::quoted(calibratorTypeId));
	    } else if (keyToken == nbColumnsKey) {
	      DT_THROW_IF(nbColumns > 0, std::logic_error, "tag " << std::quoted(nbColumnsKey)
			  << " is already processed at line " << lineCounter);
	      nbColumns = std::stoi(valueToken);
	    } else if (keyToken == runIdKey) {
	      DT_THROW_IF(parsedRunId >= 0, std::logic_error, "tag " << std::quoted(runIdKey)
			  << " is already processed at line " << lineCounter);
	      parsedRunId = std::stoi(valueToken);
	      DT_THROW_IF(map_.is_target_phase(), std::logic_error, "Map is set as target phase ID");
	      DT_THROW_IF(map_.is_target_run() and map_.get_target_run_id() != parsedRunId, std::logic_error,
			  "Not matching parsed run ID with target run ID in map");
	      map_.set_target_run_id(parsedRunId);	      
	    } else if (keyToken == phaseIdKey) {
	      DT_THROW_IF(parsedPhaseId >= 0, std::logic_error, "tag " << std::quoted(phaseIdKey)
			  << " is already processed at line " << lineCounter);
	      parsedPhaseId = std::stoi(valueToken);
	      DT_THROW_IF(map_.is_target_run(), std::logic_error, "Map is set as target run ID");
	      DT_THROW_IF(map_.is_target_phase() and map_.get_target_phase_id() != parsedPhaseId, std::logic_error,
			  "Not matching parsed phase ID with target phase ID in map");
	      map_.set_target_phase_id(parsedPhaseId);	      
	    } else {
	      DT_LOG_DEBUG(_verbosity_,
			   "Ignoring special tag " << std::quoted(keyToken)
			   << " at line " << lineCounter);
	    }
	  }
	  continue;
	}
	// Checks when header terminates:
	DT_THROW_IF(calibratorTypeId.empty(), std::logic_error,
		    "missing calibration type ID tag in file " << filepath_);
	DT_THROW_IF(parsedRunId >= 0 and parsedPhaseId >= 0, std::logic_error,
		    "tags " << std::quoted(runIdKey) << " and " << std::quoted(phaseIdKey)
		    << " cannot be used simultaneously"); 
	parsingData = true;
	processing::calibrator_parameters calibratorParameters;
	std::istringstream lineIn(line);
	int omNum = -1;
	lineIn >> omNum;
	DT_THROW_IF(!lineIn, std::logic_error, "Invalid format at line " << lineCounter);
	DT_THROW_IF(omNum < 0 and omNum > 719, std::logic_error, "Invalid OM number at line " << lineCounter);
	// DT_LOG_DEBUG(get_logging_priority(), "OM number = " << omNum);
	bool valuesOk = false;
	if (calibratorTypeId == "charge_pol1") {
	  double a = datatools::invalid_real();
	  lineIn >> std::ws >> a;
	  double constant = datatools::invalid_real();
	  DT_THROW_IF(!lineIn, std::logic_error, "Invalid format at line " << lineCounter);
	  DT_THROW_IF(a < 0.0, std::logic_error, "Invalid value for 'a' coefficient at line " << lineCounter);
	  if (a > 0.0) {
	    constant = a * CLHEP::MeV / (1e-9*CLHEP::volt*CLHEP::second);
	    calibratorParameters.put("constant", constant);
	    valuesOk = true;
	  }	
	} else if (calibratorTypeId == "charge_pol2") {
	  double a = datatools::invalid_real();
	  double b = datatools::invalid_real();
	  lineIn >> std::ws >> a >> std::ws >> b;
	  double constant1 = datatools::invalid_real();
	  double constant0 = datatools::invalid_real();
	  DT_THROW_IF(!lineIn, std::logic_error, "Invalid format at line " << lineCounter);
	  DT_THROW_IF(a < 0.0, std::logic_error, "Invalid value for 'a' coefficient at line " << lineCounter);
	  if (a > 0.0) {
	    constant1 = a * CLHEP::MeV / (1e-9*CLHEP::volt*CLHEP::second);
	    constant0 = b * CLHEP::MeV;
	    calibratorParameters.put("constant1", constant1);
	    calibratorParameters.put("constant0", constant0);
	    valuesOk = true;
	  }	
	} else if (calibratorTypeId == "scaled_amp") {
	  double s = datatools::invalid_real();
	  lineIn >> std::ws >> s;
	  double scale = datatools::invalid_real();
	  DT_THROW_IF(!lineIn, std::logic_error, "Invalid format at line " << lineCounter);
	  DT_THROW_IF(s < 0.0, std::logic_error, "Invalid value for 'a' coefficient at line " << lineCounter);
	  if (s > 0.0) {
	    scale = s * CLHEP::MeV / CLHEP::volt;
	    calibratorParameters.put("scale", scale);
	    valuesOk = true;
	  }	
	  DT_THROW(std::logic_error, "not implement method " << std::quoted(calibratorTypeId));
	}
	if (valuesOk) {
	  // Create a calibrator for this OM:
	  processing::CaloEnergyCalibratorHdl calibratorHdl
	    = caloEnergyCalibratorFactory(calibratorTypeId, calibratorParameters);
	  map_.add_calibrator(omNum, calibratorHdl);
	  // DT_LOG_DEBUG(_verbosity_, "Energy calibrator has been added for OM #" << omNum);
	}
      }
      ifile.close();
      return;
    }
 
  } // end of namespace processing

} // end of namespace snemo
