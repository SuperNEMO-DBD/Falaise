/// \file falaise/snemo/physics_model/sliced_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-28
/// Last modified: 2022-04-28
///

// Ourselves:
#include <falaise/snemo/physics_model/sliced_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_activity_model,
                                                              sliced_activity_model,
                                                              "snemo::physics_model::sliced_activity_model")

    bool sliced_activity_model::is_initialized() const
    {
      return _initialized_;
    }
    
    void sliced_activity_model::initialize(const datatools::properties & config_,
                                           const activity_model_dict_type & models_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      logging = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logging, "Entering..."); 
      time::time_point start{time::not_a_date_time};
      time::time_point stop{time::not_a_date_time};
      time::time_duration duration{time::not_a_date_time};
      std::string modelName;
      time::time_period timeSlice(time::invalid_period());
      if (config_.has_key("slice.period")) {
        std::string periodRepr = config_.fetch_string("slice.period");
        timeSlice = time::time_period_from_string(periodRepr);
        DT_THROW_IF(! time::is_valid(timeSlice), std::logic_error,
                    "Invalid format for slice period ('" << periodRepr << "')!");
      } else {
        std::string startRepr;
        std::string stopRepr;
        std::string durationRepr;
        if (config_.has_key("slice.start")) {
          startRepr = config_.fetch_string("slice.start");
          DT_LOG_DEBUG(logging, "startRepr=" << startRepr); 
        }
        if (config_.has_key("slice.stop")) {
          stopRepr = config_.fetch_string("slice.stop");
          DT_LOG_DEBUG(logging, "stopRepr=" << stopRepr); 
        } else if (config_.has_key("slice.duration")) {
          durationRepr = config_.fetch_string("slice.duration");
          DT_LOG_DEBUG(logging, "durationRepr=" << durationRepr); 
        }
        if (!startRepr.empty()) {
          start = time::time_point_from_string(startRepr);
          DT_THROW_IF(start.is_not_a_date_time(), std::logic_error,
                      "Invalid format for slice start time point ('" << startRepr << "')!");
          DT_THROW_IF(stopRepr.empty() and durationRepr.empty(), std::logic_error,
                      "Missing slice stop and slice duration!");
          if (! stopRepr.empty()) {
            stop = time::time_point_from_string(stopRepr);
            DT_THROW_IF(stop.is_not_a_date_time(), std::logic_error,
                        "Invalid format for slice stop time point ('" << stopRepr << "')!");
            timeSlice = time::time_period(start, stop);
          } else {
            duration = time::time_duration_from_string(durationRepr);
            DT_THROW_IF(duration.is_not_a_date_time(), std::logic_error,
                        "Invalid format for slice duration ('" << durationRepr << "')!");
            timeSlice = time::time_period(start, duration);
          }
        }
        DT_THROW_IF(! time::is_valid(timeSlice), std::logic_error,
                    "Invalid format for slice period!");
      }
      if (time::is_valid(timeSlice)) {
        set_time_slice(timeSlice);
      }
        
      if (config_.has_key("model")) {
        modelName = config_.fetch_string("model");
        DT_THROW_IF(! has_model(models_, modelName), std::logic_error,
                    "No model with name '" << modelName << "'!");
        set_model(get_model_ref(models_, modelName));
      }

      DT_THROW_IF(! time::is_valid(_time_slice_), std::logic_error, "No time slice is set!");
      DT_THROW_IF(_model_handle_ == nullptr, std::logic_error, "No activity model is set!");
       
      _initialized_ = true;
      DT_LOG_DEBUG(logging, "Exiting."); 
      return;
    }
    
    void sliced_activity_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Model is not initialized!");
      _initialized_ = false;
      _time_slice_ = time::invalid_period();
      _model_handle_ = nullptr;
      return;
    }
    
    bool sliced_activity_model::validate_time_point(const time::time_point & p_) const
    {
      return _time_slice_.contains(p_);
    }
    
    double sliced_activity_model::compute_activity(const time::time_point & p_) const
    {
      if (not _time_slice_.contains(p_)) {
        return 0.0;
      }
      return _model_handle_->compute_activity(p_);
    }

    time::time_period sliced_activity_model::time_span() const
    {
      return _time_slice_;
    }

    void sliced_activity_model::set_time_slice(const time::time_period & time_slice_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(!time::is_valid(time_slice_), std::domain_error, "Invalid time slice!");
      _time_slice_ = time_slice_;
      return;
    }
    
    const time::time_period & sliced_activity_model::time_slice() const
    {
      return _time_slice_;
    }
    
    void sliced_activity_model::set_model(const base_activity_model & activity_model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(! activity_model_.is_initialized(), std::logic_error,
                  "Activity model is not initialized!");
      _model_handle_ = &activity_model_;
      return;
    }

    const base_activity_model & sliced_activity_model::model() const
    {
      return *_model_handle_;
    }
                         
  }  // end of namespace physics_model

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

