#include <lttc/triplet.hh>

// Standard library:
#include <cmath>

namespace lttc {

  triplet::triplet(const point & p1_, const point & p2_, const point & p3_)
  {
    p1 = p1_;
    p2 = p2_;
    p3 = p3_;
    return;
  }

  void draw_triplet(std::ostream & out_, const triplet & t_, int tag_)
  {
    draw_point(out_, t_.p1, tag_, true);
    draw_point(out_, t_.p2, tag_, true);
    draw_point(out_, t_.p3, tag_, true);
    return;
  }
 
} // namespace lttcc 
