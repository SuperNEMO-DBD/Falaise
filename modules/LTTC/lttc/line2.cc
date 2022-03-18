#include <lttc/line2.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>
#include <mygsl/linear_system_solver.h>

namespace lttc {

  line2::line2(double a_, double b_, double c_)
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
   
  line2::line2(const point2 & position_, const vector2 & direction_)
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

  bool line2::is_valid() const
  {
    if (_a_ != _a_) return false;
    if (_b_ != _b_) return false;
    if (_c_ != _c_) return false;
    if (_r_ != _r_) return false;
    if (_theta_ != _theta_) return false;
    return true;
  }

  double line2::get_a() const
  {
    return _a_;
  }

  double line2::get_b() const
  {
    return _b_;
  }

  double line2::get_c() const
  {
    return _c_;
  }

  point2 line2::get_reference_point() const
  {
    return point2(_r_ * std::cos(_theta_), _r_ * std::sin(_theta_));
  }

  double line2::get_theta() const
  {
    return _theta_;
  }

  double line2::get_r() const
  {
    return _r_;
  }
  
  // static
  line2 line2::make_from_start_stop(const point2 & start_, const point2 & stop_)
  {
    return line2(start_, stop_ - start_);
  }
          
  // static
  line2 line2::make_from_start_direction(double x0_, double y0_, double ux_, double uy_)
  {
    return line2(point2(x0_, y0_), vector2(ux_, uy_));
  }

  // static
  line2 line2::make_from_theta_r(double theta_, double r_)
  {
    DT_THROW_IF(theta_ < 0.0 || theta_ > M_PI,
                std::logic_error,
                "Invalid (theta) line parameters!");
    if (std::abs(theta_) < 1.e-14) {
      double a = 1.0;
      double b = 0.0;
      double c = -r_;
      return line2(a, b, c);
    }
    if (std::abs(theta_ - M_PI ) < 1.e-14) {
      double a = 1.0;
      double b = 0.0;
      double c = r_;
      return line2(a, b, c);
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
    return line2(point2(xr, yr), vector2(dx, dy));
  }

  // static
  line2 line2::make_perp(const point2 & p_) const
  {
    line2 l;
    if (is_vertical()) {
      l = line2(0.0, 1.0, -p_.y());
    } else if (is_horizontal()) {
      l = line2(1.0, 0.0, -p_.x());
    } else {
      vector2 ortho = get_orthogonal_direction();
      l = line2(-ortho.y(), ortho.x(), ortho.y() * p_.x() - ortho.x() * p_.y());
    }
    return l;
  }

  bool line2::is_same(const line2 & other_, double r_tolerance_, double theta_tolerance_) const
  {
    if (std::abs(this->_r_ - other_._r_) > r_tolerance_) return false;
    if (std::abs(this->_theta_ - other_._theta_) > theta_tolerance_) return false;
    return true;
  }
 
  bool line2::is_horizontal(double tolerance_) const
  {
    if (std::abs(_a_) < tolerance_) return true;
    return false;
  }

  bool line2::is_vertical(double tolerance_) const
  {
    if (std::abs(_b_) < tolerance_) return true;
    return false;
  }
  
  double line2::distance(const point2 & pos_) const
  {
    double dist = _a_ * pos_.x() + _b_ * pos_.y() + _c_;
    return std::abs(dist);
  }
 
  point2 line2::orthogonal_projection(const point2 & pos_) const
  {
    double vx = _direction_.x();
    double vy = _direction_.y();
    double xp, yp;
    point2 orthProj;
    geomtools::invalidate(orthProj);
    if (mygsl::linear_system_2x2_solve(vx, vy, vx * pos_.x() + vy * pos_.y(),
                                       _a_, _b_, -_c_, xp, yp)) {
      orthProj.set(xp, yp);
    } 
    return orthProj;
  }
    
  vector2 line2::get_direction() const
  {
     return _direction_;
  }
    
  vector2 line2::get_orthogonal_direction() const
  {
    return vector2(_direction_.y(), -_direction_.x());
  }

  bool line2::unique_intersection(const line2 & other_, point2 & p_) const
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
   
  bool line2::unique_projection(const point2 & from_,
                                const vector2 & direction_,
                                point2 & p_) const
  {
    geomtools::invalidate(p_);
    double xA = from_.x();
    double yA = from_.y();
    double xU = direction_.x();
    double yU = direction_.y();
    double a = _a_;
    double b = _b_;
    double c = _c_;
    double den = a * xU + b * yU;
    if (std::abs(den) < 1e-14) {
      // std::cerr << "Projection direction is parallel to the line\n";
      return false;
    }
    double k = - (a * xA + b * yA + c) / den;
    if (k < 0.0) {
      // std::cerr << "Projection goes in the wrong direction\n";
      return false;
    }
    double xP = xA + k * xU;
    double yP = yA + k * yU;
    p_ = point2(xP, yP);
    return true;
  }
 
  void line2::compute(double s_, point2 & vtx_) const
  {
    vtx_ = get_reference_point() + s_ * _direction_;
    return;    
  }
  
  void line2::_compute_theta_r_from_abc()
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

  void line2::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- a = " << _a_ << std::endl;
    out_ << indent_ << "|-- b = " << _b_ << std::endl;
    out_ << indent_ << "|-- c = " << _c_ << std::endl;
    out_ << indent_ << "|-- theta = " << _theta_ << std::endl;
    out_ << indent_ << "|-- r     = " << _r_ << std::endl;
    // double h = std::hypot(_a_, _b_);
    // out_ << indent_ << "|-- h = " << h << std::endl;
    point2 refP = get_reference_point(); 
    out_ << indent_ << "|-- refpoint.x = " << refP.x() << std::endl;
    out_ << indent_ << "|-- refpoint.y = " << refP.y() << std::endl;
    out_ << indent_ << "|-- direction.x = " << _direction_.x() << std::endl;
    out_ << indent_ << "`-- direction.y = " << _direction_.y() << std::endl;
    out_ << indent_ << "    |-- dir   = " << get_direction() << std::endl;
    out_ << indent_ << "    `-- ortho = " << get_orthogonal_direction() << std::endl;
    return;
  }
   
  void line2::generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_) const
  {
    samples_.clear();
    samples_.s.clear();
    samples_.reserve(nsamples_ + 2);
    samples_.s.reserve(nsamples_ + 2);
    // std::cerr << "[devel] line2::generate_samples: nsamples_ = " << nsamples_ << '\n';
    double sa = std::min(s1_,s2_);
    double sb = std::max(s1_,s2_);
    double sstep = (sb - sa) / nsamples_;
    // std::cerr << "[devel] line2::generate_samples: tstep = " << tstep << '\n';
    for (double s = sa; s <= sb + 0.5 * sstep; s += sstep) {
      point2 p;
      this->compute(s, p);
      samples_.push_back(p);
      samples_.s.push_back(s);
    }    
    return;
  }

  void line2::clear()
  {
    geomtools::invalidate(_direction_);
    _a_ = std::numeric_limits<double>::quiet_NaN();
    _b_ = std::numeric_limits<double>::quiet_NaN();
    _c_ = std::numeric_limits<double>::quiet_NaN();
    _theta_ = std::numeric_limits<double>::quiet_NaN();
    _r_ = std::numeric_limits<double>::quiet_NaN();
    return;
  }

} // namespace lttc 
