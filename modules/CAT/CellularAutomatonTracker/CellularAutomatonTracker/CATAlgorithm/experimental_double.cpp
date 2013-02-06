/* -*- mode: c++ -*- */

#include <CATAlgorithm/experimental_double.h>
#include <cmath>
#include <limits>
#include <iomanip>
#include <mybhep/utilities.h>

namespace CAT {
  namespace topology {

    using namespace std;

    bool experimental_double::is_valid () const
    {
      return is_value_valid () && is_error_valid ();
    }
    
    bool experimental_double::is_value_valid () const
    {
      return v_ == v_;
    }
    
    bool experimental_double::is_error_valid () const
    {
      return e_ == e_;
    }

    //!Default constructor     
    experimental_double::experimental_double()
    {
      v_ = std::numeric_limits<double>::quiet_NaN ();
      e_ = std::numeric_limits<double>::quiet_NaN ();
      return;
    }

    //!Default destructor
    experimental_double::~experimental_double()
    {
      return;
    }
    

    //! constructor
    experimental_double::experimental_double(double v, double e)
    {
      v_ = v;
      e_ = e;
      return;
    }

    /*** dump ***/
    void experimental_double::dump (ostream & a_out,
                                    const std::string & a_title,
                                    const std::string & a_indent,
                                    bool a_inherit) const
    {
      a_out << std::setprecision(15) << value() << " +- " << std::setprecision(15)  << error();
      return;
    }

    //! set value and error
    void experimental_double::set(experimental_double v){
      v_ = v.value();
      e_ = v.error();
    }

    //! set value and error
    void experimental_double::set(double val, double err){
      v_ = val;
      e_ = err;
    }

    //! set value
    void experimental_double::set_value(double v){
      v_ = v;
    }

    //! set error
    void experimental_double::set_error(double e){
      e_ = e;
    }

    //! get value
    const double& experimental_double::value() const{
      return v_;
    }

    //! get error
    const double& experimental_double::error() const{
      return e_;
    }

    // Operators
    //! operador += 
    experimental_double& experimental_double::operator += (const experimental_double& p2)
    {
      experimental_double& p1= *this;
      double val = p1.value() + p2.value();
      double err = std::sqrt(mybhep::square(p1.error()) + mybhep::square(p2.error()));
      p1.set_value(val);
      p1.set_error(err);
      return p1;
    }

    //! operador -= 
    experimental_double& experimental_double::operator -= (const experimental_double& p2)
    {
      experimental_double& p1= *this;
      double val = p1.value() - p2.value();
      double err = std::sqrt(mybhep::square(p1.error()) + mybhep::square(p2.error()));
      p1.set_value(val);
      p1.set_error(err);

      return p1;
    }

    //! operador *= 
    experimental_double& experimental_double::operator *= (experimental_double a)
    {
      experimental_double& p1= *this;
      double val = p1.value()*a.value();
      double err = std::sqrt(mybhep::square(a.value()*p1.error()) + mybhep::square(p1.value()*a.error()));

      p1.set_value(val);
      p1.set_error(err);

      return p1;
    }

    //! operador *= 
    experimental_double& experimental_double::operator *= (double a)
    {
      experimental_double& p1= *this;
      double val = p1.value()*a;
      double err = p1.error()*fabs(a);

      p1.set_value(val);
      p1.set_error(err);

      return p1;
    }

    //! operador /= 
    experimental_double& experimental_double::operator /= (experimental_double a)
    {
      experimental_double& p1= *this;

      if( a.value() == 0 ){
        std::clog << " problem: division by experimental_double with value " << a.value() << std::endl;
      }

      double val = p1.value()/a.value();
      double err = std::sqrt(mybhep::square(p1.error()/a.value()) + mybhep::square(p1.value()*a.error()/mybhep::square(a.value())));
      p1.set_value(val);
      p1.set_error(err);
      return p1;
    }

    //! operador /= 
    experimental_double& experimental_double::operator /= (double a)
    {
      experimental_double& p1= *this;

      if( a == 0 ){
        std::clog << " problem: division by double with value " << a << std::endl;
      }

      double val = p1.value()/a;
      double err = p1.error()/fabs(a);
      p1.set_value(val);
      p1.set_error(err);
      return p1;
    }

    // Operations with experimental_points
    // -v
    // sin(v) 
    experimental_double experimental_sin (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(sin(v1.value()));
      v.set_error(fabs(cos(v1.value()))*v1.error());
      return v;
    }
  
    // cos(v) 
    experimental_double experimental_cos (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(cos(v1.value()));
      v.set_error(fabs(sin(v1.value()))*v1.error());
      return v;
    }
  
    // tan(v) 
    experimental_double experimental_tan (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(tan(v1.value()));
      v.set_error((1. + mybhep::square(v.value()))*v1.error());
      return v;
    }
  
    // asin(v) 
    experimental_double experimental_asin (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(asin(v1.value()));
      v.set_error(v1.error()/std::sqrt(1 - mybhep::square(v1.value())));
      return v;
    }
  
    // acos(v) 
    experimental_double experimental_acos (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(acos(v1.value()));
      v.set_error(v1.error()/std::sqrt(1 - mybhep::square(v1.value())));
      return v;
    }
  
    // atan(v) 
    experimental_double experimental_atan2 (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v;
      v.set_value(atan2(v1.value(), v2.value()));

      if( v2.value() == 0. ){  // if angle = 90 degrees,
        // obtain error from angle = 180 - (other angle)
        double den = 1 + mybhep::square(v2.value()/v1.value());
        double num = mybhep::square(v2.error()/v1.value()) + mybhep::square(v2.value()*v1.error()/mybhep::square(v1.value()));
        v.set_error(std::sqrt(num)/den);
      }
      else{

#if 0
        if( fabs(v1.error()) > fabs(v1.value()) ){
          // large error propagation for the arctan !
          // when "y" has large error, standard error propagation under-estimates the error
          double r1 = atan2(v1.value() - v1.error(), v2.value());
          double r2 = atan2(v1.value() + v1.error(), v2.value());
          v.set_error(fabs(r1 - r2)/2.);
          return v;
        }
#endif

        double den = 1 + mybhep::square(v1.value()/v2.value());
        double num = mybhep::square(v1.error()/v2.value()) + mybhep::square(v1.value()*v2.error()/mybhep::square(v2.value()));
        v.set_error(std::sqrt(num)/den);
      
      }
      return v;
    }
  
    // square(v) 
    experimental_double experimental_square (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(mybhep::square(v1.value()));
      v.set_error(2*fabs(v1.value())*v1.error());
      return v;
    }
  
    // sqrt(v) 
    experimental_double experimental_sqrt (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(std::sqrt(v1.value()));
      v.set_error(v1.error()/(2*v.value()));
      return v;
    }
  
    // cube(v) 
    experimental_double experimental_cube (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(mybhep::cube(v1.value()));
      v.set_error(3*mybhep::square(v1.value())*v1.error());
      return v;
    }
  
    // fabs(v) 
    experimental_double experimental_fabs (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(fabs(v1.value()));
      v.set_error(v1.error());
      return v;
    }
  
    experimental_double operator - (const experimental_double& v1)
    {
      experimental_double v = v1;
      v.set_value(-v1.value());
      v.set_error(v1.error());
    
      return v;
    }

    // v1+v2
    experimental_double operator + (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1; 
      v+=v2;   
      return v;
    }
    //! v1-v2
    experimental_double operator - (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1; 
      v-=v2;   
      return v;
    }

    // v*d
    experimental_double operator * (const experimental_double& v1, const experimental_double& d)
    {
      experimental_double v = v1; 
      v*=d;   
      return v;
    }

    // v/d
    experimental_double operator / (const experimental_double& v1, const experimental_double& d)
    {
      experimental_double v = v1; 
      v/=d;   
      return v;
    }

    // v*d
    experimental_double operator * (const experimental_double& v1, double d)
    {
      experimental_double v = v1; 
      experimental_double dd(d,0.);
      v*=dd;   
      return v;
    }

    // v/d
    experimental_double operator / (const experimental_double& v1, double d)
    {
      experimental_double v = v1; 
      experimental_double dd(d,0.);
      v/=dd;   
      return v;
    }

    // v/d
    experimental_double operator / (double d, const experimental_double& v1)
    {
      experimental_double dd(d,0.);
      experimental_double v = v1; 
      dd/=v;   
      return dd;
    }

    // average
    experimental_double average (const std::vector<experimental_double> vs)
    {

      if( vs.size() == 0 ){
        experimental_double bad;
        std::clog << " problem: avereging std::vector of size " << vs.size() << std::endl;
        return bad;
      }


      double mean = 0.;
      double err = 0.;

      for(std::vector<experimental_double>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        mean += iv->value();
        err += mybhep::square(iv->error());
      }

      return experimental_double(mean/vs.size(), std::sqrt(err)/vs.size());

    }

    // weighted average
    experimental_double weighted_average (const std::vector<experimental_double> vs)
    {
      double mean = 0.;
      double inverr = 0.;

      for(std::vector<experimental_double>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        mean += iv->value()/mybhep::square(iv->error());
        inverr += 1/mybhep::square(iv->error());
      }

      return experimental_double(mean/inverr, std::sqrt(1./inverr));
    }


  }

}
