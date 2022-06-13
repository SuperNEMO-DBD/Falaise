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
        std::int32_t run_id = run_description::INVALID_RUN_ID;
        double mean_nb_decays = 0.0;
        double probability = datatools::invalid_real();
        double cumul_probability = datatools::invalid_real();
        mygsl::tabulated_function cumul_dist;
      };

      /// \brief Inverse function of the decay CDF associated to a run
      class inverse_cumul_dist
      {
      public:
        inverse_cumul_dist(const mygsl::tabulated_function & cumul_dist_);
        double operator()(double p_) const;
        struct func_inverse_cumul_params
        {
          const mygsl::tabulated_function * cumul_dist = nullptr;
          double prob = 0.0;
        };
        static double func_inverse_cumul(double t_, void * params_);
      private:
        const mygsl::tabulated_function & _cumul_dist_;
        const gsl_root_fsolver_type * _solver_type_ = nullptr;
        gsl_root_fsolver * _solver_ = nullptr;
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

      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      const run_list * _runs_ = nullptr;
      const physics_model::base_activity_model * _activity_model_ = nullptr;
      time::time_duration _time_step_ = time::invalid_duration();
      std::vector<record_type> _records_;
      
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
