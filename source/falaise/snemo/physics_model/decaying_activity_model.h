/// \file falaise/snemo/physics_model/decaying_activity_model.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-28
 * Last modified: 2022-04-28
 *
 * Description: Base class for activity models
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_DECAYING_ACTIVITY_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_DECAYING_ACTIVITY_MODEL_H

// This project:
#include <falaise/snemo/physics_model/base_activity_model.h>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/utils.h>

namespace snemo {

  namespace physics_model {

    /// \brief Decaying activity model
    class decaying_activity_model
      : public base_activity_model
    {
    public:
      decaying_activity_model() = default;
      virtual ~decaying_activity_model() = default;
      virtual bool is_initialized() const override;
      virtual void initialize(const datatools::properties & config_, const activity_model_dict_type &) override;
      virtual void reset() override;
      virtual bool validate_time_point(const time::time_point & p_) const override;
      virtual double compute_activity(const time::time_point & p_) const override;
      virtual time::time_period time_span() const override;

      const time::time_point & starting_time() const;
      
      void set_starting_time(const time::time_point & t_);
 
      double starting_activity() const;

      void set_starting_activity(const double a_);
 
      double half_life() const;

      void set_half_life(const double h_);
      
    private:

      bool _initialized_ = false;
      time::time_point _starting_time_ = time::invalid_point();
      double _starting_activity_ = datatools::invalid_real();
      double _half_life_ = datatools::invalid_real();

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_activity_model, decaying_activity_model)
     
    };
 
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_DECAYING_ACTIVITY_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

