#ifndef FALAISE_LTTC_PLUGIN_HIT_SIMULATOR_HH
#define FALAISE_LTTC_PLUGIN_HIT_SIMULATOR_HH

// Standard library:
#include <cmath>
#include <random>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/clhep_units.h>

// This project:
#include <lttc/tracker_hit.hh>
#include <lttc/track.hh>
#include <lttc/sngeometry.hh>

namespace lttc {
  
  struct hit_simulator
  {
    struct config
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      double drift_radius_err = 0.15 * CLHEP::mm;
      bool add_noisy_hits = false;
      std::size_t nb_noisy_hits = 3;
    };
    
    hit_simulator(const tracker & trk, const config & cfg_);
    
    ~hit_simulator() = default;

    void generate_hits(std::default_random_engine & generator_,
                       const track & sntrack_,
                       tracker_hit_collection & hits_) const;

    void generate_noisy_hits(std::default_random_engine & generator_,
                             tracker_hit_collection & hits_) const;

    void set_trk_conds(const tracker_conditions &);

    void reset_trk_conds();

    const tracker *            sntracker = nullptr;
    config                     cfg;
    const tracker_conditions * trk_conds = nullptr;
    
  };
  
} // namespace lttc 

#endif //FALAISE_LTTC_PLUGIN_HIT_SIMULATOR_HH
