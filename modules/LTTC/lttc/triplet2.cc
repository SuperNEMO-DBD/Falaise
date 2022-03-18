#include <lttc/triplet2.hh>

// Standard library:
#include <cmath>

namespace lttc {

  triplet2::triplet2(const point2 & p1_, const point2 & p2_, const point2 & p3_)
  {
    p1 = p1_;
    p2 = p2_;
    p3 = p3_;
    return;
  }

  void draw_triplet(std::ostream & out_, const triplet2 & t_, int tag_)
  {
    draw_point(out_, t_.p1, tag_, true);
    draw_point(out_, t_.p2, tag_, true);
    draw_point(out_, t_.p3, tag_, true);
    out_ << '\n';
    return;
  }
 
} // namespace lttcc 
