/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IEDOUBLE
#define __CATAlgorithm__IEDOUBLE
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>

namespace CAT {
  namespace topology {

    using namespace std;
    using namespace mybhep;

    class experimental_double {

      // an experimental_double is composed of a value and an error

    protected:
      double v_;
      double e_;

    public:   
      //!Default constructor     
      experimental_double()
      {
        v_ = small_neg;
        e_ = small_neg;
      }

      //!Default destructor
      virtual ~experimental_double(){};

      //! constructor
      experimental_double(double v, double e){
        v_ = v;
        e_ = e;
      }

      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false) const{
        a_out << setprecision(7) << value() << " +- " << setprecision(7)  << error();
        return;
      }

      //! set value and error
      void set(experimental_double v){
        v_ = v.value();
        e_ = v.error();
      }

      //! set value and error
      void set(double val, double err){
        v_ = val;
        e_ = err;
      }

      //! set value
      void set_value(double v){
        v_ = v;
      }

      //! set error
      void set_error(double e){
        e_ = e;
      }

      //! get value
      const double& value() const{
        return v_;
      }

      //! get error
      const double& error() const{
        return e_;
      }

      // Operators
      //! operador += 
      experimental_double& operator += (const experimental_double& p2)
      {
        experimental_double& p1= *this;
        double val = p1.value() + p2.value();
        double err = sqrt(square(p1.error()) + square(p2.error()));
        p1.set_value(val);
        p1.set_error(err);
        return p1;
      }

      //! operador -= 
      experimental_double& operator -= (const experimental_double& p2)
      {
        experimental_double& p1= *this;
        double val = p1.value() - p2.value();
        double err = sqrt(square(p1.error()) + square(p2.error()));
        p1.set_value(val);
        p1.set_error(err);

        return p1;
      }

      //! operador *= 
      experimental_double& operator *= (experimental_double a)
      {
        experimental_double& p1= *this;
        double val = p1.value()*a.value();
        double err = sqrt(square(a.value()*p1.error()) + square(p1.value()*a.error()));

        p1.set_value(val);
        p1.set_error(err);

        return p1;
      }

      //! operador /= 
      experimental_double& operator /= (experimental_double a)
      {
        experimental_double& p1= *this;

        if( a.value() == 0 ){
          clog << " problem: division by experimental_double with value " << a.value() << endl;
        }

        double val = p1.value()/a.value();
        double err = sqrt(square(p1.error()/a.value()) + square(p1.value()*a.error()/square(a.value())));
        p1.set_value(val);
        p1.set_error(err);
        return p1;
      }

    };
 

    // Operations with experimental_points
    // -v
    // sin(v) 
    inline experimental_double experimental_sin (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(sin(v1.value()));
      v.set_error(fabs(cos(v1.value()))*v1.error());
      return v;
    }
  
    // cos(v) 
    inline experimental_double experimental_cos (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(cos(v1.value()));
      v.set_error(fabs(sin(v1.value()))*v1.error());
      return v;
    }
  
    // tan(v) 
    inline experimental_double experimental_tan (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(tan(v1.value()));
      v.set_error((1. + square(v.value()))*v1.error());
      return v;
    }
  
    // asin(v) 
    inline experimental_double experimental_asin (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(asin(v1.value()));
      v.set_error(v1.error()/sqrt(1 - square(v1.value())));
      return v;
    }
  
    // acos(v) 
    inline experimental_double experimental_acos (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(acos(v1.value()));
      v.set_error(v1.error()/sqrt(1 - square(v1.value())));
      return v;
    }
  
    // atan(v) 
    inline experimental_double experimental_atan2 (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v;
      v.set_value(atan2(v1.value(), v2.value()));

      if( v2.value() == 0. ){  // if angle = 90 degrees,
        // obtain error from angle = 180 - (other angle)
        double den = 1 + square(v2.value()/v1.value());
        double num = square(v2.error()/v1.value()) + square(v2.value()*v1.error()/square(v1.value()));
        v.set_error(sqrt(num)/den);
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

        double den = 1 + square(v1.value()/v2.value());
        double num = square(v1.error()/v2.value()) + square(v1.value()*v2.error()/square(v2.value()));
        v.set_error(sqrt(num)/den);
      
      }
      return v;
    }
  
    // square(v) 
    inline experimental_double experimental_square (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(square(v1.value()));
      v.set_error(2*fabs(v1.value())*v1.error());
      return v;
    }
  
    // sqrt(v) 
    inline experimental_double experimental_sqrt (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(sqrt(v1.value()));
      v.set_error(v1.error()/(2*v.value()));
      return v;
    }
  
    // cube(v) 
    inline experimental_double experimental_cube (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(cube(v1.value()));
      v.set_error(3*square(v1.value())*v1.error());
      return v;
    }
  
    // fabs(v) 
    inline experimental_double experimental_fabs (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(fabs(v1.value()));
      v.set_error(v1.error());
      return v;
    }
  
    inline experimental_double operator - (const experimental_double& v1)
    {
      experimental_double v = v1;
      v.set_value(-v1.value());
      v.set_error(v1.error());
    
      return v;
    }

    // v1+v2
    inline experimental_double operator + (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1; 
      v+=v2;   
      return v;
    }
    //! v1-v2
    inline experimental_double operator - (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1; 
      v-=v2;   
      return v;
    }

    // v*d
    inline experimental_double operator * (const experimental_double& v1, const experimental_double& d)
    {
      experimental_double v = v1; 
      v*=d;   
      return v;
    }

    // v/d
    inline experimental_double operator / (const experimental_double& v1, const experimental_double& d)
    {
      experimental_double v = v1; 
      v/=d;   
      return v;
    }

    // v*d
    inline experimental_double operator * (const experimental_double& v1, double d)
    {
      experimental_double v = v1; 
      experimental_double dd(d,0.);
      v*=dd;   
      return v;
    }

    // v/d
    inline experimental_double operator / (const experimental_double& v1, double d)
    {
      experimental_double v = v1; 
      experimental_double dd(d,0.);
      v/=dd;   
      return v;
    }

    // v/d
    inline experimental_double operator / (double d, const experimental_double& v1)
    {
      experimental_double dd(d,0.);
      experimental_double v = v1; 
      dd/=v;   
      return dd;
    }

    // average
    inline experimental_double average (const std::vector<experimental_double> vs)
    {

      if( vs.size() == 0 ){
        clog << " problem: avereging vector of size " << vs.size() << endl;
        return experimental_double(small_neg, small_neg);
      }


      double mean = 0.;
      double err = 0.;

      for(std::vector<experimental_double>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        mean += iv->value();
        err += square(iv->error());
      }

      return experimental_double(mean/vs.size(), sqrt(err)/vs.size());

    }

    // weighted average
    inline experimental_double weighted_average (const std::vector<experimental_double> vs)
    {
      double mean = 0.;
      double inverr = 0.;

      for(std::vector<experimental_double>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        mean += iv->value()/square(iv->error());
        inverr += 1/square(iv->error());
      }

      return experimental_double(mean/inverr, sqrt(1./inverr));
    }


  }

}

#endif
