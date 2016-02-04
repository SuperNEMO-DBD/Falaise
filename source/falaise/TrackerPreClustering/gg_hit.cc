// -*- mode: c++ ; -*-
/// \file falaise/TrackerPreClustering/gg_hit.cc

// Ourselves:
#include <TrackerPreClustering/gg_hit.h>

// Standard library:
#include <limits>

namespace TrackerPreClustering {

  void gg_hit::set_defaults()
  {
    id     = -1;
    module = -1;
    side   = -1;
    layer  = -1;
    row    = -1;
    x  = std::numeric_limits<double>::quiet_NaN();
    y  = std::numeric_limits<double>::quiet_NaN();
    z  = std::numeric_limits<double>::quiet_NaN();
    dz = std::numeric_limits<double>::quiet_NaN();
    r  = std::numeric_limits<double>::quiet_NaN();
    dr = std::numeric_limits<double>::quiet_NaN();
    sterile    = false;
    delayed    = false;
    peripheral = false;
    noisy      = false;
    missing_top_cathode    = false;
    missing_bottom_cathode = false;
    return;
  }

  int32_t gg_hit::get_id() const
  {
    return id;
  }

  int32_t gg_hit::get_module() const
  {
    return module;
  }

  int32_t gg_hit::get_side() const
  {
    return side;
  }

  int32_t gg_hit::get_layer() const
  {
    return layer;
  }

  int32_t gg_hit::get_row() const
  {
    return row;
  }

  bool gg_hit::has_geom_id() const
  {
    return (module >= 0) && (side >= 0) && (layer >= 0) && (row >= 0);
  }

  gg_hit::gg_hit()
  {
    set_defaults();
    return;
  }

  void gg_hit::reset()
  {
    set_defaults();
    return;
  }

  bool gg_hit::has_xy() const
  {
    return (x == x) && (y == y);
  }

  double gg_hit::get_x() const
  {
    return x;
  }

  double gg_hit::get_y() const
  {
    return y;
  }

  bool gg_hit::has_zdz() const
  {
    return (z == z) && (dz == dz);
  }

  double gg_hit::get_z() const
  {
    return z;
  }

  double gg_hit::get_dz() const
  {
    return dz;
  }

  bool gg_hit::has_rdr() const
  {
    return (r == r) && (dr == dr);
  }

  double gg_hit::get_r() const
  {
    return r;
  }

  double gg_hit::get_dr() const
  {
    return dr;
  }

  bool gg_hit::is_sterile() const
  {
    return sterile;
  }

  bool gg_hit::is_delayed() const
  {
    return delayed;
  }

  bool gg_hit::is_prompt() const
  {
    return ! is_delayed();
  }

  bool gg_hit::is_noisy() const
  {
    return noisy;
  }

  bool gg_hit::is_peripheral() const
  {
    return peripheral;
  }

  bool gg_hit::is_missing_top_cathode() const
  {
    return missing_top_cathode;
  }

  bool gg_hit::is_missing_bottom_cathode() const
  {
    return missing_bottom_cathode;
  }

  bool gg_hit::has_delayed_time() const
  {
    return (delayed_time == delayed_time)
      && (delayed_time_error == delayed_time_error);
  }

  double gg_hit::get_delayed_time() const
  {
    return delayed_time;
  }

  double gg_hit::get_delayed_time_error() const
  {
    return delayed_time_error;
  }

} // end of namespace TrackerPreClustering
