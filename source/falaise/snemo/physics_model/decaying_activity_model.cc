/// \file falaise/snemo/physics_model/decaying_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/physics_model/decaying_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_activity_model,
                                                              decaying_activity_model,
                                                              "snemo::physics_model::decaying_activity_model")

    bool decaying_activity_model::is_initialized() const
    {
      return _initialized_;
    }
    
    void decaying_activity_model::initialize(const datatools::properties & config_,
                                             const activity_model_dict_type & /*models_*/)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");

      if (config_.has_key("starting_time")) {
        std::string startTimeRepr = config_.fetch_string("starting_time");
        set_starting_time(time::time_point_from_string(startTimeRepr));
      }
 
      if (config_.has_key("starting_activity")) {
        double a = config_.fetch_real_with_explicit_dimension("starting_activity", "activity");
        set_starting_activity(a);
      }
 
      if (config_.has_key("half_life")) {
        double h = config_.fetch_real_with_explicit_dimension("half_life", "time");
        set_half_life(h);
      }
      
      DT_THROW_IF(! time::is_valid(_starting_time_), std::logic_error, "Invalid starting time!");
      DT_THROW_IF(! datatools::is_valid(_starting_activity_), std::logic_error, "Invalid starting activity!");
      DT_THROW_IF(! datatools::is_valid(_half_life_), std::logic_error, "Invalid half-life!");
      _initialized_ = true;
      return;
    }
    
    void decaying_activity_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Model is not initialized!");
      _initialized_ = false;
      _starting_time_ = time::invalid_point();
      _starting_activity_ = datatools::invalid_real();
      _half_life_ = datatools::invalid_real();
      return;
    }
    
    bool decaying_activity_model::validate_time_point(const time::time_point & p_) const
    {
      return p_ >= _starting_time_;
    }

    time::time_period decaying_activity_model::time_span() const
    {
      time::time_point t1 = _starting_time_;
      time::time_point t2 = time::pos_infin;
      return time::time_period(t1, t2);
    }
    
    double decaying_activity_model::compute_activity(const time::time_point & p_) const
    {
      if (p_ < _starting_time_) return 0.0;
      double elapsed = time::to_quantity(p_ - _starting_time_);
      return _starting_activity_ * std::exp(- std::log(2.0) *  elapsed / _half_life_);
    }

    const time::time_point & decaying_activity_model::starting_time() const
    {
      return _starting_time_;
    }
      
    void decaying_activity_model::set_starting_time(const time::time_point & t_)
    {
      _starting_time_ = t_;
      return;
    }
 
    double decaying_activity_model::starting_activity() const
    {
      return _starting_activity_;
    }

    void decaying_activity_model::set_starting_activity(const double a_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(a_ < 0.0, std::domain_error, "Invalid starting activity!");
      _starting_activity_ = a_;
      return;
    }

    double decaying_activity_model::half_life() const
    {
      return _half_life_;
    }
    
    void decaying_activity_model::set_half_life(const double h_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      DT_THROW_IF(h_ < 0.0, std::domain_error, "Invalid half life!");
      _half_life_ = h_;
      return;
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

