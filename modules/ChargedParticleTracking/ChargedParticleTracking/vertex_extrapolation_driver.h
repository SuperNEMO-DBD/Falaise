/// \file falaise/snemo/reconstruction/vertex_extrapolation_driver.h
/* Author(s)     : Sophie Blondel <blondel@lal.in2p3.fr>
 *                 Xavier Garrido <garrido@lal.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-11-13
 * Last modified : 2024-06-11
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
 * Copyright (C) 2021-2024 François Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A driver class that extrapolates track vertices on source foil and
 *   calorimeter walls.
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux:
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/bit_mask.h"
#include "bayeux/geomtools/face_intercept_info.h"
#include "bayeux/geomtools/box.h"

// This project
#include "falaise/property_set.h"
#include "falaise/snemo/datamodels/particle_track.h"
#include "falaise/snemo/datamodels/base_trajectory_pattern.h"
#include "falaise/snemo/datamodels/line_trajectory_pattern.h"
#include "falaise/snemo/datamodels/helix_trajectory_pattern.h"
#include "falaise/snemo/geometry/utils.h"

// Ourselves:
#include <ChargedParticleTracking/source_vertex_extrapolator.h>

namespace geomtools {
  class manager;
}
namespace datatools {
  class properties;
}

namespace sngeom = snemo::geometry;

namespace snemo {

  namespace datamodel {
    class tracker_trajectory;
  }

  namespace geometry {
    class locator_plugin;
  }
  
  namespace reconstruction {

    /// \brief Vertex extrapolation driver
    class vertex_extrapolation_driver {
    public:

      static const int CALO_MAIN  = 0;
      static const int CALO_XCALO = 1;

      /// Return driver id
      static const std::string & get_id();

      /// Default constructor
      vertex_extrapolation_driver() = default;

      /// Initialize the driver through configuration properties
      vertex_extrapolation_driver(const falaise::property_set & ps,
                                  const geomtools::manager * gm);

      // Destructor
      ~vertex_extrapolation_driver() = default;

      vertex_extrapolation_driver(const vertex_extrapolation_driver&) = default;
      vertex_extrapolation_driver& operator=(const vertex_extrapolation_driver&) = default;
      vertex_extrapolation_driver(vertex_extrapolation_driver&&) = default;
      vertex_extrapolation_driver& operator=(vertex_extrapolation_driver&&) = default;

      /// Main driver method
      void process(const snemo::datamodel::tracker_trajectory& trajectory_,
                   snemo::datamodel::particle_track& particle_);
  
      /// Compute vertex on source element from a linear trajectory (3D-line/3D-polyline)
      void linear_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
																							const snemo::datamodel::base_trajectory_pattern & linear_traj_,
																							uint32_t from_mask_ = snemo::geometry::vertex_info::FROM_FIRST_BIT | snemo::geometry::vertex_info::FROM_LAST_BIT) const;

      /// Compute vertex on some calo blocks from a linear trajectory (3D-line/3D-polyline)
      void linear_trajectory_calo_intercept(snemo::geometry::vertex_info_list & vertexes_,
																						const snemo::datamodel::base_trajectory_pattern & linear_traj_,
																						uint32_t track_side_,
																						uint32_t from_mask_ = snemo::geometry::vertex_info::FROM_FIRST_BIT | snemo::geometry::vertex_info::FROM_LAST_BIT) const;
 
      /// Compute vertex on source from a helix trajectory (3D-helix)
      void helix_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                             const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                             uint32_t from_mask_ = snemo::geometry::vertex_info::FROM_FIRST_BIT | snemo::geometry::vertex_info::FROM_LAST_BIT) const;
 
      /// Compute vertex on some calo blocks from a helix trajectory (3D-helix)
      void helix_trajectory_calo_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                           const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                           uint32_t track_side_,
                                           uint32_t from_mask_ = snemo::geometry::vertex_info::FROM_FIRST_BIT | snemo::geometry::vertex_info::FROM_LAST_BIT) const;
  
      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

    private:
      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & geoManager() const;

      /// Check reliability of vertices extrapolation given Geiger cells
      void _check_vertices_(const snemo::datamodel::tracker_trajectory & trajectory_,
                            std::map<snemo::geometry::vertex_info::category_type, bool> & use_vertices_);

      /// Measure vertices on the calorimeter walls and source foil
      void _measure_vertices_(const snemo::datamodel::tracker_trajectory & trajectory_,
                              snemo::datamodel::VertexHdlCollection & vertices_);

      void _post_process_source_vertex_(snemo::geometry::vertex_info_list & src_vertexes_) const;

      void _post_process_calo_vertex_(snemo::geometry::vertex_info_list & calo_vertexes_) const;
      
    private:
      
      // Configuration:
      datatools::logger::priority logPriority_ = datatools::logger::PRIO_WARNING;  //!< Logging priority
      double _intercept_tolerance_ = 1.0 * CLHEP::mm;                //!< Tolerance for intersection
      double _finder_step_ =  2.0 * CLHEP::cm;                       //!< Step length of the finder algorithms
      uint32_t _module_id_ = 0;                                      //!< Force module ID
      double _max_calo_extrapolation_xy_length_ = 15.0 * CLHEP::cm;  //!< Maximum length of the extrapolation on calo block in XY plane 
      double _max_source_extrapolation_xy_length_ = 20.0 * CLHEP::cm; //!< Maximum length of the extrapolation on source element in XY plane
      double _max_source_extrapolation_z_length_ = 30.0 * CLHEP::cm; //!< Maximum length of the extrapolation on source element in Z direction
      bool _use_linear_extrapolation_ = true; //<! Activation flag of linear extrapolation for curved trajectory (helix)
      bool _use_helix_extrapolation_ = true; //<! Activation flag of helix extrapolation for helix trajectory
			double _effectiveCaloBlockHorizontalTolerance_ = 25. * CLHEP::mm; //!< Effective tolerance on the edges of the calo block window (horizontal direction)
			double _effectiveCaloBlockVerticalTolerance_   = 30. * CLHEP::mm; //!< Effective tolerance on the edges of the calo block window (vertical direction)
			double _effectiveSourceHorizontalTolerance_    = 25. * CLHEP::mm; //!< Effective tolerance on the edges of the source volumes (horizontal direction)
			double _effectiveSourceVerticalTolerance_      = 30. * CLHEP::mm; //!< Effective tolerance on the edges of the source volumes (vertical direction)

      // Services:
      const geomtools::manager * geoManager_ = nullptr;              //!< The SuperNEMO geometry manager
      const snemo::geometry::locator_plugin * geoLocator_ = nullptr; //!< The SuperNEMO locator plugin

      // Registered source submodule/strip block assembly/strips/pads/pad bulks/calibration carriers:
      uint32_t _sourceSubmoduleType_ = geomtools::geom_id::INVALID_TYPE; //!< Source submodule type (1100 for GID)
      geomtools::geom_id _sourceSubmoduleGid_; //!< Source submodule GID
			uint32_t _sourceStripBlockAssemblyType_ = geomtools::geom_id::INVALID_TYPE; //!< Source submodule type (1115 for GID)
      geomtools::geom_id _sourceStripBlockAssemblyGid_; //!< Source strip block assembly GID
			bool _use_deformed_source_strips_ = false;
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
			double _calibration_source_extend_horizontal_ = 0.0 * CLHEP::mm;
			double _calibration_source_extend_vertical_   = 0.0 * CLHEP::mm;
      std::vector<geomtools::geom_id> _sourceCalibrationTrackGids_;
			std::unique_ptr<geomtools::box> _sourceCalibrationCarrierEffectiveBoxPtr_;

			// Source calibration spots:
      uint32_t _sourceCalibrationSpotType_ = geomtools::geom_id::INVALID_TYPE; // (1112)
      std::vector<geomtools::geom_id> _sourceCalibrationSpotGids_;

			// Source calibration carrier:
       uint32_t _sourceCalibrationCarrierType_ = geomtools::geom_id::INVALID_TYPE; // (1110)
      std::vector<geomtools::geom_id> _sourceCalibrationCarrierGids_;

      // Registered calorimeter blocks:
      uint32_t _caloSubmoduleType_ = geomtools::geom_id::INVALID_TYPE; //!< Calorimeter submodule type (for GID)
      uint32_t _caloWallType_ = geomtools::geom_id::INVALID_TYPE; //!< Calorimeter submodule type (for GID)
      uint32_t _caloBlockType_ = geomtools::geom_id::INVALID_TYPE;
      std::vector<geomtools::geom_id> _caloBlockGids_;
      uint32_t _xcaloBlockType_ = geomtools::geom_id::INVALID_TYPE;
      std::vector<geomtools::geom_id> _xcaloBlockGids_;
      uint32_t _gvetoBlockType_ = geomtools::geom_id::INVALID_TYPE;
      std::vector<geomtools::geom_id> _gvetoBlockGids_;
      std::unique_ptr<geomtools::box> _effectiveCaloBlockBoxPtr_;
      std::unique_ptr<geomtools::box> _effectiveCaloBlockBoxPtr2_;
      std::unique_ptr<geomtools::box> _effectiveXcaloBlockBoxPtr_;
      std::unique_ptr<geomtools::box> _effectiveXcaloBlockBoxPtr2_;

      // Specific dimensions about the positioning of main calo blocks (world frame)
      double _main_calo_y_first_ = datatools::invalid_real();
      double _main_calo_z_first_ = datatools::invalid_real();
      double _main_calo_y_step_ = datatools::invalid_real();
      double _main_calo_z_step_ = datatools::invalid_real();

      // Dynamic
      std::map<snemo::geometry::vertex_info::category_type, bool> _use_vertices_; //!< Vertices reliability

			std::unique_ptr<source_vertex_extrapolator> _srcVtxExtrapolator_;

    };

  } // end of namespace reconstruction

} // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::vertex_extrapolation_driver)

#endif // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
