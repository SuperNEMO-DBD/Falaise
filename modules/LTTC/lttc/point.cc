#include <lttc/point.hh>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux :
#include <datatools/utils.h>
#include <geomtools/ellipse.h>
// - GSL :
#include <gsl/gsl_math.h>

namespace lttc {

  point3 to_3d(const point2 & p2_, double z_)
  {
    return point3(p2_.x(), p2_.y(), z_);
  }

  point2 to_2d(const point3 & p3_)
  {
    return point2(p3_.x(), p3_.y());
  }

  void clear(point2 & p_)
  {
    p_.set(datatools::invalid_real(), datatools::invalid_real());
    return;
  }

  void clear(point3 & p_)
  {
    p_.set(datatools::invalid_real(), datatools::invalid_real(), datatools::invalid_real());
    return;
  }

  void draw_point(std::ostream & out_, const point2 & p_, int tag_, bool endl_)
  {
    out_ << p_.x() << ' ' << p_.y() << ' ' << tag_;
    if (endl_) out_ << '\n';
    return;
  }

  void draw_point(std::ostream & out_, const point3 & p_, int tag_, bool endl_)
  {
    out_ << p_.x() << ' ' << p_.y() << ' ' <<  p_.z() << ' ' << tag_;
    if (endl_) out_ << '\n';
    return;
  }

  void polyline2::clear()
  {
    this->std::vector<point2>::clear();
    s.clear();
    return;
  }

  void draw_polyline(std::ostream & out_, const polyline2 & pl_, int tag_)
  {
    for (size_t i = 0; i < pl_.size(); i++) {
      const point2 & pi = pl_[i];
      draw_point(out_, pi, tag_, true);
    }
    out_ << '\n';
    return;
  }
  
  void polyline3::from_2d(const polyline2 & pl2_, double z_)
  {
    this->clear();
    this->reserve(pl2_.size());
    this->s.reserve(pl2_.size());
    for (size_t i = 0; i < pl2_.size(); i++) {
      const point2 & p2 = pl2_[i];
      point3 p3 = to_3d(p2, z_);
      this->push_back(p3);
      this->s.push_back(pl2_.s[i]);
    }
    return;
  }

  void polyline3::clear()
  {
    this->std::vector<point3>::clear();
    s.clear();
    return;
  }

  void draw_polyline(std::ostream & out_, const polyline3 & pl_, int tag_)
  {
    for (const point3 & p : pl_) {
      draw_point(out_, p, tag_, true);
    }
    out_ << '\n';
    return;
  }

  point2 barycenter(const point2 & p1_, double w1_, const point2 & p2_, double w2_)
  {
    point2 p;
    double ws = w1_ + w2_;
    double w1 = w1_ / ws;
    double w2 = w2_ / ws;
    p.setX(p1_.x() * w1 + p2_.x() * w2);
    p.setY(p1_.y() * w1 + p2_.y() * w2);
    return p;
  }

  point3 barycenter(const point3 & p1_, double w1_, const point3 & p2_, double w2_)
  {
    point3 p;
    double ws = w1_ + w2_;
    double w1 = w1_ / ws;
    double w2 = w2_ / ws;
    p.setX(p1_.x() * w1 + p2_.x() * w2);
    p.setY(p1_.y() * w1 + p2_.y() * w2);
    p.setZ(p1_.z() * w1 + p2_.z() * w2);
    return p;
  }

  point2 middle(const point2 & p1_, const point2 & p2_)
  {
    return barycenter(p1_, 1.0, p2_, 1.0);
  }

  point3 middle(const point3 & p1_, const point3 & p2_)
  {
    return barycenter(p1_, 1.0, p2_, 1.0);
  }

  bool are_aligned(const point2 & p1_,
                   const point2 & p2_,
                   const point2 & p3_,
                   double tolerance_)
  {
    
    point2 p12 = (p2_ - p1_).unit();
    point2 p13 = (p3_ - p1_).unit();
    double t = p12.x() * p13.y() - p12.y() * p13.x();
    return std::abs(t) < tolerance_;
  }

} // namespace lttc 
