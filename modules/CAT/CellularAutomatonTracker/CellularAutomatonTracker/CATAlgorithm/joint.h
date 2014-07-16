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
#include <CATAlgorithm/cell_base.h>


namespace CAT {

  namespace topology{


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
      int32_t ndof_;
      double p_;

      //!Default constructor
      joint();

      //!Default destructor
      virtual ~joint();

      //! constructor
      joint(const experimental_point &epa, const experimental_point &epb, const experimental_point &epc, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;



      //! set experimental_points
      void set(const experimental_point &epa, const experimental_point &epb, const experimental_point &epc);


      //! set first experimental_points
      void set_epa(const experimental_point &epa);

      //! set second experimental_points
      void set_epb(const experimental_point &epb);

      //! set third experimental_points
      void set_epc(const experimental_point &epc);

      //! set kink phi
      void set_kink_phi(const experimental_double &phi);

      //! set kink theta
      void set_kink_theta(const experimental_double &theta);

      //! set used
      void set_used(bool used);

      //! set chi2
      void set_chi2(double chi2);
      void set_ndof(int32_t ndof);
      void set_p(double p);

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
      int32_t ndof() const;
      double p() const;

      joint invert();

      bool operator<(const joint &j) const;

      double calculate_chi2(joint j, topology::cell A, topology::cell B, joint * modified)const;

    private:

      void calculate_kinks();

    };

  }
}
#endif // __CATAlgorithm__joint_h
