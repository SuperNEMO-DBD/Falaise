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

    DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(calo_energy_calibrator,
                                                     "snemo::processing::calo_energy_calibrator/__system__")

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
 
    calo_energy calo_energy_calibrator::calibrate(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      return (*this)(pcd_calo_hit_);
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
    calo_energy_calibration_map::get_calibrator(const geomtools::geom_id & om_gid_) const
    {
      const int omNum = snemo::datamodel::om_num(om_gid_); 
      return get_calibrator(omNum);
    }

    const CaloEnergyCalibrator &
    calo_energy_calibration_map::get_calibrator(const int om_num_) const
    {
      return *_calibrators_[om_num_];
    }
      
    const CaloEnergyCalibratorHdl &
    calo_energy_calibration_map::get_calibrator_handle(const geomtools::geom_id & om_gid_) const
    {
      int omNum = snemo::datamodel::om_num(om_gid_);
      return get_calibrator_handle(omNum);
    }
            
    const CaloEnergyCalibratorHdl &
    calo_energy_calibration_map::get_calibrator_handle(const int om_num_) const
    {
      return _calibrators_[om_num_];
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
      static const std::set<std::string> _supCalTypeIds = {"scaled_amp", "pol1", "pol2"};
      return _supCalTypeIds;
    }
     
    std::unique_ptr<CaloEnergyCalibrator>
    calo_energy_calibrator_factory::operator()(const std::string & calibrator_type_id_,
					       const boost::property_tree::ptree & parameters_)
    {
      const auto & modelSystemFactoryRegister = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(calo_energy_calibrator);
      std::string calibratorTypeId = calibrator_type_id_;
      if (calibratorTypeId == "charge_pol1") {
	calibratorTypeId = "pol1";
      } else if (calibratorTypeId == "charge_pol2") {
	calibratorTypeId = "pol2";
      } else if (calibratorTypeId == "scale_amp") {
	calibratorTypeId = "scamp";
      }
      DT_THROW_IF(not modelSystemFactoryRegister.has(calibratorTypeId), std::logic_error,
		  "Factory register supports no calorimeter energy calibrator of type '"
		  << calibratorTypeId << "'!");
      const auto & modelSystemFactory = modelSystemFactoryRegister.get(calibratorTypeId);
      std::unique_ptr<CaloEnergyCalibrator> calibratorHdl(modelSystemFactory());
      calibratorHdl->configure(parameters_);
      return calibratorHdl;
    }
      
    /***** calo_energy_calibration_data_parser *****/

    calo_energy_calibration_data_parser::calo_energy_calibration_data_parser(const std::string & calibrator_type_id_,
									     const datatools::logger::priority verbosity_)
      : _calibrator_type_id_(calibrator_type_id_)
      , _verbosity_(verbosity_)
    {
      // DT_THROW_IF(not calo_energy_calibrator_factory::supported_calibrator_type_ids().count(calibrator_type_id_),
      // 		  std::logic_error,
      //		  "unsupported calorimeter energy calibrator type " << std::quoted(calibrator_type_id_));
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
	DT_THROW_IF(omNum < 0 or omNum >= (int) snemo::datamodel::number_of_oms(),
		    std::logic_error,
		    "Invalid OM number at line " << lineCounter);
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
	    calibratorParameters.put("a", constant);
	    calibratorParameters.put("a_err", constant * 0.05); // force 5% error
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
	    calibratorParameters.put("a", constant1);
	    calibratorParameters.put("b", constant0);
	    double a_err = std::fabs(a) * 0.01;
	    double b_err = std::fabs(b) * 0.1;
	    calibratorParameters.put("Var_a", a_err * a_err);
	    calibratorParameters.put("Var_b", b_err * b_err);
	    calibratorParameters.put("Cov_ab", -a_err * b_err);
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
	    calibratorParameters.put("a", scale);
	    calibratorParameters.put("a_err", scale * 0.05); // force 5% error
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
      DT_LOG_DEBUG(_verbosity_, "Parsing done.");
      return;
    }
    
    // friend
    std::ostream & operator<<(std::ostream & out_, const om_energy_calibration_record & record_)
    {
      std::ostringstream sout;
      sout << '@' << snemo::time::to_string(record_.period)
	   << " -> calibrator=" << typeid(*record_.calibrator).name();
      out_ << sout.str();
      return out_;
    }

    void om_energy_calibration_history::add(const time::time_period & period_,
					    const CaloEnergyCalibratorHdl & calibrator_hdl_)
    {
      if (_records_.size() and period_.begin() < _records_.back().period.end()) {
        DT_THROW(std::domain_error, "New period does not follow last record!");
      }
      _records_.push_back({period_, calibrator_hdl_});
      return;
    }
    
    const std::vector<om_energy_calibration_record> &
    om_energy_calibration_history::records() const
    {
      return _records_;
    }

    void om_energy_calibration_history::clear()
    {
      _records_.clear();
      return;
    }
	
    CaloEnergyCalibratorHdl
    om_energy_calibration_history::get_calibrator_handle(const time::time_point & t_) const
    {
      CaloEnergyCalibratorHdl calibratorHdl;
      for (const auto & rec : _records_ ) {
        if (rec.period.contains(t_)) {
          calibratorHdl = rec.calibrator;
          break;
        }
      }
      return calibratorHdl;
    }

    void om_energy_calibration_history::print(std::ostream & out_, const std::string & indent_) const
    {
      std::string indent = indent_;
      out_ << indent << "History records:\n";
      auto count = 0u;
      for (const auto & rec : _records_) {
	out_ << indent << " - Record #" << count << " : " << rec << '\n';
	count++;
      }
      return;
    }

  } // end of namespace processing

} // end of namespace snemo
