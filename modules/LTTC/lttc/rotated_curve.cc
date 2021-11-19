#include <lttc/rotated_curve.hh>

// Standard library:
#include <cmath>

namespace lttc {
   
  rotated_curve::rotated_curve(const i_curve & crv_, const point & center_, double angle_)
  {
    _crv_ = &crv_;
    _center_ = center_;
    _angle_ = angle_;
    return;
  }
  
  void rotated_curve::set_rotated_curve(const i_curve & crv_, const point & center_, double angle_)
  {
    _crv_ = &crv_;
    _center_ = center_;
    _angle_ = angle_;
    return;
  }

  void rotated_curve::compute(double t_, point & vtx_) const
  {
    point vtx;
    _crv_->compute(t_, vtx);
    double dx = vtx.x() - _center_.x();
    double dy = vtx.y() - _center_.y();
    double ca = std::cos(_angle_);
    double sa = std::sin(_angle_);
    double dx2 = dx * ca - dy * sa; 
    double dy2 = dx * sa + dy * ca; 
    double x = _center_.x() + dx2;
    double y = _center_.y() + dy2;
    vtx_.set(x,y);
    return;
  }

  void rotated_curve::generate_samples(double t1_, double t2_, polyline & samples_) const
  {
    polyline s;
    _crv_->generate_samples(t1_, t2_, s);
    double ca = std::cos(_angle_);
    double sa = std::sin(_angle_);
    for (const auto & p : s) {
      double dx = p.x() - _center_.x();
      double dy = p.y() - _center_.y();
      double dx2 = dx * ca - dy * sa; 
      double dy2 = dx * sa + dy * ca; 
      double x = _center_.x() + dx2;
      double y = _center_.y() + dy2;
      point tp(x, y);
      samples_.push_back(tp);
    }
    return;
  }

} // namespace lttc 
