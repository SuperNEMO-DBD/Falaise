// Ourselves:
#include "tof_computing.h"

// Standard library
#include <cmath>

// - Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

namespace gt {

  double tof_computing::beta(double energy_, double mass_)
  {
    return std::sqrt(energy_ * (energy_ + 2.*mass_)) / (energy_ + mass_);
  }

  double tof_computing::get_theoritical_time(double energy_, double mass_, double track_length_)
  {
    return track_length_ / (tof_computing::beta(energy_, mass_) * CLHEP::c_light);
  }

  double tof_computing::get_track_length(const event::calorimeter_hit & hit1_,
                                         const event::calorimeter_hit & hit2_)
  {
    return (hit1_.position - hit2_.position).mag();
  }

  double tof_computing::get_delta_time(const event::calorimeter_hit & hit1_,
                                       const event::calorimeter_hit & hit2_)
  {
    const double track_length = tof_computing::get_track_length(hit1_, hit2_);
    const double t1 = hit1_.time;
    const double t2 = hit2_.time;
    const double th_time = tof_computing::get_theoritical_time(hit1_.energy, 0, track_length);
    //the mass being 0, it just gets t=l/c (since beta=1)
    return std::abs(t1 - t2) - th_time;
  }

  double tof_computing::get_chi2(const event::calorimeter_hit & hit1_,
                                 const event::calorimeter_hit & hit2_)
  {
    const double sigma_exp = pow(hit1_.sigma_time,2) + pow(hit2_.sigma_time,2);
    return pow(tof_computing::get_delta_time(hit1_, hit2_),2)/sigma_exp;
  }

  double tof_computing::get_internal_probability(double chi2_, size_t ndf_)
  {
    return gsl_cdf_chisq_Q(chi2_, ndf_);
  }

}
