/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IJOINT
#define __CATAlgorithm__IJOINT
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

    class joint : public tracking_object{

      // a joint is composed of three experimental points


    private:
      string appname_;

      // first experimental point
      experimental_point epa_; 

      // second experimental point
      experimental_point epb_; 

      // third experimental point
      experimental_point epc_; 

      // phi kink
      experimental_double kink_phi_;

      // theta kink
      experimental_double kink_theta_;

    public:   

      // joint status
      bool used_;

      // chi2 of connection along a sequence
      double chi2_;

      //!Default constructor 
      joint()
      {
        appname_= "joint: ";
        used_ = false;
        chi2_ = 0.;
      }

      //!Default destructor
      virtual ~joint(){};

      //! constructor
      joint(experimental_point epa, experimental_point epb, experimental_point epc, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "joint: ";
        epa_ = epa;
        epb_ = epb;
        epc_ = epc;
        calculate_kinks();
        used_ = false;
        chi2_ = 0.;
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
          a_out << indent << " used: " << used() << " chi2 " << chi2() << endl;
          a_out << indent << " first point " << endl;
          this->epa().dump(a_out, "", indent + "    ");
          a_out << indent << " second point " << endl;
          this->epb().dump(a_out, "", indent + "    ");
          a_out << indent << " third point " << endl;
          this->epc().dump(a_out, "", indent + "    ");
          a_out << indent << "phi : "; kink_phi().dump(); a_out << " " << endl;
          a_out << indent << "theta : "; kink_theta().dump(); a_out << " " << endl;
          a_out << indent << " -------------- " << endl;

          return;
        }
      }



      //! set experimental_points
      void set(experimental_point epa, experimental_point epb, experimental_point epc)
      {
        epa_ = epa;
        epb_ = epb;
        epc_ = epc;
        calculate_kinks();
      }


      //! set first experimental_points
      void set_epa(experimental_point epa)
      {
        epa_ = epa;
      }

      //! set second experimental_points
      void set_epb(experimental_point epb)
      {
        epb_ = epb;
      }

      //! set third experimental_points
      void set_epc(experimental_point epc)
      {
        epc_ = epc;
      }

      //! set kink phi
      void set_kink_phi(experimental_double phi)
      {
        kink_phi_ = phi;
      }

      //! set kink theta
      void set_kink_theta(experimental_double theta)
      {
        kink_theta_ = theta;
      }

      //! set used
      void set_used(bool used)
      {
        used_ = used;
      }

      //! set chi2
      void set_chi2(double chi2)
      {
        chi2_ = chi2;
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

      //! get experimental_point c
      const experimental_point& epc()const
      {
        return epc_;
      }      

      //! get kink phi
      const experimental_double& kink_phi()const
      {
        return kink_phi_;
      }

      //! get kink theta
      const experimental_double& kink_theta()const
      {
        return kink_theta_;
      }

      //! get used
      const bool& used() const
      {
        return used_;
      }

      //! get chi2
      const double& chi2() const
      {
        return chi2_;
      }

      joint invert(){
        joint inverted;
        inverted.set_print_level(print_level());
        inverted.set_nsigma(nsigma());
        inverted.set_epa(epc());
        inverted.set_epb(epb());
        inverted.set_epc(epa());
        inverted.set_used(used());
        inverted.set_chi2(chi2());
        inverted.set_kink_phi(-kink_phi());
        inverted.set_kink_theta(-kink_theta());
        return inverted;

      }

      bool operator<(const joint &j) const
      {
        return (chi2() < j.chi2());
      }

    private:

      void calculate_kinks(){
        experimental_vector v1(epa_, epb_);
        experimental_vector v2(epb_, epc_);
        kink_phi_ = v1.kink_phi(v2);
        kink_theta_ = v1.kink_theta(v2);

      }



    };
  }
}
#endif
