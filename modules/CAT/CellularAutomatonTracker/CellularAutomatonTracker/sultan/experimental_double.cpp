/* -*- mode: c++ -*- */

#include <sultan/experimental_double.h>
#include <cmath>
#include <limits>
#include <iomanip>

namespace SULTAN {
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
    experimental_double::experimental_double(const double &v, const double &e)
    {
      v_ = v;
      e_ = e;
      return;
    }

    /*** dump ***/
    void experimental_double::dump (ostream & a_out,
                                    const std::string & /* a_title */,
                                    const std::string & /* a_indent */,
                                    bool /* a_inherit */) const
    {
      a_out << value() << " +- " << error();
      return;
    }

    //! set value and error
    void experimental_double::set(const experimental_double &v){
      v_ = v.value();
      e_ = v.error();
    }

    //! set value and error
    void experimental_double::set(const double &val, const double &err){
      v_ = val;
      e_ = err;
    }

    //! set value
    void experimental_double::set_value(const double &v){
      v_ = v;
    }

    //! set error
    void experimental_double::set_error(const double &e){
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
      double err = std::sqrt(std::pow(p1.error(),2) + std::pow(p2.error(),2));
      p1.set_value(val);
      p1.set_error(err);
      return p1;
    }

    //! operador -=
    experimental_double& experimental_double::operator -= (const experimental_double& p2)
    {
      experimental_double& p1= *this;
      double val = p1.value() - p2.value();
      double err = std::sqrt(std::pow(p1.error(),2) + std::pow(p2.error(),2));
      p1.set_value(val);
      p1.set_error(err);

      return p1;
    }

    //! operador *=
    experimental_double& experimental_double::operator *= (experimental_double a)
    {
      experimental_double& p1= *this;
      double val = p1.value()*a.value();
      double err = std::sqrt(std::pow(a.value()*p1.error(),2) + std::pow(p1.value()*a.error(),2));

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
      double err = std::sqrt(std::pow(p1.error()/a.value(),2) + std::pow(p1.value()*a.error()/std::pow(a.value(),2),2));
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

    bool experimental_double::is_less_than__optimist(const experimental_double a, double nsigmas=1)const{

      //
      // is this less than a?
      //
      // 1st case
      //   .....a....   ___this___
      //        --- delta -->
      //  delta > sigma
      //  false
      //
      // 2nd case
      //   .....a..__this___
      //        -delta->
      //  delta > 0, delta < sigma
      //  true
      //
      // 3rd case
      //   .....this..__a___
      //        <-delta-
      //  delta < 0 < sigma
      //  true
      //
      // 4th case
      //   .....this....   ___a___
      //        <-- delta ---
      //  delta < 0 < sigma
      //  true
      //

      experimental_double delta = *this - a;

      if( delta.value() > nsigmas*delta.error() ) return false;
      return true;

    }

    bool experimental_double::is_more_than__optimist(const experimental_double a, double nsigmas=1)const{

      //
      // is this more than a?
      //
      // 1st case
      //   .....a....   ___this___
      //        --- delta -->
      //  delta > sigma > - nsigma
      //  true
      //
      // 2nd case
      //   .....a..__this___
      //        -delta->
      //  delta > 0, delta < sigma, delta > - sigma
      //  true
      //
      // 3rd case
      //   .....this..__a___
      //        <-delta-
      //  delta < 0 < sigma, |delta| < sigma, delta > - sigma
      //  true
      //
      // 4th case
      //   .....this....   ___a___
      //        <-- delta ---
      //  delta < 0 < sigma, |delta| > sigma, delta < - sigma
      //  false
      //

      experimental_double delta = *this - a;
      if( delta.value() < - nsigmas*delta.error() ) return false;
      return true;

    }


    bool experimental_double::is_equal_to__optimist(const experimental_double a, double nsigmas=1)const{

      //
      // is this equal to a?
      //
      // 1st case
      //   .....a....   ___this___
      //        --- delta -->
      //  |delta| > sigma
      //  false
      //
      // 2nd case
      //   .....a..__this___
      //        -delta->
      //  |delta| < sigma
      //  true
      //
      // 3rd case
      //   .....this..__a___
      //        <-delta-
      //  |delta| < sigma
      //  true
      //
      // 4th case
      //   .....this....   ___a___
      //        <-- delta ---
      //  |delta| > sigma
      //  false
      //

      experimental_double delta = *this - a;
      if( fabs(delta.value()) > nsigmas*delta.error() ) return false;
      return true;

    }

    bool experimental_double::is_zero__optimist(double nsigmas=1)const{

      topology::experimental_double zero(0.,0.);
      experimental_double magn = experimental_fabs(*this);

      return magn.is_less_than__optimist(zero, nsigmas);

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
      v.set_error((1. + std::pow(v.value(),2))*v1.error());
      return v;
    }

    // asin(v)
    experimental_double experimental_asin (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(asin(v1.value()));
      v.set_error(v1.error()/std::sqrt(1 - std::pow(v1.value(),2)));
      return v;
    }

    // acos(v)
    experimental_double experimental_acos (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(acos(v1.value()));
      v.set_error(v1.error()/std::sqrt(1 - std::pow(v1.value(),2)));
      return v;
    }

    // atan(v)
    experimental_double experimental_atan2 (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v;
      v.set_value(atan2(v1.value(), v2.value()));

      if( v2.value() == 0. ){  // if angle = 90 degrees,
        // obtain error from angle = 180 - (other angle)
        double den = 1 + std::pow(v2.value()/v1.value(),2);
        double num = std::pow(v2.error()/v1.value(),2) + std::pow(v2.value()*v1.error()/std::pow(v1.value(),2),2);
        v.set_error(std::sqrt(num)/den);
      }
      else{

        double den = 1 + std::pow(v1.value()/v2.value(),2);
        double num = std::pow(v1.error()/v2.value(),2) + std::pow(v1.value()*v2.error()/std::pow(v2.value(),2),2);
        v.set_error(std::sqrt(num)/den);

      }
      return v;
    }

    // square(v)
    experimental_double experimental_square (const experimental_double& v1)
    {
      experimental_double v;
      v.set_value(std::pow(v1.value(),2));
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
      v.set_value(pow(v1.value(),3));
      v.set_error(3*std::pow(v1.value(),2)*v1.error());
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

    // min(v1, v2)
    experimental_double experimental_min (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1;
      if( v1.value() > v2.value() )
        v = v2;
      return v;
    }

    // max(v1, v2)
    experimental_double experimental_max (const experimental_double& v1, const experimental_double& v2)
    {
      experimental_double v = v1;
      if( v1.value() < v2.value() )
        v = v2;
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
        err += std::pow(iv->error(),2);
      }

      return experimental_double(mean/vs.size(), std::sqrt(err)/vs.size());

    }

    // weighted average
    experimental_double weighted_average (const std::vector<experimental_double> vs)
    {
      double mean = 0.;
      double inverr = 0.;
      //double sumwiei2 = 0.;

      // w_i = 1/sigma_i^2
      // mean = (sum_i w_i x_i)/(sum_k w_k)
      // sigma(mean) = 1/sqrt(sum_k w_k)

      for(std::vector<experimental_double>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        mean += iv->value()/std::pow(iv->error(),2);
        inverr += 1/std::pow(iv->error(),2);
      }

      return experimental_double(mean/inverr, 1./sqrt(inverr));
    }



  }

}
