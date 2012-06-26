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
      experimental_vector()
      {
        appname_ = "experimental_vector: ";
        x_.set_value( mybhep::small_neg );
        y_.set_value( mybhep::small_neg );
        z_.set_value( mybhep::small_neg );

        x_.set_error( mybhep::small_neg );
        y_.set_error( mybhep::small_neg );
        z_.set_error( mybhep::small_neg );

      }
      //!Default destructor
      virtual ~experimental_vector(){};

      //! constructor from coordinates
      experimental_vector(experimental_double x, experimental_double y, experimental_double z){
        appname_ = "experimental_vector: ";
        x_=x; y_=y; z_=z;
      }

      //! constructor from coordinates with error
      experimental_vector(double x, double y, double z, double ex, double ey, double ez){
        appname_ = "experimental_vector: ";
        x_.set_value(x);
        y_.set_value(y);
        z_.set_value(z);
        x_.set_error(ex);
        y_.set_error(ey);
        z_.set_error(ez);

      }

      //! constructor from two experimental points
      experimental_vector(experimental_point epa, experimental_point epb){
        appname_ = "experimental_vector: ";
        x_ = epb.x() - epa.x();
        y_ = epb.y() - epa.y();
        z_ = epb.z() - epa.z();
      }

      //! constructor from one experimental point
      experimental_vector(experimental_point ep){
        appname_ = "experimental_vector: ";
        experimental_point p0(0.,0.,0.,0.,0.,0.);
        x_ = ep.x() - p0.x();
        y_ = ep.y() - p0.y();
        z_ = ep.z() - p0.z();
      }


      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit              = false) const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << std::endl;
          a_out << indent << " x: "; (x()/mybhep::mm).dump(); a_out << " [mm ] " << std::endl;
          a_out << indent << " y: "; (y()/mybhep::mm).dump(); a_out << " [mm ] " << std::endl;
          a_out << indent << " z: "; (z()/mybhep::mm).dump(); a_out << " [mm ] " << std::endl;

          return;
        }
      }


      //! set all coordinates
      void coordinates(experimental_double x,experimental_double y, experimental_double z){
        x_=x; y_=y; z_=z;
      }
      //! set all coordinates
      void set(experimental_double x,experimental_double y, experimental_double z){
        coordinates(x,y,z);
      }

      //! set std::vector and errors
      void set(double x, double y, double z, double ex,double ey, double ez)
      {
        x_.set_value( x);
        y_.set_value( y);
        z_.set_value( z);
        x_.set_error(ex);
        y_.set_error(ey);
        z_.set_error(ez);
      }

      //! set x
      void set_x(experimental_double x){
        x_ = x;
      }

      //! set y
      void set_y(experimental_double y){
        y_ = y;
      }

      //! set z
      void set_z(experimental_double z){
        z_ = z;
      }


      //! set from two points
      void set(experimental_point epa, experimental_point epb){
        x_.set( epb.x() - epa.x());
        y_.set( epb.y() - epa.y());
        z_.set( epb.z() - epa.z());
      }

      //! get all coordinates
      const experimental_vector& coordinates()const {return *this;}
      //!get x
      const experimental_double& x() const
      {
        return x_;
      }
      //! read x
      experimental_double& x(){
        return x_;
      }

      //!get y
      const experimental_double& y() const {
        return y_;
      }
      //! read y
      experimental_double& y(){
        return y_;
      }
      //!get z
      const experimental_double& z() const {return z_;}
      //! read z
      experimental_double& z(){return z_;}

      // Operators
      // operator () returns/set x,y,z
      //! read v(i), i = 0,1,2 = x,y,z
      const experimental_double& operator () (size_t i) const
      {
        if (i==0) return x();
        if (i==1) return y();
        return z_;
      }

      //! write v(i), i = 0,1,2
      experimental_double& operator () (size_t i)
      {

        if (i==0) return x();
        if (i==1) return y();
        return z_;
      }

      // no protection in operator []
      const experimental_double& operator [] (size_t i) const
      {

        if (i==0) return x_;
        if (i==1) return y_;
        return z_;
      }

      //! write v(i), i = 0,1,2
      experimental_double& operator [] (size_t i)
      {

        if (i==0) return x_;
        if (i==1) return y_;
        return z_;
      }

      //! operador +=
      experimental_vector& operator += (const experimental_vector& p2)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) += p2(i);
        return p1;
      }

      //! operador -=
      experimental_vector& operator -= (const experimental_vector& p2)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) -= p2(i);
        return p1;
      }

      //! operador *=
      experimental_vector& operator *= (experimental_double a)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) *= a;
        return p1;
      }

      experimental_vector& operator *= (double a)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) = p1(i)*a;
        return p1;
      }

      //! operador /=
      experimental_vector& operator /= (experimental_double a)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) /= a;
        return p1;
      }

      experimental_vector& operator /= (double a)
      {
        experimental_vector& p1= *this;

        for(size_t i = 0; i < 3; i++)
          p1(i) = p1(i)/a;
        return p1;
      }

      //!get horizontal std::vector
      experimental_vector hor() {
        experimental_double newy(0.,0.);
        experimental_vector p1(x_, newy, z_);
        return p1;
      }

      //!get unit std::vector
      experimental_vector unit() {
        experimental_vector p1(x_, y_, z_);
        p1 /= p1.length().value();
        return p1;
      }

      //! get point from std::vector
      experimental_point point_from_vector()
      {
        experimental_vector& p1= *this;
        experimental_point p(p1.x(), p1.y(), p1.z());
        return p;
      }

      //! distance
      experimental_double distance(const experimental_vector& p2) const
      {
        experimental_double result;

        result.set_value( sqrt(mybhep::square(x_.value()-p2.x().value())
                               + mybhep::square(y_.value()-p2.y().value()) +
                               mybhep::square(z_.value()-p2.z().value())));
        result.set_error( sqrt( mybhep::square(x_.value()*x_.error()) + mybhep::square(p2.x().value()*p2.x().error()) +
                                mybhep::square(y_.value()*y_.error()) + mybhep::square(p2.y().value()*p2.y().error()) +
                                mybhep::square(z_.value()*z_.error()) + mybhep::square(p2.z().value()*p2.z().error()))/result.value());

        return result;
      }

      //! length
      experimental_double length() const
      {
        experimental_double result;

        result.set_value( sqrt(mybhep::square(x_.value())
                               + mybhep::square(y_.value()) +
                               mybhep::square(z_.value())));
        result.set_error( sqrt( mybhep::square(x_.value()*x_.error()) +
                                mybhep::square(y_.value()*y_.error()) +
                                mybhep::square(z_.value()*z_.error()) )/result.value());
        if( isnan(result.error()) || isinf(result.error())  ) result.set_error(mybhep::small_neg);
        return result;

      }

      //! length squared
      experimental_double length2() const
      {
        experimental_double result = experimental_square(x_) + experimental_square(y_) + experimental_square(z_);

        return result;

      }


      experimental_double phi(){

        return experimental_atan2(z_, x_);

      }

      experimental_double tan_phi(){

        if( x_.value() == 0. )
          return experimental_double(mybhep::plus_infinity, 0.);

        return z_/x_;

      }

      experimental_double theta(){
        return experimental_atan2(y_, hor().length());

      }

      experimental_double kink_phi(experimental_vector v){
        experimental_double result;
        experimental_double p1 = phi();
        experimental_double p2 = v.phi();
        double phi1 = p1.value();
        double phi2 = p2.value();
        mybhep::fix_angles(&phi1, &phi2);
        result.set_value(phi2 - phi1);
        result.set_error(sqrt(mybhep::square(p1.error()) +
                              mybhep::square(p2.error()) ));

        return result;

      }


      experimental_double kink_theta(experimental_vector v){
        experimental_double result;
        experimental_double t1 = theta();
        experimental_double t2 = v.theta();
        double theta1 = t1.value();
        double theta2 = t2.value();
        mybhep::fix_angles(&theta1, &theta2);
        result.set_value(theta2 - theta1);
        result.set_error(sqrt(mybhep::square(t1.error()) +
                              mybhep::square(t2.error()) ));

        return result;

      }



    };

    inline std::ostream& operator << (std::ostream& s, const experimental_vector& ip) {
      s << std::endl;

      s << "(" << ip.x().value() << "," <<ip.y().value() <<","<<ip.z().value() <<")";
      return s;
    }


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
    inline experimental_vector operator + (const experimental_vector& v1, const experimental_vector& v2)
    {
      experimental_vector v = v1;
      v+=v2;
      return v;
    }

    //! v1-v2
    inline experimental_vector operator - (const experimental_vector& v1, const experimental_vector& v2)
    {
      experimental_vector v = v1;
      v-=v2;
      return v;
    }

    // v*d
    inline experimental_vector operator * (const experimental_vector& v1, experimental_double d)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    inline experimental_vector operator * (const experimental_vector& v1, double d)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    // v/d
    inline experimental_vector operator / (const experimental_vector& v1, experimental_double d)
    {
      experimental_vector v = v1;
      v/=d;
      return v;
    }

    inline experimental_vector operator / (const experimental_vector& v1, double d)
    {
      experimental_vector v = v1;
      v/=d;
      return v;
    }

    // d*v
    inline experimental_vector operator * (experimental_double d, const experimental_vector& v1)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    // scalar product
    inline experimental_double operator * (const experimental_vector& a, const experimental_vector& b)
    {
      return a.x()*b.x() +
        a.y()*b.y() +
        a.z()*b.z();
    }

    // std::vectorial product
    inline experimental_vector operator ^ (const experimental_vector& a, const experimental_vector& b)
    {
      experimental_vector v;

      experimental_double dx = a.y()*b.z() - a.z()*b.y();
      experimental_double dy = a.z()*b.x() - a.x()*b.z();
      experimental_double dz = a.x()*b.y() - a.y()*b.x();

      v.set(dx, dy, dz);

      return v;
    }

  }
}

#endif
