#include <lttc/circle.hh>

// Standard library:
#include <cmath>

// This project:
#include <lttc/line2.hh>

// Third party:
// - Bayeux :
#include <datatools/logger.h>
#include <geomtools/utils.h>

namespace lttc {

  circle::circle(const point2 & center_, double r_)
  {
    _center_ = center_;
    _r_ = r_;
    return;
  }

  circle::circle(double x0_, double y0_, double r_)
  {
    _center_.set(x0_, y0_);
    _r_  = r_;
    return;
  }

  bool circle::is_valid() const
  {
    return datatools::is_valid(_r_) && geomtools::is_valid(_center_);
  }

  void circle::invalidate()
  {
    geomtools::invalidate(_center_);
    _r_ = datatools::invalid_real();
    return;
  }
 
  const point2 & circle::center() const
  {
    return _center_;
  }
    
  double circle::r() const
  {
    return _r_;
  }

  void circle::set_center(const point2 & c_)
  {
    _center_ = c_;
    return;
  }

  void circle::set_r(double r_)
  {
    _r_ = r_;
    return;
  }

  std::tuple<double,double> circle::eval(double x_) const
  {
    double t = _r_ * _r_ - (x_ - _center_.x()) * (x_ - _center_.x());
    t = std::sqrt(t);
    double rconcave = _center_.y() + t;
    double rconvex  = _center_.y() - t;
    return std::tuple<double,double>(rconcave, rconvex);
  }

  void circle::compute(double s_, point2 & vtx_) const
  {
    // vtx_.setX(_center_.x() + _r_ * std::cos(2 * M_PI * t_));
    // vtx_.setY(_center_.y() + _r_ * std::sin(2 * M_PI * t_));
    double theta = s_ / _r_;
    vtx_.setX(_center_.x() + _r_ * std::cos(theta));
    vtx_.setY(_center_.y() + _r_ * std::sin(theta));
    return;    
  }

  double circle::dist(const point2 & p_) const
  {
    vector2 cp = p_ - _center_;
    double mcp = cp.mag();
    if (mcp < _r_) {
      return _r_ - mcp;
    }
    return mcp - _r_;
  }

  void circle::generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_) const
  {
    samples_.clear();
    double sa = std::min(s1_,s2_);
    double sb = std::max(s1_,s2_);
    double sstep = (sb - sa) / nsamples_;
    for (double s = sa; s <= sb + 0.5 * sstep; s += sstep) {
      point2 p;
      this->compute(s, p);
      samples_.push_back(p);
      samples_.s.push_back(s);
    }    
    return;
  }
  
  // static
  circle circle::make_circle(const triplet2 & t_)
  {
    return make_circle(t_.p1, t_.p2, t_.p3);
  }
  
  // static
  circle circle::make_circle(const point2 & p1_, const point2 & p2_, const point2 & p3_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    // logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(logging, "Entering...");
    DT_LOG_DEBUG(logging, "  p1 = " << p1_);
    DT_LOG_DEBUG(logging, "  p2 = " << p2_);
    DT_LOG_DEBUG(logging, "  p3 = " << p3_);
    double d12 = (p2_ - p1_).mag2();
    double d23 = (p3_ - p2_).mag2();
    double d31 = (p1_ - p3_).mag2();
    const point2 * pfirst  = &p1_;
    const point2 * plast   = &p2_;
    const point2 * pmiddle = &p3_;
    double dmax = d12;
    std::string dLabel = "1-2";
    DT_LOG_DEBUG(logging, "d12  = " << d12);
    DT_LOG_DEBUG(logging, "d23  = " << d23);
    DT_LOG_DEBUG(logging, "d31  = " << d31);
    if (d23 > dmax) {
      dmax = d23;
      pfirst  = &p2_;
      plast   = &p3_;
      pmiddle = &p1_;
      dLabel = "2-3";
    }
    if (d31 > dmax) {
      dmax = d31;
      pfirst  = &p3_;
      plast   = &p1_;
      pmiddle = &p2_;
      dLabel = "3-1";
    }
    DT_LOG_DEBUG(logging, "dLabel  = " << dLabel);
    DT_LOG_DEBUG(logging, "  pfirst  = " << *pfirst);
    DT_LOG_DEBUG(logging, "  pmiddle = " << *pmiddle);
    DT_LOG_DEBUG(logging, "  plast   = " << *plast);
    line2 lFM = lttc::line2::make_from_start_stop(*pfirst, *pmiddle);
    // std::cerr << "*** devel *** FM:\n";
    // lFM.print(std::cerr);
    line2 lML = lttc::line2::make_from_start_stop(*pmiddle, *plast);
    // std::cerr << "*** devel *** ML:\n";
    // lML.print(std::cerr);
    point2 intersectionFML;
    point2 midFM = middle(*pfirst, *pmiddle);
    point2 midML = middle(*plast,  *pmiddle);
    DT_LOG_DEBUG(logging, "midFM=" << midFM);
    DT_LOG_DEBUG(logging, "midML=" << midML);
    line2 plFM = lFM.make_perp(midFM);
    // std::cerr << "*** devel *** perp(FM):\n";
    // plFM.print(std::cerr);
    line2 plML = lML.make_perp(midML);
    // std::cerr << "*** devel *** perp(ML):\n";
    // plML.print(std::cerr);
    DT_THROW_IF(!plFM.unique_intersection(plML, intersectionFML),
                std::logic_error,
                "Cannot make a circle from the 3 given points!");
    double Rest = (intersectionFML - *pmiddle).mag();
    DT_LOG_DEBUG(logging, "intersectionFML=" << intersectionFML);
    DT_LOG_DEBUG(logging, "Rest=" << Rest);
    return circle(intersectionFML, Rest);
  }

  // static
  bool circle::build_circle(const point2 & p1_, const point2 & p2_, const point2 & p3_, circle & c_)
  {
    c_.invalidate();
    try {
      circle c = make_circle(p1_, p2_, p3_);
      c_ = c;
    } catch (std::exception &) {
    }
    return c_.is_valid();
  }

} // namespace lttc 
