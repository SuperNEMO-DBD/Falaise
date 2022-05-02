/// \file falaise/snemo/physics_model/constant_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/physics_model/constant_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_activity_model,
                                                              constant_activity_model,
                                                              "snemo::physics_model::constant_activity_model")

    bool constant_activity_model::is_initialized() const
    {
      return _initialized_;
    }
    
    void constant_activity_model::initialize(const datatools::properties & config_,
                                             const activity_model_dict_type & /*models_*/)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Model is already initialized!");

      if (config_.has_key("activity")) {
        double a = config_.fetch_real_with_explicit_dimension("activity", "activity");
        set_activity(a);
      }
      
      DT_THROW_IF(! datatools::is_valid(_activity_), std::logic_error, "Invalid activity!");
      _initialized_ = true;
      return;
    }
    
    void constant_activity_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "Model is not initialized!");
      _initialized_ = false;
      _activity_ = 1.0 * CLHEP::becquerel;
      return;
    }
    
    bool constant_activity_model::validate_time_point(const time::time_point & /*p_*/) const
    {
      return true;
    }
    
    double constant_activity_model::compute_activity(const time::time_point & /*p_*/) const
    {
      return _activity_;
    }

    double constant_activity_model::activity() const
    {
      return _activity_;
    }

    void constant_activity_model::set_activity(const double & a_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Model is already initialized!");
      DT_THROW_IF(a_ < 0.0, std::domain_error,
                  "Invalid activity!");
      _activity_ = a_;
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

