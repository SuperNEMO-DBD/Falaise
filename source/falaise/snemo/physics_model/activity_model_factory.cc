/// \file falaise/snemo/physics_model/activity_model_factory.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-24
/// Last modified: 2022-05-24
///

// Ourselves:
#include <falaise/snemo/physics_model/activity_model_factory.h>

// Bayeux:
#include <bayeux/datatools/exception.h>
// #include <bayeux/datatools/clhep_units.h>
// #include <bayeux/datatools/utils.h>

namespace snemo {

  namespace physics_model {

    activity_model_factory::activity_model_factory()
    {
      return;
    }
      
    activity_model_factory::~activity_model_factory()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool activity_model_factory::is_initialized() const
    {
      return _initialized_;
    }
      
    void activity_model_factory::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Factory is already initialized!");
      std::vector<std::string> activityModelFilenames;
      if (config_.has_key("activity_models")) {
        config_.fetch("activity_models", activityModelFilenames);
      }
     
      for (std::string filename : activityModelFilenames) {
        datatools::fetch_path_with_env(filename);
        datatools::multi_properties modelDefs("name", "type");
        modelDefs.read(filename);
        load(modelDefs);
      }
      
      _initialized_ = true;
      return;
    }

    void activity_model_factory::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Factory is not initialized!");
      _initialized_ = false;

      return;
    }

    const activity_model_dict_type & activity_model_factory::get_models() const
    {
      return _models_;
    }
    
    activity_model_dict_type & activity_model_factory::grab_models()
    {
      return _models_;
    }     

    void activity_model_factory::build_model_names(std::set<std::string> & names_) const
    {
      names_.clear();
      for (const auto & modPair : _models_) {
        names_.insert(modPair.first);
      }     
      return;
    }

    bool activity_model_factory::has_model(const std::string & name_) const
    {
      return _models_.count(name_) == 1;
    }
      
    const base_activity_model & activity_model_factory::get_model(const std::string & name_) const
    {
      activity_model_dict_type::const_iterator found = _models_.find(name_);
      DT_THROW_IF(found == _models_.end(), std::logic_error, "No activity model named '" << name_ << "'!");
      return *found->second;
    }
    
    activity_model_ptr_type activity_model_factory::get_model_handle(const std::string & name_) const
    {
      activity_model_dict_type::const_iterator found = _models_.find(name_);
      DT_THROW_IF(found == _models_.end(), std::logic_error, "No activity model named '" << name_ << "'!");
      return found->second;
    }

    void activity_model_factory::load(const datatools::multi_properties & model_defs_)
    {
      const datatools::multi_properties::entries_ordered_col_type & oEntries = model_defs_.ordered_entries();
      for (auto e : oEntries) {
        std::string modelName = e->get_key();
        std::string modelType = e->get_meta();
        const datatools::properties & modelCfg = e->get_properties();
        
        const auto & modelSystemFactoryRegister = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(base_activity_model);
        DT_THROW_IF(not modelSystemFactoryRegister.has(modelType), std::logic_error,
                    "Factory register supports no activity model of type '" << modelType << "'!");
        const auto & modelSystemFactory = modelSystemFactoryRegister.get(modelType);
        activity_model_ptr_type modelHandle(modelSystemFactory());
        modelHandle->initialize(modelCfg, _models_); 
        add_model(_models_, modelName, modelHandle);
      }
 
      return;
    }
    
    // virtual
    void activity_model_factory::print_tree(std::ostream & out_,
                                            const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "System registered factories : " 
           << std::endl;
      {
        const auto & modelSystemFactoryRegister = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(base_activity_model);
        std::ostringstream indentss2;
        indentss2 << popts.indent << skip_tag;
        modelSystemFactoryRegister.tree_dump(out_, "", indentss2.str(), false);
      }
      
      out_ << popts.indent << tag
           << "Activity models : " << _models_.size()
           << std::endl;
      {
        std::uint32_t count = 0;
        for (const auto & modelPair : _models_) {
          out_ << popts.indent << skip_tag;
          if (count + 1 == _models_.size()) {
            out_ << popts.indent << last_tag;
          } else {
            out_ << popts.indent << tag;
          }
          out_ << "Activity model '" << modelPair.first << "' "
               << std::endl;
          count++; 
        }
      }
     
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << is_initialized()
           << std::endl;

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
