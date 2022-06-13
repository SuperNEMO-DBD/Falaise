//! \file   calorimeter_om_tagger.hpp
//! \brief   tracker cell tagger
//! \details Process a things object

#ifndef FALAISE_SIMRC_PLUGIN_CALORIMETER_OM_TAGGER_HPP
#define FALAISE_SIMRC_PLUGIN_CALORIMETER_OM_TAGGER_HPP

// Standard library:
#include <set>

// Third party:
// - Bayeux :
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/geomtools/manager.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/datatools/logger.h>

// Falaise:
#include <falaise/snemo/services/calorimeter_om_status_service.h>
#include <falaise/snemo/datamodels/event_header.h>
 
namespace snemo {

  namespace simulation {

    /// \brief Driver for tracker cell status tagging
    class calorimeter_om_tagger
    {
    public:
      
      calorimeter_om_tagger();
     
      ~calorimeter_om_tagger();

      void set_geometry_manager(const geomtools::manager & geomgr_);

      void set_calorimeter_om_status_service(const snemo::calorimeter_om_status_service & calorimeter_om_status_service_);

      bool is_initialized() const;
      
      void initialize(const datatools::properties & config_);
      
      void reset();

      void process(snemo::datamodel::event_header  & eh_,
                   mctools::simulated_data & sd_) const;
      
    private:

      bool _initialized_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      const geomtools::manager * _geomgr_ = nullptr;
      const snemo::calorimeter_om_status_service * _calorimeter_om_status_service_ = nullptr;
      std::set<std::string> _hitTypes_;
      int32_t _caloOmType_;
      int32_t _xcaloOmType_;
      int32_t _gvetoOmType_;
    };
    
  }  // end of namespace simulation

}  // end of namespace snemo

#endif  // FALAISE_SIMRC_PLUGIN_CALORIMETER_OM_TAGGER_HPP
