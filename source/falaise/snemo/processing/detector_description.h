/// \brief Detector description utility
#ifndef FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H
#define FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H

// - Bayeux :
#include <datatools/bit_mask.h>
#include <datatools/properties.h>
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
#include <falaise/property_set.h>

namespace snemo {

  namespace processing {

    /// \brief Pseudo service with various embedded utilities that describe the detector
    class detector_description
    {

    public:
      
      enum service_requirement {
	required_geometry = datatools::bit_mask::bit00,
	required_calo_locator = datatools::bit_mask::bit01,
	required_xcalo_locator = datatools::bit_mask::bit02,
	required_gveto_locator = datatools::bit_mask::bit03,
	required_gg_locator = datatools::bit_mask::bit04,
	required_locators =			\
	/**/ required_calo_locator |		\
	/**/ required_xcalo_locator |		\
	/**/ required_gveto_locator |		\
	/**/ required_gg_locator,			
	required_db = datatools::bit_mask::bit05,
	required_om_status = datatools::bit_mask::bit06,
	required_cell_status = datatools::bit_mask::bit07,
	required_all =					\
	/**/ required_geometry |			\
	/**/ required_locators |			\
	/**/ required_om_status |			\
	/**/ required_cell_status
      };
      
      
      bool has_geometry_manager() const;

      void set_geometry_manager(const geomtools::manager & geo_mgr_,
				const uint32_t flags_ = required_locators);
      
      const geomtools::manager & get_geometry_manager() const;
      
      bool has_calo_locator() const;
  
      bool has_xcalo_locator() const;

      bool has_gveto_locator() const;
  
      bool has_gg_locator() const;
      
      bool has_db_service() const;

      bool has_om_status_service() const;

      bool has_cell_status_service() const;
        
      const snemo::geometry::calo_locator & get_calo_locator() const;
      
      const snemo::geometry::xcalo_locator & get_xcalo_locator() const;

      const snemo::geometry::gveto_locator & get_gveto_locator() const;
 
      const snemo::geometry::gg_locator & get_gg_locator() const;
      
      void set_db_service(const snemo::db_service & dbs_);
      
      const snemo::db_service & get_db_service() const;
      
      void set_cell_status_service(const snemo::tracker_cell_status_service & css_);
      
      const snemo::tracker_cell_status_service & get_cell_status_service() const;
      
      void set_om_status_service(const snemo::calorimeter_om_status_service & oss_);
      
      const snemo::calorimeter_om_status_service & get_om_status_service() const;
 
      static  uint32_t configuration_flags_from(const datatools::properties & config_);
 
      static detector_description
      make_from_services(datatools::service_manager & services_,
			 const uint32_t flags_ = required_all);
     
      void print(std::ostream & out_, const std::string & indent_) const;
     
    private:
      
      const geomtools::manager * _geometry_manager_ = nullptr;
      const snemo::geometry::calo_locator * _calo_locator_ = nullptr; ///< main wall calorimeter locator
      const snemo::geometry::xcalo_locator * _xcalo_locator_ = nullptr; ///< X-wall calorimeter locator
      const snemo::geometry::gveto_locator * _gveto_locator_ = nullptr; ///< G-veto calorimeter locator
      const snemo::geometry::gg_locator * _gg_locator_ = nullptr; ///< Geiger cell locator
      const snemo::db_service * _db_service_ = nullptr;
      const snemo::tracker_cell_status_service * _cell_status_service_ = nullptr;
      const snemo::calorimeter_om_status_service * _om_status_service_ = nullptr;

    };

  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_DETECTOR_DESCRIPTION_H
