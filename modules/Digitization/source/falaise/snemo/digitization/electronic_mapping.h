// snemo/digitization/electronic_mapping.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H

// Standard library :
#include <set>

// - Boost:
#include <boost/cstdint.hpp>
#include <boost/bimap.hpp>

// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>
// - Bayeux/datatools :
#include <bayeux/datatools/logger.h>

// This project
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/ID_convertor.h>


namespace snemo {
  
  namespace digitization {

    class electronic_mapping 
    {

			typedef boost::bimap< geomtools::geom_id, geomtools::geom_id > ID_bimap;
			typedef ID_bimap::value_type ID_doublet;
			
    public :

			/// Set of supported types
			static const std::set<int32_t> & supported_types();

      /// Default constructor
      electronic_mapping();

      /// Destructor
      virtual ~electronic_mapping();
			
			/// Precontruct the map for a given type
			void add_preconstructed_type(int type_);

			/// Set the geometry manager
			void set_geo_manager(const geomtools::manager & mgr_);

			/// Check if the geometry manager is set
			bool geo_manager_is_set() const;	

			/// Set the module number
			void set_module_number(int module_number_);

			/// Check if the module number is set
			bool module_number_is_set() const;

      /// Initializing
      void initialize();

      /// Check if the object is initialized 
      bool is_initialized() const;

      /// Reset the object : 
      void reset();

			/// Convert geometric ID into electronic ID
			void convert_GID_to_EID(const bool tracker_trigger_mode_, const geomtools::geom_id & geom_id_, geomtools::geom_id & electronic_id_) const;

			/// Convert electronic ID into geometric ID
			void convert_EID_to_GID(const bool tracker_trigger_mode_, const geomtools::geom_id & electronic_id_, geomtools::geom_id & geom_id_) const;

		protected : 

			/// Initializing
			void _initialize();

			/// Construct the bimap for geiger category type
			void _init_geiger();

			/// Construct the bimap for main calorimeters category type
			void _init_mcalo();

			/// Construct the bimap for x-wall calorimeters category type
			void _init_x_wall();

			/// Construct the bimap for gamma-veto calorimeters category type
			void _init_gveto();
			
			/// Protected convert geometric ID into electronic ID
			void _convert_GID_to_EID(const bool tracker_trigger_mode_, const geomtools::geom_id & geom_id_, geomtools::geom_id & electronic_id_);

			/// Protected convert electronic ID into geometric ID
			void _convert_EID_to_GID(const bool tracker_trigger_mode_, const geomtools::geom_id & electronic_id_, geomtools::geom_id & geom_id_);
			
    private :
      
			// Management:
      bool _initialized_; //!< Initialization flag
			bool _geo_manager_status_; //!< Geo manager flag
			bool _module_number_status_; //!< Module number flag

			// External resources:
      const geomtools::manager * _geo_manager_; //!< Eternal geom manager

			// Configuration parameters:
			datatools::logger::priority _logging_;
			std::set<int32_t> _pre_constructed_types_; //!< Set of pre constructed types ("geiger", "mcalo", "x_wall", "gveto" are supported)
      int _module_number_; //!< Module number (demonstrator module number for the moment)

			// Working internal data/resources:
			ID_convertor _ID_convertor_; //!< ID convertor (GID to EID)
			ID_bimap _geiger_id_bimap_;  //!< Bidirectionnal map between geiger cells geom ID and corresponding electronic channel ID
			ID_bimap _mcalo_id_bimap_;   //!< Bidirectionnal map between main calorimeter geom ID and corresponding electronic channel ID
			ID_bimap _xcalo_id_bimap_;   //!< Bidirectionnal map between x-wall calorimeter geom ID and corresponding electronic channel ID
			ID_bimap _gveto_id_bimap_;   //!< Bidirectionnal map between gamma-veto geom ID and corresponding electronic channel ID
    };
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
