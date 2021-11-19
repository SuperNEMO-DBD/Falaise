#ifndef FALAISE_LTTC_PLUGIN_TRIPLET_HPP
#define FALAISE_LTTC_PLUGIN_TRIPLET_HPP

// Standard library:
#include <iostream>
#include <tuple>

// This project:
#include <lttc/point.hh>

namespace lttc {

  /// \brief Triplet of distinct points in 2D-space
  struct triplet
  {
    triplet() = default;
    
    triplet(const point & p1_, const point & p2_, const point & p3_);
    
    point p1;
    point p2;
    point p3;
    
  };

  void draw_triplet(std::ostream & out_, const triplet & t_, int tag_ = -1);
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRIPLET_HPP
