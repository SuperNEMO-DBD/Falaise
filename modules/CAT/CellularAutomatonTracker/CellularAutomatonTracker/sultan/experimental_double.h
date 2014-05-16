/* -*- mode: c++ -*- */
#ifndef __sultan__experimental_double
#define __sultan__experimental_double 1

#include <iostream>
#include <cmath>
#include <vector>

namespace SULTAN {
  namespace topology {

    class experimental_double {

      // an experimental_double is composed of a value and an error

    protected:

      double v_; /// Value
      double e_; /// Error

    public:   

      bool is_valid () const;

      bool is_value_valid () const;

      bool is_error_valid () const;

      //!Default constructor     
      experimental_double();

      //!Default destructor
      virtual ~experimental_double();

      //! constructor
      experimental_double(const double &v, const double &e);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set value and error
      void set(const experimental_double &v);

      //! set value and error
      void set(const double &val, const double &err);

      //! set value
      void set_value(const double &v);

      //! set error
      void set_error(const double &e);

      //! get value
      const double& value() const;

      //! get error
      const double& error() const;

      // Operators
      //! operador += 
      experimental_double& operator += (const experimental_double& p2);

      //! operador -= 
      experimental_double& operator -= (const experimental_double& p2);

      //! operador *= 
      experimental_double& operator *= (experimental_double a);

      //! operador *= 
      experimental_double& operator *= (double a);

      //! operador /= 
      experimental_double& operator /= (experimental_double a);

      //! operador /= 
      experimental_double& operator /= (double a);

      bool is_less_than__optimist(const experimental_double a, double nsigma)const;
    
      bool is_more_than__optimist(const experimental_double a, double nsigma)const;

      bool is_equal_to__optimist(const experimental_double a, double nsigma)const;

      bool is_zero__optimist(double nsigma)const;

      bool experimental_isnan()const;

      bool experimental_isinf()const;

    };
 

    // Operations with experimental_points
    // -v
    // sin(v) 
    experimental_double experimental_sin (const experimental_double& v1);
 
    // cos(v) 
    experimental_double experimental_cos (const experimental_double& v1);
  
    // tan(v) 
    experimental_double experimental_tan (const experimental_double& v1);
  
    // asin(v) 
    experimental_double experimental_asin (const experimental_double& v1);
  
    // acos(v) 
    experimental_double experimental_acos (const experimental_double& v1);
  
    // atan(v) 
    experimental_double experimental_atan2 (const experimental_double& v1, const experimental_double& v2);
  
    // square(v) 
    experimental_double experimental_square (const experimental_double& v1);
  
    // sqrt(v) 
    experimental_double experimental_sqrt (const experimental_double& v1);
  
    // cube(v) 
    experimental_double experimental_cube (const experimental_double& v1);
  
    // fabs(v) 
    experimental_double experimental_fabs (const experimental_double& v1);
  
    // min(v1, v2) 
    experimental_double experimental_min (const experimental_double& v1, const experimental_double& v2);
  
    // min(v1, v2) 
    experimental_double experimental_max (const experimental_double& v1, const experimental_double& v2);
  
    experimental_double operator - (const experimental_double& v1);

    // v1+v2
    experimental_double operator + (const experimental_double& v1, const experimental_double& v2);

    //! v1-v2
    experimental_double operator - (const experimental_double& v1, const experimental_double& v2);

    // v*d
    experimental_double operator * (const experimental_double& v1, const experimental_double& d);

    // v/d
    experimental_double operator / (const experimental_double& v1, const experimental_double& d);

    // v*d
    experimental_double operator * (const experimental_double& v1, double d);

    // v/d
    experimental_double operator / (const experimental_double& v1, double d);

    // v/d
    experimental_double operator / (double d, const experimental_double& v1);

    // average
    double average (const std::vector<double> vs);

    // average
    experimental_double average (const std::vector<experimental_double> vs);

    // weighted average
    experimental_double weighted_average (const std::vector<experimental_double> vs);

  }
}

#endif // __sultan__experimental_double
