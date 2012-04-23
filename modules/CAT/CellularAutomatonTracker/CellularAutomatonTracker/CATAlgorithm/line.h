/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILINE
#define __CATAlgorithm__ILINE
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>

namespace CAT {

  namespace topology{

    using namespace std;
    using namespace mybhep;

    class line : public tracking_object {

      // a line is composed of two experimental points


    private:
      string appname_;

      // first experimental point
      experimental_point epa_; 

      // second experimental point
      experimental_point epb_; 

      // axis from first to second point (not normalized)
      experimental_vector forward_axis_;

      // line status
      bool used_;

    public:   
      //!Default constructor 
      line(prlevel level=mybhep::NORMAL, double nsigma=10.)
      {
        appname_= "line: ";
        forward_axis_ = experimental_vector(small_neg,small_neg,small_neg,
                                            small_neg, small_neg, small_neg);
        used_ = false;
        set_print_level(level);
        set_nsigma(nsigma);
      }

      //!Default destructor
      virtual ~line(){};

      //! constructor
      line(experimental_point epa, experimental_point epb, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "line: ";
        epa_ = epa;
        epb_ = epb;
        set_forward_axis();
        used_ = false;
      }

      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false) const{
        {
          string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << endl;
            }

          a_out << indent << appname_ << " -------------- " << endl;
          a_out << indent << " used: " << used() << endl;
          a_out << indent << " first point " << endl;
          this->epa().dump(a_out, "", indent + "    ");
          a_out << indent << " second point " << endl;
          this->epb().dump(a_out, "", indent + "    ");
          a_out << indent << " forward axis " << endl;
          this->forward_axis().dump(a_out, "", indent + "    ");
          a_out << indent << " -------------- " << endl;

          return;
        }
      }



      //! set experimental_points
      void set(experimental_point epa, experimental_point epb)
      {
        epa_ = epa;
        epb_ = epb;
        set_forward_axis();
      }


      //! set first experimental_point
      void set_epa(experimental_point epa)
      {
        epa_ = epa;
      }

      //! set second experimental_point
      void set_epb(experimental_point epb)
      {
        epb_ = epb;
      }

      //! set used
      void set_used(bool used)
      {
        used_ = used;
      }

      //! get experimental_point a
      const experimental_point& epa()const
      {
        return epa_;
      }      

      //! get experimental_point b
      const experimental_point& epb()const
      {
        return epb_;
      }      

      //! get forward axis
      const experimental_vector& forward_axis()const
      {
        return forward_axis_;
      }

      //! get used
      const bool& used() const
      {
        return used_;
      }

      experimental_double phi(){
        return forward_axis_.phi();
      }
    
      experimental_double theta(){
        return forward_axis_.theta();
      }

      experimental_double kink_phi(line l){
        return forward_axis_.kink_phi(l.forward_axis());
      }

      experimental_double kink_theta(line l){
        return forward_axis_.kink_theta(l.forward_axis());
      }

      double chi2(line l, bool use_theta_kink){

        experimental_double phi_kink = kink_phi(l);
        experimental_double theta_kink = kink_theta(l);

        double result = square(phi_kink.value()/phi_kink.error()) ;

        if( use_theta_kink ){
          result += square(theta_kink.value()/theta_kink.error());
        }

        if( print_level() > mybhep::VERBOSE ){
          clog << appname_ << " calculate chi2: phi kink : "; (phi_kink*180./M_PI).dump();
          if( use_theta_kink ){
            clog << " theta kink : "; (theta_kink*180./M_PI).dump();
          }
          clog << " chi2 " << result << endl;
        }

        return result;
      }


      void set_a_forward_axis(experimental_vector v){
      
        forward_axis_ = v;

        return;
      }

      line invert(){
        line inverted(print_level(), nsigma());
        inverted.set_epa(epb());
        inverted.set_epb(epa());
        inverted.set_used(used());
        experimental_vector O(0.,0.,0.,0.,0.,0.);
        inverted.set_a_forward_axis(O-forward_axis());
        return inverted;

      }


    private:

      void set_forward_axis(){
      
        forward_axis_.set(epa(), epb());

        return;
      }



    };
  }
}

#endif
