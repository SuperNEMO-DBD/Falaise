
// Ourselves:
#include "calo_energy_calibrator_pol1.h"

// Standard library:
#include <cmath>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace processing {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(calo_energy_calibrator,
                                                              calo_energy_calibrator_pol1,
                                                              "pol1")


    calo_energy_calibrator_pol1::calo_energy_calibrator_pol1(const double a_,
							     const double a_err_)
    {
      set_a_params(a_, a_err_);
      return;
    }
 
    void calo_energy_calibrator_pol1::set_a_params(const double a_, const double a_err_)
    {
      DT_THROW_IF(a_ <= 0.0, std::logic_error, "invalid value for a");
      DT_THROW_IF(a_err_ <= 0.0, std::logic_error, "invalid value for a_err");
      _a_ = a_;
      _a_err_ = a_err_;
      return;
    }
     
    void calo_energy_calibrator_pol1::configure(const calibrator_parameters & parameters_)
    {
      double a = parameters_.get<double>("a");
      double a_err = parameters_.get<double>("a_err");
      set_a_params(a, a_err);
      return;
    }

    /**
     *   E = a.Q
     *   dE = E*sqrt((a_err/a)^2 + (Q_err/Q)^2)
     *
     */
    calo_energy calo_energy_calibrator_pol1::operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_charge()) {
	auto signalCharge = std::fabs(pcd_calo_hit_.get_charge());
	double hitEnergy = signalCharge * _a_;
	calibratedEnergy.value = hitEnergy;
	auto signalChargeSigma = 0.0;
	if (pcd_calo_hit_.has_sigma_charge()) {
	  signalChargeSigma = pcd_calo_hit_.get_sigma_charge();
	}
	calibratedEnergy.sigma = hitEnergy * std::hypot(_a_err_ / _a_, signalChargeSigma / signalCharge);
      }
      return calibratedEnergy;
    }
 
  }
}
