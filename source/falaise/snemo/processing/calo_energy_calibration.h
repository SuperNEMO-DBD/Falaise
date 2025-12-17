// -*- mode: c++ ; -*-
/// \file falaise/snemo/processing/calo_energy_calibration.h
/* Author(s) :    F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-15
 * Last modified: 2025-12-15
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
    
    typedef boost::property_tree::ptree calibrator_parameters;

    /// \brief Interface for calorimeter energy calibrator associated to a given OM:
    class calo_energy_calibrator
    {
    public:

      calo_energy_calibrator() = default;

      virtual ~calo_energy_calibrator() = default;

      virtual void configure(const calibrator_parameters & parameters_) = 0;
      
      virtual calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const = 0;
      
    };

    typedef calo_energy_calibrator CaloEnergyCalibrator;

    typedef std::shared_ptr<calo_energy_calibrator> CaloEnergyCalibratorHdl;

    /// \brief Calorimeter energy calibrator using scaled amplitude of the signal
    ///
    ///  E(MeV) = amplitude(Volt) * scale(MeV/Volt)
    ///
    /// Typical value for scale is : 5 MeV/Volt
    ///
    /// Type ID = "scaled_amp"
    ///
    class scaled_amplitude_calo_energy_calibrator
      : public calo_energy_calibrator
    {
    public:

      scaled_amplitude_calo_energy_calibrator();

      scaled_amplitude_calo_energy_calibrator(const double scale_);

      ~scaled_amplitude_calo_energy_calibrator() override = default;

      void set_scale(const double scale_);
 
      double get_scale() const;

      /// The parameter set contains :
      /// - a property with name "scale", real type, implicit unit "MeV/Volt"
      void configure(const calibrator_parameters & parameters_) override;
     
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;

    private:

      double _scale_ = 5 * CLHEP::MeV / CLHEP::volt;
	
    };

    /// \brief Calorimeter energy calibrator using polynomial formula
    ///
    ///  E(MeV) = charge(nV.s) * constant(MeV/(nV.s))
    ///
    /// Typical value for constant is : 0.2 MeV/(nV.s)
    ///
    /// Type ID = "charge_pol1"
    ///
    class charge_pol1_calo_energy_calibrator
      : public calo_energy_calibrator
    {
    public:
      
      charge_pol1_calo_energy_calibrator();
 
      charge_pol1_calo_energy_calibrator(const double constant_);
 
      ~charge_pol1_calo_energy_calibrator() override = default;
 
      /// The parameter set contains :
      /// - a property with name "constant", real type, implicit unit "MeV/(nV.s)"
      void configure(const calibrator_parameters & parameters_) override;

      void set_constant(const double constant_);

      double get_constant() const;
      
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;
   
    private:

      /// The constant is typically expressed in MeV/(ns.Volt)
      double _constant_ = datatools::invalid_real();
      
    };
 
    /// \brief Calorimeter energy calibrator using polynomial formula:
    ///
    ///  E(MeV) = charge(nV.s) * constant1(MeV/(nV.s)) + constant0(MeV)
    ///
    /// Typical value for constant1 is : 0.2 MeV/(nV.s)
    /// Typical value for constant0 is : 0 MeV
    ///
    /// Type ID = "charge_pol2"
    ///
    class charge_pol2_calo_energy_calibrator
      : public calo_energy_calibrator
    {
    public:
      
      charge_pol2_calo_energy_calibrator();
 
      /// The constant 1 is typically expressed in MeV/(nV.s)
      /// The constant 0 is typically expressed in MeV
      charge_pol2_calo_energy_calibrator(const double constant0_,
					 const double constant1_);
 
      ~charge_pol2_calo_energy_calibrator() override = default;
 

      /// The parameter set contains :
      /// - a property with name "constant1", real type, implicit unit "MeV/(nV.s)"
      /// - a property with name "constant0", real type, implicit unit "MeV"
      void configure(const calibrator_parameters & parameters_) override;

      void set_constant1(const double constant1_);

      void set_constant0(const double constant0_);

      double get_constant1() const;

      double get_constant0() const;
  
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;
   
    private:

      double _constant0_ = datatools::invalid_real();
      double _constant1_ = datatools::invalid_real();
      
    };

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
      
      const CaloEnergyCalibrator & get_calibrator(const geomtools::geom_id & calo_gid_) const;

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

  } // end of namespace processing

} // end of namespace snemo

#endif  // FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_H
