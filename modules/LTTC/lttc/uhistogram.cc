// \file uhistogram.cc

// Ourselves:
#include <lttc/uhistogram.hh>

// Standard library:
#include <cmath>

namespace lttc {
    
  uhistogram::uhistogram(const double xref_, const double xstep_)
  {
    _xref_ = xref_;
    _xstep_ = xstep_;
    return;
  }

  void uhistogram::initialize(const double xref_, const double xstep_)
  {
    clear();
    _xref_ = xref_;
    _xstep_ = xstep_;
    return;
  }

  double uhistogram::xref() const
  {
    return _xref_;
  }

  double uhistogram::xstep() const
  {
    return _xstep_;
  }

  int uhistogram::index(const double value_) const
  {
    return (int) (std::floor((value_ - _xref_) / _xstep_));
  }

  double uhistogram::value(const int index_) const
  {
    return _xref_ + _xstep_ * index_;
  }

  void uhistogram::fill(const double value_, const double weight_)
  {
    int idx = index(value_);
    if (value_ < _xmin_) {
      _xmin_ = value_;
    }
    if (value_ > _xmax_) {
      _xmax_ = value_;
    }
    if (_imin_ > _imax_) {
      _imin_ = idx;
      _imax_ = idx;
    } else {
      if (idx < _imin_) {
        _imin_ = idx;
      }
      if (idx > _imax_) {
        _imax_ = idx;
      }
    }
    auto found = _counts_.find(idx);
    if (found == _counts_.end()) {
      _counts_[idx] = weight_;
    } else {
      found->second += weight_;
    }
    return;
  }

  double uhistogram::xmin() const
  {
    return _xmin_;
  }

  double uhistogram::xmax() const
  {
    return _xmax_;
  }

  double uhistogram::count(const int index_) const
  {
    const auto & found = _counts_.find(index_);
    if (found == _counts_.end()) {
      return 0.0;
    }
    return found->second;
  }
 
  void uhistogram::clear()
  {
    _xref_ = std::numeric_limits<double>::quiet_NaN();
    _xstep_ = std::numeric_limits<double>::quiet_NaN();
    _xmin_ = std::numeric_limits<double>::infinity();
    _xmax_ = -std::numeric_limits<double>::infinity();
    _imin_ = 0;
    _imax_ = -1;
    _counts_.clear();     
    return;
  }

  void uhistogram::store(std::ostream & out_) const
  {
    for (const auto & i : _counts_) {
      double x = value(i.first);
      out_.precision(15);
      out_ << x << ' ' << x + _xstep_ << ' ' << i.second << std::endl;
    }
    out_ << std::endl;
    return;
  }

} // namespace lttc 
