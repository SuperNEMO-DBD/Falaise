#include <lttc/track.hh>

// Standard library:
// #include <cmath>

// Third party:
// - Bayeux :
#include <geomtools/utils.h>

// This project:
#include <lttc/rectangle.hh>

namespace lttc {

  track::track(int id_, const polyline & pl_, const uint32_t flags_)
    : id(id_)
    , flags(flags_)
    , pl(pl_)
  {
    geomtools::invalidate(kink);
    return;
  }

  void track::draw(std::ostream & out_) const
  {
    draw_polyline(out_, this->pl);
    if (flags & KINK) {
      rectangle rKink(kink - vector2(5,5), 10, 10);
      rKink.draw(out_);
    }
    return;
  }

} // namespace lttc 

  
