// -*- mode: c++ ; -*-
/// \file falaise/snemo/processing/calo_energy_calibration.h
/* Author(s) :    F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-15
 * Last modified: 2026-05-05
 *
 * Description:
 *
 *   Calorimeter energy calibration tools
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_H
#define FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_H 1

// Standard library:
#include <utility>
#include <memory>
#include <vector>

// - Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/factory_macros.h>

// This project:
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>
#include <falaise/snemo/rc/run_description.h>
#include <falaise/snemo/rc/run_phase.h>

namespace snemo {

  namespace processing {

    /// \brief Result of calorimeter hit energy calibration
    struct calo_energy
    {
      double value = datatools::invalid_real(); ///< Central value
      double sigma = datatools::invalid_real(); ///< Uncertainty

      bool has_value() const;
      bool has_sigma() const;
      void reset();
    };

    /// \brief All calorimeter energy calibrator are configured using
    //         an instance of the boost::property_tree::ptree container class.
    typedef boost::property_tree::ptree calibrator_parameters;

    /// \brief Interface for calorimeter energy calibrator associated to a given OM:
    class calo_energy_calibrator
    {
    public:

      calo_energy_calibrator() = default;

      virtual ~calo_energy_calibrator() = default;

      virtual void configure(const calibrator_parameters & parameters_) = 0;
      
      virtual calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const = 0;

      calo_energy calibrate(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const;

      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(calo_energy_calibrator)
       
    };

    typedef calo_energy_calibrator CaloEnergyCalibrator;

    typedef std::shared_ptr<calo_energy_calibrator> CaloEnergyCalibratorHdl;

    class calo_energy_calibrator_factory
    {
    public:

      static std::set<std::string> supported_calibrator_type_ids();
      
      std::unique_ptr<CaloEnergyCalibrator> operator()(const std::string & calibrator_type_id_,
						       const calibrator_parameters & parameters_);
    };
    
     
    /// \brief Calorimeter energy calibration map
    ///
    /// A calorimeter energy calibration map can refer to:
    /// - a single run
    /// - a run phase (and thus each run which belongs to the phase)
    ///
    class calo_energy_calibration_map
    {
    public:

      static const unsigned int NB_OMS = 720;
      static const unsigned int NB_OMS_SNDEMO = 712;

      calo_energy_calibration_map();

      void set_target_run_id(const rc::run_id_type target_run_id_);

      void set_target_phase_id(const rc::run_phase_id_type target_phase_id_);

      rc::run_id_type get_target_run_id() const;
  
      rc::run_phase_id_type get_target_phase_id() const;
       
      bool is_target_run() const;

      bool is_target_phase() const;

      bool has_calibrator(const int om_num_) const;
      
      void add_calibrator(const int om_num_, const CaloEnergyCalibratorHdl & calibrator_handle_);
      
      const CaloEnergyCalibrator & get_calibrator(const int om_num_) const;
      
      const CaloEnergyCalibrator & get_calibrator(const geomtools::geom_id & om_gid_) const;
      
      const CaloEnergyCalibratorHdl & get_calibrator_handle(const int om_num_) const;
      
      const CaloEnergyCalibratorHdl & get_calibrator_handle(const geomtools::geom_id & om_gid_) const;

      void reset_calibrators();
      
    private:

      rc::run_id_type _target_run_id_ = rc::run_description::INVALID_RUN_ID; ///< If set, the map is dedicated to a single run
      rc::run_phase_id_type _target_phase_id_ = rc::run_phase::INVALID_PHASE_ID; ///< If set, the map is dedicated to a single run phase
      std::vector<CaloEnergyCalibratorHdl> _calibrators_; ///< Array of calo energy calibrators associated to each OM
      
    };

    
    class calo_energy_calibration_data_parser
    {
    public:
      
      calo_energy_calibration_data_parser(const std::string & calibrator_type_id_,
					  const datatools::logger::priority verbosity_ = datatools::logger::PRIO_ERROR);

      void parse(const std::string & filepath_, calo_energy_calibration_map & map_);
      
    private:
      
      std::string _calibrator_type_id_;
      datatools::logger::priority _verbosity_ = datatools::logger::PRIO_ERROR;
      
    };

    struct om_energy_calibration_record
    {
      time::time_period period;
      CaloEnergyCalibratorHdl calibrator;

      friend std::ostream & operator<<(std::ostream & out_, const om_energy_calibration_record & record_);
    };
    
    class om_energy_calibration_history
    {
    public:
      om_energy_calibration_history() = default;
      void add(const time::time_period & period_, const CaloEnergyCalibratorHdl & calibrator_);
      void clear();
      const std::vector<om_energy_calibration_record> & records() const;
      CaloEnergyCalibratorHdl get_calibrator_handle(const time::time_point & t_) const;
      void print(std::ostream & out_, const std::string & indent_ = "") const;
			
    private:
      std::vector<om_energy_calibration_record> _records_;
    };

  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_H
