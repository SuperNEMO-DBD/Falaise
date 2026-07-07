// -*- mode: c++ ; -*-

#ifndef FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL2_H
#define FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL2_H 1

#include <falaise/snemo/processing/calo_energy_calibration.h>

namespace snemo {
  namespace processing {
    
    /// \brief Calorimeter energy calibrator using polynomial formula: E = a.Q + b
    ///
    ///  E(MeV) = charge(nV.s) * a(MeV/(nV.s)) + b(MeV)
    ///
    /// Typical value for a is : 0.2 MeV/(nV.s)
    /// Typical value for b is : 0.0 MeV
    ///
    /// Type ID = "pol2"
    ///
    class calo_energy_calibrator_pol2
      : public calo_energy_calibrator
    {
    public:
          
      calo_energy_calibrator_pol2() = default;
 
      calo_energy_calibrator_pol2(const double a_,
				  const double b_,
				  const double Var_a_,
				  const double Var_b_,
				  const double Cov_ab_);
 
      ~calo_energy_calibrator_pol2() override = default;

      void set_ab_params(const double a_,
			 const double b_,
			 const double Var_a_,
			 const double Var_b_,
			 const double Cov_ab_);
      
      void configure(const calibrator_parameters & parameters_) override;
      
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;
   
    private:

      double _a_ = datatools::invalid_real();     /// MeV/(ns.Volt)
      double _b_ = datatools::invalid_real();     /// MeV
      double _Var_a_ = datatools::invalid_real(); /// [MeV/(ns.Volt)]^2
      double _Var_b_ = datatools::invalid_real(); /// MeV^2
      double _Cov_ab_ = datatools::invalid_real(); 
      
      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(calo_energy_calibrator,
							   calo_energy_calibrator_pol2)
      
    };
    
  }
}

#endif // FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_POL2_H
