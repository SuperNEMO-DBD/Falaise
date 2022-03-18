#include <lttc/fitted_line2.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>

// - GSL :
#include <gsl/gsl_statistics.h>

namespace lttc {

  void draw_fitted_line(std::ostream & out_,
                        const fitted_line2 & fline_,
                        int tag_)
  {
    line2 l0 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l1 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l2 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    line2 l3 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    lttc::polyline2 pl[4];
    double t     = 100;
    for (int i = 0; i < 4; i++) {
      l1.generate_samples(-t, t, pl[i]);
      lttc::draw_polyline(out_, pl[i], tag_);
      out_ << '\n';
    }
    return;
  }

  void distance_to_fitted_line(const fitted_line2 & fline_,
                               const point2 & pos_,
                               double & distance_,
                               double & distance_err_)
  {
    line2 l0 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l1 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l2 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    line2 l3 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    double d[4];
    d[0] = l0.distance(pos_);
    d[1] = l1.distance(pos_);
    d[2] = l2.distance(pos_);
    d[3] = l3.distance(pos_);
    distance_     = gsl_stats_mean(d, 1, 4);
    distance_err_ = gsl_stats_sd_m(d, 1, 4, distance_);
    return;
  }
 
  void orthogonal_projection_to_fitted_line(const fitted_line2 & fline_,
                                            const point2 & pos_,
                                            fitted_point2 & fpoint_)
  {
    line2 lc = line2::make_from_theta_r(fline_.theta, fline_.r);
    double ct = std::cos(fline_.theta);
    double st = std::sin(fline_.theta);
    point2 M0;
    lc.compute(0.0, M0);
    vector2 u(ct, st);
    double angleU = u.phi();
    vector2 M0P = pos_ - M0;
    double angleMP = M0P.phi();
    double alpha = angleMP - angleU;
    double angleFitPoint = (fline_.theta - alpha) - 0.5 * M_PI;
    // std::cerr << "[devel] angleFitPoint = " << angleFitPoint / CLHEP::degree << "\n";
    double ca = std::cos(angleFitPoint);
    double sa = std::sin(angleFitPoint);
    line2 l0 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l1 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     + fline_.r_err);
    line2 l2 = line2::make_from_theta_r(fline_.theta - fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    line2 l3 = line2::make_from_theta_r(fline_.theta + fline_.theta_err,
                                        fline_.r     - fline_.r_err);
    point2 p[4];
    p[0] = l0.orthogonal_projection(pos_);
    p[1] = l1.orthogonal_projection(pos_);
    p[2] = l2.orthogonal_projection(pos_);
    p[3] = l3.orthogonal_projection(pos_);
    double x[4], y[4];
    for (int i =0; i < (int) 4; i++) {
      x[i] = p[i].x();
      y[i] = p[i].y();
    }
    double xbar = gsl_stats_mean(x, 1, 4);
    double ybar = gsl_stats_mean(y, 1, 4);
    fpoint_.x   = gsl_stats_mean(x, 1, 4);
    fpoint_.y   = gsl_stats_mean(y, 1, 4);
    for (int i =0; i < (int) 4; i++) {
      double xci = p[i].x() - xbar;
      double yci = p[i].y() - ybar;
      x[i] =  ca * xci + sa *  yci;
      y[i] = -sa * xci + ca *  yci;
    }
    fpoint_.x_err = gsl_stats_sd_m(x, 1, 4, 0.0);
    fpoint_.y_err = gsl_stats_sd_m(y, 1, 4, 0.0);
    fpoint_.angle = angleFitPoint;
    return;
  }

  bool fitted_line2::is_valid() const
  {
    if (not datatools::is_valid(theta)) return false;
    if (not datatools::is_valid(r)) return false;
    if (not datatools::is_valid(theta_err)) return false;
    if (not datatools::is_valid(r_err)) return false;
    return true;
  }

  void fitted_line2::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- theta     = " << theta / CLHEP::degree << " degree" << std::endl;
    out_ << indent_ << "|-- theta_err = " << theta_err / CLHEP::degree << " degree" << std::endl;
    out_ << indent_ << "|-- r         = " << r / CLHEP::mm << " mm"  << std::endl;
    out_ << indent_ << "`-- r_err     = " << r_err / CLHEP::mm << " mm"  << std::endl;
    return;
  }
 
} // namespace lttc 
