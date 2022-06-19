#ifndef FALAISE_GEOMETRY_FITTED_LINE2_HH
#define FALAISE_GEOMETRY_FITTED_LINE2_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <falaise/geometry/line2.hh>
#include <falaise/geometry/fitted_point2.hh>

namespace falaise {
  
  namespace geometry {

    /// \brief Fitted line in 2D XY-space
    struct fitted_line2
    {
      bool is_valid() const;
      void print(std::ostream & out_, const std::string & indent_ = "") const;
    
      // Atributes:
      double theta = std::numeric_limits<double>::quiet_NaN();     ///< Angle [0, pi]
      double r = std::numeric_limits<double>::quiet_NaN();         ///< orthogonal radius to the origin
      double theta_err = std::numeric_limits<double>::quiet_NaN(); ///< Error on angle
      double r_err = std::numeric_limits<double>::quiet_NaN();     ///< Error on radius
    };

    void draw_fitted_line(std::ostream & out_,
                          const fitted_line2 & fline_,
                          int tag_ = 0);

    void distance_to_fitted_line(const fitted_line2 & fline_,
                                 const point2 & pos_,
                                 double & distance_,
                                 double & distance_err_);
 
    void orthogonal_projection_to_fitted_line(const fitted_line2 & fline_,
                                              const point2 & pos_,
                                              fitted_point2 & fpoint_);

  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_FITTED_LINE2_HH
