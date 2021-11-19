#include <lttc/point.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools :
#include <datatools/utils.h>

namespace lttc {

  void clear(point & p_)
  {
    p_.set(datatools::invalid_real(), datatools::invalid_real());
    return;
  }

  void draw_point(std::ostream & out_, const point & p_, int tag_, bool endl_)
  {
    out_ << p_.x() << ' ' << p_.y() << ' ' << "0.0" << ' ' << tag_;
    if (endl_) out_ << '\n';
    return;
  }

  void draw_polyline(std::ostream & out_, const polyline & pl_, int tag_)
  {
    for (const auto & p : pl_) {
      draw_point(out_, p, tag_, true);
    }
    out_ << '\n';
    return;
  }

  point barycenter(const point & p1_, double w1_, const point & p2_, double w2_)
  {
    point p;
    double ws = w1_ + w2_;
    double w1 = w1_ / ws;
    double w2 = w2_ / ws;
    p.setX(p1_.x() * w1 + p2_.x() * w2);
    p.setY(p1_.y() * w1 + p2_.y() * w2);
    return p;
  }

  point middle(const point & p1_, const point & p2_)
  {
    return barycenter(p1_, 1.0, p2_, 1.0);
  }

  bool are_aligned(const point & p1_,
                   const point & p2_,
                   const point & p3_,
                   double tolerance_)
  {
    
    point p12 = (p2_ - p1_).unit();
    point p13 = (p3_ - p1_).unit();
    double t = p12.x() * p13.y() - p12.y() * p13.x();
    // t = p12.cross(p13).mag();
    return std::abs(t) < tolerance_;
  }
  
} // namespace lttc 
