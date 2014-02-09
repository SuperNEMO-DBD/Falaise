// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/drawing.cc
 */

// Ourselves:
#include <TrackFit/drawing.h>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/clhep.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/placement.h>

namespace TrackFit {

  bool drawing::is_debug() const
  {
    return _debug_;
  }

  void drawing::set_debug(bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  drawing::drawing()
  {
    _debug_ = false;
    return;
  }

  drawing::~drawing()
  {
    return;
  }

  void drawing::draw(std::ostream & out_,
                     const geomtools::placement & p_,
                     const gg_hit & hit_) const
  {
    geomtools::vector_3d p0, p1, p2;
    p0.set(hit_.get_x(), hit_.get_y(), hit_.get_z());
    p1.set(hit_.get_x(), hit_.get_y(), hit_.get_z() - hit_.get_sigma_z());
    p2.set(hit_.get_x(), hit_.get_y(), hit_.get_z() + hit_.get_sigma_z());

    geomtools::gnuplot_draw::draw_segment(out_,
                                          p_.get_translation(),
                                          p_.get_rotation(),
                                          p1,
                                          p2);

    geomtools::gnuplot_draw::draw_circle(out_,
                                         p_.get_translation() + p0,
                                         p_.get_rotation(),
                                         hit_.get_r() - hit_.get_sigma_r());

    geomtools::gnuplot_draw::draw_circle(out_,
                                         p_.get_translation() + p0,
                                         p_.get_rotation(),
                                         hit_.get_r() + hit_.get_sigma_r());
    return;
  }

  void drawing::draw(std::ostream & out_,
                     const geomtools::placement & p_,
                     const gg_hits_col & hits_) const
  {
    for(gg_hits_col::const_iterator i = hits_.begin();
        i != hits_.end();
        i++) {
      const gg_hit & hit = *i;
      draw(out_, p_, hit);
    }
    return;
  }

} // end of namespace TrackFit
