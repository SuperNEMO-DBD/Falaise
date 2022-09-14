/// \file falaise/snemo/physics_model/scaled_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-16
/// Last modified: 2022-05-16
///

// Ourselves:
#include <falaise/snemo/physics_model/scaled_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_activity_model,
                                                              scaled_activity_model,
                                                              "snemo::physics_model::scaled_activity_model")

    bool scaled_activity_model::is_initialized() const
    {
      return _initialized_;
    }
    
    void scaled_activity_model::initialize(const datatools::properties & config_,
                                           const activity_model_dict_type & models_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      logging = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logging, "Entering..."); 

      double scaleFactor = datatools::invalid_real();
      std::string modelName;
      
      if (config_.has_key("scale_factor")) {
        scaleFactor = config_.fetch_real("scale_factor");
        DT_THROW_IF(! datatools::is_valid(scaleFactor), std::logic_error, "Invalid activity scale factor!");
      }
      if (datatools::is_valid(scaleFactor)) {
        set_scale_factor(scaleFactor);
      }
        
      if (config_.has_key("model")) {
        modelName = config_.fetch_string("model");
        DT_THROW_IF(! has_model(models_, modelName), std::logic_error,
                    "No model with name '" << modelName << "'!");
        set_model(get_model_ref(models_, modelName));
      }

      DT_THROW_IF(! datatools::is_valid(_scale_factor_), std::logic_error, "No activity scale factor is set!");
      DT_THROW_IF(_model_handle_ == nullptr, std::logic_error, "No activity model is set!");
       
      _initialized_ = true;
      DT_LOG_DEBUG(logging, "Exiting."); 
      return;
    }
    
    void scaled_activity_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Model is not initialized!");
      _initialized_ = false;
      _scale_factor_ = datatools::invalid_real();
      _model_handle_ = nullptr;
      return;
    }
    
    bool scaled_activity_model::validate_time_point(const time::time_point & /* p_ */) const
    {
      return true;
    }
    
    double scaled_activity_model::compute_activity(const time::time_point & p_) const
    {
      double scaledActivity = _scale_factor_ * _model_handle_->compute_activity(p_);
      // std::cerr << "scaled_activity_model::compute_activity : scaledActivity=" << scaledActivity / CLHEP::becquerel << " Bq\n";
      return scaledActivity;
    }

    time::time_period scaled_activity_model::time_span() const
    {
      return _model_handle_->time_span();
    }
 
    void scaled_activity_model::set_scale_factor(const double factor_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(!datatools::is_valid(factor_) or factor_ <= 0.0, std::domain_error, "Invalid activity scale factor!");
      _scale_factor_ = factor_;
      return;
    }
    
    double scaled_activity_model::scale_factor() const
    {
      return _scale_factor_;
    }
    
    void scaled_activity_model::set_model(const base_activity_model & activity_model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(! activity_model_.is_initialized(), std::logic_error,
                  "Activity model is not initialized!");
      _model_handle_ = &activity_model_;
      return;
    }

    const base_activity_model & scaled_activity_model::model() const
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

