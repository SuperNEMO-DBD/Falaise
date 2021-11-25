#include <lttc/line.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>
#include <mygsl/linear_system_solver.h>

// - GSL :
#include <gsl/gsl_statistics.h>

namespace lttc {

  line::line(double a_, double b_, double c_)
  {
    double h = std::hypot(a_, b_);
    DT_THROW_IF(h < 1.e-14, std::logic_error, "Invalid (a,b) line parameters!");
    _a_ = a_ / h;
    _b_ = b_ / h;
    _c_ = c_ / h;
    if (_b_ < 0.0) {
      _a_ *= -1.0;
      _b_ *= -1.0;
      _c_ *= -1.0;
    }
    _direction_.set(-_b_, _a_);
    _compute_theta_r_from_abc();
    return;
  }
   
  line::line(const point & position_, const vector2 & direction_)
  {
    DT_THROW_IF(direction_.mag() < 1.e-14, std::logic_error, "Invalid line direction!");
    vector2 direction = direction_.unit();
    vector2 n(direction.y(), -direction.x());
    double a1 = n.x();
    double b1 = n.y();
    double c1 = -n.x() * position_.x() - n.y() * position_.y();
    double h = std::hypot(a1, b1);
    _a_ = a1 / h;
    _b_ = b1 / h;
    _c_ = c1 / h; 
    if (_b_ < 0.0) {
      _a_ *= -1.0;
      _b_ *= -1.0;
      _c_ *= -1.0;
    }
    _direction_.set(-_b_, _a_);
    _compute_theta_r_from_abc();
    return;
  }

  bool line::is_valid() const
  {
    if (_a_ != _a_) return false;
    if (_b_ != _b_) return false;
    if (_c_ != _c_) return false;
    if (_r_ != _r_) return false;
    if (_theta_ != _theta_) return false;
    return true;
  }

  double line::get_a() const
  {
    return _a_;
  }

  double line::get_b() const
  {
    return _b_;
  }

  double line::get_c() const
  {
    return _c_;
  }

  point line::get_reference_point() const
  {
    return point(_r_ * std::cos(_theta_), _r_ * std::sin(_theta_));
  }

  double line::get_theta() const
  {
    return _theta_;
  }

  double line::get_r() const
  {
    return _r_;
  }
  
  // static
  line line::make_from_start_stop(const point & start_, const point & stop_)
  {
    return line(start_, stop_ - start_);
  }
          
  // static
  line line::make_from_start_direction(double x0_, double y0_, double ux_, double uy_)
  {
    return line(point(x0_, y0_), vector2(ux_, uy_));
  }

  // static
  line line::make_from_theta_r(double theta_, double r_)
  {
    DT_THROW_IF(theta_ < 0.0 || theta_ > M_PI,
                std::logic_error,
                "Invalid (theta) line parameters!");
    if (std::abs(theta_) < 1.e-14) {
      double a = 1.0;
      double b = 0.0;
      double c = -r_;
      return line(a, b, c);
    }
    if (std::abs(theta_ - M_PI ) < 1.e-14) {
      double a = 1.0;
      double b = 0.0;
      double c = r_;
      return line(a, b, c);
    }
    double ct = std::cos(theta_);
    double st = std::sin(theta_);
    double xr = r_ * ct;
    double yr = r_ * st;
    double dx = 1.0;
    double dy = -ct / st;
    double hd = std::hypot(dx, dy);
    dx /= hd;
    dy /= hd;
    return line(point(xr, yr), vector2(dx, dy));
  }

  // static
  line line::make_perp(const point & p_) const
  {
    line l;
    if (is_vertical()) {
      l = line(0.0, 1.0, -p_.y());
    } else if (is_horizontal()) {
      l = line(1.0, 0.0, -p_.x());
    } else {
      vector2 ortho = get_orthogonal_direction();
      l = line(-ortho.y(), ortho.x(), ortho.y() * p_.x() - ortho.x() * p_.y());
    }
    return l;
  }

  bool line::is_same(const line & other_, double r_tolerance_, double theta_tolerance_) const
  {
    if (std::abs(this->_r_ - other_._r_) > r_tolerance_) return false;
    if (std::abs(this->_theta_ - other_._theta_) > theta_tolerance_) return false;
    return true;
  }
 
  bool line::is_horizontal(double tolerance_) const
  {
    if (std::abs(_a_) < tolerance_) return true;
    return false;
  }

  bool line::is_vertical(double tolerance_) const
  {
    if (std::abs(_b_) < tolerance_) return true;
    return false;
  }
  
  double line::distance(const point & pos_) const
  {
    double dist = _a_ * pos_.x() + _b_ * pos_.y() + _c_;
    return std::abs(dist);
  }
 
  vector2 line::orthogonal_projection(const point & pos_) const
  {
    double vx = _direction_.x();
    double vy = _direction_.y();
    double xp, yp;
    vector2 orthProj;
    geomtools::invalidate(orthProj);
    if (mygsl::linear_system_2x2_solve(vx, vy, vx * pos_.x() + vy * pos_.y(),
                                       _a_, _b_, -_c_, xp, yp)) {
      orthProj.set(xp, yp);
    } 
    return orthProj;
  }
    
  vector2 line::get_direction() const
  {
     return _direction_;
  }
    
  vector2 line::get_orthogonal_direction() const
  {
    return vector2(_direction_.y(), -_direction_.x());
  }

  bool line::unique_intersection(const line & other_, point & p_) const
  {
    p_.setX(datatools::invalid_real());
    p_.setY(datatools::invalid_real());
    double den = _a_ * other_._b_ - _b_ * other_._a_;
    if (std::abs(den) < 1e-14) return false;
    double x0 = (_b_ * other_._c_ - _c_ * other_._b_) / den;
    double y0 = (_c_ * other_._a_ - _a_ * other_._c_) / den;
    p_.set(x0, y0);
    return true;
  }
 
  void line::compute(double t_, point & vtx_) const
  {
    vtx_ = get_reference_point() + t_ * _direction_;
    return;    
  }
  
  void line::_compute_theta_r_from_abc()
  {
    double tolerance = 1e-15;
    if (std::abs(_a_) < tolerance) {
      _theta_ = M_PI / 2;
      // horizontal:
      double y0 = -_c_ / _b_;
      if (y0 < -tolerance) {
        _r_ = y0;
      } else if (y0 > +tolerance) {
        _r_ = y0;
      } else {
        _r_ = 0.0;
      }
      return;
    }
    if (std::abs(_b_) < tolerance) {
      // vertical:
      _theta_ = 0.0;
      double x0 = -_c_ / _a_;
      if (x0 < -tolerance) {
        _r_ = x0;
      } else if (x0 > +tolerance) {
        _r_ = x0;
      } else {
        _r_ = 0.0;
      }
      return;
    }
    _theta_ = std::atan2(_b_, _a_);
    _r_ = -_c_;
    if (_theta_ < 0.0) {
      _theta_ += 2*M_PI;
    }
    if (_theta_ >= M_PI) {
      _theta_ -= M_PI;
      _r_ *= -1.0;
    }
    return;
  }

  void line::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- a = " << _a_ << std::endl;
    out_ << indent_ << "|-- b = " << _b_ << std::endl;
    out_ << indent_ << "|-- c = " << _c_ << std::endl;
    out_ << indent_ << "|-- theta = " << _theta_ << std::endl;
    out_ << indent_ << "|-- r     = " << _r_ << std::endl;
    // double h = std::hypot(_a_, _b_);
    // out_ << indent_ << "|-- h = " << h << std::endl;
    point refP = get_reference_point(); 
    out_ << indent_ << "|-- refpoint.x = " << refP.x() << std::endl;
    out_ << indent_ << "|-- refpoint.y = " << refP.y() << std::endl;
    out_ << indent_ << "|-- direction.x = " << _direction_.x() << std::endl;
    out_ << indent_ << "`-- direction.y = " << _direction_.y() << std::endl;
    out_ << indent_ << "    |-- dir   = " << get_direction() << std::endl;
    out_ << indent_ << "    `-- ortho = " << get_orthogonal_direction() << std::endl;
    return;
  }
   
  void line::generate_samples(double t1_, double t2_, polyline & samples_) const
  {
    double tstep = 0.05;
    for (double t = t1_; t <= t2_ + 0.5 * tstep; t += tstep) {
      point p;
      this->compute(t, p);
      samples_.push_back(p);
    }    
    return;
  }

  void line::clear()
  {
    geomtools::invalidate(_direction_);
    _a_ = std::numeric_limits<double>::quiet_NaN();
    _b_ = std::numeric_limits<double>::quiet_NaN();
    _c_ = std::numeric_limits<double>::quiet_NaN();
    _theta_ = std::numeric_limits<double>::quiet_NaN();
    _r_ = std::numeric_limits<double>::quiet_NaN();
    return;
  }

  void draw_fitted_line(std::ostream & out_,
                        const fitted_line & fline_,
                        int /*tag_*/)
  {
    line l0 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l1 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l2 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     - fline_.r_err);
    line l3 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
                                      fline_.r     - fline_.r_err);
    lttc::polyline pl[4];
    double t     = 100;
    for (int i = 0; i < 4; i++) {
      l1.generate_samples(-t, t, pl[i]);
      lttc::draw_polyline(out_, pl[i]);
      out_ << '\n';
    }

  }

  void distance_to_fitted_line(const fitted_line & fline_,
                               const point & pos_,
                               double & distance_,
                               double & distance_err_)
  {
    line l0 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l1 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l2 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     - fline_.r_err);
    line l3 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
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
 
  void orthogonal_projection_to_fitted_line(const fitted_line & fline_,
                                            const point & pos_,
                                            fitted_point & fpoint_)
  {
    line lc = line::make_from_theta_r(fline_.theta, fline_.r);
    double ct = std::cos(fline_.theta);
    double st = std::sin(fline_.theta);
    point M0;
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
    line l0 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l1 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     + fline_.r_err);
    line l2 = line::make_from_theta_r(fline_.theta - fline_.theta_err,
                                      fline_.r     - fline_.r_err);
    line l3 = line::make_from_theta_r(fline_.theta + fline_.theta_err,
                                      fline_.r     - fline_.r_err);
    point p[4];
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

  bool fitted_line::is_valid() const
  {
    if (not datatools::is_valid(theta)) return false;
    if (not datatools::is_valid(r)) return false;
    if (not datatools::is_valid(theta_err)) return false;
    if (not datatools::is_valid(r_err)) return false;
    return true;
  }

  void fitted_line::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- theta     = " << theta / CLHEP::degree << " degree" << std::endl;
    out_ << indent_ << "|-- theta_err = " << theta_err / CLHEP::degree << " degree" << std::endl;
    out_ << indent_ << "|-- r         = " << r / CLHEP::mm << " mm"  << std::endl;
    out_ << indent_ << "`-- r_err     = " << r_err / CLHEP::mm << " mm"  << std::endl;
    return;
  }
 
} // namespace lttc 
