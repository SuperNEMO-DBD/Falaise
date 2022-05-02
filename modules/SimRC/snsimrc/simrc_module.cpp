// falaise/snemo/simulation/simrc_module.cc

// Ourselves:
#include "simrc_module.hpp"

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// - Falaise:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/property_set.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/services/geometry.h>

namespace snemo {

  namespace simulation {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(simrc_module,
                                      "snemo::simulation::simrc_module")


    // Constructor :
    simrc_module::simrc_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    simrc_module::~simrc_module()
    {
      if (is_initialized()) {
        simrc_module::reset();
      }
      return;
    }

    void simrc_module::_set_defaults()
    {
      _geoManager_ = nullptr;
      _SDTag_.clear();
      return;
    }

    const geomtools::manager & simrc_module::get_geometry_manager() const
    {
      return *_geoManager_;
    }

    void simrc_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geoManager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geoManager_->get_setup_label();
      DT_THROW_IF(setup_label != "snemo::demonstrator",
                  std::logic_error, "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void simrc_module::initialize(const datatools::properties & config_,
                                  datatools::service_manager & services_,
                                  dpp::module_handle_dict_type & /* unused */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(config_);

      falaise::property_set ps{config_};
      _SDTag_ = ps.get<std::string>("SD_label", snedm::labels::simulated_data());

      snemo::service_handle<snemo::geometry_svc> geoSVC{services_};
      set_geometry_manager(*(geoSVC.operator->()));
      _caloTypes_ = {"calo", "xcalo", "gveto"};
      _set_initialized(true);
      return;
    }

    void simrc_module::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the dead OM tagger :
      // if (_deadOmTagger_) {
      //        if (deadOmTagger_->is_initialized()) {
      //          _deadOmTagger_->reset();
      //        }
      //        _deadOmTagger_.reset();
      // }
      // Reset the dead cell tagger :
      // if (_deadCellTagger_) {
      //        if (_deadCellTagger_->is_initialized()) {
      //          _deadCellTagger_->reset();
      //        }
      //        _deadCellTagger_.reset();
      // }
      _set_defaults();
      return;
    }

    dpp::base_module::process_status simrc_module::process(datatools::things & event_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
       
      // Check tracker clustering data
      if (!event_.has(_SDTag_)) {
        DT_THROW_IF(true, std::logic_error, "Missing simulated data to be processed !");
      }
      mctools::simulated_data & sd = event_.grab<mctools::simulated_data>(_SDTag_);
      
      // Main processing method :
      _process_impl(sd);
      
      return dpp::base_module::PROCESS_SUCCESS;
    }

    void simrc_module::_process_impl(mctools::simulated_data & sd_)
    {
      // Loop over all 'calorimeter hit' categories :
      for (const std::string & caloType : _caloTypes_) {
        if (!sd_.has_step_hits(caloType)) {
          continue;
        }
        auto & mcHits = sd_.grab_step_hits(caloType);
        for (auto & caloMcHit : mcHits) {
          // const geomtools::geom_id & geomID = caloMcHit->get_geom_id();
          if (true) { 
            caloMcHit->grab_auxiliaries().store_flag("snemo.rc.off");
          }
        }
      }
      
      // 'tracker hit' category :
      static const std::string ggType{"gg"};
      if (sd_.has_step_hits(ggType)) {
        auto & mcHits = sd_.grab_step_hits(ggType);
        for (auto & trackerMcHit : mcHits) {
          // const geomtools::geom_id & geomID = trackerMcHit->get_geom_id();
          if (true) { 
            trackerMcHit->grab_auxiliaries().store_flag("snemo.rc.off");
          }
        }       
      }
      
      return;
    }

  }  // end of namespace simulation

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::simulation::simrc_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::simulation::simrc_module, ocd_) {
  ocd_.set_class_name("snemo::simulation::simrc_s2c_module");
  ocd_.set_class_description(
      "A module that applies real running conditions to simulated data");
  ocd_.set_class_library("Falaise_SimRC");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
                               "Example: \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::simulation::simrc_module,
                               "snemo::simulation::simrc_module")

// end
    
