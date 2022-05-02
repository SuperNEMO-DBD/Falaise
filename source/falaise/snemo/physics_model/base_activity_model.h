/// \file falaise/snemo/physics_model/base_activity_model.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-28
 * Last modified: 2022-04-28
 *
 * Description: Base class for activity models
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_BASE_ACTIVITY_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_BASE_ACTIVITY_MODEL_H

// Standard Library:
#include <cstdlib>
#include <string>
#include <memory>
#include <map>

// Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/factory_macros.h>

// This project:
#include <falaise/snemo/time/time_utils.h>

namespace snemo {

  namespace physics_model {

    class base_activity_model;

    typedef std::shared_ptr<base_activity_model> activity_model_ptr_type;
    typedef std::map<std::string, activity_model_ptr_type> activity_model_dict_type;

    bool has_model(const activity_model_dict_type & models_,
                   const std::string & model_name_);

    activity_model_ptr_type get_model(const activity_model_dict_type & models_,
                                      const std::string & model_name_);

    const base_activity_model & get_model_ref(const activity_model_dict_type & models_,
                                              const std::string & model_name_);

    void add_model(activity_model_dict_type & models_,
                  const std::string & model_name_,
                   const  activity_model_ptr_type & model_ptr_);
    
    /// \brief Base class for all activity models
    class base_activity_model
    {
    public:
      base_activity_model() = default;
      virtual ~base_activity_model() = default;
      virtual bool is_initialized() const = 0;
      virtual void initialize(const datatools::properties & config_, const activity_model_dict_type &) = 0;
      virtual void reset() = 0;
      virtual bool validate_time_point(const time::time_point & p_) const = 0;
      virtual double compute_activity(const time::time_point & p_) const = 0;

      void initialize_standalone(const datatools::properties & config_);
      void initialize_simple();
      datatools::logger::priority get_logging() const;
      void set_logging(datatools::logger::priority);
      
    private:

      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_activity_model)
      
    };
  
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_BASE_ACTIVITY_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

