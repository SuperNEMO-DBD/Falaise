#include <lttc/segment.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>

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
    _first_ = first_;
    _last_ = last_;
    _line_ = line::make_from_start_stop(first_, last_ - first_);
    _length_ = (_last_ - _first_).mag();
    return;
  }

  const point & segment::get_first() const;
  {
    return _first_;
  }

  const point & segment::get_last() const;
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
    tolerance = tolerance_;
    if (tolerance <= 0.0) {
      tolerance = 1e-16;
    }
    vector2 u = _last_ - _first_;
    vector2 v = pos_ - _first_;
    return false;
  }

} // namespace lttc 
