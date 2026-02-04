// falaise/snemo/simulation/tracker_cell_tagger.cc

// Ourselves:
#include "tracker_cell_tagger.hpp"

// Falaise:
#include <falaise/snemo/datamodels/geomid_utils.h>

namespace snemo {

  namespace simulation {

    tracker_cell_tagger::tracker_cell_tagger()
    {
      return;
    }

    tracker_cell_tagger::~tracker_cell_tagger()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void tracker_cell_tagger::set_geometry_manager(const geomtools::manager & geomgr_)
    {
      _geomgr_ = &geomgr_;
      return;
    }

    void tracker_cell_tagger::set_tracker_cell_status_service(const snemo::tracker_cell_status_service & tracker_cell_status_service_)
    {
      DT_THROW_IF(not tracker_cell_status_service_.is_initialized(),
                  std::logic_error,
                  "Tracker cell service is not initialized!");
      _tracker_cell_status_service_ = &tracker_cell_status_service_;
      return;
    }

    bool tracker_cell_tagger::is_initialized() const
    {
      return _initialized_;
    }
  
    void tracker_cell_tagger::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Tracker cell status tagger is already initialized!");    
      if (config_.has_flag("debug")) {
        _logging_ = datatools::logger::PRIO_DEBUG;
      } else if (config_.has_flag("trace")) {
        _logging_ = datatools::logger::PRIO_TRACE;
      }
      DT_LOG_DEBUG(_logging_, "Initializing tracker cell tagger...");
      DT_THROW_IF(_geomgr_ == nullptr, std::logic_error,
                  "No geometry manager!");
      DT_THROW_IF(_tracker_cell_status_service_ == nullptr, std::logic_error,
                  "No tracker cell status service!");
      const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
      _ggCellType_ = idMgr.categories_by_name().find("drift_cell")->second.get_type();
      _initialized_ = true;
      return;
    }
      
    void tracker_cell_tagger::reset()
    {
      DT_THROW_IF(not is_initialized(), std::logic_error,
                  "Tracker cell tagger is not initialized!");
      _initialized_ = false;    
      return;
    }

    void tracker_cell_tagger::process(snemo::datamodel::event_header & eh_,
                                      mctools::simulated_data & sd_) const
    {
      DT_LOG_TRACE(_logging_, "Processing MC tracker cells from SD bank...");
      const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
      if (eh_.has_mc_timestamp()) {
        DT_LOG_DEBUG(_logging_, "Found MC timestamp in EH.");
        const auto & eventTimeStamp = eh_.get_mc_timestamp();
        // std::int32_t eventRunId = eh_.get_mc_run_id();  
        static const std::string ggType{"gg"};
        if (sd_.has_step_hits(ggType)) {
          auto & mcHits = sd_.grab_step_hits(ggType);
          for (auto & trackerMcHit : mcHits) {
            const geomtools::geom_id & hitGeomID = trackerMcHit->get_geom_id();
	    DT_LOG_DEBUG(_logging_, "hit geom ID = " << hitGeomID);
	    geomtools::geom_id ggCellID;
	    ggCellID.set_type(_ggCellType_);
	    ggCellID.set_depth(4); 
	    DT_LOG_DEBUG(_logging_, "ggCellID = " << ggCellID);
	    idMgr.extract(hitGeomID, ggCellID); 
	    DT_LOG_DEBUG(_logging_, "Hit in tracker cell " << ggCellID);
	    std::uint32_t cellStatus =
              _tracker_cell_status_service_->get_cell_status(ggCellID, eventTimeStamp);
 	    // DT_LOG_DEBUG(_logging_, "==> Found status {"
	    // 		 << snemo::rc::tracker_cell_status::status_to_string(cellStatus) << "} for cell " << ggCellID
	    // 		 << " {#" << snemo::datamodel::gg_num(ggCellID) << "} @ " << eventTimeStamp);	      
            if (cellStatus !=  snemo::rc::tracker_cell_status::CELL_GOOD) {
	      DT_LOG_DEBUG(_logging_, ">>>>> non GOOD status = "
			   << snemo::rc::tracker_cell_status::status_to_string(cellStatus) << "} for cell " << ggCellID
			   << " {#" << snemo::datamodel::gg_num(ggCellID) << "} @ " << eventTimeStamp);
	      // DT_THROW(std::logic_error, ">>>>> non GOOD status = "
	      // 	       << snemo::rc::tracker_cell_status::status_to_string(cellStatus) << "} for cell " << ggCellID
	      // 	       << " {#" << snemo::datamodel::gg_num(ggCellID) << "} @ " << eventTimeStamp);
              trackerMcHit->grab_auxiliaries().store("snemo.rc.tracker_cell_status", (std::int32_t) cellStatus);
            }
          }       
        }
      }
      return;
    }

  }  // end of namespace simulation

}  // end of namespace snemo

