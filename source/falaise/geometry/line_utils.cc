#include <falaise/geometry/line_utils.hh>

// Standard library:
#include <cmath>

namespace falaise {
  
  namespace geometry {

    bool matching_lines(const geomtools::line_3d & l1_,
                        const geomtools::line_3d & l2_,
                        const double xy_distance_tolerance_,
                        const double z_distance_tolerance_)
    {
      double xy_max_dist2 = xy_distance_tolerance_ * xy_distance_tolerance_;
      double z_max_dist = std::abs(z_distance_tolerance_);
      geomtools::vector_3d l1_1 = l1_.get_first();
      geomtools::vector_3d l1_2 = l1_.get_last();
      geomtools::vector_3d l2_1 = l2_.get_first();
      geomtools::vector_3d l2_2 = l2_.get_last();
      auto deltaFirst = l1_1 - l2_1;
      auto deltaLast  = l1_2 - l2_2;
      geomtools::vector_2d deltaFirst_xy(deltaFirst.x(), deltaFirst.y()); 
      geomtools::vector_2d deltaLast_xy(deltaLast.x(), deltaLast.y()); 
      if (deltaFirst_xy.mag2() >= xy_max_dist2) return false;
      if (deltaLast_xy.mag2() >= xy_max_dist2) return false;
      if (deltaFirst.z() >= z_max_dist) return false;
      if (deltaLast.z() >= z_max_dist) return false;
      return true;
    }
    
  } // namespace geometry 

} // namespace falaise 
