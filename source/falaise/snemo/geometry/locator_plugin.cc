// -*- mode: c++ ; -*-
/** \file falaise/snemo/geometry/locator_plugin.cc */

// Ourselves:
#include <falaise/snemo/geometry/locator_plugin.h>

// This project:
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace geometry {

    GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(locator_plugin,"snemo::geometry::locator_plugin");

    const snemo::geometry::gg_locator & locator_plugin::get_gg_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return *_gg_locator_;
    }

    const snemo::geometry::calo_locator & locator_plugin::get_calo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return *_calo_locator_;
    }

    const snemo::geometry::xcalo_locator & locator_plugin::get_xcalo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return *_xcalo_locator_;
    }

    const snemo::geometry::gveto_locator & locator_plugin::get_gveto_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return *_gveto_locator_;
    }

    const locator_plugin::locator_dict_type & locator_plugin::get_locators() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return _locators_;
    }

    locator_plugin::locator_dict_type & locator_plugin::grab_locators()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Locator plugin is not initialized !");
      return _locators_;
    }

    locator_plugin::locator_plugin()
    {
      _initialized_ = false;
      return;
    }

    locator_plugin::~locator_plugin()
    {
      if(is_initialized()) {
          reset();
        }
      return;
    }

    bool locator_plugin::is_initialized() const
    {
      return _initialized_;
    }

    int locator_plugin::initialize(const datatools::properties & config_,
                                   const geomtools::manager::plugins_dict_type & /*plugins_*/,
                                   const datatools::service_dict_type & /*services_*/)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Plugin is already initialized !");

      geomtools::manager::base_plugin::_basic_initialize(config_);

      DT_LOG_NOTICE(get_logging_priority(), "Building the embedded locators...");
      _build_locators(config_);

      _initialized_ = true;
      return 0;
    }

    int locator_plugin::reset()
    {
      for(locator_dict_type::iterator iloc = _locators_.begin();
           iloc != _locators_.end(); ++iloc) {
        geomtools::base_locator & a_locator = iloc->second.locator_handle.grab();
        a_locator.reset();
      }
      _initialized_ = false;
      return 0;
    }

    void locator_plugin::_build_locators(const datatools::properties & config_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      DT_THROW_IF(! config_.has_key("locators.module_number"),
                   std::logic_error,
                   "Missing 'locators.module_number' key !");
      const unsigned int module_number = config_.fetch_integer("locators.module_number");

      {
        // Add a new entry for geiger locator
        const std::string name = "gg";
        locator_entry_type tmp_entry;
        tmp_entry.label = name;
        _locators_[name] = tmp_entry;
        _locators_[name].locator_handle.reset(new gg_locator(get_geo_manager(), module_number));
        _locators_[name].locator_handle.grab().initialize(config_);
        _gg_locator_ = dynamic_cast<const gg_locator*>(&_locators_[name].locator_handle.get());
      }
      {
        // Add a new entry for main wall locator
        const std::string name = "calo";
        locator_entry_type tmp_entry;
        tmp_entry.label = name;
        _locators_[name] = tmp_entry;
        _locators_[name].locator_handle.reset(new calo_locator(get_geo_manager(), module_number));
        _locators_[name].locator_handle.grab().initialize(config_);
        _calo_locator_ = dynamic_cast<const calo_locator*>(&_locators_[name].locator_handle.get());
      }
      {
        // Add a new entry for X wall locator
        const std::string name = "xcalo";
        locator_entry_type tmp_entry;
        tmp_entry.label = name;
        _locators_[name] = tmp_entry;
        _locators_[name].locator_handle.reset(new xcalo_locator(get_geo_manager(), module_number));
        _locators_[name].locator_handle.grab().initialize(config_);
        _xcalo_locator_ = dynamic_cast<const xcalo_locator*>(&_locators_[name].locator_handle.get());
      }
      {
        // Add a new entry for geiger locator
        const std::string name = "gveto";
        locator_entry_type tmp_entry;
        tmp_entry.label = name;
        _locators_[name] = tmp_entry;
        _locators_[name].locator_handle.reset(new gveto_locator(get_geo_manager(), module_number));
        _locators_[name].locator_handle.grab().initialize(config_);
        _gveto_locator_ = dynamic_cast<const gveto_locator*>(&_locators_[name].locator_handle.get());
      }

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace geometry

} // end of snemo
