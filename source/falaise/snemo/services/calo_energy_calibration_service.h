// -*- mode: c++ ; -*-
//! \file falaise/snemo/services/calo_energy_calibration_service.h
// Falaise service for run infos

#ifndef SNEMO_SERVICES_CALO_ENERGY_CALIBRATION_SERVICE_H
#define SNEMO_SERVICES_CALO_ENERGY_CALIBRATION_SERVICE_H

// Standard library:
#include <utility>
#include <cstdint>
#include <map>

// - Bayeux:
#include <bayeux/datatools/base_service.h>
#include <bayeux/datatools/clhep_units.h>
	  
// - Falaise:
#include "falaise/snemo/services/service_traits.h"
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/processing/calo_energy_calibration.h>
#include <falaise/snemo/services/run_info_service.h>
#include <falaise/snemo/time/time_utils.h>

namespace snemo {
   
  /// \brief Calorimeter energy calibration service
  class calo_energy_calibration_service
    : public datatools::base_service
  {
  public:

    enum struct calibrator_type {
      undefined,
      scaled_amplitude,    ///< ID = "scamp"
      charge_polynomial_1, ///< ID = "pol1"
      charge_polynomial_2  ///< ID = "pol2"
    };

    enum struct data_access_mode {
      undefined,
      files,
      database
    };
 
    calo_energy_calibration_service();

    ~calo_energy_calibration_service() override;

    void set_run_info_service(const run_info_service & run_info_service_);
    
    bool is_initialized() const override;

    int initialize(const datatools::properties & config_,
		   datatools::service_dict_type & services_) override;
    
    int reset() override;

    bool use_fallback_calibrator() const;
   
    // const snemo::processing::calo_energy_calibrator &
    // get_calibrator(const time::time_point & time_point_,
    // 		   const geomtools::geom_id & om_gid_) const;
    
    // const snemo::processing::calo_energy_calibrator &
    // get_calibrator(const time::time_point & time_point_,
    // 		   const int om_num_) const;
   
    snemo::processing::CaloEnergyCalibratorHdl 
    get_calibrator_handle(const time::time_point & time_point_,
			  const geomtools::geom_id & om_gid_) const;
     
    snemo::processing::CaloEnergyCalibratorHdl 
    get_calibrator_handle(const time::time_point & time_point_,
			  const int om_num_) const;
 
    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    typedef std::map<geomtools::geom_id, snemo::processing::om_energy_calibration_history> history_map_type;
     
    bool has_om_history(const geomtools::geom_id & gid_) const;
   
    const history_map_type & get_histories() const;
   
  private:

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _prepare_from_files_();

    void _load_calib_map_(const std::int32_t run_id_);

    void _load_calib_map_from_files_(const std::int32_t run_id_);
    
  private:

    bool _initialized_ = false; ///< Initialization flag
    data_access_mode _data_access_ = data_access_mode::undefined; ///< Data access mode

    // Database mode:
    std::string _db_label_;
    const snemo::db_service * _db_service_ = nullptr;
    history_map_type _histories_;

    // Files mode:
    std::string _files_data_path_;
    calibrator_type _calibrator_type_ = calibrator_type::undefined;
    
    bool _use_fallback_calibrator_ = false;
    double _fallback_scale_ = 5.0 * CLHEP::MeV / CLHEP::volt;
    double _fallback_scale_err_ = 0.5 * CLHEP::MeV / CLHEP::volt;
    processing::CaloEnergyCalibratorHdl _fallback_calibrator_handle_;

    std::string _run_info_label_;
    rc::run_id_type _max_run_id_ = 20000;
    rc::run_phase_id_type _max_phase_id_ = 100;
    const run_info_service * _run_info_service_ = nullptr; ///< Handle to the run info service
    std::set<rc::run_id_type> _runs_with_run_based_map_; ///< List of run IDs with individual energy calibration data
    std::set<rc::run_phase_id_type> _phases_with_map_; ///< List of phase IDs with energy calibration data
    std::shared_ptr<processing::calo_energy_calibration_map> _current_calib_map_handle_;

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(calo_energy_calibration_service)
      
  };

} // namespace snemo

#endif // SNEMO_SERVICES_CALO_ENERGY_CALIBRATION_SERVICE_H
