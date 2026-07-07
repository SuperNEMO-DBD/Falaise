
// Ourselves:
#include "calo_energy_calibrator_pol2.h"

// Standard library:
#include <cmath>

#include <gsl/gsl_fit.h>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace processing {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(calo_energy_calibrator,
                                                              calo_energy_calibrator_pol2,
                                                              "pol2")

    calo_energy_calibrator_pol2::calo_energy_calibrator_pol2(const double a_,
							     const double b_,
							     const double Var_a_,
							     const double Var_b_,
							     const double Cov_ab_)
    {
      set_ab_params(a_, b_, Var_a_, Var_b_, Cov_ab_);
      return;
    }
 
    void calo_energy_calibrator_pol2::set_ab_params(const double a_,
						    const double b_,
						    const double Var_a_,
						    const double Var_b_,
						    const double Cov_ab_)
    {
      DT_THROW_IF(a_ <= 0.0, std::logic_error, "invalid value for a");
      DT_THROW_IF(Var_a_ < 0.0, std::logic_error, "invalid value for Var_a");
      DT_THROW_IF(Var_b_ < 0.0, std::logic_error, "invalid value for Var_b");
      _a_ = a_;
      _b_ = b_;
      _Var_a_ = Var_a_;
      _Var_b_ = Var_b_;
      _Cov_ab_ = Cov_ab_;
      return;
    }
     
    void calo_energy_calibrator_pol2::configure(const calibrator_parameters & parameters_)
    {
      double a = parameters_.get<double>("a");
      double b = parameters_.get<double>("b");
      double Var_a = parameters_.get<double>("Var_a");
      double Var_b = parameters_.get<double>("Var_b");
      double Cov_ab = parameters_.get<double>("Cov_ab");
      set_ab_params(a, b, Var_a, Var_b, Cov_ab);
      return;
    }

    /**
     *  E = b + a.Q
     *  E_err = sqrt(Var_b + Q * (2 * Cov_ab + Var_a * Q)) // see GSL source code
     *
     */
    calo_energy calo_energy_calibrator_pol2::operator()(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_) const
    {
      calo_energy calibratedEnergy;
      if (pcd_calo_hit_.has_charge()) {
	auto signalCharge = std::fabs(pcd_calo_hit_.get_charge());
	double hitEnergy = signalCharge * _a_ + _b_;
	calibratedEnergy.value = hitEnergy;
	if (pcd_calo_hit_.has_sigma_charge()) {
	  double E;
	  double E_err;
	  gsl_fit_linear_est(signalCharge, _b_, _a_, _Var_b_, _Cov_ab_, _Var_a_, &E, &E_err);
	  calibratedEnergy.value = E;
	  calibratedEnergy.sigma = E_err;
	}
      }
      return calibratedEnergy;
    }
 
  }
}
