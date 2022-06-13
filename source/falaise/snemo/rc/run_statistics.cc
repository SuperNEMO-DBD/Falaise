/// \file falaise/snemo/rc/run_statistics.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-16
/// Last modified: 2022-05-16
///

// Ourselves:
#include <falaise/snemo/rc/run_statistics.h>

// Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace rc {

    // static
    double run_statistics::inverse_cumul_dist::func_inverse_cumul(double t_, void * params_)
    {
      const func_inverse_cumul_params * params
        = static_cast<func_inverse_cumul_params*>(params_);
      const mygsl::tabulated_function * cumul_dist = params->cumul_dist;
      double prob = params->prob;
      return cumul_dist->eval(t_) - prob;
    }

    run_statistics::inverse_cumul_dist::inverse_cumul_dist(const mygsl::tabulated_function & cumul_dist_)
      : _cumul_dist_(cumul_dist_)
    {
      _solver_type_ = gsl_root_fsolver_falsepos;
      _solver_ = gsl_root_fsolver_alloc(_solver_type_);
      return;
    }

    double run_statistics::inverse_cumul_dist::operator()(double p_) const
    {
      // datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      // logging = datatools::logger::PRIO_DEBUG;
      DT_THROW_IF(p_ < 0.0 or p_ > 1.0, std::domain_error, "Invalid probability !");
      double tRoot = datatools::invalid_real();
      func_inverse_cumul_params params = {&_cumul_dist_, p_};
      gsl_function F;
      F.function = &func_inverse_cumul;
      F.params = &params;
      double t_min = _cumul_dist_.x_min();
      double t_max = _cumul_dist_.x_max();
      // DT_LOG_DEBUG(logging, "t_min=" << t_min / CLHEP::second << " s");
      // DT_LOG_DEBUG(logging, "t_max=" << t_max / CLHEP::second << " s");
      gsl_root_fsolver_set(_solver_, &F, t_min, t_max);
      int iter = 0;
      int status = GSL_CONTINUE;
      int max_iter = 100;
      do {
        iter++;
        status = gsl_root_fsolver_iterate(_solver_);
        tRoot = gsl_root_fsolver_root(_solver_);
        t_min = gsl_root_fsolver_x_lower(_solver_);
        t_max = gsl_root_fsolver_x_upper(_solver_);
        status = gsl_root_test_interval(t_min, t_max, 1.0 * CLHEP::microsecond, 0.001);
        
      } while (status == GSL_CONTINUE && iter < max_iter);
      // DT_THROW_IF(
      return tRoot;
    }

    void run_statistics::set_logging(const datatools::logger::priority logging_)
    {
      _logging_ = logging_;
      return;
    }
    
    void run_statistics::set_runs(const run_list & runs_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Run statistics is initialized!");
      _runs_ = &runs_;
      return;
    }
                        
    const run_list & run_statistics::runs() const
    {
      return *_runs_;
    }

    void run_statistics::set_activity_model(const physics_model::base_activity_model & activity_model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Run statistics is initialized!");
      _activity_model_ = &activity_model_;
      return;
    }

    const physics_model::base_activity_model & run_statistics::activity_model() const
    {
      return *_activity_model_;
    }

    void run_statistics::set_time_step(const time::time_duration & time_step_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Run statistics is initialized!");
      _time_step_ = time_step_;
      return;
    }
                        
    bool run_statistics::is_initialized() const
    {
      return _initialized_;
    }
                        
    void run_statistics::initialize() 
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
      _sync_();
      _initialized_ = true;
      return;
    }
                        
    void run_statistics::reset() 
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized!");
      _initialized_ = false;
      _records_.clear();
      _time_step_ = time::invalid_duration();
      _activity_model_ = nullptr;
      _runs_ = nullptr;
      return;
    }

    void run_statistics::_sync_()
    {
      datatools::logger::priority logging = _logging_;
      std::set<std::int32_t> runIds;
      _runs_->build_run_ids(runIds);
      _records_.reserve(_runs_->size());
      for (auto runId : runIds) {
        DT_LOG_DEBUG(logging, "Run ID=" << runId);
        double meanNumberOfDecays = 0.0;
        record_type rec;
        rec.run_id = runId;
        const run_description & runDesc = _runs_->get_run(runId);
        const std::vector<time::time_period> & runSlices = runDesc.slices();
        rec.cumul_dist.add_point(0.0, 0.0);
        const time::time_period & runPeriod = runDesc.period();
        // Scan all active slices in the run:
        for (unsigned int iSlice = 0; iSlice < runSlices.size(); iSlice++) {
          const time::time_period & slicePeriod = runSlices[iSlice];
          time::time_point t = slicePeriod.begin();
          while (t < slicePeriod.end()) {
            time::time_duration dt = _time_step_;
            if (t + dt > slicePeriod.end()) {
              dt = slicePeriod.end() - t;
            }
            time::time_point timeInSlice = t + dt / 2;
            if (_activity_model_->validate_time_point(timeInSlice)) {
              double act = _activity_model_->compute_activity(timeInSlice);
              double numberOfDecays = act * time::to_quantity(dt);
              meanNumberOfDecays += numberOfDecays;
            }
            time::time_duration from_run_begin_elapsed = t + dt - runPeriod.begin();
            rec.cumul_dist.add_point(time::to_quantity(from_run_begin_elapsed), meanNumberOfDecays);
            // Increment step time:
            t += dt;
          }
        }
        rec.mean_nb_decays = meanNumberOfDecays;
        if (rec.mean_nb_decays > 0) {
          rec.cumul_dist.scale(1. / meanNumberOfDecays);
        }
        rec.cumul_dist.lock_table("linear");
        DT_LOG_DEBUG(logging, "  mean_nb_decays=" << rec.mean_nb_decays);
        rec.probability = rec.mean_nb_decays;
        DT_LOG_DEBUG(logging, "  probability=" << rec.probability);
        rec.cumul_probability = rec.mean_nb_decays;
        if (_records_.size() > 0) {
          rec.cumul_probability += _records_.back().cumul_probability;
        }
        DT_LOG_DEBUG(logging, "  cumul probability=" << rec.cumul_probability);
        _records_.push_back(rec);
      }
      double totalCumul = _records_.back().cumul_probability;
      DT_LOG_DEBUG(logging, "  totalCumul=" << totalCumul);
      for (auto & rec : _records_) {
        rec.probability /= totalCumul;
        rec.cumul_probability /= totalCumul;
        DT_LOG_DEBUG(logging, "Run ID=" << rec.run_id);
        DT_LOG_DEBUG(logging, "  mean_nb_decays=" << rec.mean_nb_decays);
        DT_LOG_DEBUG(logging, "  probability=" << rec.probability);
        DT_LOG_DEBUG(logging, "  cumul probability=" << rec.cumul_probability);
      }
      return;
    }

    const std::vector<run_statistics::record_type> & run_statistics::records() const
    {
      return _records_;
    }
    
    // virtual
    void run_statistics::print_tree(std::ostream & out_,
                                    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
       
      out_ << popts.indent << tag
           << "Initialized : " << std::boolalpha << _initialized_ << std::endl;
      
      out_ << popts.indent << tag
           << "Runs : " << std::endl;
      {
        boost::property_tree::ptree popts2;
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        popts2.put("indent", indent2.str());
        _runs_->print_tree(out_, popts2);
      }
       
      out_ << popts.indent << tag
           << "Activity model : " << _activity_model_ << std::endl;
       
      out_ << popts.indent << tag
           << "Time step : " << time::to_string(_time_step_) << std::endl;
      
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Records : " << _records_.size() << std::endl;

      {
        unsigned int nbRec = 0;
        for (const record_type & rec : _records_) {
          out_ << popts.indent << inherit_skip_tag(popts.inherit);
          if (nbRec + 1 ==  _records_.size()) {
            out_ << last_tag;  
          } else {
            out_ << tag;  
          }
          out_ << "Run #" << rec.run_id << " : {nb-decays=" << rec.mean_nb_decays
               << ";prob=" << rec.probability << ";cumul-prob=" << rec.cumul_probability
               << ";cumul-dist-size=" << rec.cumul_dist.size()
               << '}'
               << std::endl;
          nbRec++;
        }
      }
       
      return;
    }

  }  // end of namespace rc

}  // end of namespace snemo
