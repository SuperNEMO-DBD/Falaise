#include <lttc/segment2.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>
// - GSL :
#include <gsl/gsl_math.h>

namespace lttc {

  segment2::segment2()
  {
    geomtools::invalidate(_first_);
    geomtools::invalidate(_last_);
    datatools::invalidate(_length_);
  }
 
  segment2::segment2(const point2 & first_, const point2 & last_)
  {
    set(first_, last_);
    return;
  }
  
  bool segment2::is_valid() const
  {
    if (not geomtools::is_valid(_first_)) return false;
    if (not geomtools::is_valid(_last_)) return false;
    return true;
  }

  void segment2::set(const point2 & first_, const point2 & last_)
  {
    double l = (_last_ - _first_).mag();
    DT_THROW_IF(l == 0, std::logic_error, "First and last ends are equal!");
    _first_ = first_;
    _last_ = last_;
    _line_ = line2::make_from_start_stop(first_, last_ - first_);
    _length_ = l;
    return;
  }

  const point2 & segment2::get_first() const
  {
    return _first_;
  }

  const point2 & segment2::get_last() const
  {
    return _first_;
  }

  double segment2::get_length() const
  {
    return _length_;
  }

  const line2 & segment2::get_line() const
  {
    return _line_;
  }

  void segment2::reset()
  {
    geomtools::invalidate(_first_);
    geomtools::invalidate(_last_);
    datatools::invalidate(_length_);
    _line_.clear();
    return;
  }

  bool segment2::contains(const point2 & pos_, double tolerance_) const
  {
    double tolerance = tolerance_;
    if (tolerance <= 0.0) {
      tolerance = 1e-16;
    }
    double dx1 = pos_.x() - _first_.x();
    double dy1 = pos_.y() - _first_.y();
    double dx2 = _last_.x() - _first_.x();
    double dy2 = _last_.y() - _first_.y();
    double t = dx1 * dy2 - dy1 * dx2;
    if (std::abs(t) > gsl_pow_2(tolerance)) return false;
    if (std::abs(dx2) > tolerance) {
      double lambda = dx1 / dx2;
      return lambda >= 0.0 and lambda <= 1.0;
    }
    double lambda = dy1 / dy2;
    return lambda >= 0.0 and lambda <= 1.0;
  }

} // namespace lttc 
