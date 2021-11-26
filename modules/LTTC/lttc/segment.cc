#include <lttc/segment.hh>

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

  segment::segment()
  {
    geomtools::invalidate(_first_);
    geomtools::invalidate(_last_);
    datatools::invalidate(_length_);
  }
 
  segment::segment(const point & first_, const point & last_)
  {
    set(first_, last_);
    return;
  }
  
  bool segment::is_valid() const
  {
    if (not geomtools::is_valid(_first_)) return false;
    if (not geomtools::is_valid(_last_)) return false;
    return true;
  }

  void segment::set(const point & first_, const point & last_)
  {
    double l = (_last_ - _first_).mag();
    DT_THROW_IF(l == 0, std::logic_error, "First and last ends are equal!");
    _first_ = first_;
    _last_ = last_;
    _line_ = line::make_from_start_stop(first_, last_ - first_);
    _length_ = l;
    return;
  }

  const point & segment::get_first() const
  {
    return _first_;
  }

  const point & segment::get_last() const
  {
    return _first_;
  }

  double segment::get_length() const
  {
    return _length_;
  }

  const line & segment::get_line() const
  {
    return _line_;
  }

  void segment::reset()
  {
    geomtools::invalidate(_first_);
    geomtools::invalidate(_last_);
    datatools::invalidate(_length_);
    _line_.clear();
    return;
  }

  bool segment::contains(const point & pos_, double tolerance_) const
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
