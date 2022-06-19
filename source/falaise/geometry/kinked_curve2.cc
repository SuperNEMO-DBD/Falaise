#include <falaise/geometry/kinked_curve2.hh>

// Standard library:
#include <cmath>

namespace falaise {
  
  namespace geometry {
   
    kinked_curve2::kinked_curve2(const i_curve2 & crv_, double s_pivot_, double angle_)
    {
      _crv_ = &crv_;
      _s_pivot_ = s_pivot_;
      _angle_ = angle_;
      _crv_->compute(_s_pivot_, _pivot_);
      _kinked_arm_.set_rotated_curve(*_crv_, _pivot_, _angle_);
      return;
    }

    double kinked_curve2::get_s_pivot() const
    {
      return _s_pivot_;
    }

    double kinked_curve2::get_angle() const
    {
      return _angle_;
    }
 
    const point2 & kinked_curve2::get_pivot() const
    {
      return _pivot_;
    }

    void kinked_curve2::compute(double s_, point2 & vtx_) const
    {
      if (s_ < _s_pivot_) {
        _crv_->compute(s_, vtx_);
      } else {
        _kinked_arm_.compute(s_, vtx_);
      }
      return;
    }

    void kinked_curve2::generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_) const
    {
      samples_.clear();
      if (s1_ < _s_pivot_ and s2_ < _s_pivot_) {
        _crv_->generate_samples(s1_, s2_, samples_, nsamples_);
      } else if (s1_ > _s_pivot_ and s2_ > _s_pivot_) {
        _kinked_arm_.generate_samples(s1_, s2_, samples_, nsamples_);
      } else {
        _crv_->generate_samples(s1_, _s_pivot_, samples_, nsamples_);
        polyline2 armSamples;
        _kinked_arm_.generate_samples(_s_pivot_, s2_, armSamples, nsamples_);
        for (size_t i = 0; i < armSamples.size(); i++) {
          samples_.push_back(armSamples[i]);
          samples_.s.push_back(armSamples.s[i]);
        }
      }
      return;
    }
  
  } // namespace geometry 

} // namespace falaise 
