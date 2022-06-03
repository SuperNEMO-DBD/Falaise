/// \file falaise/snemo/rc/mc_event_distribution.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-16
 * Last modified: 2022-05-16
 *
 * Description: MC event distribution
 */

#ifndef FALAISE_SNEMO_RC_MC_EVENT_DISTRIBUTION_H
#define FALAISE_SNEMO_RC_MC_EVENT_DISTRIBUTION_H

// Standard library:
#include <vector>
#include <tuple>
#include <fstream>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/mygsl/tabulated_function.h>

// GSL:
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

// This project:
#include <falaise/snemo/rc/run_statistics.h>
#include <falaise/snemo/physics_model/base_activity_model.h>

namespace snemo {
  
  namespace rc {
    
    /// \brief Monte Carlo event distribution associated to a run list and an activity model
    class mc_event_distribution
      : public datatools::i_tree_dumpable
    {
    public:

      mc_event_distribution(std::uint32_t nb_events_,
                            datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
         
      ~mc_event_distribution();

      static std::unique_ptr<mc_event_distribution>
      make_regular_sampling(const run_statistics & run_stats_,
                            std::uint32_t nb_events_,
                            datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
      
      static std::unique_ptr<mc_event_distribution>
      make_random_sampling(const run_statistics & run_stats_,
                           std::uint32_t nb_events_,
                           unsigned long seed_ = 0,
                           datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

      static std::unique_ptr<mc_event_distribution>
      make_from_file(std::uint32_t nb_events_,
                     const std::string & path_,
                     datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
      
      struct event_rc_timestamping
      {
        std::int32_t run_id = -1;
        time::time_point decay_time = time::invalid_point();
      };

      bool has_next_decay() const;

      event_rc_timestamping next_decay();

      struct data_per_run
      {
        std::int32_t run_id = -1;
        std::vector<time::time_point> decay_timestamps;
      };

      inline std::uint32_t get_current_decay_index() const
      {
        return _current_decay_index_;
      }
 
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

      /// Writer (ASCII format) for sampled and ordered MC event timestamps 
      class writer
      {
      public:
        writer(std::string filepath_, std::uint32_t nb_timestamps_);
        void write(const event_rc_timestamping &);

      private:
        int _counter_ = 0;
        std::ofstream _fout_;
      };

      class reader
      {
      public:
        reader(std::string filepath_, std::uint32_t nb_timestamps_);
        ~reader();
        void read(event_rc_timestamping &);
        bool is_terminated() const;
      private:
        std::string _filepath_;
        std::uint32_t _nb_timestamps_;
        int _counter_ = 0;
        std::ifstream _fin_;
        bool _terminated_ = false;
      };
      
    private:

      void _precompute_sampling_();

      void _load_next_decay_from_file_();

      // Configuration:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      const run_statistics * _run_stats_ = nullptr;
      std::uint32_t _nb_events_ = 0; /// Must be > 0 (and probably < 1e7)
      unsigned long _seed_ = 0;

      // Working internal data:

      // Reader mode (timestamps are loaded from a precomputed set stored in a file)
      std::unique_ptr<reader> _reader_;
      event_rc_timestamping _parsed_next_decay_;

      // Sampling modes:
      // - Random mode only:
      const gsl_rng_type * _T_ = nullptr;
      gsl_rng * _prng_ = nullptr;
      // - Random and regular modes: 
      std::vector<std::uint32_t> _nb_decays_per_run_;
      std::vector<data_per_run> _per_run_data_;
      std::int32_t  _current_run_index_ = -1;
      std::uint32_t _current_decay_index_ = 0;
      std::uint32_t _current_decay_index_in_run_ = 0;
         
    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_MC_EVENT_DISTRIBUTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
