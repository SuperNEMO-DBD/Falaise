/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IEVEC
#define __CATAlgorithm__IEVEC
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/experimental_point.h>

namespace CAT{
  namespace topology{


    // an experimental std::vector is composed of an ordinary std::vector (x, y, z)
    // with corresponding error (ex, ey, ez)

    class experimental_vector {
    private:
      std::string appname_;

      //! x coordinate
      experimental_double x_;
      //! y coordinate
      experimental_double y_;
      //! z coordinate
      experimental_double z_;

    public:
      //!Default constructor
      experimental_vector();

      //!Default destructor
      virtual ~experimental_vector();

      //! constructor from coordinates
      experimental_vector(const experimental_double &x, const experimental_double &y, const experimental_double &z);

      //! constructor from coordinates with error
      experimental_vector(double x, double y, double z, double ex, double ey, double ez);

      //! constructor from two experimental points
      experimental_vector(const experimental_point &epa, const experimental_point &epb);

      //! constructor from one experimental point
      experimental_vector(const experimental_point &ep);


      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit              = false) const;

      //! set all coordinates
      void coordinates(const experimental_double &x,const experimental_double &y, const experimental_double &z);

      //! set all coordinates
      void set(const experimental_double &x, const experimental_double &y, const experimental_double &z);

      //! set std::vector and errors
      void set(double x, double y, double z, double ex,double ey, double ez);

      //! set x
      void set_x(const experimental_double &x);

      //! set y
      void set_y(const experimental_double &y);

      //! set z
      void set_z(const experimental_double &z);

      //! set from two points
      void set(const experimental_point &epa, const experimental_point &epb);

      //! get all coordinates
      const experimental_vector& coordinates()const;

      const experimental_double& x() const;

      //! read x
      experimental_double& x();

      //!get y
      const experimental_double& y() const;

      //! read y
      experimental_double& y();

      //!get z
      const experimental_double& z() const;

      //! read z
      experimental_double& z();

      // Operators
      // operator () returns/set x,y,z
      //! read v(i), i = 0,1,2 = x,y,z
      const experimental_double& operator () (size_t i) const;

      //! write v(i), i = 0,1,2
      experimental_double& operator () (size_t i);

      // no protection in operator []
      const experimental_double& operator [] (size_t i) const;

      //! write v(i), i = 0,1,2
      experimental_double& operator [] (size_t i);

      //! operador +=
      experimental_vector& operator += (const experimental_vector& p2);

      //! operador -=
      experimental_vector& operator -= (const experimental_vector& p2);

      //! operador *=
      experimental_vector& operator *= (experimental_double a);

      experimental_vector& operator *= (double a);

      //! operador /=
      experimental_vector& operator /= (experimental_double a);

      experimental_vector& operator /= (double a);

      //!get horizontal std::vector
      experimental_vector hor() const;

      //!get unit std::vector
      experimental_vector unit() const;

      //! get point from std::vector
      experimental_point point_from_vector();

      //! distance
      experimental_double distance(const experimental_vector& p2) const;

      //! length
      experimental_double length() const;

      //! length squared
      experimental_double length2() const;

      experimental_double phi() const;

      experimental_double tan_phi();

      experimental_double theta() const;

      experimental_double kink_phi(const experimental_vector &v);

      experimental_double kink_theta(const experimental_vector &v);


    };

    std::ostream& operator << (std::ostream& s, const experimental_vector& ip);


    // Operations with experimental_vectors
    // -v
    /*
      inline experimental_vector operator - (const experimental_vector& v1)
      {
      experimental_vector v = v1;
      experimental_double newx;
      newx.set_value(-v1.x().value());
      newx.set_error(v1.x().error());
      experimental_double newy;
      newy.set_value(-v1.y().value());
      newy.set_error(v1.y().error());
      experimental_double newz;
      newz.set_value(-v1.z().value());
      newz.set_error(v1.z().error());
      v.set_x(newx);
      v.set_y(newy);
      v.set_z(newz);

      return v;
      }
    */

    // v1+v2
    experimental_vector operator + (const experimental_vector& v1, const experimental_vector& v2);

    //! v1-v2
    experimental_vector operator - (const experimental_vector& v1, const experimental_vector& v2);

    // v*d
    experimental_vector operator * (const experimental_vector& v1, experimental_double d);

    experimental_vector operator * (const experimental_vector& v1, double d);

    // v/d
    experimental_vector operator / (const experimental_vector& v1, experimental_double d);

    experimental_vector operator / (const experimental_vector& v1, double d);

    // d*v
    experimental_vector operator * (experimental_double d, const experimental_vector& v1);

    // scalar product
    experimental_double operator * (const experimental_vector& a, const experimental_vector& b);

    // std::vectorial product
    experimental_vector operator ^ (const experimental_vector& a, const experimental_vector& b);

  }
}

#endif
