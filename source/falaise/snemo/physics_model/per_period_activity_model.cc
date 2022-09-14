/// \file falaise/snemo/physics_model/per_period_activity_model.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-16
/// Last modified: 2022-05-16
///

// Ourselves:
#include <falaise/snemo/physics_model/per_period_activity_model.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/snemo/physics_model/constant_activity_model.h>

namespace snemo {

  namespace physics_model {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_activity_model,
                                                              per_period_activity_model,
                                                              "snemo::physics_model::per_period_activity_model")

    bool per_period_activity_model::is_initialized() const
    {
      return _initialized_;
    }
    
    const base_activity_model * per_period_activity_model::slice_data_type::model() const
    {
      if (model_handle != nullptr) {
        return model_handle;
      } else if (safe_handle) {
        return safe_handle.get();
      }
      return nullptr;
    }

    void per_period_activity_model::_sync_(const time::time_period & time_slice_)
    {
      DT_THROW_IF(not time::is_valid(time_slice_), std::logic_error, "Invalid time slice period!");
      if (not time::is_valid(_begin_)) {
        _begin_ = time_slice_.begin();
      } 
      if (time::is_valid(_end_)) {
        DT_THROW_IF(time_slice_.begin() < _end_, std::logic_error, "New time slice does not follow previous one!");
      }
      _end_ = time_slice_.end();
    }
   
    void per_period_activity_model::add_slice(const time::time_period & time_slice_,
                                              const base_activity_model & model_handle_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      _sync_(time_slice_);
      slice_data_type sliceData;
      sliceData.period = time_slice_;
      sliceData.model_handle = &model_handle_;
      _slices_.push_back(sliceData);
      return;
    }
    
    void per_period_activity_model::add_slice_with_const_activity(const time::time_period & time_slice_,
                                                                  const double activity_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Model is already initialized!");
      _sync_(time_slice_);
      slice_data_type sliceData;
      sliceData.period = time_slice_;
      sliceData.safe_handle = std::make_shared<constant_activity_model>();
      sliceData.safe_handle->set_activity(activity_);
      sliceData.safe_handle->initialize_simple();
      _slices_.push_back(sliceData);
      return;
    }
     
    void per_period_activity_model::initialize(const datatools::properties & config_,
                                             const activity_model_dict_type & models_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Model is already initialized!");

      std::uint32_t numberOfSlices = 0;
      
      if (config_.has_key("number_of_slices")) {
        numberOfSlices = config_.fetch_positive_integer("number_of_slices");
        DT_THROW_IF(numberOfSlices < 1, std::logic_error, "Invalid number of activity slices!");
        for (std::uint32_t iSlice = 0; iSlice < numberOfSlices; iSlice++) {
          std::string slicePeriodKey = "slice_" + std::to_string(iSlice) + ".period";
          DT_THROW_IF(not config_.has_key(slicePeriodKey), std::logic_error,
                      "Missing slice period '" << slicePeriodKey << "'!");
          std::string slicePeriodRepr = config_.fetch_string(slicePeriodKey);
          auto slicePeriod = time::time_period_from_string(slicePeriodRepr);
          DT_THROW_IF(not time::is_valid(slicePeriod), std::logic_error,
                      "Invalid slice period '" << slicePeriodRepr << "'!");

          std::string sliceActivityModelKey = "slice_" + std::to_string(iSlice) + ".model";
          if (config_.has_key(sliceActivityModelKey)) {
            std::string sliceActivityModelName= config_.fetch_string(sliceActivityModelKey);
            DT_THROW_IF(! has_model(models_, sliceActivityModelName), std::logic_error,
                        "No model with name '" << sliceActivityModelName << "'!");
            add_slice(slicePeriod, get_model_ref(models_, sliceActivityModelName));         
          } else {
            std::string sliceConstActivityKey = "slice_" + std::to_string(iSlice) + ".activity";
            if (config_.has_key(sliceConstActivityKey)) {
              double activity = config_.fetch_real_with_explicit_dimension(sliceConstActivityKey, "activity");
              add_slice_with_const_activity(slicePeriod, activity);         
            } else {
              DT_THROW(std::logic_error, "Missing constant activity for this slice!");
            }
          }
        }     
      }
      DT_THROW_IF(_slices_.size() < 1, std::logic_error, "Invalid number of activity slices!");
      _initialized_ = true;
      return;
    }
    
    void per_period_activity_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "Model is not initialized!");
      _initialized_ = false;
      _slices_.clear();
      return;
    }
    
    bool per_period_activity_model::validate_time_point(const time::time_point & p_) const
    {
      if (p_ < _begin_ or p_ >= _end_) {
        return false;
      }
      // for (const auto & slice : _slices_) {
      //   if (slice.period.contains(p_)) return true;
      // }
      return true;
    }
    
    double per_period_activity_model::compute_activity(const time::time_point & p_) const
    {
      double act = datatools::invalid_real();
      if (p_ < _begin_ or p_ >= _end_) {
        return act;
      }
      act = 0.0;
      for (const auto & slice : _slices_) {
        if (slice.period.contains(p_)) { 
          act = slice.model()->compute_activity(p_);
          break;
        }
      }
      return act;
    }

    time::time_period per_period_activity_model::time_span() const
    {
       return time::time_period(_begin_, _end_);
    }

    // virtual
    void per_period_activity_model::print_tree(std::ostream & out_,
                                               const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Initialized : " << std::boolalpha << _initialized_ << std::endl;

      out_ << popts.indent << tag
           << "Slices : " << _slices_.size() << std::endl;
      {
        unsigned int count = 0;
        for (const auto & slice : _slices_) {
          out_ << popts.indent << skip_tag;
          if (count + 1 == _slices_.size()) {
            out_ << last_tag;
          } else {
            out_ << tag;
          }
          out_ << "Slice #" << count << " : "
               << time::to_string(slice.period, time::TIME_PERIOD_FORMAT_EXCLUDE_END) << "\n";
          
          count++;
        }
      }
 
      out_ << popts.indent << tag
           << "Begin : " << time::to_string(_begin_) << std::endl;
 
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "End : " << time::to_string(_end_) << std::endl;
  

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

