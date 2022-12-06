/// \file falaise/snemo/physics_model/per_period_activity_model.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-28
 * Last modified: 2022-04-28
 *
 * Description: Per period activity model
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_PER_PERIOD_ACTIVITY_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_PER_PERIOD_ACTIVITY_MODEL_H

// Standard Library:
#include <cstdlib>
#include <string>
#include <memory>
#include <map>

// Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/factory_macros.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/physics_model/base_activity_model.h>
#include <falaise/snemo/physics_model/constant_activity_model.h>

namespace snemo {

  namespace physics_model {

    /// \brief Per period activity model
    class per_period_activity_model
      : public base_activity_model
      , public datatools::i_tree_dumpable
    {
    public:
      
      per_period_activity_model() = default;
      virtual ~per_period_activity_model() = default;
      virtual bool is_initialized() const override;
      virtual void initialize(const datatools::properties & config_,
                              const activity_model_dict_type &) override;
      virtual void reset() override;
      virtual bool validate_time_point(const time::time_point & p_) const override;
      virtual double compute_activity(const time::time_point & p_) const override;
      virtual time::time_period time_span() const override;

      struct slice_data_type
      {
        time::time_period period = time::invalid_period();
        const base_activity_model * model_handle = nullptr;
        std::shared_ptr<constant_activity_model> safe_handle;
        const base_activity_model * model() const;
      };
      
      void add_slice(const time::time_period & time_slice_,
                     const base_activity_model & model_handle_);
      
      void add_slice_with_const_activity(const time::time_period & time_slice_,
                                         const double activity_);
     
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;
      
    private:

      void _sync_(const time::time_period & time_slice_);

      bool _initialized_ = false;
      std::vector<slice_data_type> _slices_; ///< Collection of time slices each with a specific activity model
      time::time_point _begin_ = time::invalid_point();
      time::time_point _end_ = time::invalid_point();
      
      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_activity_model, per_period_activity_model)
     
    };
 
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_PER_PERIOD_ACTIVITY_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/



  
