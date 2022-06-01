//! \file   event_timestamper.hpp
//! \brief   Event timestamping
//! \details Process a things object

#ifndef FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP
#define FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP

// Third party:
// - Bayeux :
#include <bayeux/datatools/logger.h>
#include <bayeux/geomtools/manager.h>

// Falaise:
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/rc/run_list.h>
#include <falaise/snemo/physics_model/activity_model_factory.h>
#include <falaise/snemo/rc/run_statistics.h>
#include <falaise/snemo/rc/mc_event_distribution.h>

namespace snemo {

  namespace simulation {

    class event_timestamper
    {
    public:

      event_timestamper();

      bool is_initialized() const;
      
      void initialize(const datatools::properties & config_);
      
      void reset();
 
      void process(snemo::datamodel::event_header & eh_) const;

      
    private:

      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      
      std::unique_ptr<snemo::rc::run_list> _runList_;
      snemo::physics_model::activity_model_factory _actModelFactory_;
      snemo::physics_model::activity_model_ptr_type _mcActivityModel_;
      std::unique_ptr<snemo::rc::run_statistics> _mcRunStatistics_;
      std::unique_ptr<snemo::rc::mc_event_distribution> _mcEventDistribution_;
      
    };
       
  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SIMRC_PLUGIN_EVENT_TIMESTAMPER_HPP
