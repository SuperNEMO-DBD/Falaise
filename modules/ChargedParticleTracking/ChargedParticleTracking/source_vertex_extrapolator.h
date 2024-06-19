/// \file falaise/snemo/reconstruction/vertex_extrapolation_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-06-12
 * Last modified : 2024-06-12
 * Copyright (C) 2024 François Mauger <mauger@lpccaen.in2p3.fr>
 *   
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_SOURCE_VERTEX_EXTRAPOLATOR_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_SOURCE_VERTEX_EXTRAPOLATOR_H

// Standard library:
#include <memory>
#include <vector>

// Third party:
// - Bayeux:
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/geomtools/geom_id.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/geomtools/box.h"

// Falaise:
#include "falaise/snemo/geometry/locator_plugin.h"
#include "falaise/snemo/datamodels/base_trajectory_pattern.h"
#include "falaise/snemo/geometry/utils.h" // for snemo::geometry::vertex_info_list

namespace snemo {

  namespace reconstruction {

    /// \brief Source vertex extrapolator
    class source_vertex_extrapolator
    {
    public:

      struct config_type
      {
        datatools::logger::priority verbosity = datatools::logger::PRIO_WARNING;
        std::string locator_plugin_name = "";
        double intercept_tolerance = 1.0 * CLHEP::mm;               //!< Tolerance for intersection
        double finder_step = 2.0 * CLHEP::cm;                       //!< Step length of the finder algorithms
        uint32_t module_id = 0;                                     //!< Force module ID
        double max_source_extrapolation_horizontal_length = 30.0 * CLHEP::cm; //!< Maximum length of the extrapolation on source element in XY plane
        double max_source_extrapolation_vertical_length = 30.0 * CLHEP::cm; //!< Maximum length of the extrapolation on source element in vertical direction
        double effective_source_horizontal_tolerance = 25.0 * CLHEP::mm; //!< Effective tolerance on the edges of the source volumes (horizontal direction)
        double effective_source_vertical_tolerance   = 30.0 * CLHEP::mm;  //!< Effective tolerance on the edges of the source volumes (vertical direction)
        double calibration_source_extend_horizontal = 10.0 * CLHEP::mm;
        double calibration_source_extend_vertical   = 20.0 * CLHEP::mm;
				double cone_angle = 5.0 * CLHEP::degree;
				uint32_t cone_nrays = 6;
				uint32_t cone_nlayers = 1;
				double duplicate_tolerance = 1.0 * CLHEP::mm;
				bool debug_no_duplicate_detect = false;
 				bool debug_no_post_processing = false;
    }; 
      
      source_vertex_extrapolator(const geomtools::manager & gm_,
                                 const config_type & config_);

      const config_type & config() const;
        
      /// Compute vertexes on source elements from a trajectory
      void trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                       const snemo::datamodel::base_trajectory_pattern & traj_,
                                       int from_) const;


    private:

			struct intercept_input
			{
				int from = 0;
				bool cone_axis = false;
				bool reference_plane = false;
				geomtools::vector_3d traj_end_pos;
				geomtools::vector_3d traj_end_dir;
			};

			struct vertex_working_data
			{
				geomtools::geom_id gid;
				uint32_t n = 0;
				double xbar = 0.0;
				double ybar = 0.0;
				double zbar = 0.0;
				double x2bar = 0.0;
				double y2bar = 0.0;
				double z2bar = 0.0;
				uint32_t nbEdges = 0;
				snemo::geometry::vertex_info vtx;
			};
        
      void _trajectory_source_intercept_(snemo::geometry::vertex_info_list & vertexes_,
																				 const intercept_input & input_) const;

      void _initialize_();
      
    private:

      // Copnfiguration:
      config_type _config_;

      // Services:
      const geomtools::manager * _geoManager_ = nullptr;              //!< The SuperNEMO geometry manager
      const snemo::geometry::locator_plugin * _geoLocator_ = nullptr; //!< The SuperNEMO locator plugin
      const geomtools::mapping * _geoMapping_ = nullptr; //!< The SuperNEMO geometry mapping
      const geomtools::id_mgr * _geoIdMgr_ = nullptr; //!< The SuperNEMO geometry ID manager
      
      // Registered source submodule/strip block assembly/strips/pads/pad bulks/calibration carriers:

      // Source submodule:
      uint32_t _sourceSubmoduleType_ = geomtools::geom_id::INVALID_TYPE; //!< Source submodule type (1100 for GID)
      geomtools::geom_id _sourceSubmoduleGid_; //!< Source submodule GID

      // Source strip block assembly:
      uint32_t _sourceStripBlockAssemblyType_ = geomtools::geom_id::INVALID_TYPE; //!< Source submodule type (1115 for GID)
      geomtools::geom_id _sourceStripBlockAssemblyGid_; //!< Source strip block assembly GID
      std::unique_ptr<geomtools::box> _sourceStripBlockAssemblyBoxPtr_;
      std::unique_ptr<geomtools::box> _sourcePlaneBoxPtr_;
      const geomtools::placement * _sourceStripBlockAssemblyPlacement_ = nullptr;
      
      bool _use_deformed_source_strips_ = false;

      // Source strips:
      uint32_t _sourceStripType_ = geomtools::geom_id::INVALID_TYPE; // (1102)
      std::vector<geomtools::geom_id> _sourceStripGids_;
      int32_t _sourceStripMinId_ =  100000; 
      int32_t _sourceStripMaxId_ = -100000;
      
      uint32_t _sourcePadType_ = geomtools::geom_id::INVALID_TYPE; // (1130)
      std::vector<geomtools::geom_id> _sourcePadGids_;

      uint32_t _sourcePadBulkType_ = geomtools::geom_id::INVALID_TYPE; // (1131)
      std::vector<geomtools::geom_id> _sourcePadBulkGids_;
      
      // Source strip gaps:
      uint32_t _sourceStripGapType_ = geomtools::geom_id::INVALID_TYPE; // (1113)
      std::vector<geomtools::geom_id> _sourceStripGapGids_;
      double _sourceStripEdgeGapX_ = datatools::invalid_real();
      double _sourceStripEdgeGapZ_ = datatools::invalid_real();
      std::unique_ptr<geomtools::box> _sourceStripEdgeGapBoxPtr_;
      double _sourceStripGapX_ = datatools::invalid_real();
      double _sourceStripGapZ_ = datatools::invalid_real();
      std::unique_ptr<geomtools::box> _sourceStripGapBoxPtr_;

      // Source calibration tracks:
      uint32_t _sourceCalibTrackType_ = geomtools::geom_id::INVALID_TYPE; // (1103)
      int32_t _sourceCalibTrackMinId_ =  100000; 
      int32_t _sourceCalibTrackMaxId_ = -100000;
      std::vector<geomtools::geom_id> _sourceCalibTrackGids_;
      double _sourceCalibTrackX_ = datatools::invalid_real();
      double _sourceCalibTrackZ_ = datatools::invalid_real();
      double _sourceCalibTrackHeight_ = datatools::invalid_real();
      std::unique_ptr<geomtools::box> _sourceCalibTrackBoxPtr_;
      std::vector<geomtools::geom_id> _sourceCalibrationTrackGids_;
      
      // Source calibration spots:
      uint32_t _sourceCalibrationSpotType_ = geomtools::geom_id::INVALID_TYPE; // (1112)
      std::vector<geomtools::geom_id> _sourceCalibrationSpotGids_;

      // Source calibration carrier:
      uint32_t _sourceCalibrationCarrierType_ = geomtools::geom_id::INVALID_TYPE; // (1110)
      std::vector<geomtools::geom_id> _sourceCalibrationCarrierGids_;
      const geomtools::box * _sourceCalibrationCarrierBoxPtr_ = nullptr;
      std::unique_ptr<geomtools::box> _sourceCalibrationCarrierEffectiveBoxPtr_;

    };

  } // end of namespace reconstruction

} // end of namespace snemo

#endif // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_SOURCE_VERTEX_EXTRAPOLATOR_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
