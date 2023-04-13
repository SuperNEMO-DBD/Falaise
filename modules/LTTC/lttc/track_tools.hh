#ifndef FALAISE_LTTC_PLUGIN_TRACK_TOOLS_HH
#define FALAISE_LTTC_PLUGIN_TRACK_TOOLS_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>
#include <lttc/line.hh>

namespace lttc {

  struct track_path_collision
  {
  };

  struct track_path_interceptor
  {
    bool find(const track_path_data & tp1_,
              const track_path_data & tp2_,
              track_path_collision & tpc_);

  };
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRACK_TOOLS_HH
