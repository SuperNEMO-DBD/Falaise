#include <falaise/geometry/translated_curve2.hh>

// Standard library:
#include <cmath>

namespace falaise {
  
  namespace geometry {
   
    translated_curve2::translated_curve2(const i_curve2 & crv_, const point2 & translation_)
    {
      _crv_ = &crv_;
      _translation_ = translation_;
      return;
    }

    translated_curve2::translated_curve2(const i_curve2 & crv_, double dx_, double dy_)
    {
      _crv_ = &crv_;
      _translation_.set(dx_, dy_);
      return;
    }

    point2 translated_curve2::get_reference_point() const
    {
      return _crv_->get_reference_point();
    }

    void translated_curve2::compute(double t_, point2 & vtx_) const
    {
      point2 vtx;
      _crv_->compute(t_, vtx);
      vtx_ = vtx + _translation_;
      return;
    }

    const point2 & translated_curve2::translation() const
    {
      return _translation_;
    }

    void translated_curve2::generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_) const
    {
      polyline2 origSamples;
      _crv_->generate_samples(s1_, s2_, origSamples, nsamples_);
      samples_.clear();
      samples_.reserve(origSamples.size());
      samples_.s.reserve(origSamples.size());
      for (size_t i = 0; i < origSamples.size(); i++) {
        const auto & pi = origSamples[i];
        double si = origSamples.s[i];
        point2 tpi = pi;
        tpi += _translation_;
        samples_.push_back(tpi);
        samples_.s.push_back(si);
      }
      return;
    }

  } // namespace geometry 

} // namespace falaise 
