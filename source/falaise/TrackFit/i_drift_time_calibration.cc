/// \file falaise/TrackFit/i_drift_time_calibration.cc

// Ourselves:
#include <TrackFit/i_drift_time_calibration.h>

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

namespace TrackFit {

  bool i_drift_time_calibration::drift_time_is_valid(double drift_time_) const
  {
    return drift_time_ >= 0.0;
  }

  bool i_drift_time_calibration::radius_is_valid(double radius_) const
  {
    return radius_ >= 0.0;
  }

  f_time_radius::f_time_radius()
  {
    mode = DRIFT_TIME_TO_RADIUS;
    x0   = 0.    * CLHEP::ns;
    x1   = 50.   * CLHEP::ns;
    x2   = 200.  * CLHEP::ns;
    x3   = 1500. * CLHEP::ns;
    x4   = 4000. * CLHEP::ns;
    y0   = 0.0   * CLHEP::mm;
    y1   = 1.0   * CLHEP::mm;
    y2   = 5.0   * CLHEP::mm;
    y3   = 14.5  * CLHEP::mm;
    y4   = 17.0  * CLHEP::mm;
  }

  void f_time_radius::set_drift_time_to_radius()
  {
    mode = DRIFT_TIME_TO_RADIUS;
    return;
  }

  void f_time_radius::set_radius_to_drift_time()
  {
    mode = RADIUS_TO_DRIFT_TIME;
    return;
  }

  double f_time_radius::operator()(double u_) const
  {
    const double A1 = (y1 - y0) /(x1 - x0);
    const double B1 = y1 - A1 * x1;
    const double A2 = (y2 - y1) /(x2 - x1);
    const double B2 = y2 - A2 * x2;
    const double A3 = (y3 - y2) /(sqrt(x3) - sqrt(x2));
    const double B3 = y3 - A3 * sqrt(x3);
    const double A4 = (y4 - y3) /(x4 - x3);
    const double B4 = y4 - A4 * x4;

    if (mode == DRIFT_TIME_TO_RADIUS) {
      const double t = u_;
      DT_THROW_IF (t < x0, std::logic_error, "mode=[t -> r]: invalid drift time=" << t << "(t<" << x0 << ")!");
      if (t < x1) {
        double z = A1 * t + B1;
        if (z < 0) z = 0.0;
        return z;
      }
      if (t < x2) return A2 * t + B2;
      if (t < x3) return A3 * sqrt(t) + B3;
      return A4 * t  + B4;
    } else {
      const double r = u_;
      DT_THROW_IF (r < 0.0, std::logic_error, "mode=[r -> t]: invalid drift distance=" << r << "(r<0)!");
      if (r < y0) return(x0); //r=y0; // protect against very small drift distance
      if (r < y1) return(r - B1) / A1;
      if (r < y2) return(r - B2) / A2;
      if (r < y3) return pow((r - B3) /A3, 2);
      return(r - B4) / A4;
    }
  }

  default_drift_time_calibration::default_drift_time_calibration()
  {
    td.x0 = 0.0     * CLHEP::ns;
    td.x1 = 50.0    * CLHEP::ns;
    td.x2 = 200.0   * CLHEP::ns;
    td.x3 = 1500.0  * CLHEP::ns;
    td.x4 = 4000.0  * CLHEP::ns;
    td.x5 = 10000.0 * CLHEP::ns;
    td.y0 = 0.0     * CLHEP::mm;
    td.y1 = 1.0     * CLHEP::mm;
    td.y2 = 5.0     * CLHEP::mm;
    td.y3 = 15.0    * CLHEP::mm;
    td.y4 = 20.0    * CLHEP::mm;
    dt    = td;
    td.set_drift_time_to_radius();
    dt.set_radius_to_drift_time();
    rmax  = td.y4;
    sigma_r = 1.0 * CLHEP::mm;
    return;
  }

  double default_drift_time_calibration::get_sensitive_cell_radius() const
  {
    return td.y4;
  }

  double default_drift_time_calibration::get_max_cell_radius() const
  {
    return rmax;
  }

  void default_drift_time_calibration::drift_time_to_radius(double time_,
                                                            double & radius_,
                                                            double & sigma_radius_) const
  {
    const double time = time_;
    radius_ = td(time);
    sigma_radius_ = sigma_r;
    if (time_ > td.x3) {
      radius_ = 0.5 *(td.y3 + rmax);
      sigma_radius_ = 0.5 *(rmax - td.y3);
    }
    if (time_ > td.x5) {
      radius_ = 0.;
      sigma_radius_ = 0.3 * rmax;
    }
    return;
  }

  void default_drift_time_calibration::radius_to_drift_time(double radius_,
                                                            double & time_,
                                                            double & sigma_time_) const
  {
    double r1 = radius_ - sigma_r;
    if (r1 < 0) r1 = 0.0;
    double t1 = dt(r1);
    if (t1 < 0) t1 = 0.0;
    const double r2 = radius_ + sigma_r;
    const double t2 = dt(r2);
    time_       = (t2 + t1) * 0.5;
    sigma_time_ = (t2 - t1) * 0.5;
    return;
  }

  new_drift_time_calibration::new_drift_time_calibration()
  {
    td.x0 = 0.0     * CLHEP::ns;
    td.x1 = 50.0    * CLHEP::ns;
    td.x2 = 300.0   * CLHEP::ns;
    td.x3 = 1500.0  * CLHEP::ns;
    td.x4 = 4000.0  * CLHEP::ns;
    td.x5 = 10000.0 * CLHEP::ns;
    td.y0 = 0.0     * CLHEP::mm;
    td.y1 = 1.0     * CLHEP::mm;
    td.y2 = 5.0     * CLHEP::mm;
    td.y3 = 14.0    * CLHEP::mm;
    td.y4 = 17.0    * CLHEP::mm;
    dt    = td;
    td.set_drift_time_to_radius();
    dt.set_radius_to_drift_time();
    rmax  = td.y4;
    sigma_r = 1.0 * CLHEP::mm;
    return;
  }

  double new_drift_time_calibration::get_sensitive_cell_radius() const
  {
    return td.y4;
  }

  double new_drift_time_calibration::get_max_cell_radius() const
  {
    return rmax;
  }

  void new_drift_time_calibration::drift_time_to_radius(double time_,
                                                        double & radius_,
                                                        double & sigma_radius_) const
  {
    const double time = time_;
    radius_ = td(time);
    sigma_radius_ = sigma_r;
    if (radius_ > td.y3)
      sigma_radius_ = 3. * sigma_r;
    if (radius_ > td.y4)
      sigma_radius_ = 5. * sigma_r;
    if (time_ > td.x5) {
      radius_ = 0.;
      sigma_radius_ = 0.3 * rmax;
    }
    return;
  }

  void new_drift_time_calibration::radius_to_drift_time(double radius_,
                                                        double & time_,
                                                        double & sigma_time_) const
  {
    double r1 = radius_ - sigma_r;
    if (r1 < 0) r1 = 0.0;
    double r2 = radius_ + sigma_r;
    double t1 = dt(r1);
    if (t1 < 0) t1 = 0.0;
    double t2 = dt(r2);
    time_       = (t2 + t1) * 0.5;
    sigma_time_ = (t2 - t1) * 0.5;
    return;
  }

} // end of namespace TrackFit
