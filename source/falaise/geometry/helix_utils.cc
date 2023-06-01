#include <falaise/geometry/helix_utils.hh>

// Standard library:
#include <cmath>

namespace falaise {
  
  namespace geometry {

    bool matching_helices(const geomtools::helix_3d & h1_,
                          const geomtools::helix_3d & h2_,
                          const double xy_distance_tolerance_,
                          const double z_distance_tolerance_)
    {
      double xy_max_dist2 = xy_distance_tolerance_ * xy_distance_tolerance_;
      double z_max_dist = std::abs(z_distance_tolerance_);
      geomtools::vector_3d h1_1 = h1_.get_first();
      geomtools::vector_3d h1_2 = h1_.get_point(0.5 * (h1_.get_t1() + h1_.get_t2()));
      geomtools::vector_3d h1_3 = h1_.get_last();
      geomtools::vector_3d h2_1 = h2_.get_first();
      geomtools::vector_3d h2_2 = h2_.get_point(0.5 * (h1_.get_t1() + h1_.get_t2()));
      geomtools::vector_3d h2_3 = h2_.get_last();
      auto deltaFirst  = h1_1 - h2_1;
      auto deltaMedium = h1_2 - h2_2;
      auto deltaLast   = h1_3 - h2_3;
      geomtools::vector_2d deltaFirst_xy(deltaFirst.x(), deltaFirst.y()); 
      geomtools::vector_2d deltaMedium_xy(deltaMedium.x(), deltaMedium.y()); 
      geomtools::vector_2d deltaLast_xy(deltaLast.x(), deltaLast.y()); 
      if (deltaFirst_xy.mag2() >= xy_max_dist2) return false;
      if (deltaMedium_xy.mag2() >= xy_max_dist2) return false;
      if (deltaLast_xy.mag2() >= xy_max_dist2) return false;
      if (deltaFirst.z() >= z_max_dist) return false;
      if (deltaMedium.z() >= z_max_dist) return false;
      if (deltaLast.z() >= z_max_dist) return false;
      return true;
    }
    

  } // namespace geometry 

} // namespace falaise 
