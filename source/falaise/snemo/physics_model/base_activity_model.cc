/// \file falaise/snemo/physics_model/base_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-28
/// Last modified: 2022-04-28
///

// Ourselves:
#include <falaise/snemo/physics_model/base_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_activity_model,
                                                     "snemo::physics_model::base_activity_model/__system__")

    bool has_model(const activity_model_dict_type & models_,
                   const std::string & model_name_)
    {
      return models_.count(model_name_);
    }

    activity_model_ptr_type get_model(const activity_model_dict_type & models_,
                                      const std::string & model_name_)
    {
      auto found = models_.find(model_name_);
      DT_THROW_IF(found == models_.end(), std::logic_error,
                  "No modle named '" << model_name_ << "'!");
      return found->second;
    }

    const base_activity_model & get_model_ref(const activity_model_dict_type & models_,
                                              const std::string & model_name_)
    {
      auto found = models_.find(model_name_);
      DT_THROW_IF(found == models_.end(), std::logic_error,
                  "No modle named '" << model_name_ << "'!");
      return *found->second;
    }
      

    void add_model(activity_model_dict_type & models_,
                   const std::string & model_name_,
                   const activity_model_ptr_type & model_ptr_)
    {
      DT_THROW_IF(has_model(models_, model_name_), std::logic_error,
                  "A model named '" << model_name_ << "' already exists!");
      models_[model_name_] = model_ptr_;
      return;
    }
 
    void base_activity_model::initialize_standalone(const datatools::properties & config_)
    {
      activity_model_dict_type dummy;
      initialize(config_, dummy);
      return;
    }
  
    void base_activity_model::initialize_simple()
    {
      datatools::properties dummy;
      initialize_standalone(dummy);
      return;
    }
    
    datatools::logger::priority base_activity_model::get_logging() const
    {
      return _logging_;
    }
    
    void base_activity_model::set_logging(datatools::logger::priority log_)
    {
      _logging_ = log_;
      return;
    }
                                
  }  // end of namespace physics_model

}  // end of namespace snemo
