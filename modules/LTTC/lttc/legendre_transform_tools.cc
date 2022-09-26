#include <lttc/legendre_transform_tools.hh>

// Standard library:
#include <cmath>

namespace lttc {

  legendre_transform::legendre_transform(double x_, double y_, double r_)
    : x(x_)
    , y(y_)
    , r(r_)
  {
    return;
  }

  ltr_type legendre_transform::eval(double theta_) const
  {
    double ct = std::cos(theta_);
    double st = std::sin(theta_);
    double rt = x * ct + y * st;
    ltr_type res;
    res.concave = rt + r; 
    res.convex  = rt - r;
    return res;
  }

  ltr_type legendre_transform::operator()(double theta_) const
  {
    return eval(theta_);
  }

  lt_gauss_func::lt_gauss_func(double t0_, double t0_err_, double r0_, double r0_err_)
    : theta0(t0_)
    , theta0_err(t0_err_)
    , r0(r0_)
    , r0_err(r0_err_)
  {
    return;
  }

  double lt_gauss_func::operator()(double t_, double r_) const
  {
    double dt = (t_ - theta0) / theta0_err;
    double dr = (r_ - r0) / r0_err;
    return std::exp(-0.5 * ((dt * dt) + (dr * dr)));
  }

} // namespace lttc

  
