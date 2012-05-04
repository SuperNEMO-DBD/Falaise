/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__joint_h
#define __CATAlgorithm__joint_h 1
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/tracking_object.h>


namespace CAT {

  namespace topology{

    using namespace mybhep;

    class joint : public tracking_object{

      // a joint is composed of three experimental points

    private:
      std::string appname_;

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
      joint();

      //!Default destructor
      virtual ~joint();

      //! constructor
      joint(experimental_point epa, experimental_point epb, experimental_point epc, prlevel level=mybhep::NORMAL, double nsigma=10.);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;
 


      //! set experimental_points
      void set(experimental_point epa, experimental_point epb, experimental_point epc);


      //! set first experimental_points
      void set_epa(experimental_point epa);
 
      //! set second experimental_points
      void set_epb(experimental_point epb);

      //! set third experimental_points
      void set_epc(experimental_point epc);
 
      //! set kink phi
      void set_kink_phi(experimental_double phi);

      //! set kink theta
      void set_kink_theta(experimental_double theta);

      //! set used
      void set_used(bool used);

      //! set chi2
      void set_chi2(double chi2);
 
      //! get experimental_point a
      const experimental_point& epa()const;

      //! get experimental_point b
      const experimental_point& epb()const;

      //! get experimental_point c
      const experimental_point& epc()const;
 
      //! get kink phi
      const experimental_double& kink_phi()const;

      //! get kink theta
      const experimental_double& kink_theta()const;

      //! get used
      bool used() const;

      //! get chi2
      double chi2() const;
 
      joint invert();

      bool operator<(const joint &j) const;

    private:

      void calculate_kinks();

    };

  }
}
#endif // __CATAlgorithm__joint_h
