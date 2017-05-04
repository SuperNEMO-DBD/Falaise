/* -*- mode: c++ -*- */
#include <sultan/LinearRegression.h>

namespace SULTAN {
namespace topology{



    //! set
    void LinearRegression::set(const std::vector<double> &xi, const std::vector<double> &yi)
      {
        xi_ = xi;
        yi_ = yi;
      }


    //! set xi
    void LinearRegression::set_xi(const std::vector<double> &xi)
      {
        xi_ = xi;
      }

    //! set yi
    void LinearRegression::set_yi(const std::vector<double> &yi)
      {
        yi_ = yi;
      }

    //! set y0
    void LinearRegression::set_y0(const double &y0)
      {
        y0_ = y0;
      }

    //! set tangent
    void LinearRegression::set_tangent(const double &tangent)
      {
        tangent_ = tangent;
      }

    //! get xi
    const std::vector<double>& LinearRegression::xi()const
    {
      return xi_;
    }

    //! get yi
    const std::vector<double>& LinearRegression::yi()const
    {
      return yi_;
    }

    //! get y0
    const double& LinearRegression::y0()const
    {
      return y0_;
    }

    //! get tangent
    const double& LinearRegression::tangent()const
    {
      return tangent_;
    }



    bool LinearRegression::fit(void){


      if( xi_.size() != yi_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::LinearRegression::fit: problem: in least square regression, sizes x " << xi_.size() << " y " << yi_.size() << std::endl;
        }
        return false;
      }

      double Sw = 0.;
      double Swxx = 0.;
      double Swx = 0.;
      double Swxy = 0.;
      double Swy = 0.;

      for(std::vector<double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
        {
          Sw ++;
          Swxx += mybhep::square(*it);
          Swx += *it;
          double y = yi_[it - xi_.begin()];
          Swxy += *it *y;
          Swy += y;
        }

      double delta = Sw*Swxx - mybhep::square(Swx);

      if( delta == 0.){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::LinearRegression::fit: problem: in least square regression, delta " << delta << " Sw " << Sw << " Swx " << Swx << " Swxx " << Swxx << std::endl;
        }
        return false;
      }

      double a = (Swxx*Swy - Swx*Swxy)/delta;
      double b = (Sw*Swxy - Swx*Swy)/delta;
      set_y0(a);
      set_tangent(b);

      return true;

    }


    double LinearRegression::position(const double &x){

      return y0() + tangent()*x;
    }


    void LinearRegression::invert(){
      // go from:
      // y = y0 + tangent x
      // to
      // x = y0' + tangent' y
      // y0' = - y0/tangent = - y0 tangent'
      // tangent' = 1 / tangent

      std::vector<double> tmp = xi();
      set_xi(yi());
      set_yi(tmp);
      set_y0(- y0()/tangent());
      set_tangent(1./tangent());


    }

    double LinearRegression::parameter(const double &x){

      double y = position(x);

      double theta = atan(tangent());

      double uy = sin(theta);

      double parameter = ( uy == 0. ? 0. : (y - y0())/uy);

      return parameter;
    }



  }

}
