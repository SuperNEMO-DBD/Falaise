// -*- mode: c++ ; -*-
// gg_track_utils.cc

// Ourselves:
#include <falaise/gg_track_utils.h>

namespace snemo {
  namespace mc {

    // static
    const std::string & gg_track::minimum_approach_distance_key()
    {
      static const std::string s("gg_track.minimum_approach_distance");
      return s;
    }

    // static
    const std::string & gg_track::minimum_approach_position_key()
    {
      static const std::string s("gg_track.minimum_approach_position");
      return s;
    }

    // static
    const std::string & gg_track::missing_geiger_hit_flag()
    {
      static const std::string s("gg_track.missing_geiger_hit");
      return s;
    }

  } // end of namespace mc

} // end of namespace snemo

// end of gg_track_utils.cc
