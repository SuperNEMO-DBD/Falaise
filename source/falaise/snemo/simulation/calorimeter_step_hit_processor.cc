/** \file falaise/snemo/simulation/calorimeter_step_hit_processor.cc */

// Ourselves:
#include <falaise/snemo/simulation/calorimeter_step_hit_processor.h>

namespace snemo {

  namespace simulation {

    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT (calorimeter_step_hit_processor,
                                                       "snemo::simulation::calorimeter_step_hit_processor");

    bool calorimeter_step_hit_processor::locate_calorimeter_block(const geomtools::vector_3d & position_,
                                                                  geomtools::geom_id & gid_) const
    {
      if (_calo_locator_.find_block_geom_id(position_, gid_))
        {
          DT_LOG_DEBUG(get_logging_priority(), "Find step within main wall calorimeter");
          return true;
        }
      if (_xcalo_locator_.find_block_geom_id(position_, gid_))
        {
          DT_LOG_DEBUG(get_logging_priority(), "Find step within X-wall calorimeter");
          return true;
        }
      if (_gveto_locator_.find_block_geom_id(position_, gid_))
        {
          DT_LOG_DEBUG(get_logging_priority(), "Find step within gamma veto calorimeter");
          return true;
        }

      return this->mctools::calorimeter_step_hit_processor::locate_calorimeter_block(position_, gid_);
    }

    void calorimeter_step_hit_processor::initialize(const ::datatools::properties & config_,
                                                    ::datatools::service_manager & service_mgr_)
    {
      this->mctools::calorimeter_step_hit_processor::initialize(config_, service_mgr_);

      // 2014-05-21 XG : Use demonstrator module i.e. module number 0
      const uint32_t module_number = 0;
      DT_LOG_WARNING (get_logging_priority(), "Use default module number " << module_number);

      _calo_locator_.set_module_number(module_number);
      _calo_locator_.set_geo_manager(*_geom_manager);
      _calo_locator_.initialize();
      _xcalo_locator_.set_module_number(module_number);
      _xcalo_locator_.set_geo_manager(*_geom_manager);
      _xcalo_locator_.initialize();
      _gveto_locator_.set_module_number(module_number);
      _gveto_locator_.set_geo_manager(*_geom_manager);
      _gveto_locator_.initialize();

      return;
    }

  } // end of namespace simulation

} // end of namespace snemo
