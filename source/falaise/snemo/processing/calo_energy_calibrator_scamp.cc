// Ourselves:
#include "calo_energy_calibrator_scamp.h"

// Standard library:
#include <cmath>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace processing {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(calo_energy_calibrator,
                                                              calo_energy_calibrator_scamp,
                                                              "scamp")


    calo_energy_calibrator_scamp::calo_energy_calibrator_scamp(const double a_,
							     const double a_err_)
    {
      set_a_params(a_, a_err_);
      return;
    }
 
    void calo_energy_calibrator_scamp::set_a_params(const double a_, const double a_err_)
    {
      DT_THROW_IF(a_ <= 0.0, std::logic_error, "invalid value for a");
      DT_THROW_IF(a_err_ <= 0.0, std::logic_error, "invalid value for a_err");
      _a_ = a_;
      _a_err_ = a_err_;
      return;
    }
     
    void calo_energy_calibrator_scamp::configure(const calibrator_parameters & parameters_)
    {
      double a = parameters_.get<double>("a");
      double a_err = parameters_.get<double>("a_err");
      set_a_params(a, a_err);
      return;
    }

    /**
     *   E = a.A
     *   E_err = E * hypot(a_err/a, A_err/A)
     *
     */
    calo_energy calo_energy_calibrator_scamp::operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_amplitude()) {
	auto signalAmplitude = std::fabs(pcd_calo_hit_.get_amplitude());
	double hitEnergy = signalAmplitude * _a_;
	calibratedEnergy.value = hitEnergy;
	if (pcd_calo_hit_.has_sigma_amplitude()) {
	  auto signalAmplitudeSigma = pcd_calo_hit_.get_sigma_amplitude();
	  calibratedEnergy.sigma = hitEnergy * std::hypot(_a_err_ / _a_, signalAmplitudeSigma / signalAmplitude);
	}
      }
      return calibratedEnergy;
    }
 
  }
}
