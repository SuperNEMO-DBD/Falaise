/// \file falaise/snemo/physics_model/scaled_activity_model.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-16
 * Last modified: 2022-05-16
 *
 * Description: Slided activity model
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_SCALED_ACTIVITY_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_SCALED_ACTIVITY_MODEL_H

// Standard Library:
#include <cstdlib>
#include <string>
#include <memory>
#include <map>

// Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/factory_macros.h>

// This project:
#include <falaise/snemo/physics_model/base_activity_model.h>

namespace snemo {

  namespace physics_model {

    /// \brief Scaled activity model
    class scaled_activity_model
      : public base_activity_model
    {
    public:
      
      scaled_activity_model() = default;
      virtual ~scaled_activity_model() = default;
      virtual bool is_initialized() const override;
      virtual void initialize(const datatools::properties & config_, const activity_model_dict_type &) override;
      virtual void reset() override;
      virtual bool validate_time_point(const time::time_point & p_) const override;
      virtual double compute_activity(const time::time_point & p_) const override;
      virtual time::time_period time_span() const override;

      void set_scale_factor(const double factor_);
      double scale_factor() const;
      void set_model(const base_activity_model & activity_model_);
      const base_activity_model & model() const;
      
    private:

      bool _initialized_ = false;
      double _scale_factor_ = datatools::invalid_real();
      const base_activity_model * _model_handle_ = nullptr;

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_activity_model, scaled_activity_model)
     
    };
 
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_SCALED_ACTIVITY_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/



  
