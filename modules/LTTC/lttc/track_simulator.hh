#ifndef FALAISE_LTTC_PLUGIN_TRACK_SIMULATOR_HH
#define FALAISE_LTTC_PLUGIN_TRACK_SIMULATOR_HH

// Standard library:
#include <random>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>

// This project:
#include <lttc/sngeometry.hh>
#include <lttc/track.hh>

namespace lttc {

  /// \brief Track random generator 
  struct track_simulator
  {
    enum track_shape_type
      {
       TRACK_SHAPE_UNDEFINED = 0,
       TRACK_SHAPE_LINE      = 0x1,
       TRACK_SHAPE_CIRCLE    = 0x2,
      };

    struct config
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      track_shape_type track_shape = TRACK_SHAPE_LINE;
      bool kinked_trajectory = false;
      double kinked_prob = 1.0;
    };
    
    track_simulator(const tracker & sntracker_, const config & cfg_);
    
    ~track_simulator() = default;

    void shoot(std::default_random_engine & generator_, track & trk_);

  private:
    
    const tracker * _sntracker_ = nullptr;
    config          _cfg_;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRACK_SIMULATOR_HPP
