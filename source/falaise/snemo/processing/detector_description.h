
#ifndef FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H
#define FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H

// - Bayeux :
#include <geomtools/manager.h>

// - Falaise :
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/services/calorimeter_om_status_service.h>

namespace snemo {

  namespace processing {

    /// \brief Pseudo service with various embedded utilities that describe the detector
    class detector_description
    {

    public:
      
      bool has_geometry_manager() const
      {
        return _geometry_manager_ != nullptr;
      }

      void set_geometry_manager(const geomtools::manager & geo_mgr_)
      {
        _geometry_manager_ = &geo_mgr_;
        const snemo::geometry::locator_plugin & locators
          = _geometry_manager_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
        if (locators.hasCaloLocator()) {
          _calo_locator_ = &locators.caloLocator();
        }
        if (locators.hasXCaloLocator()) {
          _xcalo_locator_ = &locators.xcaloLocator();
        }
        if (locators.hasGVetoLocator()) {
          _gveto_locator_ = &locators.gvetoLocator();
        }
        if (locators.hasGeigerLocator()) {
          _gg_locator_ = &locators.geigerLocator();
        }
        return;
      }

      bool has_calo_locator() const
      {
        return _calo_locator_ != nullptr;
      }
  
      bool has_xcalo_locator() const
      {
        return _xcalo_locator_ != nullptr;
      }
  
      bool has_gveto_locator() const
      {
        return _gveto_locator_ != nullptr;
      }
  
      bool has_gg_locator() const
      {
        return _gg_locator_ != nullptr;
      }
 
      bool has_db_service() const
      {
        return _db_service_ != nullptr;
      }

      bool has_om_status_service() const
      {
        return _om_status_service_ != nullptr;
      }

      bool has_cell_status_service() const
      {
        return _cell_status_service_ != nullptr;
      }
       
      const geomtools::manager & get_geometry_manager() const
      {
        return *_geometry_manager_;
      }
        
      const snemo::geometry::calo_locator & get_calo_locator() const 
      {
        return *_calo_locator_;
      }

      const snemo::geometry::xcalo_locator & get_xcalo_locator() const 
      {
        return *_xcalo_locator_;
      }

      const snemo::geometry::gveto_locator & get_gveto_locator() const
      {
        return *_gveto_locator_;
      }

      const snemo::geometry::gg_locator & get_gg_locator() const
      {
        return *_gg_locator_;
      }
      
      void set_db_service(const snemo::db_service & dbs_)
      {
        _db_service_ = &dbs_;
      }
 
      const snemo::db_service & get_db_service() const
      {
        return *_db_service_;
      }
     
      void set_cell_status_service(const snemo::tracker_cell_status_service & css_)
      {
        _cell_status_service_ = &css_;
      }
 
      const snemo::tracker_cell_status_service & get_cell_status_service() const
      {
        return *_cell_status_service_;
      }
      
      void set_om_status_service(const snemo::calorimeter_om_status_service & oss_)
      {
        _om_status_service_ = &oss_;
      }

      const snemo::calorimeter_om_status_service & get_om_status_service() const
      {
        return *_om_status_service_;
      }
    
    private:
      
      const geomtools::manager * _geometry_manager_ = nullptr;
      const snemo::geometry::calo_locator * _calo_locator_ = nullptr; ///< main wall calorimeter locator
      const snemo::geometry::xcalo_locator * _xcalo_locator_ = nullptr; ///< X-wall calorimeter locator
      const snemo::geometry::gveto_locator * _gveto_locator_ = nullptr;
      const snemo::geometry::gg_locator * _gg_locator_ = nullptr;
      const snemo::db_service * _db_service_ = nullptr;
      const snemo::tracker_cell_status_service * _cell_status_service_ = nullptr;
      const snemo::calorimeter_om_status_service * _om_status_service_ = nullptr;

    };

  }  // end of namespace processing

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H
