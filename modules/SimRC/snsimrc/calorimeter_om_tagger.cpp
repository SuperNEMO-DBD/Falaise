// falaise/snemo/simulation/calorimeter_om_tagger.cc

// Ourselves:
#include "calorimeter_om_tagger.hpp"

namespace snemo {

  namespace simulation {

    calorimeter_om_tagger::calorimeter_om_tagger()
    {
      return;
    }

    calorimeter_om_tagger::~calorimeter_om_tagger()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void calorimeter_om_tagger::set_geometry_manager(const geomtools::manager & geomgr_)
    {
      _geomgr_ = &geomgr_;
      return;
    }

    void calorimeter_om_tagger::set_calorimeter_om_status_service(const snemo::calorimeter_om_status_service & calorimeter_om_status_service_)
    {
      DT_THROW_IF(not calorimeter_om_status_service_.is_initialized(),
                  std::logic_error,
                  "Calorimeter OM status service is not initialized!");
      _calorimeter_om_status_service_ = &calorimeter_om_status_service_;
      return;
    }

    bool calorimeter_om_tagger::is_initialized() const
    {
      return _initialized_;
    }
  
    void calorimeter_om_tagger::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Calorimeter OM tagger is already initialized!");    
      if (config_.has_flag("debug")) {
        _logging_ = datatools::logger::PRIO_DEBUG;
      } else if (config_.has_flag("trace")) {
        _logging_ = datatools::logger::PRIO_TRACE;
      }
      DT_LOG_TRACE(_logging_, "Initializing calorimeter OM tagger...");
      DT_THROW_IF(_geomgr_ == nullptr, std::logic_error,
                  "No geometry manager!");
      DT_THROW_IF(_calorimeter_om_status_service_ == nullptr, std::logic_error,
                  "No calorimeter OM status service!");

      // List of calo hits to be processed:
      if (config_.has_flag("tag_calo_hits")) {
        _hitTypes_.insert("calo");
      }
      if (config_.has_flag("tag_xcalo_hits")) {
        _hitTypes_.insert("xcalo");
      }
      if (config_.has_flag("tag_gveto_hits")) {
        _hitTypes_.insert("gveto");
      }
      
      if (_hitTypes_.size() == 0) {
        // Default :
        _hitTypes_.insert("calo");
        _hitTypes_.insert("xcalo");
        _hitTypes_.insert("gveto");
      }
      const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
      _caloOmType_  = idMgr.categories_by_name().find("calorimeter_optical_module")->second.get_type();
      _xcaloOmType_ = idMgr.categories_by_name().find("xcalo_optical_module")->second.get_type();
      _gvetoOmType_ = idMgr.categories_by_name().find("gveto_optical_module")->second.get_type();
      
      _initialized_ = true;
      return;
    }
      
    void calorimeter_om_tagger::reset()
    {
      DT_THROW_IF(not is_initialized(), std::logic_error,
                  "Calorimeter OM tagger is not initialized!");
      _initialized_ = false;    
      return;
    }

    void calorimeter_om_tagger::process(snemo::datamodel::event_header & eh_,
                                        mctools::simulated_data & sd_) const
    {
      DT_LOG_TRACE(_logging_, "Processing MC calorimeter OMs from SD bank...");
      const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
 
      if (eh_.has_mc_timestamp()) {
        DT_LOG_TRACE(_logging_, "Found MC timestamp in EH.");
        const auto & eventTimeStamp = eh_.get_mc_timestamp();
        for (auto hitType : _hitTypes_) {
          uint32_t omType = geomtools::geom_id::INVALID_TYPE;
          if (hitType == "calo") {
            omType = _caloOmType_;
          } else if (hitType == "xcalo") {
            omType = _xcaloOmType_;
          } else if (hitType == "gveto") {
            omType = _gvetoOmType_;
          }
          if (sd_.has_step_hits(hitType)) {
            auto & mcHits = sd_.grab_step_hits(hitType);
            for (auto & mcHit : mcHits) {
              const geomtools::geom_id & blockGeomID = mcHit->get_geom_id();
              DT_LOG_TRACE(_logging_, "Calorimeter block geom ID = " << blockGeomID);
              geomtools::geom_id omGeomID(omType, geomtools::geom_id::INVALID_ADDRESS);
              idMgr.extract(blockGeomID, omGeomID); 
              DT_LOG_TRACE(_logging_, "Calorimeter OM geom ID = " << omGeomID);
              std::uint32_t omStatus =
                _calorimeter_om_status_service_->get_om_status(omGeomID, eventTimeStamp);
              DT_LOG_TRACE(_logging_, "Calorimeter OM RC status = " << omStatus);
              if (omStatus != snemo::rc::calorimeter_om_status::OM_GOOD) { 
                mcHit->grab_auxiliaries().store("snemo.rc.calorimeter_om_status", (std::int32_t) omStatus);
              }
            }       
          }
        }
      }
      
      return;
    }

  }  // end of namespace simulation

}  // end of namespace snemo

