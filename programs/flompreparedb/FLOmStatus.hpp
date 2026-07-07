#ifndef FLOMSTATUS_HPP
#define FLOMSTATUS_HPP

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <map>

#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/service_manager.h"

#include <falaise/snemo/time/time_utils.h>
#include <falaise/snemo/rc/calorimeter_om_status.h>
#include <falaise/snemo/services/calorimeter_om_status_service.h>

namespace FLOmStatus {

  struct Config
  {
    datatools::logger::priority verbosity = datatools::logger::PRIO_ERROR;
    
    /// Geometry configuration:
    std::string geomConfigUrn{"urn:snemo:demonstrator:geometry:default"};

    /// Variants configuration:   
    std::string variantConfigUrn{"urn:snemo:demonstrator:geometry:variants:service:default"};  
    std::string variantConfigPath;  
    std::string variantProfileUrn;  
    std::string variantProfilePath;  
    std::string servicesConfigUrn{"urn:snemo:demonstrator:geometry:6.0:services"};  
    std::string servicesConfigPath;

    std::string omStatusPath = "";
    std::string calibDataPath = ""; //"${SNOMCALIBRATIONDATA_DIR}";
    std::string calibVersion = "0.1";
    std::string target; // "status" or "ecalib"
    std::string calibMethod = ""; // a * Q + b
    std::string inputFileList;
    std::vector<std::string> inputFiles;
    std::string outputFormat;
    std::string outputFile;
    double badFitThreshold = 0.05;
    
    void print(std::ostream & out_) const;
    
  };

  /// Data structure representing energy calibration informations for the
  /// "charge_pol1" method (E=aQ)
  struct om_energy_calib_charge_pol1_params
  {
    double a = std::numeric_limits<double>::quiet_NaN();
    double var_a = std::numeric_limits<double>::quiet_NaN();
    double fit_chi2 = std::numeric_limits<double>::quiet_NaN();
    int    fit_ndof = 0;
    double bad_fit = false;
    
    friend std::ostream & operator<<(std::ostream & out_, const om_energy_calib_charge_pol1_params & params_);
  };

  /// Data structure representing energy calibration informations for the
  /// "charge_pol2" method (E=aQ+b)
  struct om_energy_calib_charge_pol2_params
  {
    double a = std::numeric_limits<double>::quiet_NaN();
    double b = std::numeric_limits<double>::quiet_NaN();
    double var_a = std::numeric_limits<double>::quiet_NaN();
    double var_b = std::numeric_limits<double>::quiet_NaN();
    double cov_ab = std::numeric_limits<double>::quiet_NaN();
    double fit1_chi2 = std::numeric_limits<double>::quiet_NaN();
    int    fit1_ndof = 0;
    double fit2_chi2 = std::numeric_limits<double>::quiet_NaN();
    int    fit2_ndof = 0;
    double bad_fit = false;
    
    friend std::ostream & operator<<(std::ostream & out_, const om_energy_calib_charge_pol2_params & params_);
  };

  /// Data structure representing status and energy calibration informations for a given OM
  struct om_record
  {
    int om_num = -1; ///< OM identifier
    snemo::time::time_point start_period; ///< Start of the validity period
    snemo::time::time_point stop_period; ///< Stop of the validity period
    uint16_t status = snemo::rc::calorimeter_om_status::OM_GOOD; ///< OM status
    std::string calib_method = ""; ///< Calibration method identifier
    int calib_version = 1; ///< Calibration method version
    om_energy_calib_charge_pol1_params pol1_params; ///< Calibration parameters for method "pol1"
    om_energy_calib_charge_pol2_params pol2_params; ///< Calibration parameters for method "pol2"
    
    int parse(const std::vector<std::string> & tokens_,
	      const std::string & calib_method_,
	      const int format_id_);
    friend std::ostream & operator<<(std::ostream & out_, const om_record &);
  };

  typedef std::vector<om_record> om_record_col;
  // typedef std::map<int, om_record> om_record_dict;
  typedef std::map<int, om_record_col> om_record_dict;

  struct Application
  {
    Config config;
    datatools::configuration::variant_service variantService;
    datatools::service_manager services;
    
    void init(const Config & config);

    void reset();

    int run();

    void load_list(const std::string & list_path_);

    int load_file(const std::string & filepath_, om_record_dict & dict_);

  private:

    void _init_om_status_service_(const std::string & path_);

    snemo::calorimeter_om_status_service _om_status_service_;

    int _current_calib_run_id_ = -1;
    
  };
  
}

#endif // FLOMSTATUS_HPP
