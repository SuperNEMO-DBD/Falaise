
#ifndef FL_SIMRC_APP_PARAMS_HPP
#define FL_SIMRC_APP_PARAMS_HPP

// Standard Library:
#include <string>
#include <cstdint>

// Third Party:
#include <bayeux/datatools/logger.h>

namespace FLSimRC {

  struct app_params
  {
    /// Verbosity level
    datatools::logger::priority log_level;
    
    /// Configuration file for the activity model factory
    std::string activity_model_factory_conf_path;

    /// Selected activity model from the factory
    std::string activity_model_name;

    /// Path to the run list definition file
    std::string run_list_path;

    /// Total number of simulated events to be timestamped
    std::uint32_t number_of_mc_events;

    /// Path to the output file with MC events' generated timestamps
    std::string event_timestamps_path;

    /// Path to the output file with MC events' generated time quantity
    std::string event_times_path;

    /// Timestamp generation mode (random, regular)
    std::string timestamp_generation_mode;

    /// Seed for the random number generator in "random" mode
    unsigned long timestamp_generation_random_seed = 0; // XXX

    //! Build a default arguments set:
    static app_params make_default();
    
  };

} // namespace FLSimRC

#endif  // FL_SIMRC_APP_PARAMS_HPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
