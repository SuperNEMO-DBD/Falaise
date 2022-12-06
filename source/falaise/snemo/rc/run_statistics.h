/// \file falaise/snemo/rc/run_statistics.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-16
 * Last modified: 2022-05-16
 *
 * Description: Run statistics
 */

#ifndef FALAISE_SNEMO_RC_RUN_STATISTICS_H
#define FALAISE_SNEMO_RC_RUN_STATISTICS_H

// Standard library:
#include <vector>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/mygsl/tabulated_function.h>
#include <bayeux/datatools/logger.h>

// GSL:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

// This project:
#include <falaise/snemo/rc/run_list.h>
#include <falaise/snemo/physics_model/base_activity_model.h>
#include <falaise/snemo/time/time_utils.h>

namespace snemo {
  
  namespace rc {

    /// \brief Run statistics
    class run_statistics
      : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Run statistics record
      struct record_type
      {
        run_id_type run_id = run_description::INVALID_RUN_ID; ///< Run ID
        double mean_nb_decays = 0.0; ///< Mean number of expected decay in the run
        double probability = datatools::invalid_real(); ///< Probability that an event belongs to this run
        double cumul_probability = datatools::invalid_real(); ///< Cumulative probability that an event belongs to this run of previous run
        mygsl::tabulated_function cumul_dist; ///< Tabulated cumulative probability function for this run
      };

      /// \brief Inverse function of the decay CDF associated to a run
      class inverse_cumul_dist
      {
      public:
        inverse_cumul_dist(const mygsl::tabulated_function & cumul_dist_);
        ~inverse_cumul_dist();
        double operator()(double p_) const;
        struct func_inverse_cumul_params
        {
          const mygsl::tabulated_function * cumul_dist = nullptr; ///< cdf
          double prob = 0.0; ///< random probability
        };
        /// Plain wrapper function for an instance of the inverse_cumul_dist class (interface for GSL root solver)
        static double func_inverse_cumul(double t_, void * params_);
      private:
        const mygsl::tabulated_function & _cumul_dist_; ///< cdf
        const gsl_root_fsolver_type * _solver_type_ = nullptr; ///< GSL solver type
        gsl_root_fsolver * _solver_ = nullptr; ///< GSL root solver
      };

      void set_logging(const datatools::logger::priority logging_);
      
      void set_runs(const run_list & runs_);
      
      void set_activity_model(const physics_model::base_activity_model & activity_model_);
      
      void set_time_step(const time::time_duration & time_step_);
      
      bool is_initialized() const;
      
      void initialize();
      
      void reset();
      
      const run_list & runs() const;

      const physics_model::base_activity_model & activity_model() const;

      const std::vector<record_type> & records() const;
      
      friend class mc_event_distribution;
      
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      void _sync_();

      bool _initialized_ = false; ///< Initialization flag
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; ///< Logging level
      const run_list * _runs_ = nullptr; ///< Handle to an external run list (example: from a 'run_info_service')
      const physics_model::base_activity_model * _activity_model_ = nullptr; ///< Handle to an activity model
      time::time_duration _time_step_ = time::invalid_duration(); ///< Time step used to integrate the expected number of events in run slices
      std::vector<record_type> _records_; ///< Records associated to each run from the source run list
      
    };
   
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_STATISTICS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
