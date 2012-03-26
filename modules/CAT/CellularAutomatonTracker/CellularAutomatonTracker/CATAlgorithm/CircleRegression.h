/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICIRCLEREGRESSION
#define __CATAlgorithm__ICIRCLEREGRESSION
#include <iostream>
#include <cmath>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/circle.h>

namespace CAT{
  namespace topology{


    using namespace std;


    class CircleRegression : public tracking_object{

      // operate weighted circle regression on points (xi, yi)
      // to find center and radius of best fitting circle

    private:

      string appname_;

      circle c_;

      vector<experimental_double> yi_;
      vector<experimental_double> xi_;


    public:

      //!Default constructor 
      CircleRegression(prlevel level=mybhep::NORMAL, double nsigma=10.)
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
      CircleRegression(vector<experimental_double> xi, vector<experimental_double> yi, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "CircleRegression: ";
        xi_ = xi;
        yi_ = yi;
      }

      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false){
        {
          string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << endl;
            }
        
          a_out << indent << appname_ << " -------------- " << endl;
          a_out << indent << " points: " << endl;
          for(vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it){
            experimental_double y = yi_[it - xi_.begin()];
            experimental_double phi = c_.phi_of_point(experimental_point(*it, experimental_double(0.,0.), y));
            a_out << indent << " .. x "; it->dump(); a_out << " y "; yi_[it - xi_.begin()].dump(); a_out << " predicted x "; position(phi).x().dump(); a_out << " y "; position(phi).z().dump(); a_out << " "  << endl;
          }
          a_out << indent << " circle: " << endl;
          circle().dump();
          a_out << indent << " -------------- " << endl;

          return;
        }
      }



      //! set 
      void set(vector<experimental_double> xi, vector<experimental_double> yi)
      {
        xi_ = xi;
        yi_ = yi;
      }


      //! set xi
      void set_xi(vector<experimental_double> xi)
      {
        xi_ = xi;
      }

      //! set yi
      void set_yi(vector<experimental_double> yi)
      {
        yi_ = yi;
      }

      //! set circle
      void set_circle(circle c)
      {
        c_ = c;
      }

      //! get xi
      const vector<experimental_double>& xi()const
      {
        return xi_;
      }      

      //! get yi
      const vector<experimental_double>& yi()const
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
            clog << " problem: in circle regression, sizes x " << xi_.size() << " y " << yi_.size() << endl;
          }
          return false;
        }

        experimental_double xc, yc, r;


        bool method1 = false;
        // method 1: R. Bullock, http://www.dtcenter.org/met/users/docs/write_ups/circle_fit.pdf
        // method 2: D. Umbach, K. N. Jones, http://www.cs.bsu.edu/homepages/kerryj/kjones/circles.pdf

        vector<double> ui;
        vector<double> vi;
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
          clog << " xave " << xave << " yave " << yave << endl;
      
        }

        for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
          {
            double y = yi_[it - xi_.begin()].value();
            double yerr = yi_[it - xi_.begin()].error();
            double w = 1./(square(it->error())) + 1./(square(yerr));
            if( isnan(w) || isinf(w) )
              w = 1.;
            Sw += w;

            if( method1 ){
              double u = it->value() - xave;
              double v = y - yave;
              Swuu += w*square(u);
              Swuv += w*u*v;
              Swvv += w*square(v);
              Swuuu += w*cube(u);
              Swuuv += w*square(u)*v;
              Swuvv += w*u*square(v);
              Swvvv += w*cube(v);
            } else {
              Swx += w*it->value();
              Swy += w*y;
              Swxx += w*square(it->value());
              Swxy += w*it->value()*y;
              Swyy += w*square(y);
              Swxyy += w*it->value()*square(y);
              Swxxy += w*square(it->value())*y;
              Swyyy += w*cube(y);
              Swxxx += w*cube(it->value());
            }
          }
      
        if( method1 ){
          delta = Swuu*Swvv - square(Swuv);
        
          if( delta == 0.){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, delta " << delta << " Swuu " << Swuu << " Swvv " << Swvv << " Swuv " << Swuv << endl;
            }
            return false;
          }
      
          double uc = (Swuuu + Swuvv)/(2.*delta);
          double vc = (Swuuv + Swvvv)/(2.*delta);
          double erruc = 0.;
          double errvc = 0.;
          double alpha = square(uc) + square(vc) + (Swuu + Swvv)/Sw;
          double erralpha = 0.;
        
          clog << " uc " << uc << " vc " << vc << endl;
        
          xc.set(uc + xave, erruc);
          yc.set(vc + yave, errvc);
          r.set(sqrt(alpha), erralpha/(2.*sqrt(alpha)));
        }
        else{
          double A = Sw*Swxx - square(Swx);
          double B = Sw*Swxy - Swx*Swy;
          double C = Sw*Swyy - square(Swy);
          double D = (Sw*Swxyy - Swx*Swyy + Sw*Swxxx - Swx*Swxx)/2.;
          double E = (Sw*Swxxy - Swy*Swxx + Sw*Swyyy - Swy*Swyy)/2.;
          delta = A*C - square(B);
        
          if( isnan(A) || isinf(A) ){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, A " << A << " Sw " << Sw << " Swxx " << Swxx << " Swx " << Swx << endl;
            }
            return false;
          }
          if( isnan(B) || isinf(B) ){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, B " << B << " Sw " << Sw << " Swxy " << Swxy << " Swx " << Swx << " Swy " << Swy << endl;
            }
            return false;

          }
          if( isnan(C) || isinf(C) ){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, C " << C << " Sw " << Sw << " Swyy " << Swyy << " Swy " << Swy << endl;
            }
            return false;

          }
          if( isnan(D) || isinf(D) ){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, D " << D << " Sw " << Sw << " Swxyy " << Swxyy << " Swx " << Swx << " Swyy " << Swyy << " Swxxx " << Swxxx << " Swxx " << Swxx <<endl;
            }
            return false;

          }
          if( isnan(E) || isinf(E) ){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, E " << E << " Sw " << Sw << " Swxxy " << Swxxy << " Swy " << Swy << " Swxx " << Swxx << " Swyyy " << Swyyy << " Swyy " << Swyy <<endl;
            }
            return false;

          }


          if( delta == 0.){
            if( print_level() >= mybhep::NORMAL ){
              clog << " problem: in circle regression, delta " << delta << " A " << A << " C " << C << " B " << B << endl;
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
              rsum += sqrt(square(u) + square(v));
            }

          r.set(rsum/xi_.size() , 0. );

          if( print_level() >= mybhep::VVERBOSE ){
            clog << " fitted circle through " << xi_.size() << " points: xc: "; xc.dump(); clog << " yc: "; yc.dump(); clog << " r: "; r.dump(); clog << " " << endl;
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

