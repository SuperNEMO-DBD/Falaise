#include <lttc/kinked_curve.hh>

// Standard library:
#include <cmath>

namespace lttc {
   
  kinked_curve::kinked_curve(const i_curve & crv_, double tpivot_, double angle_)
  {
    _crv_ = &crv_;
    _tpivot_ = tpivot_;
    _angle_ = angle_;
    _crv_->compute(_tpivot_, _pivot_);
    _kinked_arm_.set_rotated_curve(*_crv_, _pivot_, _angle_);
    return;
  }

  double kinked_curve::get_tpivot() const
  {
    return _tpivot_;
  }

  double kinked_curve::get_angle() const
  {
    return _angle_;
  }
 
  const point & kinked_curve::get_pivot() const
  {
    return _pivot_;
  }

  void kinked_curve::compute(double t_, point & vtx_) const
  {
    if (t_ < _tpivot_) {
      _crv_->compute(t_, vtx_);
    } else {
      _kinked_arm_.compute(t_, vtx_);
    }
    return;
  }

  void kinked_curve::generate_samples(double t1_, double t2_, polyline & samples_) const
  {
    if (t1_ < _tpivot_ and t2_ < _tpivot_) {
      _crv_->generate_samples(t1_, t2_, samples_);
    } else if (t1_ > _tpivot_ and t2_ > _tpivot_) {
      _kinked_arm_.generate_samples(t1_, t2_, samples_);
    } else {
      _crv_->generate_samples(t1_, _tpivot_, samples_);
      _kinked_arm_.generate_samples(_tpivot_, t2_, samples_);      
    }
    return;
  }

} // namespace lttc 
