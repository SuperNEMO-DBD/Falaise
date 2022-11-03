#include <falaise/geometry/rotated_curve2.hh>

// Standard library:
#include <cmath>

namespace falaise {
  
  namespace geometry {
   
    rotated_curve2::rotated_curve2(const i_curve2 & crv_, const point2 & center_, double angle_)
    {
      _crv_ = &crv_;
      _center_ = center_;
      _angle_ = angle_;
      return;
    }
  
    void rotated_curve2::set_rotated_curve(const i_curve2 & crv_, const point2 & center_, double angle_)
    {
      _crv_ = &crv_;
      _center_ = center_;
      _angle_ = angle_;
      return;
    }

    void rotated_curve2::compute(double s_, point2 & vtx_) const
    {
      point2 vtx;
      _crv_->compute(s_, vtx);
      double dx = vtx.x() - _center_.x();
      double dy = vtx.y() - _center_.y();
      double ca = std::cos(_angle_);
      double sa = std::sin(_angle_);
      double dx2 = dx * ca - dy * sa; 
      double dy2 = dx * sa + dy * ca; 
      double x = _center_.x() + dx2;
      double y = _center_.y() + dy2;
      vtx_.set(x, y);
      return;
    }

    void rotated_curve2::generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_) const
    {
      polyline2 samples;
      _crv_->generate_samples(s1_, s2_, samples, nsamples_);
      double ca = std::cos(_angle_);
      double sa = std::sin(_angle_);
      samples_.clear();
      samples_.reserve(samples.size());
      samples_.s.reserve(samples.size());
      for (int i = 0; i < (int) samples.size(); i++) {
        const auto & pi = samples[i];
        double si = samples.s[i];
        double dx = pi.x() - _center_.x();
        double dy = pi.y() - _center_.y();
        double dx2 = dx * ca - dy * sa; 
        double dy2 = dx * sa + dy * ca; 
        double x = _center_.x() + dx2;
        double y = _center_.y() + dy2;
        point2 pt(x, y);
        samples_.push_back(pt);
        samples_.s.push_back(si);
      }
      return;
    }

  } // namespace geometry 

} // namespace falaise 
