/* -*- mode: c++ -*- */
#include <sultan/experimental_vector.h>

namespace SULTAN{
  namespace topology{

    //!Default constructor
    experimental_vector::experimental_vector()
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
    experimental_vector::~experimental_vector()
    {
    }

    //! constructor from coordinates
    experimental_vector::experimental_vector(const experimental_double &x, const experimental_double &y, const experimental_double &z){
      appname_ = "experimental_vector: ";
      x_=x; y_=y; z_=z;
    }

    //! constructor from coordinates with error
    experimental_vector::experimental_vector(double x, double y, double z, double ex, double ey, double ez){
      appname_ = "experimental_vector: ";
      x_.set_value(x);
      y_.set_value(y);
      z_.set_value(z);
      x_.set_error(ex);
      y_.set_error(ey);
      z_.set_error(ez);

    }

    //! constructor from two experimental points
    experimental_vector::experimental_vector(const experimental_point &epa, const experimental_point &epb){
      appname_ = "experimental_vector: ";
      x_ = epb.x() - epa.x();
      y_ = epb.y() - epa.y();
      z_ = epb.z() - epa.z();
    }

    //! constructor from one experimental point
    experimental_vector::experimental_vector(const experimental_point &ep){
      appname_ = "experimental_vector: ";
      experimental_point p0(0.,0.,0.,0.,0.,0.);
      x_ = ep.x() - p0.x();
      y_ = ep.y() - p0.y();
      z_ = ep.z() - p0.z();
    }


    /*** dump ***/
    void experimental_vector::dump (std::ostream & a_out,
                                    const std::string & a_title,
                                    const std::string & a_indent,
                                    bool a_inherit              ) const{
      {
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

	a_out << indent << " x ("; x().dump(); a_out << ") , ("; y().dump(); a_out << ") , ("; z().dump(); a_out << std::endl;

        return;
      }
    }


    //! set all coordinates
    void experimental_vector::set(const experimental_double &x, const experimental_double &y, const experimental_double &z){
      x_=x; y_=y; z_=z;
    }

    //! set std::vector and errors
    void experimental_vector::set(double x, double y, double z, double ex,double ey, double ez)
    {
      x_.set_value( x);
      y_.set_value( y);
      z_.set_value( z);
      x_.set_error(ex);
      y_.set_error(ey);
      z_.set_error(ez);
    }

    //! set x
    void experimental_vector::set_x(const experimental_double &x){
      x_ = x;
    }

    //! set y
    void experimental_vector::set_y(const experimental_double &y){
      y_ = y;
    }

    //! set z
    void experimental_vector::set_z(const experimental_double &z){
      z_ = z;
    }


    //! set from two points
    void experimental_vector::set(const experimental_point &epa, const experimental_point &epb){
      x_.set( epb.x() - epa.x());
      y_.set( epb.y() - epa.y());
      z_.set( epb.z() - epa.z());
    }

    //! get all coordinates
    const experimental_vector& experimental_vector::coordinates()const {return *this;}

    //!get x
    const experimental_double& experimental_vector::x() const
    {
      return x_;
    }
    //! read x
    experimental_double& experimental_vector::x(){
      return x_;
    }

    //!get y
    const experimental_double& experimental_vector::y() const {
      return y_;
    }
    //! read y
    experimental_double& experimental_vector::y(){
      return y_;
    }
    //!get z
    const experimental_double& experimental_vector::z() const {return z_;}
    //! read z
    experimental_double& experimental_vector::z(){return z_;}

    // Operators
    // operator () returns/set x,y,z
    //! read v(i), i = 0,1,2 = x,y,z
    const experimental_double& experimental_vector::operator () (size_t i) const
    {
      if (i==0) return x();
      if (i==1) return y();
      return z_;
    }

    //! write v(i), i = 0,1,2
    experimental_double& experimental_vector::operator () (size_t i)
    {

      if (i==0) return x();
      if (i==1) return y();
      return z_;
    }

    // no protection in operator []
    const experimental_double& experimental_vector::operator [] (size_t i) const
    {

      if (i==0) return x_;
      if (i==1) return y_;
      return z_;
    }

    //! write v(i), i = 0,1,2
    experimental_double& experimental_vector::operator [] (size_t i)
    {

      if (i==0) return x_;
      if (i==1) return y_;
      return z_;
    }

    //! operador +=
    experimental_vector& experimental_vector::operator += (const experimental_vector& p2)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) += p2(i);
      return p1;
    }

    //! operador -=
    experimental_vector& experimental_vector::operator -= (const experimental_vector& p2)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) -= p2(i);
      return p1;
    }

    //! operador *=
    experimental_vector& experimental_vector::operator *= (experimental_double a)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) *= a;
      return p1;
    }

    experimental_vector& experimental_vector::operator *= (double a)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) = p1(i)*a;
      return p1;
    }

    //! operador /=
    experimental_vector& experimental_vector::operator /= (experimental_double a)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) /= a;
      return p1;
    }

    experimental_vector& experimental_vector::operator /= (double a)
    {
      experimental_vector& p1= *this;

      for(size_t i = 0; i < 3; i++)
        p1(i) = p1(i)/a;
      return p1;
    }

    //!get horizontal std::vector
    experimental_vector experimental_vector::hor() const{
      experimental_double newz(0.,0.);
      experimental_vector p1(x_, y_, newz);
      return p1;
    }

    //!get unit std::vector
    experimental_vector experimental_vector::unit() const{
      experimental_vector p1(x_, y_, z_);
      p1 /= p1.length().value();
      return p1;
    }

    //! get point from std::vector
    experimental_point experimental_vector::point_from_vector()
    {
      experimental_vector& p1= *this;
      experimental_point p(p1.x(), p1.y(), p1.z());
      return p;
    }

    //! distance
    experimental_double experimental_vector::distance(const experimental_vector& p2) const
    {
      experimental_double result;

      result.set_value( std::sqrt(pow(x_.value()-p2.x().value(),2) +
                                  pow(y_.value()-p2.y().value(),2) +
                                  pow(z_.value()-p2.z().value(),2)));
      result.set_error( std::sqrt( pow(x_.value()*x_.error(),2) + pow(p2.x().value()*p2.x().error(),2) +
                                   pow(y_.value()*y_.error(),2) + pow(p2.y().value()*p2.y().error(),2) +
                                   pow(z_.value()*z_.error(),2) + pow(p2.z().value()*p2.z().error(),2))/result.value());

      return result;
    }

    //! length
    experimental_double experimental_vector::length() const
    {
      experimental_double result;

      result.set_value( std::sqrt(pow(x_.value(),2) +
                                  pow(y_.value(),2) +
                                  pow(z_.value(),2)));
      result.set_error( std::sqrt( pow(x_.value()*x_.error(),2) +
                                   pow(y_.value()*y_.error(),2) +
                                   pow(z_.value()*z_.error(),2) )/result.value());
      if( isnan(result.error()) || isinf(result.error())  ) result.set_error(mybhep::small_neg);
      return result;

    }

    //! length squared
    experimental_double experimental_vector::length2() const
    {
      experimental_double result = experimental_square(x_) + experimental_square(y_) + experimental_square(z_);

      return result;

    }


    experimental_double experimental_vector::phi() const{

      return experimental_atan2(y_, x_);

    }

    experimental_double experimental_vector::tan_phi(){

      if( x_.value() == 0. )
        return experimental_double(mybhep::plus_infinity, 0.);

      return y_/x_;

    }

    experimental_double experimental_vector::theta() const{
      return experimental_atan2(z_, hor().length());

    }

    experimental_double experimental_vector::kink_phi(const experimental_vector &v){
      experimental_double result;
      experimental_double p1 = phi();
      experimental_double p2 = v.phi();
      double phi1 = p1.value();
      double phi2 = p2.value();
      result.set_value(phi2 - phi1);
      result.set_error(std::sqrt(pow(p1.error(),2) +
                                 pow(p2.error(),2) ));

      return result;

    }


    experimental_double experimental_vector::kink_theta(const experimental_vector &v){
      experimental_double result;
      experimental_double t1 = theta();
      experimental_double t2 = v.theta();
      double theta1 = t1.value();
      double theta2 = t2.value();
      result.set_value(theta2 - theta1);
      result.set_error(std::sqrt(pow(t1.error(),2) +
                                 pow(t2.error(),2) ));

      return result;

    }



    std::ostream& operator << (std::ostream& s, const experimental_vector& ip) {
      s << std::endl;

      s << "(" << ip.x().value() << "," <<ip.y().value() <<","<<ip.z().value() <<")";
      return s;
    }
    // v1+v2
    experimental_vector operator + (const experimental_vector& v1, const experimental_vector& v2)
    {
      experimental_vector v = v1;
      v+=v2;
      return v;
    }

    //! v1-v2
    experimental_vector operator - (const experimental_vector& v1, const experimental_vector& v2)
    {
      experimental_vector v = v1;
      v-=v2;
      return v;
    }

    // v*d
    experimental_vector operator * (const experimental_vector& v1, experimental_double d)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    experimental_vector operator * (const experimental_vector& v1, double d)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    // v/d
    experimental_vector operator / (const experimental_vector& v1, experimental_double d)
    {
      experimental_vector v = v1;
      v/=d;
      return v;
    }

    experimental_vector operator / (const experimental_vector& v1, double d)
    {
      experimental_vector v = v1;
      v/=d;
      return v;
    }

    // d*v
    experimental_vector operator * (experimental_double d, const experimental_vector& v1)
    {
      experimental_vector v = v1;
      v*=d;
      return v;
    }

    // scalar product
    experimental_double operator * (const experimental_vector& a, const experimental_vector& b)
    {
      return a.x()*b.x() +
        a.y()*b.y() +
        a.z()*b.z();
    }

    // std::vectorial product
    experimental_vector operator ^ (const experimental_vector& a, const experimental_vector& b)
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
