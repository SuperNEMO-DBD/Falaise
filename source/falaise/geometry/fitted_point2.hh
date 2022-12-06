
#ifndef FALAISE_GEOMETRY_FITTED_POINT2_HH
#define FALAISE_GEOMETRY_FITTED_POINT2_HH

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Bayeux :
#include <datatools/utils.h>

// This project:
#include <falaise/geometry/point.hh>

namespace falaise {
  
  namespace geometry {

    /// \brief Fitted point in XY plane
    struct fitted_point2
    {
      double x     = datatools::invalid_real(); ///< X coordinate
      double y     = datatools::invalid_real(); ///< Y coordinate
      double x_err = datatools::invalid_real(); ///< Error on X coordinate
      double y_err = datatools::invalid_real(); ///< Error on Y coordinate
      double angle = 0.0; ///< Angle of the main axis w/r to the x-axis

      bool is_valid() const;
      point2 center() const;
      void reset();
      void draw(std::ostream & out_, double nsigma_ = 1.0,
                bool no_cross_ = false, double z_ = 0.0, int tag_ = 0) const;
      bool inside(const point2 & p_, double nsigma_ = 1.0) const;
      double residual(const point2 & p_) const;
      double nsigma(const point2 & p_) const;
      bool equal(const fitted_point2 & other_, double nsigma_ = 1.0) const;
      friend std::ostream & operator<<(std::ostream & out_, const fitted_point2 & fp_);
    };
 
  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_FITTED_POINT2_HH
