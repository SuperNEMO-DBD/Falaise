// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/gg_track_utils.cc

// Ourselves:
#include <falaise/snemo/datamodels/gg_track_utils.h>

namespace snemo {

  namespace datamodel {

    // static
    const std::string & gg_track::minimum_approach_distance_key()
    {
      static const std::string _s("gg_track.minimum_approach_distance");
      return _s;
    }

    // static
    const std::string & gg_track::minimum_approach_position_key()
    {
      static const std::string _s("gg_track.minimum_approach_position");
      return _s;
    }

    // static
    const std::string & gg_track::missing_geiger_hit_flag()
    {
      static const std::string _s("gg_track.missing_geiger_hit");
      return _s;
    }

  } // end of namespace datamodel

} // end of namespace snemo
