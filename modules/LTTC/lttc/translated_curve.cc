#include <lttc/translated_curve.hh>

// Standard library:
#include <cmath>

namespace lttc {
   
  translated_curve::translated_curve(const i_curve & crv_, const point & translation_)
  {
    _crv_ = &crv_;
    _translation_ = translation_;
    return;
  }

  translated_curve::translated_curve(const i_curve & crv_, double dx_, double dy_)
  {
    _crv_ = &crv_;
    _translation_.set(dx_, dy_);
    return;
  }

  void translated_curve::compute(double t_, point & vtx_) const
  {
    point vtx;
    _crv_->compute(t_, vtx);
    vtx_ = vtx + _translation_;
    return;
  }

  const point & translated_curve::translation() const
  {
    return _translation_;
  }

  void translated_curve::generate_samples(double t1_, double t2_, polyline & samples_) const
  {
    polyline s;
    _crv_->generate_samples(t1_, t2_, s);
    for (const auto & p : s) {
      point tp = p;
      tp += _translation_;
      samples_.push_back(tp);
    }
    return;
  }

} // namespace lttc 
