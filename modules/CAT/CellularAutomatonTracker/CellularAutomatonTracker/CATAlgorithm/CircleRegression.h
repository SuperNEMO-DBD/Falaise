/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICIRCLEREGRESSION
#define __CATAlgorithm__ICIRCLEREGRESSION
#include <iostream>
#include <cmath>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/circle.h>

namespace CAT{
  namespace topology{




    class CircleRegression : public tracking_object{

      // operate weighted circle regression on points (xi, yi)
      // to find center and radius of best fitting circle

    private:

      std::string appname_;

      circle c_;

      std::vector<experimental_double> yi_;
      std::vector<experimental_double> xi_;


    public:

      //!Default constructor
      CircleRegression(mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.)
      {
        appname_= "CircleRegression: ";
        c_ = circle();
        xi_.clear();
        yi_.clear();
        set_print_level(level);
        set_nsigma(nsigma);
      }

      //!Default destructor
      virtual ~CircleRegression(){};

      //! constructor
      CircleRegression(std::vector<experimental_double> xi, std::vector<experimental_double> yi, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "CircleRegression: ";
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
            experimental_double y = yi_[it - xi_.begin()];
            experimental_double phi = c_.phi_of_point(experimental_point(*it, experimental_double(0.,0.), y));
            a_out << indent << " .. x "; it->dump(); a_out << " y "; yi_[it - xi_.begin()].dump(); a_out << " predicted x "; position(phi).x().dump(); a_out << " y "; position(phi).z().dump(); a_out << " "  << std::endl;
          }
          a_out << indent << " circle: " << std::endl;
          circle().dump();
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

      //! set circle
      void set_circle(circle c)
      {
        c_ = c;
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

      //! get circle
      const circle& c()const
      {
        return c_;
      }



      bool fit(void){


        if( xi_.size() != yi_.size() ){
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: in circle regression, sizes x " << xi_.size() << " y " << yi_.size() << std::endl;
          }
          return false;
        }

        experimental_double xc, yc, r;


        bool method1 = false;
        // method 1: R. Bullock, http://www.dtcenter.org/met/users/docs/write_ups/circle_fit.pdf
        // method 2: D. Umbach, K. N. Jones, http://www.cs.bsu.edu/homepages/kerryj/kjones/circles.pdf

        std::vector<double> ui;
        std::vector<double> vi;
        double Sw = 0.;
        double Swx = 0.;
        double Swy = 0.;
        double Swxx = 0.;
        double Swxy = 0.;
        double Swyy = 0.;
        double Swxxx = 0.;
        double Swxyy = 0.;
        double Swxxy = 0.;
        double Swyyy = 0.;
        double Swuu = 0.;
        double Swuv = 0.;
        double Swvv = 0.;
        double Swuuu = 0.;
        double Swuuv = 0.;
        double Swuvv = 0.;
        double Swvvv = 0.;

        double delta, xave, yave;

        if( method1 ){
          xave = average(xi_).value();
          yave = average(yi_).value();
          std::clog << " xave " << xave << " yave " << yave << std::endl;

        }

        for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
          {
            double y = yi_[it - xi_.begin()].value();
            double yerr = yi_[it - xi_.begin()].error();
            double w = 1./(mybhep::square(it->error())) + 1./(mybhep::square(yerr));
            if( isnan(w) || isinf(w) )
              w = 1.;
            Sw += w;

            if( method1 ){
              double u = it->value() - xave;
              double v = y - yave;
              Swuu += w*mybhep::square(u);
              Swuv += w*u*v;
              Swvv += w*mybhep::square(v);
              Swuuu += w*mybhep::cube(u);
              Swuuv += w*mybhep::square(u)*v;
              Swuvv += w*u*mybhep::square(v);
              Swvvv += w*mybhep::cube(v);
            } else {
              Swx += w*it->value();
              Swy += w*y;
              Swxx += w*mybhep::square(it->value());
              Swxy += w*it->value()*y;
              Swyy += w*mybhep::square(y);
              Swxyy += w*it->value()*mybhep::square(y);
              Swxxy += w*mybhep::square(it->value())*y;
              Swyyy += w*mybhep::cube(y);
              Swxxx += w*mybhep::cube(it->value());
            }
          }

        if( method1 ){
          delta = Swuu*Swvv - mybhep::square(Swuv);

          if( delta == 0.){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, delta " << delta << " Swuu " << Swuu << " Swvv " << Swvv << " Swuv " << Swuv << std::endl;
            }
            return false;
          }

          double uc = (Swuuu + Swuvv)/(2.*delta);
          double vc = (Swuuv + Swvvv)/(2.*delta);
          double erruc = 0.;
          double errvc = 0.;
          double alpha = mybhep::square(uc) + mybhep::square(vc) + (Swuu + Swvv)/Sw;
          double erralpha = 0.;

          std::clog << " uc " << uc << " vc " << vc << std::endl;

          xc.set(uc + xave, erruc);
          yc.set(vc + yave, errvc);
          r.set(std::sqrt(alpha), erralpha/(2.*std::sqrt(alpha)));
        }
        else{
          double A = Sw*Swxx - mybhep::square(Swx);
          double B = Sw*Swxy - Swx*Swy;
          double C = Sw*Swyy - mybhep::square(Swy);
          double D = (Sw*Swxyy - Swx*Swyy + Sw*Swxxx - Swx*Swxx)/2.;
          double E = (Sw*Swxxy - Swy*Swxx + Sw*Swyyy - Swy*Swyy)/2.;
          delta = A*C - mybhep::square(B);

          if( isnan(A) || isinf(A) ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, A " << A << " Sw " << Sw << " Swxx " << Swxx << " Swx " << Swx << std::endl;
            }
            return false;
          }
          if( isnan(B) || isinf(B) ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, B " << B << " Sw " << Sw << " Swxy " << Swxy << " Swx " << Swx << " Swy " << Swy << std::endl;
            }
            return false;

          }
          if( isnan(C) || isinf(C) ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, C " << C << " Sw " << Sw << " Swyy " << Swyy << " Swy " << Swy << std::endl;
            }
            return false;

          }
          if( isnan(D) || isinf(D) ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, D " << D << " Sw " << Sw << " Swxyy " << Swxyy << " Swx " << Swx << " Swyy " << Swyy << " Swxxx " << Swxxx << " Swxx " << Swxx <<std::endl;
            }
            return false;

          }
          if( isnan(E) || isinf(E) ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, E " << E << " Sw " << Sw << " Swxxy " << Swxxy << " Swy " << Swy << " Swxx " << Swxx << " Swyyy " << Swyyy << " Swyy " << Swyy <<std::endl;
            }
            return false;

          }


          if( delta == 0.){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: in circle regression, delta " << delta << " A " << A << " C " << C << " B " << B << std::endl;
            }
            return false;
          }

          xc.set((D*C - B*E)/delta, 0.);
          yc.set((A*E - B*D)/delta, 0.);

          double rsum = 0.;
          for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
            {
              double u = it->value() - xc.value();
              double y = yi_[it - xi_.begin()].value();
              double v = y - yc.value();
              rsum += std::sqrt(mybhep::square(u) + mybhep::square(v));
            }

          r.set(rsum/xi_.size() , 0. );

          if( print_level() >= mybhep::VVERBOSE ){
            std::clog << " fitted circle through " << xi_.size() << " points: xc: "; xc.dump(); std::clog << " yc: "; yc.dump(); std::clog << " r: "; r.dump(); std::clog << " " << std::endl;
          }
        }

        c_ = circle(experimental_point(xc, experimental_double(0.,0.), yc), r, print_level(), nsigma());

        return true;

      }


      experimental_point position(experimental_double phi){

        return c_.position(phi);
      }



    };

  }

}

#endif

