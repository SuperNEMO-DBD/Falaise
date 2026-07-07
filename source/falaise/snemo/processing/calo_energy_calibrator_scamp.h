// -*- mode: c++ ; -*-

#ifndef FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_SCAMP_H
#define FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_SCAMP_H 1

#include <falaise/snemo/processing/calo_energy_calibration.h>

namespace snemo {
  namespace processing {
    
    /// \brief Calorimeter energy calibrator using polynomial formula: E = a.A
    ///
    ///  E(MeV) = Amplitude(V) * a(MeV/(V))
    ///
    /// Typical value for constant is : 5 MeV/(nV.s)
    ///
    /// Type ID = "scamp"
    ///
    class calo_energy_calibrator_scamp
      : public calo_energy_calibrator
    {
    public:
          
      calo_energy_calibrator_scamp() = default;
 
      calo_energy_calibrator_scamp(const double a_, const double a_err_);
 
      ~calo_energy_calibrator_scamp() override = default;

      void set_a_params(const double a_, const double a_err_);
      
      void configure(const calibrator_parameters & parameters_) override;
      
      calo_energy operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const override;
   
    private:

      /// The constants are typically expressed in MeV/Volt
      double _a_ = datatools::invalid_real();
      double _a_err_ = datatools::invalid_real();
      
      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(calo_energy_calibrator, calo_energy_calibrator_scamp)
      
    };
    
  }
}

#endif // FALAISE_SNEMO_PROCESSING_CALO_ENERGY_CALIBRATION_SCAMP_H
