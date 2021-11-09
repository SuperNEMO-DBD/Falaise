/// \file falaise/snemo/reconstruction/vertex_extrapolation_driver.cc

// Ourselves:
#include <ChargedParticleTracking/vertex_extrapolation_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/i_shape_3d.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/helix_intercept.h>

namespace snemo {

  namespace reconstruction {

    const std::string &vertex_extrapolation_driver::get_id() {
      static const std::string s("VED");
      return s;
    }

    const geomtools::manager &vertex_extrapolation_driver::geoManager() const {
      DT_THROW_IF(geoManager_ == nullptr, std::logic_error, "No geometry manager is setup !");
      return *geoManager_;
    }

    /// Initialize the driver through configuration properties
    vertex_extrapolation_driver::vertex_extrapolation_driver(const falaise::property_set &ps,
                                                             const geomtools::manager *gm) {
      logPriority_ =
        datatools::logger::get_priority(ps.get<std::string>("logging.priority", "fatal"));
      geoManager_ = gm;
      auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
      geoLocator_ = snemo::geometry::getSNemoLocator(geoManager(), locator_plugin_name);
      const geomtools::mapping & geoMapping = geoManager().get_mapping();
      const geomtools::id_mgr & geoIdMgr = geoManager().get_id_mgr();
      if (datatools::logger::is_debug(logPriority_)) {
        geoIdMgr.tree_dump(std::cerr, "Geometry ID manager: ", "[debug] ");
        geoIdMgr.print_list_of_categories("print", {}, std::cerr);
      }

      _intercept_tolerance_
        = ps.get<falaise::length_t>("intercept.tolerance", {1.0, "mm"})();
      _finder_step_
        = ps.get<falaise::length_t>("finder.step", {2.0, "cm"})();
      _max_calo_extrapolation_xy_length_
        = ps.get<falaise::length_t>("max_calo_extrapolation.xy_length", {15.0, "cm"})();
      _max_source_extrapolation_xy_length_
        = ps.get<falaise::length_t>("max_source_extrapolation.xy_length",{30.0, "cm"})();
  
      // Identify source strips:
      _sourceStripType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_strip")) {
        _sourceStripType_ = geoIdMgr.get_category_info("source_strip").get_type();
        geomtools::geom_id sourceStripGidPattern(_sourceStripType_,
                                                 _module_id_,
                                                 geomtools::geom_id::ANY_ADDRESS);
        geoMapping.compute_matching_geom_id(sourceStripGidPattern, _sourceStripGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Found " << _sourceStripGids_.size() << " source strips");
  
      // Identify source pads:
      _sourcePadType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_pad")) {
        _sourcePadType_ = geoIdMgr.get_category_info("source_pad").get_type();
        geomtools::geom_id sourcePadGidPattern(_sourcePadType_,
                                               _module_id_,
                                               geomtools::geom_id::ANY_ADDRESS,
                                               geomtools::geom_id::ANY_ADDRESS);
        geoMapping.compute_matching_geom_id(sourcePadGidPattern, _sourcePadGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Found " << _sourcePadGids_.size() << " source pads");

      // Identify source pad bulks (deformed pads from SNRS):
      _sourcePadBulkType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_pad_bulk")) {
        // Only detected for the SNRS deformed source strips:
        _sourcePadBulkType_ = geoIdMgr.get_category_info("source_pad_bulk").get_type();
        geomtools::geom_id sourcePadBulkGidPattern(_sourcePadBulkType_,
                                                   _module_id_,
                                                   geomtools::geom_id::ANY_ADDRESS,
                                                   geomtools::geom_id::ANY_ADDRESS);
        geoMapping.compute_matching_geom_id(sourcePadBulkGidPattern, _sourcePadBulkGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Found " << _sourcePadBulkGids_.size() << " source pad bulks");

      if (_sourcePadBulkGids_.size()) {
        _use_deformed_source_strips_ = true;
        DT_LOG_DEBUG(logPriority_, "Using deformed source strips.");
      }

      // Identify calibration sources:
      _sourceCalibrationSpotType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_calibration_spot")) {
        _sourceCalibrationSpotType_ = geoIdMgr.get_category_info("source_calibration_spot").get_type();
        geomtools::geom_id sourceCalibrarionSpotGidPattern(_sourceCalibrationSpotType_,
                                                           _module_id_,
                                                           geomtools::geom_id::ANY_ADDRESS,  // track
                                                           geomtools::geom_id::ANY_ADDRESS); // position
        geoMapping.compute_matching_geom_id(sourceCalibrarionSpotGidPattern, _sourceCalibrationSpotGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Source calibration spot type = " << _sourceCalibrationSpotType_);
      DT_LOG_DEBUG(logPriority_, "Found " << _sourceCalibrationSpotGids_.size() << " calibration source spots");
  
      // Identify main calo blocks:
      _caloBlockType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("calorimeter_block")) {
        _caloBlockType_ = geoIdMgr.get_category_info("calorimeter_block").get_type();
        geomtools::geom_id caloBlockGidPattern(_caloBlockType_,
                                               _module_id_,
                                               geomtools::geom_id::ANY_ADDRESS,  // side
                                               geomtools::geom_id::ANY_ADDRESS,  // column
                                               geomtools::geom_id::ANY_ADDRESS,  // row
                                               1); // part (front part of the scintillator block)
        geoMapping.compute_matching_geom_id(caloBlockGidPattern, _caloBlockGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Calorimeter block type = " << _caloBlockType_);
      DT_LOG_DEBUG(logPriority_, "Found " << _caloBlockGids_.size() << " main calo blocks (front)");
   
      // Identify xcalo blocks:
      _xcaloBlockType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("xcalo_block")) {
        _xcaloBlockType_ = geoIdMgr.get_category_info("xcalo_block").get_type();
        geomtools::geom_id xcaloBlockGidPattern(_xcaloBlockType_,
                                                _module_id_,
                                                geomtools::geom_id::ANY_ADDRESS,  // side
                                                geomtools::geom_id::ANY_ADDRESS,  // wall
                                                geomtools::geom_id::ANY_ADDRESS,  // column
                                                geomtools::geom_id::ANY_ADDRESS); // row
        geoMapping.compute_matching_geom_id(xcaloBlockGidPattern, _xcaloBlockGids_);
      }
      DT_LOG_DEBUG(logPriority_, "X-calorimeter block type = " << _xcaloBlockType_);
      DT_LOG_DEBUG(logPriority_, "Found " << _xcaloBlockGids_.size() << " X-calo blocks");
 
      // Identify gveto blocks:
      _gvetoBlockType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("gveto_block")) {
        _gvetoBlockType_ = geoIdMgr.get_category_info("gveto_block").get_type();
        geomtools::geom_id gvetoBlockGidPattern(_gvetoBlockType_,
                                                _module_id_,
                                                geomtools::geom_id::ANY_ADDRESS,  // side
                                                geomtools::geom_id::ANY_ADDRESS,  // wall
                                                geomtools::geom_id::ANY_ADDRESS); // column
        geoMapping.compute_matching_geom_id(gvetoBlockGidPattern, _gvetoBlockGids_);
      }
      DT_LOG_DEBUG(logPriority_, "G-veto block type = " << _gvetoBlockType_);
      DT_LOG_DEBUG(logPriority_, "Found " << _gvetoBlockGids_.size() << " G-veto blocks");
  
      return;
    }

    void vertex_extrapolation_driver::process(const snemo::datamodel::tracker_trajectory &trajectory_,
                                              snemo::datamodel::particle_track &particle_) {
      this->_measure_vertices_(trajectory_, particle_.get_vertices());
    }

    void vertex_extrapolation_driver::helix_trajectory_calo_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                      const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                                                      uint32_t track_side_,
                                                                      uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logPrio, "\nSearch helix intercepts on calorimeter blocks: ");
      vertexes_.clear();
      const geomtools::helix_3d & helix = helix_traj_.get_helix();
      static const int CALO_MAIN  = 0;
      static const int CALO_XCALO = 1;
      std::set<int> blockTypes;
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_MAIN_CALORIMETER)->second) {
        blockTypes.insert(CALO_MAIN);
      }
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_X_CALORIMETER)->second) {
        blockTypes.insert(CALO_XCALO);
      }
      snemo::geometry::vertex_info_list caloVertexes[2];
      using snemo::geometry::vertex_info;
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        DT_LOG_DEBUG(logPrio, "   from " << vertex_info::from_to_label(iFrom));
        vertex_info::from_bit_type from_bit = vertex_info::FROM_FIRST_BIT;
        if (iFrom == vertex_info::FROM_LAST) from_bit = vertex_info::FROM_LAST_BIT;
        for (int iBlockType : blockTypes) {
          const std::vector<geomtools::geom_id> * blockGidsPtr = nullptr;
          snemo::datamodel::particle_track::vertex_type vtxType = snemo::datamodel::particle_track::VERTEX_NONE;
          if (iBlockType == CALO_MAIN) {
            blockGidsPtr = &_caloBlockGids_;  // Main walls
            vtxType = snemo::datamodel::particle_track::VERTEX_ON_MAIN_CALORIMETER;
            DT_LOG_DEBUG(logPrio, "   to main calo blocks");
          } else if (iBlockType == CALO_XCALO) {
            blockGidsPtr = &_xcaloBlockGids_; // X-calo walls
            vtxType = snemo::datamodel::particle_track::VERTEX_ON_X_CALORIMETER;
            DT_LOG_DEBUG(logPrio, "   to X-calo blocks");
          }    
          for (uint32_t iBlock = 0; iBlock < blockGidsPtr->size(); iBlock++) {
            const geomtools::geom_id & blockGid = (*blockGidsPtr)[iBlock];
            if (blockGid.get(0) != _module_id_ or blockGid.get(1) != track_side_ ) {
              /// Reject blocks in other modules and sides:
              continue;
            }
            DT_LOG_DEBUG(logPrio, "  Searching helix intercept on block #" << blockGid << " from " << vertex_info::from_to_label(iFrom) << " ...");
            const geomtools::geom_info  & blockGinfo  = geoManager().get_mapping().get_geom_info(blockGid);
            const geomtools::logical_volume & blockLog   = blockGinfo.get_logical();
            const geomtools::i_shape_3d & blockShape     = blockLog.get_shape();
            const geomtools::placement  & blockPlacement = blockGinfo.get_world_placement();
            snemo::geometry::helix_intercept hBlockIntercept(helix,
                                                             blockShape,
                                                             blockPlacement,
                                                             _finder_step_,
                                                             _intercept_tolerance_,
                                                             logPrio);
            DT_LOG_DEBUG(logPrio, "    Block shape : '" << blockShape.get_shape_name() << "'");
            snemo::geometry::helix_intercept::extrapolation_info blockEi;
            bool success = hBlockIntercept.find_intercept(blockEi, from_bit);
            if (success) {
              DT_LOG_DEBUG(logPrio, "    Found helix intercept on calo block #" << blockGid
                           << " at distance = " << blockEi.extrapolated_length / CLHEP::cm << " cm");
              vertex_info blockVtxInfo;
              blockVtxInfo.type = vtxType;
              blockVtxInfo.from = iFrom;
              blockVtxInfo.gid = blockGid;
              blockVtxInfo.face_intercept = blockEi.fii;
              blockVtxInfo.distance = blockEi.extrapolated_length;
              blockVtxInfo.distance_xy = blockEi.extrapolated_xy_length;
              blockVtxInfo.tolerance = _intercept_tolerance_;
              if (datatools::logger::is_debug(logPrio)) {
                blockVtxInfo.print(std::cerr, "[debug] ");
              }
              if (blockVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                caloVertexes[iFrom].push_back(blockVtxInfo);
              } else {
                DT_LOG_DEBUG(logPrio, "    Helix intercept is to far from the calo block");
              }
            } else {
              DT_LOG_DEBUG(logPrio, "    No helix intercept on calo block #" << blockGid);
            }
          }
        }  
        // Find the first intercept on a calorimeter block:
        int closestIndex = -1;
        double closestDist = datatools::invalid_real();
        for (unsigned int i = 0; i < caloVertexes[iFrom].size(); i++) {
          const vertex_info & caloVtxInfo = caloVertexes[iFrom][i];
          if (closestIndex == -1 or caloVtxInfo.distance < closestDist) {
            closestIndex = i;
            closestDist = caloVtxInfo.distance;
          } 
        }
        if (closestIndex >= 0) {
          vertexes_.push_back(caloVertexes[iFrom][closestIndex]);
        }
        caloVertexes[iFrom].clear();
      }
      return;
    }
  
    /* 
     *
     *                    first               last   
     *  FROM_LAST           +-------------------+===>[ ] 
     *                                        direction
     *
     *              first                      last   
     *  FROM_FIRST   [ ]<===+-------------------+
     *                direction
     */
   
    void vertex_extrapolation_driver::line_trajectory_calo_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                     const snemo::datamodel::line_trajectory_pattern & line_traj_,
                                                                     uint32_t track_side_,
                                                                     uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logPrio, "\nSearch line intercepts on calorimeter blocks: ");
      vertexes_.clear();
      const geomtools::line_3d & line = line_traj_.get_segment();
      static const int CALO_MAIN  = 0;
      static const int CALO_XCALO = 1;
      std::set<int> blockTypes;
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_MAIN_CALORIMETER)->second) {
        blockTypes.insert(CALO_MAIN);
      }
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_X_CALORIMETER)->second) {
        blockTypes.insert(CALO_XCALO);
      }
      snemo::geometry::vertex_info_list caloVertexes[2];
      using snemo::geometry::vertex_info;
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        // Default configured for 'last'
        geomtools::vector_3d beginPoint = line.get_first();
        geomtools::vector_3d endPoint   = line.get_last();
        if (iFrom == vertex_info::FROM_FIRST) {
          std::swap(beginPoint, endPoint);
        }
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        double distRef2End = (endPoint - refPoint).mag();
        const geomtools::vector_3d direction = (endPoint - beginPoint).unit();
        for (int iBlockType : blockTypes) {
          const std::vector<geomtools::geom_id> * blockGidsPtr = nullptr;
          snemo::datamodel::particle_track::vertex_type vtxType = snemo::datamodel::particle_track::VERTEX_NONE;
          if (iBlockType == CALO_MAIN) {
            blockGidsPtr = &_caloBlockGids_;  // Main walls
            vtxType = snemo::datamodel::particle_track::VERTEX_ON_MAIN_CALORIMETER;
          } else if (iBlockType == CALO_XCALO) {
            blockGidsPtr = &_xcaloBlockGids_; // X-calo walls
            vtxType = snemo::datamodel::particle_track::VERTEX_ON_X_CALORIMETER;
          }    
          for (uint32_t iBlock = 0; iBlock < blockGidsPtr->size(); iBlock++) {
            const geomtools::geom_id & blockGid = (*blockGidsPtr)[iBlock];
            if (blockGid.get(0) != _module_id_ or blockGid.get(1) != track_side_ ) {
              /// Reject blocks in other modules and sides:
              continue;
            }
            DT_LOG_DEBUG(logPrio, "Searching line intercept on block #" << blockGid << "...");
            const geomtools::geom_info  & blockGinfo  = geoManager().get_mapping().get_geom_info(blockGid);
            const geomtools::logical_volume & blockLog   = blockGinfo.get_logical();
            const geomtools::i_shape_3d & blockShape     = blockLog.get_shape();
            const geomtools::placement  & blockPlacement = blockGinfo.get_world_placement();
            geomtools::vector_3d blockRefPoint;
            blockPlacement.mother_to_child(refPoint, blockRefPoint);
            geomtools::vector_3d blockDirection;
            blockPlacement.mother_to_child_direction(direction, blockDirection);
            DT_LOG_DEBUG(logPrio, "    Ref point   : " << geomtools::to_xyz(blockRefPoint) );
            DT_LOG_DEBUG(logPrio, "    Direction   : " << geomtools::to_xyz(blockDirection) );
            DT_LOG_DEBUG(logPrio, "    Block shape : '" << blockShape.get_shape_name() << "'");
            geomtools::face_intercept_info blockFii;
            bool success = blockShape.find_intercept(blockRefPoint, blockDirection, blockFii, _intercept_tolerance_);
            if (success) {
              DT_LOG_DEBUG(logPrio, "    Found line intercept on calo block #" << blockGid);
              geomtools::vector_3d blockImpact = blockFii.get_impact();
              geomtools::vector_3d blockWorldImpact;
              blockPlacement.child_to_mother(blockImpact, blockWorldImpact);
              blockFii.set_impact(blockWorldImpact);
              double distRef2Impact = (blockWorldImpact - refPoint).mag();
              double extrapolationDist = distRef2Impact - distRef2End;
              vertex_info blockVtxInfo;
              blockVtxInfo.type = vtxType;
              blockVtxInfo.from = iFrom;
              blockVtxInfo.gid = blockGid;
              blockVtxInfo.face_intercept = blockFii;
              if (extrapolationDist > 0.0) {
                blockVtxInfo.distance = extrapolationDist;
                blockVtxInfo.distance_xy = (blockWorldImpact - refPoint).perp();
              } else {
                blockVtxInfo.distance = 0.0;
                blockVtxInfo.distance_xy = 0.0;
              }
              blockVtxInfo.tolerance = _intercept_tolerance_;
              if (datatools::logger::is_debug(logPrio)) {
                blockVtxInfo.print(std::cerr, "[debug] ");
              }
              if (blockVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                caloVertexes[iFrom].push_back(blockVtxInfo);
              } else {
                DT_LOG_DEBUG(logPrio, "    Helix intercept is to far from the calo block");
              }
            }
          }
        }  
        // Find the first intercept on a calorimeter block:
        int closestIndex = -1;
        double closestDist = datatools::invalid_real();
        for (unsigned int i = 0; i < caloVertexes[iFrom].size(); i++) {
          const vertex_info & caloVtxInfo = caloVertexes[iFrom][i];
          if (closestIndex == -1 or caloVtxInfo.distance < closestDist) {
            closestIndex = i;
            closestDist = caloVtxInfo.distance;
          } 
        }
        if (closestIndex >= 0) {
          vertexes_.push_back(caloVertexes[iFrom][closestIndex]);
        }
        caloVertexes[iFrom].clear();
      }
      return;
    }

  
    void vertex_extrapolation_driver::line_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                       const snemo::datamodel::line_trajectory_pattern & line_traj_,
                                                                       uint32_t from_mask_) const
    {
      DT_LOG_DEBUG(logPriority_, "Search line intercepts on source elements from mask = " << from_mask_);
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      vertexes_.clear();
      const geomtools::line_3d & line = line_traj_.get_segment();
      using snemo::geometry::vertex_info;
      bool use_foils = false;
      bool use_calib_src = false;
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL)->second) {
        use_foils = true;
      }
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_CALIBRATION_SOURCE)->second) {
        use_calib_src = true;
      }
      for (int iFrom = vertex_info::FROM_LAST; iFrom <= vertex_info::FROM_FIRST; iFrom++) {
        std::vector<vertex_info> srcVertexes;
        DT_LOG_DEBUG(logPrio, "From : " << iFrom);
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        geomtools::vector_3d beginPoint = line.get_first();
        geomtools::vector_3d endPoint   = line.get_last();
        if (iFrom == vertex_info::FROM_FIRST) {
          std::swap(beginPoint, endPoint);
        }
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        double distRef2End = (endPoint - refPoint).mag();
        const geomtools::vector_3d direction = (endPoint - beginPoint).unit();
        if (use_foils) {
          for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
            const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
            DT_LOG_DEBUG(logPrio, "  Source strip GID : " << sourceStripGid);
            uint32_t sourceStripId = sourceStripGid.get(1);
            if (sourceStripGid.get(0) != _module_id_) { 
              continue;
            }
            DT_LOG_DEBUG(logPrio, "  Searching line intercept on strip #" << sourceStripId);
            const geomtools::geom_info  & sourceStripGinfo     = geoManager().get_mapping().get_geom_info(sourceStripGid);
            const geomtools::logical_volume & sourceStripLog   = sourceStripGinfo.get_logical();
            const geomtools::i_shape_3d & sourceStripShape     = sourceStripLog.get_shape();
            const geomtools::placement  & sourceStripPlacement = sourceStripGinfo.get_world_placement();
            geomtools::vector_3d srcStripRefPoint;
            sourceStripPlacement.mother_to_child(refPoint, srcStripRefPoint);
            geomtools::vector_3d srcStripDirection;
            sourceStripPlacement.mother_to_child_direction(direction, srcStripDirection);
            DT_LOG_DEBUG(logPrio, "  Ref point   : " << geomtools::to_xyz(srcStripRefPoint) );
            DT_LOG_DEBUG(logPrio, "  Direction   : " << geomtools::to_xyz(srcStripDirection) );
            DT_LOG_DEBUG(logPrio, "  Source strip shape  : '" << sourceStripShape.get_shape_name() << "'");
            DT_LOG_DEBUG(logPrio, "  Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
            geomtools::face_intercept_info srcStripFii;
            bool success = sourceStripShape.find_intercept(srcStripRefPoint,
                                                           srcStripDirection,
                                                           srcStripFii,
                                                           _intercept_tolerance_);
            if (! success) {
              // No intersection with the strip volume: no chance to intersect a pad ! We give up.
              continue;
            }
            DT_LOG_DEBUG(logPrio, "  ===> Found line intercept on strip #" << sourceStripId);
            geomtools::vector_3d srcStripImpact = srcStripFii.get_impact();
            geomtools::vector_3d srcStripWorldImpact;
            sourceStripPlacement.child_to_mother(srcStripImpact, srcStripWorldImpact);
            srcStripFii.set_impact(srcStripWorldImpact);
            // Search intercept on daughter pad volumes:
            for (uint32_t j = 0; j < _sourcePadGids_.size(); j++) {
              const geomtools::geom_id & sourcePadGid = _sourcePadGids_[j];
              if (sourcePadGid.get(1) != sourceStripId) {
                /// Reject pads in other strips:
                continue;
              }
              uint32_t sourcePadId = sourcePadGid.get(2);
              DT_LOG_DEBUG(logPrio, "  ===> Searching line intercept on pad #" << sourcePadId << " in strip #" << sourceStripId);
              const geomtools::geom_info  & sourcePadGinfo     = geoManager().get_mapping().get_geom_info(sourcePadGid);
              const geomtools::logical_volume & sourcePadLog   = sourcePadGinfo.get_logical();
              const geomtools::i_shape_3d & sourcePadShape     = sourcePadLog.get_shape();
              const geomtools::placement  & sourcePadPlacement = sourcePadGinfo.get_world_placement();
              geomtools::vector_3d srcPadRefPoint;
              sourcePadPlacement.mother_to_child(refPoint, srcPadRefPoint);
              geomtools::vector_3d srcPadDirection;
              sourcePadPlacement.mother_to_child_direction(direction, srcPadDirection);
              DT_LOG_DEBUG(logPrio, "  Ref point        : " << geomtools::to_xyz(srcPadRefPoint) );
              DT_LOG_DEBUG(logPrio, "  Direction        : " << geomtools::to_xyz(srcPadDirection) );
              DT_LOG_DEBUG(logPrio, "  Source pad shape : '" << sourcePadShape.get_shape_name() << "'");
              geomtools::face_intercept_info srcPadFii;
              success = sourcePadShape.find_intercept(srcPadRefPoint,
                                                      srcPadDirection,
                                                      srcPadFii,
                                                      _intercept_tolerance_);
              if (! success) {
                DT_LOG_DEBUG(logPrio, "  ===> Give up strip #" << sourceStripId);
                continue;
              }
              geomtools::vector_3d srcPadImpact = srcPadFii.get_impact();
              geomtools::vector_3d srcPadWorldImpact;
              sourcePadPlacement.child_to_mother(srcPadImpact, srcPadWorldImpact);
              DT_LOG_DEBUG(logPrio, "  ===> Found line intercept on pad #" << sourcePadId << " at " << geomtools::to_xyz(srcPadWorldImpact));
              srcPadFii.set_impact(srcPadWorldImpact);
              double srcPadDistRef2Impact = (srcPadWorldImpact - refPoint).mag();
              double srcPadExtrapolationDist = srcPadDistRef2Impact - distRef2End;
              vertex_info padVtxInfo;
              padVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL;
              padVtxInfo.from = iFrom;
              padVtxInfo.gid = sourcePadGid;
              padVtxInfo.face_intercept = srcPadFii;
              if (srcPadExtrapolationDist > 0.0) {
                padVtxInfo.distance = srcPadDistRef2Impact;
                padVtxInfo.distance_xy = (srcPadWorldImpact - refPoint).perp();
              } else {
                padVtxInfo.distance = 0.0;
                padVtxInfo.distance_xy = 0.0;
              }
              padVtxInfo.tolerance = _intercept_tolerance_;
              // Search intercept on daughter pad bulk volumes:
              bool candidate_bulk = false;
              for (uint32_t k = 0; k < _sourcePadBulkGids_.size(); k++) {
                const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[k];
                if (sourcePadBulkGid.get(1) != sourceStripId or sourcePadBulkGid.get(2) != sourcePadId ) {
                  /// Reject pad bulks in other strips and pads:
                  continue;
                }
                candidate_bulk = true;
                DT_LOG_DEBUG(logPrio, "    Searching line intercept on pad bulk #" << sourcePadId << " in strip #" << sourceStripId);
                const geomtools::geom_info  & sourcePadBulkGinfo  = geoManager().get_mapping().get_geom_info(sourcePadBulkGid);
                const geomtools::logical_volume & sourcePadBulkLog   = sourcePadBulkGinfo.get_logical();
                const geomtools::i_shape_3d & sourcePadBulkShape     = sourcePadBulkLog.get_shape();
                const geomtools::placement  & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
                geomtools::vector_3d srcPadBulkRefPoint;
                sourcePadBulkPlacement.mother_to_child(refPoint, srcPadBulkRefPoint);
                geomtools::vector_3d srcPadBulkDirection;
                sourcePadBulkPlacement.mother_to_child_direction(direction, srcPadBulkDirection);
                DT_LOG_DEBUG(logPrio, "    Ref point : " << geomtools::to_xyz(srcPadBulkRefPoint) );
                DT_LOG_DEBUG(logPrio, "    Direction   : " << geomtools::to_xyz(srcPadBulkDirection) );
                DT_LOG_DEBUG(logPrio, "    Source pad bulk shape : '" << sourcePadBulkShape.get_shape_name() << "'");
                geomtools::face_intercept_info srcPadBulkFii;
                success = sourcePadBulkShape.find_intercept(srcPadBulkRefPoint,
                                                            srcPadBulkDirection,
                                                            srcPadBulkFii,
                                                            _intercept_tolerance_);
                if (! success) {
                  DT_LOG_DEBUG(logPrio, "  ===> Give up strip/pad = " << sourceStripId << '/' << sourcePadId);
                  continue;
                }
                geomtools::vector_3d srcPadBulkImpact = srcPadBulkFii.get_impact();
                geomtools::vector_3d srcPadBulkWorldImpact;
                sourcePadBulkPlacement.child_to_mother(srcPadBulkImpact, srcPadBulkWorldImpact);
                DT_LOG_DEBUG(logPrio, "  ===> Found line intercept on pad bulk #" << sourcePadId
                             << " at " << geomtools::to_xyz(srcPadBulkWorldImpact));
                srcPadBulkFii.set_impact(srcPadBulkWorldImpact);
                double srcPadBulkDistRef2Impact = (srcPadBulkWorldImpact - refPoint).mag();
                double srcPadBulkExtrapolationDist = srcPadBulkDistRef2Impact - distRef2End;
                vertex_info padBulkVtxInfo;
                padBulkVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL;
                padBulkVtxInfo.from = iFrom;
                padBulkVtxInfo.gid = sourcePadBulkGid;
                padBulkVtxInfo.face_intercept = srcPadBulkFii;
                if (srcPadBulkExtrapolationDist > 0.0) {
                  padBulkVtxInfo.distance = srcPadBulkDistRef2Impact;
                  padBulkVtxInfo.distance_xy = (srcPadBulkWorldImpact - refPoint).perp();
                } else {
                  padBulkVtxInfo.distance = 0.0;
                  padBulkVtxInfo.distance_xy = 0.0;
                }
                padBulkVtxInfo.tolerance = _intercept_tolerance_;
                if (padBulkVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  DT_LOG_DEBUG(logPrio, "  ======> add padBulkVtxInfo");
                  srcVertexes.push_back(padBulkVtxInfo);
                } else {
                  DT_LOG_DEBUG(logPrio, "    Line intercept is to far from the pad bulk");
                }
              }
              if (! candidate_bulk) { 
                // Validate pad intercept if no candidate pad bulk
                if (padVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  DT_LOG_DEBUG(logPrio, "  ======> add padVtxInfo");
                  srcVertexes.push_back(padVtxInfo);
                } else {
                  DT_LOG_DEBUG(logPrio, "    Line intercept is to far from the pad");
                }
              }
            }   
          }
        }

        if (use_calib_src) {
          // Calibration spot:
          for (uint32_t iSpot = 0; iSpot < _sourceCalibrationSpotGids_.size(); iSpot++) {
            const geomtools::geom_id & sourceCalibrationSpotGid = _sourceCalibrationSpotGids_[iSpot];
            DT_LOG_DEBUG(logPrio, "Searching line intercept on calibration source spot #" << sourceCalibrationSpotGid);
            const geomtools::geom_info  & sourceCalibrationSpotGinfo  = geoManager().get_mapping().get_geom_info(sourceCalibrationSpotGid);
            const geomtools::logical_volume & sourceCalibrationSpotLog   = sourceCalibrationSpotGinfo.get_logical();
            const geomtools::i_shape_3d & sourceCalibrationSpotShape     = sourceCalibrationSpotLog.get_shape();
            const geomtools::placement  & sourceCalibrationSpotPlacement = sourceCalibrationSpotGinfo.get_world_placement();
            geomtools::vector_3d srcCalibrationSpotRefPoint;
            sourceCalibrationSpotPlacement.mother_to_child(refPoint, srcCalibrationSpotRefPoint);
            geomtools::vector_3d srcCalibrationSpotDirection;
            sourceCalibrationSpotPlacement.mother_to_child_direction(direction, srcCalibrationSpotDirection);
            DT_LOG_DEBUG(logPrio, "    Ref point   : " << geomtools::to_xyz(srcCalibrationSpotRefPoint) );
            DT_LOG_DEBUG(logPrio, "    Direction   : " << geomtools::to_xyz(srcCalibrationSpotDirection) );
            DT_LOG_DEBUG(logPrio, "    Calibration spot shape : '" << sourceCalibrationSpotShape.get_shape_name() << "'");
            geomtools::face_intercept_info srcCalibrationSpotFii;
            bool success = sourceCalibrationSpotShape.find_intercept(srcCalibrationSpotRefPoint,
                                                                     srcCalibrationSpotDirection,
                                                                     srcCalibrationSpotFii,
                                                                     _intercept_tolerance_);
            if (! success) {
              DT_LOG_DEBUG(logPrio, "  ===> Give up calibration source");
              continue;
            }
            DT_LOG_DEBUG(logPrio, " ===> Found line intercept on calibration source spot #" << sourceCalibrationSpotGid);
            geomtools::vector_3d srcCalibrationSpotImpact = srcCalibrationSpotFii.get_impact();
            geomtools::vector_3d srcCalibrationSpotWorldImpact;
            sourceCalibrationSpotPlacement.child_to_mother(srcCalibrationSpotImpact, srcCalibrationSpotWorldImpact);
            srcCalibrationSpotFii.set_impact(srcCalibrationSpotWorldImpact);
            double srcCalibrationSpotDistRef2Impact = (srcCalibrationSpotWorldImpact - refPoint).mag();
            double srcCalibrationSpotExtrapolationDist = srcCalibrationSpotDistRef2Impact - distRef2End;
            vertex_info calibrationSpotVtxInfo;
            calibrationSpotVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_CALIBRATION_SOURCE;
            calibrationSpotVtxInfo.from = iFrom;
            calibrationSpotVtxInfo.gid = sourceCalibrationSpotGid;
            calibrationSpotVtxInfo.face_intercept = srcCalibrationSpotFii;
            if (srcCalibrationSpotExtrapolationDist > 0.0) {
              calibrationSpotVtxInfo.distance = srcCalibrationSpotExtrapolationDist;
              calibrationSpotVtxInfo.distance_xy = (srcCalibrationSpotWorldImpact - refPoint).perp();
            } else {
              calibrationSpotVtxInfo.distance = 0.0;
              calibrationSpotVtxInfo.distance_xy = 0.0;
            }
            calibrationSpotVtxInfo.tolerance = _intercept_tolerance_;
            if (calibrationSpotVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
              srcVertexes.push_back(calibrationSpotVtxInfo);
            } else {
              DT_LOG_DEBUG(logPrio, "    Line intercept is to far from the caibration source spot");
            }
          }
        }
    
        int closestIndex = -1;
        double closestDist = datatools::invalid_real();
        for (unsigned int i = 0; i < srcVertexes.size(); i++) {
          const vertex_info & vtxInfo = srcVertexes[i];
          if (closestIndex == -1 or vtxInfo.distance < closestDist) {
            closestIndex = i;
            closestDist = vtxInfo.distance;
          }
        }
        if (closestIndex >= 0) {
          vertexes_.push_back(srcVertexes[closestIndex]);
        }
        srcVertexes.clear();
      } // for (int iFrom=...) 
      return;
    }

    void vertex_extrapolation_driver::helix_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                        const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                                                        uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logPrio, "Search helix intercepts on source elements from mask = " << from_mask_);
      vertexes_.clear();
      const geomtools::helix_3d & helix = helix_traj_.get_helix();
      using snemo::geometry::vertex_info;
      bool use_foils = false;
      bool use_calib_src = false;
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL)->second) {
        use_foils = true;
      }
      if (_use_vertices_.find(snemo::datamodel::particle_track::VERTEX_ON_CALIBRATION_SOURCE)->second) {
        use_calib_src = true;
      }
       for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        std::vector<vertex_info> srcVertexes;
        DT_LOG_DEBUG(logPrio, "From : " << iFrom);
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        vertex_info::from_bit_type from_bit = vertex_info::FROM_FIRST_BIT;
        if (iFrom == vertex_info::FROM_LAST) from_bit = vertex_info::FROM_LAST_BIT;
        if (use_foils) {
          for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
            const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
            DT_LOG_DEBUG(logPrio, "  Source strip GID : " << sourceStripGid);
            uint32_t sourceStripId = sourceStripGid.get(1);
            if (sourceStripGid.get(0) != _module_id_) { 
              continue;
            }
            DT_LOG_DEBUG(logPrio, "  Searching helix intercept on strip #" << sourceStripId);
            const geomtools::geom_info  & sourceStripGinfo     = geoManager().get_mapping().get_geom_info(sourceStripGid);
            const geomtools::logical_volume & sourceStripLog   = sourceStripGinfo.get_logical();
            const geomtools::i_shape_3d & sourceStripShape     = sourceStripLog.get_shape();
            const geomtools::placement  & sourceStripPlacement = sourceStripGinfo.get_world_placement();
            DT_LOG_DEBUG(logPrio, "  Source strip shape  : '" << sourceStripShape.get_shape_name() << "'");
            DT_LOG_DEBUG(logPrio, "  Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
            snemo::geometry::helix_intercept hStripIntercept(helix,
                                                             sourceStripShape,
                                                             sourceStripPlacement,
                                                             _finder_step_,
                                                             _intercept_tolerance_,
                                                             logPrio);
            snemo::geometry::helix_intercept::extrapolation_info srcStripEi;
            bool success = hStripIntercept.find_intercept(srcStripEi, from_bit);
            if (! success) {
              // No intersection with the strip volume: no chance to intersect a pad ! We give up.
              continue;
            }
            DT_LOG_DEBUG(logPrio, "  ===> Found helix intercept on strip #" << sourceStripId);
            geomtools::vector_3d srcStripImpact = srcStripEi.fii.get_impact();
            geomtools::vector_3d srcStripWorldImpact;
            // sourceStripPlacement.child_to_mother(srcStripImpact, srcStripWorldImpact);
            srcStripEi.fii.set_impact(srcStripWorldImpact);
            // Search intercept on daughter pad volumes:
            for (uint32_t j = 0; j < _sourcePadGids_.size(); j++) {
              const geomtools::geom_id & sourcePadGid = _sourcePadGids_[j];
              if (sourcePadGid.get(1) != sourceStripId) {
                /// Reject pads in other strips:
                continue;
              }
              uint32_t sourcePadId = sourcePadGid.get(2);
              DT_LOG_DEBUG(logPrio, "  ===> Searching helix intercept on pad #" << sourcePadId << " in strip #" << sourceStripId);
              const geomtools::geom_info  & sourcePadGinfo     = geoManager().get_mapping().get_geom_info(sourcePadGid);
              const geomtools::logical_volume & sourcePadLog   = sourcePadGinfo.get_logical();
              const geomtools::i_shape_3d & sourcePadShape     = sourcePadLog.get_shape();
              const geomtools::placement  & sourcePadPlacement = sourcePadGinfo.get_world_placement();
              DT_LOG_DEBUG(logPrio, "  Source pad shape : '" << sourcePadShape.get_shape_name() << "'");
              snemo::geometry::helix_intercept hPadIntercept(helix,
                                                             sourcePadShape,
                                                             sourcePadPlacement,
                                                             _finder_step_,
                                                             _intercept_tolerance_,
                                                             logPrio);
              snemo::geometry::helix_intercept::extrapolation_info srcPadEi;
              success = hPadIntercept.find_intercept(srcPadEi, from_bit);
              if (! success) {
                DT_LOG_DEBUG(logPrio, "  ===> Give up strip #" << sourceStripId);
                continue;
              }
              DT_LOG_DEBUG(logPrio, "  ===> Found helix intercept on pad #" << sourcePadId
                           << " at " << geomtools::to_xyz(srcPadEi.fii.get_impact()));
              vertex_info padVtxInfo;
              padVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL;
              padVtxInfo.from = iFrom;
              padVtxInfo.gid = sourcePadGid;
              padVtxInfo.face_intercept = srcPadEi.fii;
              padVtxInfo.distance = srcPadEi.extrapolated_length;
              padVtxInfo.distance_xy = srcPadEi.extrapolated_xy_length;
              padVtxInfo.tolerance = _intercept_tolerance_;
              // Search intercept on daughter pad bulk volumes:
              bool candidate_bulk = false;
              for (uint32_t k = 0; k < _sourcePadBulkGids_.size(); k++) {
                const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[k];
                if (sourcePadBulkGid.get(1) != sourceStripId or sourcePadBulkGid.get(2) != sourcePadId ) {
                  /// Reject pad bulks in other strips and pads:
                  continue;
                }
                candidate_bulk = true;
                DT_LOG_DEBUG(logPrio, "    Searching helix intercept on pad bulk #" << sourcePadId << " in strip #" << sourceStripId);
                const geomtools::geom_info  & sourcePadBulkGinfo  = geoManager().get_mapping().get_geom_info(sourcePadBulkGid);
                const geomtools::logical_volume & sourcePadBulkLog   = sourcePadBulkGinfo.get_logical();
                const geomtools::i_shape_3d & sourcePadBulkShape     = sourcePadBulkLog.get_shape();
                const geomtools::placement  & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
                DT_LOG_DEBUG(logPrio, "    Source pad bulk shape : '" << sourcePadBulkShape.get_shape_name() << "'");
                snemo::geometry::helix_intercept hPadBulkIntercept(helix,
                                                                   sourcePadBulkShape,
                                                                   sourcePadBulkPlacement,
                                                                   _finder_step_,
                                                                   _intercept_tolerance_,
                                                                   logPrio);
                snemo::geometry::helix_intercept::extrapolation_info srcPadBulkEi;
                success = hPadBulkIntercept.find_intercept(srcPadBulkEi,
                                                           from_bit);
                if (! success) {
                  DT_LOG_DEBUG(logPrio, "  ===> Give up strip/pad = " << sourceStripId << '/' << sourcePadId);
                  continue;
                }
                DT_LOG_DEBUG(logPrio, "  ===> Found helix intercept on pad bulk #" << sourcePadId
                             << " at " << geomtools::to_xyz(srcPadBulkEi.fii.get_impact()));
                vertex_info padBulkVtxInfo;
                padBulkVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL;
                padBulkVtxInfo.from = iFrom;
                padBulkVtxInfo.gid = sourcePadBulkGid;
                padBulkVtxInfo.face_intercept = srcPadBulkEi.fii;
                padBulkVtxInfo.distance = srcPadBulkEi.extrapolated_length;
                padBulkVtxInfo.distance_xy = srcPadBulkEi.extrapolated_xy_length;
                padBulkVtxInfo.tolerance = _intercept_tolerance_;
                if (padBulkVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  DT_LOG_DEBUG(logPrio, "  ======> add padBulkVtxInfo");
                  srcVertexes.push_back(padBulkVtxInfo);
                } else {
                  DT_LOG_DEBUG(logPrio, "    Helix intercept is to far from the pad bulk");
                }
              }
              if (! candidate_bulk) { 
                // Validate pad intercept if no candidate pad bulk
                if (padVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  DT_LOG_DEBUG(logPrio, "  ======> add padVtxInfo");
                  srcVertexes.push_back(padVtxInfo);
                } else {
                  DT_LOG_DEBUG(logPrio, "    Helix intercept is to far from the pad");
                }
              }
            }   
          }
        }
        if (use_calib_src) {
          // Calibration spot:
          for (uint32_t iSpot = 0; iSpot < _sourceCalibrationSpotGids_.size(); iSpot++) {
            const geomtools::geom_id & sourceCalibrationSpotGid = _sourceCalibrationSpotGids_[iSpot];
            DT_LOG_DEBUG(logPrio, "Searching helix intercept on calibration source spot #" << sourceCalibrationSpotGid);
            const geomtools::geom_info  & sourceCalibrationSpotGinfo  = geoManager().get_mapping().get_geom_info(sourceCalibrationSpotGid);
            const geomtools::logical_volume & sourceCalibrationSpotLog   = sourceCalibrationSpotGinfo.get_logical();
            const geomtools::i_shape_3d & sourceCalibrationSpotShape     = sourceCalibrationSpotLog.get_shape();
            const geomtools::placement  & sourceCalibrationSpotPlacement = sourceCalibrationSpotGinfo.get_world_placement();
            DT_LOG_DEBUG(logPrio, "    Calibration spot shape : '" << sourceCalibrationSpotShape.get_shape_name() << "'");
            snemo::geometry::helix_intercept hCalibrationSpotIntercept(helix,
                                                                       sourceCalibrationSpotShape,
                                                                       sourceCalibrationSpotPlacement,
                                                                       _finder_step_,
                                                                       _intercept_tolerance_,
                                                                       logPrio);
            snemo::geometry::helix_intercept::extrapolation_info srcCalibrationSpotEi;
            bool success = hCalibrationSpotIntercept.find_intercept(srcCalibrationSpotEi, from_bit);
            if (! success) {
              DT_LOG_DEBUG(logPrio, "  ===> Give up calibration source");
              continue;
            }
            DT_LOG_DEBUG(logPrio, " ===> Found helix intercept on calibration source spot #" << sourceCalibrationSpotGid);
            geomtools::vector_3d srcCalibrationSpotImpact = srcCalibrationSpotEi.fii.get_impact();
            geomtools::vector_3d srcCalibrationSpotWorldImpact;
            sourceCalibrationSpotPlacement.child_to_mother(srcCalibrationSpotImpact, srcCalibrationSpotWorldImpact);
            srcCalibrationSpotEi.fii.set_impact(srcCalibrationSpotWorldImpact);
            vertex_info calibrationSpotVtxInfo;
            calibrationSpotVtxInfo.type = snemo::datamodel::particle_track::VERTEX_ON_CALIBRATION_SOURCE;
            calibrationSpotVtxInfo.from = iFrom;
            calibrationSpotVtxInfo.gid = sourceCalibrationSpotGid;
            calibrationSpotVtxInfo.face_intercept = srcCalibrationSpotEi.fii;
            calibrationSpotVtxInfo.distance = srcCalibrationSpotEi.extrapolated_length;
            calibrationSpotVtxInfo.distance_xy = srcCalibrationSpotEi.extrapolated_xy_length;
            calibrationSpotVtxInfo.tolerance = _intercept_tolerance_;
            if (calibrationSpotVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
              srcVertexes.push_back(calibrationSpotVtxInfo);
            } else {
              DT_LOG_DEBUG(logPrio, "    Helix intercept is to far from the calibration source spot");
            }
          }
        }
  
        int closestIndex = -1;
        double closestDist = datatools::invalid_real();
        for (unsigned int i = 0; i < srcVertexes.size(); i++) {
          const vertex_info & vtxInfo = srcVertexes[i];
          if (closestIndex == -1 or vtxInfo.distance < closestDist) {
            closestIndex = i;
            closestDist = vtxInfo.distance;
          }
        }
        if (closestIndex >= 0) {
          vertexes_.push_back(srcVertexes[closestIndex]);
        }
        srcVertexes.clear();
      } // for (int iFrom=...) 
      return;
    }
  
    void vertex_extrapolation_driver::_measure_vertices_(const snemo::datamodel::tracker_trajectory &trajectory_,
                                                         snemo::datamodel::particle_track::vertex_collection_type &vertices_)
    {
      auto logPrio = logPriority_;
      namespace snedm = snemo::datamodel;
      vertices_.clear();
  
      // Extract the side from the geometry ID of the tracker trajectory object:
      if (!trajectory_.has_geom_id()) {
        DT_LOG_ERROR(logPrio, "Tracker trajectory has no geometry ID, cannot extrapolate vertices");
        return;
      }
      const geomtools::geom_id & gid = trajectory_.get_geom_id();
      const geomtools::id_mgr  & id_mgr = geoManager().get_id_mgr();
      if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side")) {
        DT_LOG_ERROR(logPrio, "Trajectory geom ID " << gid << " has no 'module' or 'side' address");
        return;
      }
      const int trackSide = id_mgr.get(gid, "side");

      // Set the calorimeter locators :
      // const snemo::geometry::calo_locator &calo_locator = geoLocator_->caloLocator();
      // const snemo::geometry::xcalo_locator &xcalo_locator = geoLocator_->xcaloLocator();
      // const snemo::geometry::gveto_locator &gveto_locator = geoLocator_->gvetoLocator();
      // const double xcalo_bd[2] = {calo_locator.getXCoordOfWallWindow(snemo::geometry::side_t::BACK),
      //                             calo_locator.getXCoordOfWallWindow(snemo::geometry::side_t::FRONT)};
      // const double ycalo_bd[2] = {
      //     xcalo_locator.getYCoordOfWallWindow(trackSide, snemo::geometry::xcalo_wall_t::LEFT),
      //     xcalo_locator.getYCoordOfWallWindow(trackSide, snemo::geometry::xcalo_wall_t::RIGHT)};
      // const double zcalo_bd[2] = {
      //     gveto_locator.getZCoordOfWallWindow(trackSide, snemo::geometry::gveto_wall_t::BOTTOM),
      //     gveto_locator.getZCoordOfWallWindow(trackSide, snemo::geometry::gveto_wall_t::TOP)};

      // Check Geiger cell location wrt vertex extrapolation
      _use_vertices_.clear();
      this->_check_vertices_(trajectory_, _use_vertices_);

      // Look first if the trajectory pattern is an helix or a line:
      const snedm::base_trajectory_pattern &a_track_pattern = trajectory_.get_pattern();
      const std::string &a_pattern_id = a_track_pattern.get_pattern_id();

      // Extrapolated vertices:
      snemo::geometry::vertex_info_list vertice_infos;
  
      DT_LOG_DEBUG(logPrio, "Pattern ID : '" << a_pattern_id << "'");

      // ----- Start of line pattern handling
      if (a_pattern_id == snedm::line_trajectory_pattern::pattern_id()) {
        const auto & ltp = dynamic_cast<const snedm::line_trajectory_pattern &>(a_track_pattern);
        bool find_on_source = true; 
        bool find_on_calo   = true;
        if ((_use_vertices_.find(snedm::particle_track::VERTEX_ON_SOURCE_FOIL)->second == false)
            and (_use_vertices_.find(snedm::particle_track::VERTEX_ON_CALIBRATION_SOURCE)->second == false)) {
          find_on_source = false;
        }
        if ((_use_vertices_.find(snedm::particle_track::VERTEX_ON_MAIN_CALORIMETER)->second == false)
            and (_use_vertices_.find(snedm::particle_track::VERTEX_ON_X_CALORIMETER)->second == false)) {
          find_on_calo = false;
        }
        
        // Vertex on source strips:
        if (find_on_source) {      
          snemo::geometry::vertex_info_list sourceVertexes;
          line_trajectory_source_intercept(sourceVertexes, ltp);
          DT_LOG_DEBUG(logPrio, "Line source vertexes = " << sourceVertexes.size());
          for (const auto & v : sourceVertexes) {
            DT_LOG_DEBUG(logPrio, "Line source vertexes:");
            if (datatools::logger::is_debug(logPrio)) {
              v.print(std::cerr);
            }
          }
          if (sourceVertexes.size()) {
            // Extrapolated vertex:
            for (const auto & sv : sourceVertexes) {
              vertice_infos.push_back(sv);
            }
          }
        }

        if (find_on_calo) {
          snemo::geometry::vertex_info_list caloVertexes;
          line_trajectory_calo_intercept(caloVertexes, ltp, trackSide);
          DT_LOG_DEBUG(logPrio, "Calorimeter vertexes = " << caloVertexes.size());
          for (const auto & v : caloVertexes) {
            DT_LOG_DEBUG(logPrio, "Calorimeter vertexes:");
            if (datatools::logger::is_debug(logPrio)) {
              v.print(std::cerr);
            }
          }
          if (caloVertexes.size()) {
            // Extrapolated vertex:
            for (const auto & cv : caloVertexes) {
              vertice_infos.push_back(cv);
            }
          }
        }

        /*
        // This *looks* like it finds the two vertices closest to the end points of
        // the line pattern
        std::pair<double, double> min_distances;
        datatools::infinity(min_distances.first);
        datatools::infinity(min_distances.second);
        auto jt1 = vtxlist.begin();
        auto jt2 = vtxlist.begin();
        for (auto it = vtxlist.begin(); it != vtxlist.end(); ++it) {
        const double l1 = (first - it->first).mag();
        const double l2 = (last - it->first).mag();

        if (l1 < l2) {
        if (l1 > min_distances.first) {
        continue;
        }
        jt1 = it;
        min_distances.first = l1;
        } else {
        if (l2 > min_distances.second) {
        continue;
        }
        jt2 = it;
        min_distances.second = l2;
        }
        }
        */

        /*
        // Create a mutable line object to set the new position
        auto a_mutable_line = const_cast<geomtools::line_3d *>(&a_line);
        if (use_vertices[jt1->second]) {
        a_mutable_line->set_first(jt1->first);
        vertices.push_back(std::make_pair(jt1->second, jt1->first));
        } else {
        vertices.push_back(
        std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_line.get_first()));
        }
        if (use_vertices[jt2->second]) {
        a_mutable_line->set_last(jt2->first);
        vertices.push_back(std::make_pair(jt2->second, jt2->first));
        } else {
        vertices.push_back(
        std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_line.get_last()));
        }
        */
    
      } // ----- end of line pattern handling

      // ---- start of helix pattern handling
      if (a_pattern_id == snedm::helix_trajectory_pattern::pattern_id()) {
        const auto & htp = dynamic_cast<const snedm::helix_trajectory_pattern &>(a_track_pattern);
        bool find_on_source = true;
        bool find_on_calo   = true;
        if ((_use_vertices_.find(snedm::particle_track::VERTEX_ON_SOURCE_FOIL)->second == false)
            and (_use_vertices_.find(snedm::particle_track::VERTEX_ON_CALIBRATION_SOURCE)->second == false)) {
          find_on_source = false;
        }
        if ((_use_vertices_.find(snedm::particle_track::VERTEX_ON_MAIN_CALORIMETER)->second == false)
            and (_use_vertices_.find(snedm::particle_track::VERTEX_ON_X_CALORIMETER)->second == false)) {
          find_on_calo = false;
        }

        // Vertex on source strips:
        if (find_on_source) {      
          snemo::geometry::vertex_info_list sourceVertexes;
          helix_trajectory_source_intercept(sourceVertexes, htp);
          DT_LOG_DEBUG(logPrio, "Helix source vertexes = " << sourceVertexes.size());
          for (const auto & v : sourceVertexes) {
            DT_LOG_DEBUG(logPrio, "Helix source vertexes:");
            if (datatools::logger::is_debug(logPrio)) {
              v.print(std::cerr);
            }
          }
          if (sourceVertexes.size()) {
            // Extrapolated vertex:
            for (const auto & sv : sourceVertexes) {
              vertice_infos.push_back(sv);
            }
          }
        }

        if (find_on_calo) {
          snemo::geometry::vertex_info_list caloVertexes;
          helix_trajectory_calo_intercept(caloVertexes, htp, trackSide);
          DT_LOG_DEBUG(logPrio, "Helix calorimeter vertexes = " << caloVertexes.size());
          for (const auto & v : caloVertexes) {
            DT_LOG_DEBUG(logPrio, "Helix calorimeter vertexes:");
            if (datatools::logger::is_debug(logPrio)) {
              v.print(std::cerr);
            }
          }
          if (caloVertexes.size()) {
            // Extrapolated vertex:
            for (const auto & cv : caloVertexes) {
              vertice_infos.push_back(cv);
            }
          }
        }

        /*
        // Store all the computed t parameter values
        std::map<double, std::string> tparams;

        // Source foil
        {
        const double xfoil = 0.0 * CLHEP::mm;
        const double xcenter = hcenter.x();
        const double cangle = (xfoil - xcenter) / hradius;

        if (std::fabs(cangle) < 1.0) {
        const double angle = std::acos(cangle);
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(+angle),
        snedm::particle_track::vertex_on_source_foil_label()));
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(-angle),
        snedm::particle_track::vertex_on_source_foil_label()));
        }
        }

        // Calorimeter walls
        {
        for (const double xwall : xcalo_bd) {
        const double xcenter = hcenter.x();
        const double cangle = (xwall - xcenter) / hradius;

        if (std::fabs(cangle) < 1.0) {
        const double angle = std::acos(cangle);
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(+angle),
        snedm::particle_track::vertex_on_main_calorimeter_label()));
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(-angle),
        snedm::particle_track::vertex_on_main_calorimeter_label()));
        }
        }
        }

        // X-walls
        {
        for (const double ywall : ycalo_bd) {
        const double ycenter = hcenter.y();
        const double cangle = (ywall - ycenter) / hradius;

        if (std::fabs(cangle) < 1.0) {
        double angle = std::asin(cangle);
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle),
        snedm::particle_track::vertex_on_x_calorimeter_label()));
        const double mean_angle = (a_helix.get_angle1() + a_helix.get_angle2()) / 2.0;
        angle = (mean_angle < 0.0) ? (-M_PI - angle) : (+M_PI - angle);
        tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle),
        snedm::particle_track::vertex_on_x_calorimeter_label()));
        }
        }
        }

        // Choose which helix angle to change
        const double t1 = a_helix.get_t1();
        const double t2 = a_helix.get_t2();

        auto new_ts = std::make_pair(datatools::invalid_real(), datatools::invalid_real());
        std::pair<double, double> min_distances;
        datatools::infinity(min_distances.first);
        datatools::infinity(min_distances.second);
        std::pair<std::string, std::string> category_flags;

        for (const auto &tp : tparams) {
        const double t = tp.first;
        const std::string &category = tp.second;

        // Calculate delta t values as well as new lengths
        const double delta1 = std::fabs(t1 - t);
        const double delta2 = std::fabs(t2 - t);

        // Keep smallest distance but remove also too long extrapolation
        if (delta1 < delta2) {
        if (delta1 > min_distances.first) {
        continue;
        }
        new_ts.first = t;
        category_flags.first = category;
        min_distances.first = delta1;
        } else {
        if (delta2 > min_distances.second) {
        continue;
        }
        new_ts.second = t;
        category_flags.second = category;
        min_distances.second = delta2;
        }
        }

        // New angle & calorimeter category (if length not too long)
        const double delta2length =
        2 * M_PI * hypot(a_helix.get_radius(), a_helix.get_step() / (2 * M_PI));
        const double length = a_helix.get_length();
        // Create a mutable helix object to set the new angle
        auto a_mutable_helix = const_cast<geomtools::helix_3d *>(&a_helix);
        if (datatools::is_valid(new_ts.first)) {
        const double new_length = delta2length * std::abs(new_ts.first - a_helix.get_t1());
        const std::string &category = category_flags.first;
        if (use_vertices[category] && new_length < length) {
        a_mutable_helix->set_t1(new_ts.first);
        vertices.push_back(std::make_pair(category, a_mutable_helix->get_first()));
        } else {
        vertices.push_back(
        std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_helix.get_first()));
        }
        }
        if (datatools::is_valid(new_ts.second)) {
        const double new_length = delta2length * std::abs(new_ts.second - a_helix.get_t2());
        const std::string &category = category_flags.second;
        if (use_vertices[category] && new_length < length) {
        a_mutable_helix->set_t2(new_ts.second);
        vertices.push_back(std::make_pair(category, a_mutable_helix->get_last()));
        } else {
        vertices.push_back(
        std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_helix.get_last()));
        }
        }
        */
      }
      // ----- end of helix pattern

      // Save new vertices:
      for (const auto &vtxInfo : vertice_infos) {
        auto spot = datatools::make_handle<geomtools::blur_spot>();
        spot->set_hit_id(vertices_.size());
        spot->set_geom_id(vtxInfo.gid);
        spot->grab_auxiliaries().store(snedm::particle_track::vertex_from_key(),
                                        snemo::geometry::vertex_info::from_to_label(vtxInfo.from));
        spot->grab_auxiliaries().store(snedm::particle_track::vertex_type_key(),
                                        snedm::particle_track::vertex_type_to_label(vtxInfo.type));
        if (datatools::is_valid(vtxInfo.distance)) {
          spot->grab_auxiliaries().store_with_explicit_unit(snedm::particle_track::vertex_distance_key(), vtxInfo.distance);
          spot->grab_auxiliaries().set_unit_symbol(snedm::particle_track::vertex_distance_key(), "mm");
        }
        if (datatools::is_valid(vtxInfo.distance_xy)) {
          spot->grab_auxiliaries().store_with_explicit_unit(snedm::particle_track::vertex_distance_xy_key(), vtxInfo.distance_xy);
          spot->grab_auxiliaries().set_unit_symbol(snedm::particle_track::vertex_distance_xy_key(), "mm");
        }
        spot->set_blur_dimension(geomtools::blur_spot::dimension_three);
        spot->set_position(vtxInfo.face_intercept.get_impact());
        spot->set_errors(vtxInfo.tolerance);
        vertices_.push_back(spot);     
      }

      // // Save new vertices 
      // for (const auto &vertice : vertices) {
      //   const std::string &flag = vertice.first;
      //   const geomtools::vector_3d &pos = vertice.second;
      //   // Check vertex side is on the same side as the trajectory
      //   if ((trackSide == snemo::geometry::side_t::BACK && pos.x() > 0.0) ||
      //       (trackSide == snemo::geometry::side_t::FRONT && pos.x() < 0.0)) {
      //     // Is this an error or warning?
      //     DT_LOG_WARNING(logPrio, "Closest vertex is on the opposite side!");
      //   }
      //   auto spot = datatools::make_handle<geomtools::blur_spot>();
      //   spot->set_hit_id(vertices_.size());
      //   spot->grab_auxiliaries().update(snedm::particle_track::vertex_type_key(), flag);
      //   // For now it is dimension 3 with no errors nor rotation defined:
      //   spot->set_blur_dimension(geomtools::blur_spot::dimension_three);
      //   spot->set_position(pos); 
      //   vertices_.push_back(spot);
      // }

      //
      // Future implementation:= ???
      //
      //   double sigma_x = ???; // Computed from the TrackFit error matrix
      //   double sigma_y = ???; // Computed from the TrackFit error matrix
      //   double sigma_z = ???; // Computed from the TrackFit error matrix
      //   spot.grab_placement().set_translation(it->second);
      //   if (snemo::datamodel::particle_track::vertex_is_on_main_calorimeter(spot)) {
      //     //
      //     // Possibility:
      //     //    +---------------------------------+
      //     //   /                                 /|
      //     //  /                                 / |
      //     // +---------------------------------+  |
      //     // |           ^ z        Calo block |  |
      //     // |  sigma(z) :            entrance |  |
      //     // |          ---            surface |  |
      //     // |         / : \                   |  |
      //     // |        /  :  \ sigma(y)         |  |
      //     // |    - - |- + -|- - -> y          |  |
      //     // |        | /:  |                  |  |
      //     // |  --->   / : /  2D blur spot     |  +
      //     // | normal / ---                    | /
      //     // |       L   :                     |/
      //     // +---------------------------------+
      //     //
      //     //
      //     spot.set_blur_dimension(geomtools::blur_spot::dimension_two);
      //     if (side == snemo::geometry::side_t::BACK) {
      //       spot.grab_placement().set_orientation(ROTATION_AXIS_Y, 90.0 * CLHEP::degree);
      //     } else {
      //       spot.grab_placement().set_orientation(ROTATION_AXIS_Y, -90.0 * CLHEP::degree);
      //     }
      //     spot.set_errors(sigma_y, sigma_z);
      //   } else ...
      //
    }

    void vertex_extrapolation_driver::_check_vertices_(const snemo::datamodel::tracker_trajectory &trajectory_,
                                                       std::map<snemo::datamodel::particle_track::vertex_type, bool> & use_vertices_) {
      if (!trajectory_.has_cluster()) {
        return;
      }
      // Reset values of booleans
      namespace snedm = snemo::datamodel;
      use_vertices_.clear();
      use_vertices_[snedm::particle_track::VERTEX_ON_SOURCE_FOIL] = false;
      use_vertices_[snedm::particle_track::VERTEX_ON_MAIN_CALORIMETER] = false;
      use_vertices_[snedm::particle_track::VERTEX_ON_X_CALORIMETER] = false;
      use_vertices_[snedm::particle_track::VERTEX_ON_GAMMA_VETO] = false;
      use_vertices_[snedm::particle_track::VERTEX_ON_CALIBRATION_SOURCE] = false;

      const snedm::tracker_cluster &a_cluster = trajectory_.get_cluster();
      const snedm::TrackerHitHdlCollection &the_hits = a_cluster.hits();
      for (const datatools::handle<snedm::calibrated_tracker_hit>& a_hit : the_hits) {
        const geomtools::geom_id &a_gid = a_hit->get_geom_id();

        // Extract layer
        const snemo::geometry::gg_locator &gg_locator = geoLocator_->geigerLocator();
        const uint32_t layer = gg_locator.getLayerAddress(a_gid);
        if (layer < 2) {
          // Extrapolate vertex to the foil or calib. sources if the first GG layers are fired
          use_vertices_[snedm::particle_track::VERTEX_ON_SOURCE_FOIL] = true;
          use_vertices_[snedm::particle_track::VERTEX_ON_CALIBRATION_SOURCE] = true;
        }

        const uint32_t side = gg_locator.getSideAddress(a_gid);
        if (layer >= gg_locator.numberOfLayers(side) - 2) {
          use_vertices_[snedm::particle_track::VERTEX_ON_MAIN_CALORIMETER] = true;
        }

        const uint32_t row = gg_locator.getRowAddress(a_gid);
        if (row <= 1 || row >= gg_locator.numberOfRows(side) - 1) {
          use_vertices_[snedm::particle_track::VERTEX_ON_X_CALORIMETER] = true;
        }
      }
    }

    // static
    void vertex_extrapolation_driver::init_ocd(datatools::object_configuration_description &ocd_) {
      // Prefix "VED" stands for "Vertex Extrapolation Driver" :
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "VED.");

      {
        // Description of the 'VED.use_linear_extrapolation' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("VED.use_linear_extrapolation")
          .set_from("snemo::reconstruction::vertex_extrapolation_driver")
          .set_terse_description("Use a linear extrapolation to determine vertex position")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_mandatory(false)
          .set_default_value_boolean(false)
          .set_long_description("Feature to be implemented")
          .add_example(
                       "Set the default value::                      \n"
                       "                                             \n"
                       "  VED.use_linear_extrapolation : boolean = 0 \n"
                       "                                             \n");
      }
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::vertex_extrapolation_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::vertex_extrapolation_driver");
  ocd_.set_class_description("A driver class for vertex extrapolation algorithm");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation(
                               "This drivers does the extrapolation of tracker trajectories     \n"
                               "and builds the list of vertices attached to the particle track. \n");

  // Invoke specific OCD support :
  ::snemo::reconstruction::vertex_extrapolation_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::vertex_extrapolation_driver,
                               "snemo::reconstruction::vertex_extrapolation_driver")

// end of falaise/snemo/reconstruction/vertex_extrapolation_driver.cc
