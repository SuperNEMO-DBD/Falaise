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
    return std::abs(t) < tolerance_;
  }

  bool fitted_point::is_valid() const
  {
    if (x != x) return false;
    return true;
  }
 
  void fitted_point::reset()
  {
    x = datatools::invalid_real();
    y = datatools::invalid_real();
    x_err = datatools::invalid_real();
    y_err = datatools::invalid_real();
    angle = 0.0;
    return;
  }

  void fitted_point::draw(std::ostream & out_, double nsigma_, bool no_cross_, int tag_) const
  {
    double nsigma = nsigma_;
    if (nsigma <= 0.0) nsigma = 1.0;
    bool draw_cross = ! no_cross_;
    geomtools::ellipse drawnEllipse(x_err * nsigma, y_err * nsigma, 0.0, 2 * M_PI);
    double ct = std::cos(this->angle);
    double st = std::sin(this->angle);    
    for (geomtools::angular_range::iterator iter(drawnEllipse.get_angle_domain(), 128);
         !iter;
         ++iter) {
      double thetai = *iter;
      double xi, yi;
      drawnEllipse.compute_x_y_from_theta(thetai, xi, yi);
      double xi2 = ct * xi - st * yi;
      double yi2 = st * xi + ct * yi;      
      out_ << (this->x + xi2) << ' ' << (this->y + yi2) << ' ' << tag_ << '\n';
    }
    out_ << '\n';
    if (draw_cross) {
      point vtx[4];
      vtx[0].set(-x_err, 0.0); 
      vtx[1].set(+x_err, 0.0); 
      vtx[2].set(   0.0, -y_err); 
      vtx[3].set(   0.0, +y_err);
      for (int i = 0; i < 4; i++) {
        double xi2 = ct * vtx[i].x() - st * vtx[i].y();
        double yi2 = st * vtx[i].x() + ct * vtx[i].y();       
        out_ << (this->x - xi2) << ' ' <<  (this->y - yi2) << ' '  << tag_ << '\n';
        if (i % 2 == 1) {
          out_ << '\n';
        }
      }
    }
    return;
  }

  bool fitted_point::inside(const point & p_, double nsigma_) const
  {
    double x1 = p_.x() - this->x;
    double y1 = p_.y() - this->y;
    double ct = std::cos(this->angle);
    double st = std::sin(this->angle);
    double x2 = ct * x1 + st * y1;
    double y2 = -st * x1 + ct * y1;
    double a = nsigma_ * x_err;
    double b = nsigma_ * y_err;
    return gsl_pow_2(x2 / a) + gsl_pow_2(y2 / b) <= 1.0;
  }
  
} // namespace lttc 
