#ifndef FALAISE_LTTC_PLUGIN_TRIPLET2_HPP
#define FALAISE_LTTC_PLUGIN_TRIPLET2_HPP

// Standard library:
#include <iostream>
#include <tuple>

// This project:
#include <lttc/point.hh>

namespace lttc {

  /// \brief Triplet of distinct points in 2D-space
  struct triplet2
  {
    triplet2() = default;
    
    triplet2(const point2 & p1_, const point2 & p2_, const point2 & p3_);

    // Attributes:
    point2 p1;
    point2 p2;
    point2 p3;
    
  };

  void draw_triplet(std::ostream & out_, const triplet2 & t_, int tag_ = -1);
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRIPLET2_HPP
