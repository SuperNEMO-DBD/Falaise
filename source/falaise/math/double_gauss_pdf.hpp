/// \file falaise/math/double_gauss_pdf.hpp
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2024-07-02
 * Last modified: 2024-07-02
 *
 * Description: 
 *
 */

#ifndef FALAISE_MATH_DOUBLE_GAUSS_PDF_HPP
#define FALAISE_MATH_DOUBLE_GAUSS_PDF_HPP

// Standard library:
#include <cmath>

// Bayeux:
#include <bayeux/mygsl/rng.h>

namespace falaise {

  namespace math {

    class double_gauss_pdf
    {
    public:
      
      double_gauss_pdf(const double xmax_,
											 const double sigma_left_,
											 const double sigma_right_);
      
      double xmax() const;

      double sigma_left() const;

      double sigma_right() const;
      
      double a() const;
      
      double b() const;

      double operator()(const double x_) const;

    private:
      
      double _xmax_ = 0.0;
      double _sigma_left_ = 1.0;
      double _sigma_right_ = 1.0;
      double _a_ = 1.0;
      double _b_ = 1.0;
      
    };

    class double_gauss_generator
    {
    public:

      double_gauss_generator(const double_gauss_pdf & pdf_);

      double shoot(mygsl::rng &) const;
      
    private:

      const double_gauss_pdf & _pdf_;
      double _rmed_;
      
    };
 
  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_MATH_DOUBLE_GAUSS_PDF_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

