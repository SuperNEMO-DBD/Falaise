/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILINEARREGRESSION
#define __CATAlgorithm__ILINEARREGRESSION

#include <iostream>
#include <cmath>
#include <CATAlgorithm/CAT_config.h>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/circle.h>

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

    //!Default destructor
    virtual ~LinearRegression(){};

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
                       bool a_inherit          = false)
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



    //! set
    void set(std::vector<experimental_double> xi, std::vector<experimental_double> yi);


    //! set xi
    void set_xi(std::vector<experimental_double> xi);

    //! set yi
    void set_yi(std::vector<experimental_double> yi);

    //! set y0
    void set_y0(experimental_double y0);

    //! set tangent
    void set_tangent(experimental_double tangent);

    //! get xi
    const std::vector<experimental_double>& xi()const;

    //! get yi
    const std::vector<experimental_double>& yi()const;

    //! get y0
    const experimental_double& y0()const;

    //! get tangent
    const experimental_double& tangent()const;

    bool fit(void);

#if CAT_WITH_DEVEL_ROOT == 1
    bool root_fit(void);
#endif // CAT_WITH_DEVEL_ROOT == 1

    experimental_double position(experimental_double x);

    void invert();



  };

}
}
#endif

