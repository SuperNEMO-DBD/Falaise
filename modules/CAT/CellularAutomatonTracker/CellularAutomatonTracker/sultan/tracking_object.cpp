/* -*- mode: c++ -*- */

#include <sultan/tracking_object.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/math/special_functions/gamma.hpp>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

namespace SULTAN {
namespace topology {

using namespace mybhep;

void tracking_object::set_probmin(double probmin) { probmin_ = probmin; }

double tracking_object::probmin() const { return probmin_; }

double tracking_object::probof(double chi2, int ndof) const {
  // double p  =  TMath::Prob(chi2, ndof);
  double p = 0.;
  if (ndof && chi2) p = boost::math::gamma_q<double, double>(0.5 * ndof, 0.5 * chi2);
  return p;
}

double tracking_object::get_probmin() const { return probmin_; }

/*
  inline void print_clock(){
  clock.dump();
  return;
  }
*/

}  // namespace topology
}  // namespace SULTAN
