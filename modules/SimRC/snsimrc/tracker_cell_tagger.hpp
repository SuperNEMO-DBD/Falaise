//! \file   tracker_cell_tagger.hpp
//! \brief   tracker cell tagger
//! \details Process a things object

#ifndef FALAISE_SIMRC_PLUGIN_TRACKER_CELL_TAGGER_HPP
#define FALAISE_SIMRC_PLUGIN_TRACKER_CELL_TAGGER_HPP

// Third party:
// - Bayeux :
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/geomtools/manager.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/datatools/logger.h>

// Falaise:
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/datamodels/event_header.h>
 
namespace snemo {

  namespace simulation {

    /// \brief Driver for tracker cell status tagging
    class tracker_cell_tagger
    {
    public:
      
      tracker_cell_tagger();
     
      ~tracker_cell_tagger();

      void set_geometry_manager(const geomtools::manager & geomgr_);

      void set_tracker_cell_status_service(const snemo::tracker_cell_status_service & tracker_cell_status_service_);

      bool is_initialized() const;
      
      void initialize(const datatools::properties & config_);
      
      void reset();

      void process(snemo::datamodel::event_header  & eh_,
                   mctools::simulated_data & sd_) const;
      
    private:

      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      const geomtools::manager * _geomgr_ = nullptr;
      const snemo::tracker_cell_status_service * _tracker_cell_status_service_ = nullptr;
      
    };
    
  }  // end of namespace simulation

}  // end of namespace snemo

#endif  // FALAISE_SIMRC_PLUGIN_TRACKER_CELL_TAGGER_HPP
