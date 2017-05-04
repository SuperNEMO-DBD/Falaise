/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__broken_line_h
#define __CATAlgorithm__broken_line_h 1
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


    class broken_line : public tracking_object{

      // a broken_line is composed of a vector of experimental points

    private:
      std::string appname_;

    public:

      // points
      std::vector<experimental_point> eps_;

      // chi2 of connection along a sequence
      double chi2_;
      int32_t ndof_;
      double p_;
      size_t ifirst_;
      size_t ilast_;

      //!Default constructor
      broken_line();

      //!Default destructor
      virtual ~broken_line();

      //! constructor
      broken_line(const std::vector<experimental_point> &eps, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;



      //! set experimental_points
      void set(const std::vector<experimental_point> &eps);

      //! set chi2
      void set_chi2(double chi2);
      void set_ndof(int32_t ndof);
      void set_p(double p);
      void set_ifirst(size_t i);
      void set_ilast(size_t i);

      //! get experimental_points
      const std::vector<experimental_point> & eps()const;

      //! get chi2
      double chi2() const;
      int32_t ndof() const;
      double p() const;
      size_t ifirst() const;
      size_t ilast() const;

      void calculate_chi2();


    };

  }
}
#endif // __CATAlgorithm__broken_line_h
