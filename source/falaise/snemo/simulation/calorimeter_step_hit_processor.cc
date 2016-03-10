// falaise/snemo/simulation/calorimeter_step_hit_processor.cc

// Ourselves:
#include <falaise/snemo/simulation/calorimeter_step_hit_processor.h>

// This project:
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace simulation {

    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(calorimeter_step_hit_processor,
                                                      "snemo::simulation::calorimeter_step_hit_processor")

    bool calorimeter_step_hit_processor::locate_calorimeter_block(const geomtools::vector_3d & position_,
                                                                  geomtools::geom_id & gid_) const
    {
      if (_locator_plugin_->get_calo_locator().find_block_geom_id(position_, gid_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find step within main wall calorimeter");
        return true;
      }
      if (_locator_plugin_->get_xcalo_locator().find_block_geom_id(position_, gid_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find step within X-wall calorimeter");
        return true;
      }
      if (_locator_plugin_->get_gveto_locator().find_block_geom_id(position_, gid_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find step within gamma veto calorimeter");
        return true;
      }
      // Fallback locator from the parent class:
      return this->mctools::calorimeter_step_hit_processor::locate_calorimeter_block(position_, gid_);
    }

    void calorimeter_step_hit_processor::initialize(const ::datatools::properties & config_,
                                                    ::datatools::service_manager & service_mgr_)
    {
      this->mctools::calorimeter_step_hit_processor::initialize(config_, service_mgr_);

      // Get geometry locator plugin
      const geomtools::manager & geo_mgr = get_geom_manager();
      std::string locator_plugin_name;
      if (config_.has_key("locator_plugin_name")) {
        locator_plugin_name = config_.fetch_string("locator_plugin_name");
      } else {
        // If no locator plugin name is set, then search for the first one available
        // from the geometry manager:
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
            DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      DT_THROW_IF (! geo_mgr.has_plugin(locator_plugin_name) ||
                   ! geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name),
                   std::logic_error,
                   "Found no locator plugin named '" << locator_plugin_name << "'");
      _locator_plugin_ = &geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);

      return;
    }

  } // end of namespace simulation

} // end of namespace snemo
