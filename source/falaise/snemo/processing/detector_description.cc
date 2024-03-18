/// \file snemo/processing/detector_description.cc

// Ourselves:
#include <falaise/snemo/processing/detector_description.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/geometry/config.h>

namespace snemo {

  namespace processing {
      
    bool detector_description::has_geometry_manager() const
    {
      return _geometry_manager_ != nullptr;
    }

    void detector_description::set_geometry_manager(const geomtools::manager & geo_mgr_,
						    const uint32_t flags_)
    {
      const std::string & geo_setup_label = geo_mgr_.get_setup_label();
      DT_THROW_IF(geo_setup_label != "snemo::demonstrator",
                  std::logic_error, "Invalid geometry setup label '" << geo_setup_label << "' !");
      _geometry_manager_ = &geo_mgr_;
      
      const snemo::geometry::locator_plugin & locators
	= _geometry_manager_->get_plugin<snemo::geometry::locator_plugin>(snemo::geometry::default_locators_driver_name());

      if (flags_ & required_calo_locator) {
	DT_THROW_IF(not locators.hasCaloLocator(), std::logic_error, "No required calo locator!");
	_calo_locator_ = &locators.caloLocator();
      }
      if (flags_ & required_xcalo_locator) {
	DT_THROW_IF(not locators.hasXCaloLocator(), std::logic_error, "No required xcalo locator!");
	_xcalo_locator_ = &locators.xcaloLocator();
      }
      if (flags_ & required_gveto_locator) {
	DT_THROW_IF(not locators.hasGVetoLocator(), std::logic_error, "No required gveto locator!");
	_gveto_locator_ = &locators.gvetoLocator();
      }
      if (flags_ & required_gg_locator) {
	DT_THROW_IF(not locators.hasGeigerLocator(), std::logic_error, "No required gg locator!");
	_gg_locator_ = &locators.geigerLocator();
      }
      return;
    }
          
    const geomtools::manager & detector_description::get_geometry_manager() const
    {
      return *_geometry_manager_;
    }
        
   
    bool detector_description::has_calo_locator() const
    {
      return _calo_locator_ != nullptr;
    }
  
    bool detector_description::has_xcalo_locator() const
    {
      return _xcalo_locator_ != nullptr;
    }
  
    bool detector_description::has_gveto_locator() const
    {
      return _gveto_locator_ != nullptr;
    }
  
    bool detector_description::has_gg_locator() const
    {
      return _gg_locator_ != nullptr;
    }
 
    bool detector_description::has_db_service() const
    {
      return _db_service_ != nullptr;
    }

    bool detector_description::has_om_status_service() const
    {
      return _om_status_service_ != nullptr;
    }

    bool detector_description::has_cell_status_service() const
    {
      return _cell_status_service_ != nullptr;
    }
       
    const snemo::geometry::calo_locator & detector_description::get_calo_locator() const 
    {
      return *_calo_locator_;
    }

    const snemo::geometry::xcalo_locator & detector_description::get_xcalo_locator() const 
    {
      return *_xcalo_locator_;
    }

    const snemo::geometry::gveto_locator & detector_description::get_gveto_locator() const
    {
      return *_gveto_locator_;
    }

    const snemo::geometry::gg_locator & detector_description::get_gg_locator() const
    {
      return *_gg_locator_;
    }
      
    void detector_description::set_db_service(const snemo::db_service & dbs_)
    {
      _db_service_ = &dbs_;
    }
 
    const snemo::db_service & detector_description::get_db_service() const
    {
      return *_db_service_;
    }
     
    void detector_description::set_cell_status_service(const snemo::tracker_cell_status_service & css_)
    {
      _cell_status_service_ = &css_;
    }
 
    const snemo::tracker_cell_status_service & detector_description::get_cell_status_service() const
    {
      return *_cell_status_service_;
    }
      
    void detector_description::set_om_status_service(const snemo::calorimeter_om_status_service & oss_)
    {
      _om_status_service_ = &oss_;
    }

    const snemo::calorimeter_om_status_service & detector_description::get_om_status_service() const
    {
      return *_om_status_service_;
    }
 
    // static
    uint32_t detector_description::configuration_flags_from(const datatools::properties & config_)
    {
      uint32_t detector_description_flags = 0;
      falaise::property_set localPS{config_};
      auto ddRequirements = localPS.get<falaise::property_set>("services", {});
      if (ddRequirements.get<bool>("geometry", false)) {
	detector_description_flags |= detector_description::required_geometry;
	if (ddRequirements.get<bool>("calo_locator", false)) {
	  detector_description_flags |= detector_description::required_calo_locator;
	}
	if (ddRequirements.get<bool>("xcalo_locator", false)) {
	  detector_description_flags |= detector_description::required_xcalo_locator;
	}
	if (ddRequirements.get<bool>("gveto_locator", false)) {
	  detector_description_flags |= detector_description::required_gveto_locator;
	}
 	if (ddRequirements.get<bool>("gg_locator", false)) {
	  detector_description_flags |= detector_description::required_gg_locator;
	}
      }
      if (ddRequirements.get<bool>("db", false)) {
 	detector_description_flags |= detector_description::required_db;
      }
      if (ddRequirements.get<bool>("om_status", false)) {
 	detector_description_flags |= detector_description::required_om_status;
      }
      if (ddRequirements.get<bool>("cell_status", false)) {
 	detector_description_flags |= detector_description::required_cell_status;
      }
      return detector_description_flags;
    }
 
    // static
    detector_description detector_description::make_from_services(datatools::service_manager & services_,
								  const uint32_t flags_)
    {
      detector_description d;

      if (flags_ & required_geometry) {
	snemo::service_handle<snemo::geometry_svc> geoSvc{services_};
	d.set_geometry_manager(*geoSvc.instance(), flags_);
      }
       
      if (flags_ & required_db) {
	// DB service :
	snemo::service_handle<snemo::db_service> dbSvc{services_};
	d.set_db_service(*dbSvc.instance());
      }
     
      if (flags_ & required_cell_status) {
	// Tracker cell status service :
	snemo::service_handle<tracker_cell_status_service> cellStatusSvc{services_};
	d.set_cell_status_service(*cellStatusSvc.instance());
	//d.set_cell_status_service(*(cellStatusSvc.operator->()));
      }

      if (flags_ & required_om_status) {
	// Calorimeter OM status service :
	snemo::service_handle<calorimeter_om_status_service> omStatusSvc{services_};
	d.set_om_status_service(*omStatusSvc.instance());
      }
      
      return d;
    }
 
  } // namespace processing

} // end of namespace snemo
