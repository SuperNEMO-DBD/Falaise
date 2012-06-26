/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILINEARREGRESSION
#define __CATAlgorithm__ILINEARREGRESSION
#include <iostream>
#include <cmath>
#include <CATAlgorithm/experimental_double.h>

namespace CAT {
namespace topology{




  class LinearRegression : public tracking_object{

    // operate weighted linear regression with formula:
    //  y = y0 + tangent x
    // to find coefficients y0 and tangent

  private:

    std::string appname_;

    experimental_double y0_;

    experimental_double tangent_;

    std::vector<experimental_double> yi_;
    std::vector<experimental_double> xi_;


  public:

    //!Default constructor
    LinearRegression(mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.)
    {
      appname_= "LinearRegression: ";
      y0_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      tangent_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      xi_.clear();
      yi_.clear();
      set_print_level(level);
      set_nsigma(nsigma);
    }

    //!Default destructor
    virtual ~LinearRegression(){};

    //! constructor
    LinearRegression(std::vector<experimental_double> xi, std::vector<experimental_double> yi, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "LinearRegression: ";
      xi_ = xi;
      yi_ = yi;
    }

    /*** dump ***/
    virtual void dump (std::ostream & a_out         = std::clog,
                       const std::string & a_title  = "",
                       const std::string & a_indent = "",
                       bool a_inherit          = false){
      {
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " -------------- " << std::endl;
        a_out << indent << " points: " << std::endl;
        for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it){
          a_out << indent << " .. x "; it->dump(); a_out << " y "; yi_[it - xi_.begin()].dump(); a_out << " predicted "; position(*it).dump(); a_out << " " << std::endl;
        }
        a_out << indent << " y0: "; y0().dump(); a_out << " " << std::endl;
        a_out << indent << " tangent: "; tangent().dump(); a_out << " " << std::endl;

        a_out << indent << " -------------- " << std::endl;

        return;
      }
    }



    //! set
    void set(std::vector<experimental_double> xi, std::vector<experimental_double> yi)
      {
        xi_ = xi;
        yi_ = yi;
      }


    //! set xi
    void set_xi(std::vector<experimental_double> xi)
      {
        xi_ = xi;
      }

    //! set yi
    void set_yi(std::vector<experimental_double> yi)
      {
        yi_ = yi;
      }

    //! set y0
    void set_y0(experimental_double y0)
      {
        y0_ = y0;
      }

    //! set tangent
    void set_tangent(experimental_double tangent)
      {
        tangent_ = tangent;
      }

    //! get xi
    const std::vector<experimental_double>& xi()const
    {
      return xi_;
    }

    //! get yi
    const std::vector<experimental_double>& yi()const
    {
      return yi_;
    }

    //! get y0
    const experimental_double& y0()const
    {
      return y0_;
    }

    //! get tangent
    const experimental_double& tangent()const
    {
      return tangent_;
    }



    bool fit(void){


      if( xi_.size() != yi_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: in least square regression, sizes x " << xi_.size() << " y " << yi_.size() << std::endl;
        }
        return false;
      }

      double Sw = 0.;
      double Swxx = 0.;
      double Swx = 0.;
      double Swxy = 0.;
      double Swy = 0.;

      for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
        {
          double w = 1./(mybhep::square(it->error()));
          Sw += w;
          Swxx += w*mybhep::square(it->value());
          Swx += w*it->value();
          double y = yi_[it - xi_.begin()].value();
          Swxy += w*it->value()*y;
          Swy += w*y;
        }

      double delta = Sw*Swxx - mybhep::square(Swx);

      if( delta == 0.){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: in least square regression, delta " << delta << " Sw " << Sw << " Swx " << Swx << " Swxx " << Swxx << std::endl;
        }
        return false;
      }

      double a = (Swxx*Swy - Swx*Swxy)/delta;
      double b = (Sw*Swxy - Swx*Swy)/delta;
      double erra, errb;

      if( Swxx/delta > 0. ){
        erra = std::sqrt(Swxx/delta);
      }
      else{
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: linear regression sy02 " << Swxx/delta << " Swxx " << Swxx << " delta " << delta << std::endl;
        }
        return false;
      }

      if( Sw/delta > 0. ){
        errb = std::sqrt(Sw/delta);
      }
      else{
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: linear regression stangent2 " << Sw/delta << " Sw " << Sw << " delta " << delta << std::endl;
        }
        return false;
      }

      set_y0(experimental_double(a, erra));
      set_tangent(experimental_double(b, errb));

      return true;

    }

    experimental_double position(experimental_double x){

      return y0() + tangent()*x;
    }


    void invert(){
      // go from:
      // y = y0 + tangent x
      // to
      // x = y0' + tangent' y
      // y0' = - y0/tangent = - y0 tangent'
      // tangent' = 1 / tangent

      std::vector<experimental_double> tmp = xi();
      set_xi(yi());
      set_yi(tmp);
      set_y0(- y0()/tangent());
      set_tangent(1./tangent());

      return;

    }


  };

}
}

#endif

