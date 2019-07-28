// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/drawing.cc
 */

// Ourselves:
#include <TrackFit/drawing.h>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/clhep.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/line_3d.h>
#include <geomtools/placement.h>

namespace TrackFit {

bool drawing::is_debug() const { return _debug_; }

void drawing::set_debug(bool new_value_) {
  _debug_ = new_value_;
}

drawing::drawing() {
  _debug_ = false;
}

drawing::~drawing() = default;

void drawing::draw(std::ostream& out_, const geomtools::placement& p_, const gg_hit& hit_) const {
  // Bounding box:
  geomtools::gnuplot_draw::xyz_range& BB = geomtools::gnuplot_draw::bounding_box();
  BB.reset_ranges();
  BB.activate();

  // Color context:
  geomtools::color::context& CC = geomtools::gnuplot_draw::color_context();
  CC.set_color_code(geomtools::color::COLOR_BLUE);

  geomtools::vector_3d p0, p1, p2;
  p0.set(hit_.get_x(), hit_.get_y(), hit_.get_z());
  p1.set(hit_.get_x(), hit_.get_y(), hit_.get_z() - hit_.get_sigma_z());
  p2.set(hit_.get_x(), hit_.get_y(), hit_.get_z() + hit_.get_sigma_z());

  geomtools::line_3d line(p1, p2);
  geomtools::gnuplot_draw::draw_line(out_, p_.get_translation(), p_.get_rotation(), line);

  if (hit_.get_r() - hit_.get_sigma_r() > 0.0) {
    geomtools::gnuplot_draw::draw_circle(out_, p_.get_translation() + p0, p_.get_rotation(),
                                         hit_.get_r() - hit_.get_sigma_r());
  }

  geomtools::gnuplot_draw::draw_circle(out_, p_.get_translation() + p0, p_.get_rotation(),
                                       hit_.get_r() + hit_.get_sigma_r());
}

void drawing::draw(std::ostream& out_, const geomtools::placement& p_,
                   const gg_hits_col& hits_) const {
  for (const auto & hit : hits_) {
    draw(out_, p_, hit);
  }
  }

}  // end of namespace TrackFit
