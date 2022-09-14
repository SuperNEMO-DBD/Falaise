/// \file falaise/snemo/physics_model/constant_activity_model.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-28
 * Last modified: 2022-04-28
 *
 * Description: Base class for activity models
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_CONSTANT_ACTIVITY_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_CONSTANT_ACTIVITY_MODEL_H

// This project:
#include <falaise/snemo/physics_model/base_activity_model.h>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace physics_model {

    /// \brief Constant activity model
    class constant_activity_model
      : public base_activity_model
    {
    public:
      constant_activity_model() = default;
      virtual ~constant_activity_model() = default;
      virtual bool is_initialized() const override;
      virtual void initialize(const datatools::properties & config_, const activity_model_dict_type &) override;
      virtual void reset() override;
      virtual bool validate_time_point(const time::time_point & p_) const override;
      virtual double compute_activity(const time::time_point & p_) const override;
      virtual time::time_period time_span() const override;


      double activity() const;

      void set_activity(const double & a_);
      
    private:

      bool _initialized_ = false;
      double _activity_ = 1.0 * CLHEP::becquerel;

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_activity_model, constant_activity_model)
     
    };
 
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_CONSTANT_ACTIVITY_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

