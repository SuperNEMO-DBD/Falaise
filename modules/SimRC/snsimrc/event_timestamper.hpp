//! \file   event_timestamper.hpp
//! \brief   Event timestamping
//! \details Process a things object

#ifndef FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP
#define FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP

// Third party:
// - Bayeux :
#include <bayeux/datatools/logger.h>
#include <bayeux/geomtools/manager.h>

// Falaise:
// #include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/rc/run_list.h>
#include <falaise/snemo/physics_model/activity_model_factory.h>
#include <falaise/snemo/rc/run_statistics.h>
#include <falaise/snemo/rc/mc_event_distribution.h>

namespace snemo {

  namespace simulation {

    /// \brief The MC event timestamper assign a run ID and a timestamp to a collection of input events
    ///
    /// It uses its embedded *MC event distribution* object supporting 2 modes:
    /// 1) The distribution object can be fed with a precomputed list of
    ///    run ID/timestamp pairs stored in a file
    /// 2) The distribution of events (recommanded: no more than 10^7 events) is computed
    ///    taking into account a list of runs (running periods) and a decay activity model
    class event_timestamper
    {
    public:

      event_timestamper();

      bool is_initialized() const;

      /** \brief Initialization of the sampler from a set of properties
       *
       * Supported properties:
       *
       *   Debug flag:
       *     debug : boolean = true|false
       *
       *   Number of requested timestamped events:
       *     number_of_events : integer = 100000
       *
       *   Path of a file containing pre-computed timestamps:
       *     mc_event_timestamps_file : string as path = "/tmp/timestamps.lis"
       *
       * The properties below are only relevant if the 'mc_event_timestamps_file" is not used.
       *
       *   Paths of files containind definitions of runs:
       *     run_lists : string[2] as path = "/tmp/phase_1_runs.lis" "/tmp/phase_2_runs.lis" 
       *
       *   Path of the configuration file for the activity model factory:
       *     activity_model_factory.config : string as path = "/tmp/activity_model_factory.conf" 
       *
       *   Name of the selected activity model:
       *     activity_model : string = "constant" 
       *
       *   Duration of time step for sampling the event time distribution:
       *     run_statistics.time_step : string = "00:15:00" # 15 minutes step
       *
       *   Seed for the random sampling of event timestamps:
       *     random_seed : integer = 314159
       *   If 'random_seed' equals 0, then the algorithm uses a regular probability 
       *   sampling using the cumulative distribution tabulated function computed from 
       *   the statistics of the runs.
       *
       *   Timestamp reuse factor:
       *     timestamp_reuse_factor : integer = 100
       *   The property allows to reuse the same sampled run_id/timestamp pair for several simulated events
       *   up to an occurence of 'timestamp_reuse_factor'. This is useful if the number of sampled timestamps 
       *   is lower than the requested number of events. 
       *
       */
      void initialize(const datatools::properties & config_);
      
      void reset();

      /// Main method to enrich an *event header* bank with a run ID and an event timestamp
      void process(snemo::datamodel::event_header & eh_);

      snemo::rc::mc_event_distribution & mcEventDistribution();
      
    private:

      // Management:
      bool _initialized_ = false; ///< Initialization flag
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; ///< Verbosity

      // Configuration:
      std::unique_ptr<snemo::rc::run_list> _runList_; ///< The list if runs
      snemo::physics_model::activity_model_factory _actModelFactory_; ///< The factory for activity models
      snemo::physics_model::activity_model_ptr_type _mcActivityModel_; ///< The selected activity model
      std::unique_ptr<snemo::rc::run_statistics> _mcRunStatistics_; ///< Statistics data computed from the models above
      std::unique_ptr<snemo::rc::mc_event_distribution> _mcEventDistribution_; ///< The distribution of event timestamps
      std::uint32_t _timestamp_reuse_factor_ = 1; ///< Reuse factor (integer >=1) for sampled timestamps

      // Running attributes:
      snemo::rc::mc_event_distribution::event_rc_timestamping _current_timestamping_;
      std::uint32_t _counter_ = 0;
      std::uint32_t _current_timestamping_reuse_loop_ = 0;
      
    };
       
  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP
