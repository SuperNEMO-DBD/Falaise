// Ourseles:
#include <lttc/fitted_point2.hh>

// Third party:
// - Bayeux :
#include <geomtools/ellipse.h>
// - GSL :
#include <gsl/gsl_math.h>

namespace lttc {

  point2 fitted_point2::center() const
  {
    return point2(x, y);
  }
  
  bool fitted_point2::is_valid() const
  {
    if (x != x) return false;
    if (y != y) return false;
    return true;
  }
 
  void fitted_point2::reset()
  {
    x = datatools::invalid_real();
    y = datatools::invalid_real();
    x_err = datatools::invalid_real();
    y_err = datatools::invalid_real();
    angle = 0.0;
    return;
  }

  void fitted_point2::draw(std::ostream & out_, double nsigma_, bool no_cross_, double z_, int tag_) const
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
      out_ << (this->x + xi2) << ' ' << (this->y + yi2) << ' ' << z_ << ' ' << tag_ << '\n';
    }
    out_ << '\n';
    if (draw_cross) {
      point2 vtx[4];
      vtx[0].set(-x_err,    0.0); 
      vtx[1].set(+x_err,    0.0); 
      vtx[2].set(   0.0, -y_err); 
      vtx[3].set(   0.0, +y_err);
      for (int i = 0; i < 4; i++) {
        double xi2 = ct * vtx[i].x() - st * vtx[i].y();
        double yi2 = st * vtx[i].x() + ct * vtx[i].y();       
        out_ << (this->x - xi2) << ' ' <<  (this->y - yi2) << ' ' << z_ << ' ' << tag_ << '\n';
        if (i % 2 == 1) {
          out_ << '\n';
        }
      }
    }
    return;
  }

  bool fitted_point2::inside(const point2 & p_, double nsigma_) const
  {
    double x1 = p_.x() - this->x;
    double y1 = p_.y() - this->y;
    double ct = std::cos(this->angle);
    double st = std::sin(this->angle);
    double x2 =  ct * x1 + st * y1;
    double y2 = -st * x1 + ct * y1;
    double a = nsigma_ * x_err;
    double b = nsigma_ * y_err;
    if (gsl_pow_2(x2 / a) + gsl_pow_2(y2 / b) > 1.0) return false;
    return true;
  }
  
  double fitted_point2::residual(const point2 & p_) const
  {
    double x1 = p_.x() - this->x;
    double y1 = p_.y() - this->y;
    double ct = std::cos(this->angle);
    double st = std::sin(this->angle);
    // double x2 =  ct * x1 + st * y1;
    // double y2 = -st * x1 + ct * y1;
    // return (gsl_pow_2(x2 / x_err) + gsl_pow_2(y2 / y_err));
    double x2 =  (ct * x1 + st * y1) / x_err;
    double y2 = (-st * x1 + ct * y1) / y_err;
    return gsl_pow_2(x2) + gsl_pow_2(y2);
  }

  double fitted_point2::nsigma(const point2 & p_) const
  {
    return std::sqrt(residual(p_));
  }

  bool fitted_point2::equal(const fitted_point2 & other_, double nsigma_) const
  {
    if (this->nsigma(point2(other_.x, other_.y)) <= nsigma_) return true;
    if (other_.nsigma(point2(this->x, this->y)) <= nsigma_) return true;
    return false;
  }

  std::ostream & operator<<(std::ostream & out_, const fitted_point2 & fp_)
  {
    out_ << "{x=" << fp_.x  << ",y=" << fp_.y << ",x_err=" << fp_.x_err << ",y_err=" << fp_.y_err
         << ",angle=" << fp_.angle << '}';
    return out_;
  }

} // namespace lttc 
