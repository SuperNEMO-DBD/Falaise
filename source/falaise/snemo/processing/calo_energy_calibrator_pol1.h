// -*- mode: c++ ; -*-

#ifndef FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL1_H
#define FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL1_H 1

#include <falaise/snemo/processing/calo_energy_calibration.h>

namespace snemo {
  namespace processing {
    
    /// \brief Calorimeter energy calibrator using polynomial formula: E = a.Q
    ///
    ///  E(MeV) = charge(nV.s) * a(MeV/(nV.s))
    ///
    /// Typical value for constant is : 0.2 MeV/(nV.s)
    ///
    /// Type ID = "pol1"
    ///
    class calo_energy_calibrator_pol1
      : public calo_energy_calibrator
    {
    public:
          
      calo_energy_calibrator_pol1() = default;
 
      calo_energy_calibrator_pol1(const double a_, const double a_err_);
 
      ~calo_energy_calibrator_pol1() override = default;

      void set_a_params(const double a_, const double a_err_);
      
      void configure(const calibrator_parameters & parameters_) override;
      
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;
   
    private:

      /// The constants are typically expressed in MeV/(ns.Volt)
      double _a_ = datatools::invalid_real();
      double _a_err_ = datatools::invalid_real();
      
      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(calo_energy_calibrator, calo_energy_calibrator_pol1)
      
    };
    
  }
}

#endif // FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL1_H
