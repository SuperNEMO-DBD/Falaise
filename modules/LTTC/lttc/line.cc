#include <lttc/line.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>

namespace lttc {

  line::line(double a_, double b_, double c_)
  {
    double h = std::hypot(a_, b_);
    DT_THROW_IF(h < 1.e-14, std::logic_error, "Invalid (a,b) line parameters!");
    a = a_ / h;
    b = b_ / h;
    c = c_ / h;
    _compute_refdir();
    return;
  }
   
  line::line(const point & start_, const vector2 & direction_)
  {
    start = start_;
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG,
    //              "start=" << start_ << ' ' << "direction=" << direction_);
    DT_THROW_IF(direction_.mag() < 1.e-14,
                std::logic_error,
                "Invalid line direction!");
    // double dx = direction_.x();
    // double dy = direction_.y();
    // double dn = std::hypot(dx, dy);
    // DT_THROW_IF(dn < 1.e-14, std::logic_error,
    //             "Invalid (dx=" << std::to_string(dx) << "," << "dy=" << std::to_string(dy) << ") line parameters!");
    direction = direction_.unit(); // .set(dx / dn, dy / dn);
    _compute_abc();
    return;
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

  bool line::is_horizontal(double tolerance_) const
  {
    if (std::abs(a) < tolerance_) return true;
    return false;
  }

  bool line::is_vertical(double tolerance_) const
  {
    if (std::abs(b) < tolerance_) return true;
    return false;
  }
  
  double line::distance(const point & pos_) const
  {
    double dist = a * pos_.x() + b * pos_.y() + c;
    return std::abs(dist);
  }
    
  vector2 line::get_direction() const
  {
    // double h = hypot(a,b);
    // return vector2(-b/h, a/h);
    return direction;
  }
    
  vector2 line::get_orthogonal_direction() const
  {
    // double h = hypot(a,b);
    // return vector2(a/h, b/h);
    return vector2(direction.y(), -direction.x());
  }

  bool line::unique_intersection(const line & other_, point & p_) const
  {
    p_.setX(datatools::invalid_real());
    p_.setY(datatools::invalid_real());
    double den = a * other_.b - b * other_.a;
    if (std::abs(den) < 1e-14) return false;
    double x0 = (b * other_.c - c * other_.b) / den;
    double y0 = (c * other_.a - a * other_.c) / den;
    p_.set(x0, y0);
    return true;
  }
 
  void line::compute(double t_, point & vtx_) const
  {
    vtx_ = start + t_ * direction;
    return;    
  }

  void line::_compute_refdir()
  {
    if (std::abs(c) < 1e-14) {
      start.set(0.0, 0.0);
      t0 = 0.0;
      double dn = std::sqrt(1.0 + (a*a)/(b*b));
      direction.set(1.0 / dn, -a / b / dn);
      return;
    }
    if (std::abs(a) < 1e-14) {
      start.set(0.0, -c);
      t0 = 0.0;
      direction.set(1.0, 0.0);
      return;
    }
    if (std::abs(b) < 1e-14) {
      start.set(-c, 0.0);
      t0 = 0.0;
      direction.set(0.0, 1.0);
      return;
    }
    double dx = -c / a;
    double dy =  c / b;
    double hd = std::hypot(dx, dy);
    start.set(- a * c / (a * a + b * b), - b * c / (a * a + b * b));
    direction.set(dx / hd, dy / hd);
    return;
  }
 
  void line::_compute_abc()
  {
    a = direction.y();
    b = -direction.x();
    c = direction.x() * start.y() - direction.y() * start.x();
    return;
  }

  void line::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- a = " << a << std::endl;
    out_ << indent_ << "|-- b = " << b << std::endl;
    out_ << indent_ << "|-- c = " << c << std::endl;
    double h = std::hypot(a, b);
    out_ << indent_ << "|-- h = " << h << std::endl;
    out_ << indent_ << "|-- t0 = " << t0 << std::endl;
    out_ << indent_ << "|-- start.x = " << start.x() << std::endl;
    out_ << indent_ << "|-- start.y = " << start.y() << std::endl;
    out_ << indent_ << "|-- direction.x = " << direction.x() << std::endl;
    out_ << indent_ << "`-- direction.y = " << direction.y() << std::endl;
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
    geomtools::invalidate(start);
    geomtools::invalidate(direction);
    // clear(start);
    // clear(direction);
    t0 = 0.0;
    a = std::numeric_limits<double>::quiet_NaN();
    b = std::numeric_limits<double>::quiet_NaN();
    c = std::numeric_limits<double>::quiet_NaN();
    return;
  }

} // namespace lttc 
