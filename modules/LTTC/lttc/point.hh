#ifndef FALAISE_LTTC_PLUGIN_POINT_HH
#define FALAISE_LTTC_PLUGIN_POINT_HH

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Bayeux/geomtools :
#include <geomtools/utils.h>

namespace lttc {

  typedef geomtools::vector_2d point;
  typedef geomtools::vector_2d vector2;
  typedef std::vector<point>   polyline;

  void clear(point & p_);

  point barycenter(const point & p1_, double w1_, const point & p2_, double w2_);
  
  point middle(const point & p1_, const point & p2_);
 
  bool are_aligned(const point & p1_,
                   const point & p2_,
                   const point & p3_,
                   double tolerance_= 1e-14);

  void draw_point(std::ostream & out_, const point & p_, int tag_ = 0, bool endl_ = true);

  std::ostream & operator<<(std::ostream & out_, const polyline & pl_);

  void draw_polyline(std::ostream & out_, const polyline & pl_, int tag_ = 0);
  
} // namespace lttc

#endif // FALAISE_LTTC_PLUGIN_POINT_HH
