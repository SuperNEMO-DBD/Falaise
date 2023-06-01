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
    
    // static
    const int vertex_extrapolation_driver::CALO_MAIN;
    
    // static
    const int vertex_extrapolation_driver::CALO_XCALO;

    const std::string & vertex_extrapolation_driver::get_id()
    {
      static const std::string s("VED");
      return s;
    }

    const geomtools::manager & vertex_extrapolation_driver::geoManager() const
    {
      DT_THROW_IF(geoManager_ == nullptr, std::logic_error,
                  "No geometry manager is setup !");
      return *geoManager_;
    }

    /// Initialize the driver through configuration properties
    vertex_extrapolation_driver::vertex_extrapolation_driver(const falaise::property_set & ps,
                                                             const geomtools::manager * gm)
    {
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
      DT_LOG_DEBUG(logPriority_, "Intercept tolerance = " << _intercept_tolerance_ / CLHEP::mm << " mm");
      DT_LOG_DEBUG(logPriority_, "Finder step = " << _finder_step_ / CLHEP::mm << " mm");
      DT_LOG_DEBUG(logPriority_, "Max calo extrapolation XY-length = " << _max_calo_extrapolation_xy_length_ / CLHEP::cm << " cm");
      DT_LOG_DEBUG(logPriority_, "Max source extrapolation XY-length = " << _max_source_extrapolation_xy_length_ / CLHEP::cm << " cm");

      // Identify source submodule:
      _sourceSubmoduleType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_submodule")) {
        _sourceSubmoduleType_ = geoIdMgr.get_category_info("source_submodule").get_type();
        geomtools::geom_id sourceSubmoduleGidPattern(_sourceSubmoduleType_, _module_id_);
        std::vector<geomtools::geom_id> sourceSubmoduleGids;
        geoMapping.compute_matching_geom_id(sourceSubmoduleGidPattern, sourceSubmoduleGids);
        DT_THROW_IF(sourceSubmoduleGids.size() != 1, std::logic_error, "Should have found 1 unique source submodule!");
        _sourceSubmoduleGid_ = sourceSubmoduleGids[0];
      }
      DT_LOG_DEBUG(logPriority_, "Found source submodule GID=" << _sourceSubmoduleGid_ << " source pads");

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
      _sourceStripMinId_ =  100000;
      _sourceStripMaxId_ = -100000;
      for (const auto & stripGid : _sourceStripGids_) {
        int32_t stripId = (int32_t) stripGid.get(1);
        if (stripId > _sourceStripMaxId_) {
          _sourceStripMaxId_ = stripId;
        }
        if (stripId < _sourceStripMinId_) {
          _sourceStripMinId_ = stripId;
        }
      }
      DT_LOG_DEBUG(logPriority_, "Min source strip ID=" << _sourceStripMinId_);
      DT_LOG_DEBUG(logPriority_, "Max source strip ID=" << _sourceStripMaxId_);

      // Identify source strip gaps:
      _sourceStripGapType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_strip_gap")) {
        _sourceStripGapType_ = geoIdMgr.get_category_info("source_strip_gap").get_type();
        geomtools::geom_id sourceStripGapGidPattern(_sourceStripGapType_,
                                                 _module_id_,
                                                 geomtools::geom_id::ANY_ADDRESS);
        geoMapping.compute_matching_geom_id(sourceStripGapGidPattern, _sourceStripGapGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Found " << _sourceStripGapGids_.size() << " source strip gaps");
      if (_sourceStripGapGids_.size()) {
        DT_LOG_DEBUG(logPriority_, "Extracting geometry infos about source strip gaps...");
        const geomtools::geom_info & srcStripGapGinfo = geoManager().get_mapping().get_geom_info(_sourceStripGapGids_[0]);
        const geomtools::logical_volume & srcStripGapLog   = srcStripGapGinfo.get_logical();
        const geomtools::i_shape_3d & srcStripGapShape     = srcStripGapLog.get_shape();    
        const geomtools::placement & srcStripGapPlacement = srcStripGapGinfo.get_world_placement();
        _sourceStripGapX_ = srcStripGapPlacement.get_translation().x();
        _sourceStripGapZ_ = srcStripGapPlacement.get_translation().z();
        const geomtools::box & srcStripGapBox = dynamic_cast<const geomtools::box &>(srcStripGapShape);
        _sourceStripGapHeight_ = srcStripGapBox.get_z();
        _sourceStripGapBoxPtr_ = std::make_unique<geomtools::box>(4.0 * CLHEP::mm, srcStripGapBox.get_y(), _sourceStripGapHeight_);
      }
      
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
      } else {
        DT_LOG_DEBUG(logPriority_, "Using flat source strips.");
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

      // Identify calibration source strip:
      _sourceCalibTrackType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("source_calibration_track")) {
        _sourceCalibTrackType_ = geoIdMgr.get_category_info("source_calibration_track").get_type();
        geomtools::geom_id sourceCalibTrackGidPattern(_sourceCalibTrackType_,
                                                 _module_id_,
                                                 geomtools::geom_id::ANY_ADDRESS);
        geoMapping.compute_matching_geom_id(sourceCalibTrackGidPattern, _sourceCalibTrackGids_);
      }
      DT_LOG_DEBUG(logPriority_, "Found " << _sourceCalibTrackGids_.size() << " source calibration tracks");
      if (_sourceCalibTrackGids_.size()) {
        DT_LOG_DEBUG(logPriority_, "Extracting geometry infos about source calibration tracks...");
        const geomtools::geom_info & srcCalibTrackGinfo = geoManager().get_mapping().get_geom_info(_sourceCalibTrackGids_[0]);
        const geomtools::logical_volume & srcCalibTrackLog   = srcCalibTrackGinfo.get_logical();
        const geomtools::i_shape_3d & srcCalibTrackShape     = srcCalibTrackLog.get_shape();    
        const geomtools::placement & srcCalibTrackPlacement = srcCalibTrackGinfo.get_world_placement();
        _sourceCalibTrackX_ = srcCalibTrackPlacement.get_translation().x();
        _sourceCalibTrackZ_ = srcCalibTrackPlacement.get_translation().z();
        const geomtools::box & srcCalibTrackBox = dynamic_cast<const geomtools::box &>(srcCalibTrackShape);
        _sourceCalibTrackHeight_ = srcCalibTrackBox.get_z();
        _sourceCalibTrackBoxPtr_ = std::make_unique<geomtools::box>(4.0 * CLHEP::mm, srcCalibTrackBox.get_y(), _sourceCalibTrackHeight_);
      }
 
      // Identify main calo submodule:
      _caloSubmoduleType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("calorimeter_submodule")) {
        _caloSubmoduleType_ = geoIdMgr.get_category_info("calorimeter_submodule").get_type();
      }
      _caloWallType_ = geomtools::geom_id::INVALID_TYPE;
      if (geoIdMgr.has_category_info("calorimeter_wall")) {
        _caloWallType_ = geoIdMgr.get_category_info("calorimeter_wall").get_type();
      }
      DT_LOG_DEBUG(logPriority_, "Calorimeter submodule type = " << _caloSubmoduleType_);
      DT_LOG_DEBUG(logPriority_, "Calorimeter wall type = " << _caloWallType_);
 
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
        {
          geomtools::geom_id caloBlockGid00(_caloBlockType_, _module_id_, 0, 0, 0, 1);
          geomtools::geom_id caloBlockGid11(_caloBlockType_, _module_id_, 0, 1, 1, 1);
          const geomtools::geom_info & block0Ginfo = geoManager().get_mapping().get_geom_info(caloBlockGid00);
          const geomtools::placement & block0Placement = block0Ginfo.get_world_placement();
          const geomtools::geom_info & block1Ginfo = geoManager().get_mapping().get_geom_info(caloBlockGid11);
          const geomtools::placement & block1Placement = block1Ginfo.get_world_placement();
          _main_calo_y_first_ = block0Placement.get_translation().y();
          _main_calo_z_first_ = block0Placement.get_translation().z();
          _main_calo_y_step_ = block1Placement.get_translation().y() - block0Placement.get_translation().y();
          _main_calo_z_step_ = block1Placement.get_translation().z() - block0Placement.get_translation().z();
          DT_LOG_DEBUG(logPriority_, "Main calo y-first = " << _main_calo_y_first_ / CLHEP::mm << " mm");
          DT_LOG_DEBUG(logPriority_, "Main calo y-step  = " << _main_calo_y_step_ / CLHEP::mm << " mm");
          DT_LOG_DEBUG(logPriority_, "Main calo z-first = " << _main_calo_z_first_ / CLHEP::mm << " mm");
          DT_LOG_DEBUG(logPriority_, "Main calo z-step  = " << _main_calo_z_step_ / CLHEP::mm << " mm");
          const geomtools::logical_volume & block0Logical = block0Ginfo.get_logical();
          const geomtools::box & block0Box = dynamic_cast<const geomtools::box &>(block0Logical.get_shape());

          // Create an effective box for main calorimeter front blocks, inflated by some tolerance
          _effectiveCaloBlockBoxPtr_ = std::make_unique<geomtools::box>();
          // Const cast workaround because of a bug in Bayeux:
          const_cast<geomtools::box &>(block0Box).compute_inflated(*_effectiveCaloBlockBoxPtr_,
                                                                   _effectiveCaloBlockXyTolerance_,
                                                                   _effectiveCaloBlockXyTolerance_,
                                                                   0. * CLHEP::mm);
          
          // Create an effective box for main calorimeter front blocks, inflated by some tolerance
          _effectiveCaloBlockBoxPtr2_ = std::make_unique<geomtools::box>();
          // Const cast workaround because of a bug in Bayeux:
          const_cast<geomtools::box &>(block0Box).compute_deflated(*_effectiveCaloBlockBoxPtr2_,
                                                                   _effectiveCaloBlockXyTolerance_,
                                                                   _effectiveCaloBlockXyTolerance_,
                                                                   0. * CLHEP::mm);
        }
 
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
        {
          geomtools::geom_id xcaloBlockGid(_xcaloBlockType_, _module_id_, 0, 0, 0, 0);
          const geomtools::geom_info & xcaloBlockGinfo = geoManager().get_mapping().get_geom_info(xcaloBlockGid);
          const geomtools::logical_volume & xcaloBlockLogical = xcaloBlockGinfo.get_logical();
          const geomtools::box & xcaloBlockBox = dynamic_cast<const geomtools::box &>(xcaloBlockLogical.get_shape());

          // Create an effective box for X calorimeter front blocks, inflated by some tolerance
          _effectiveXcaloBlockBoxPtr_ = std::make_unique<geomtools::box>();
          // Const cast workaround because of a bug in Bayeux:
          const_cast<geomtools::box &>(xcaloBlockBox).compute_inflated(*_effectiveXcaloBlockBoxPtr_,
                                                                       _effectiveCaloBlockXyTolerance_,
                                                                       _effectiveCaloBlockXyTolerance_,
                                                                       0. * CLHEP::mm);

          // Create an effective box for X calorimeter front blocks, inflated by some tolerance
          _effectiveXcaloBlockBoxPtr2_ = std::make_unique<geomtools::box>();
          // Const cast workaround because of a bug in Bayeux:
          const_cast<geomtools::box &>(xcaloBlockBox).compute_deflated(*_effectiveXcaloBlockBoxPtr2_,
                                                                       _effectiveCaloBlockXyTolerance_,
                                                                       _effectiveCaloBlockXyTolerance_,
                                                                       0. * CLHEP::mm);
         }
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

    void vertex_extrapolation_driver::process(const snemo::datamodel::tracker_trajectory & trajectory_,
                                              snemo::datamodel::particle_track & particle_)
    {
      this->_measure_vertices_(trajectory_, particle_.get_vertices());
      return;
    }

    void vertex_extrapolation_driver::helix_trajectory_calo_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                      const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                                                      uint32_t track_side_,
                                                                      uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      datatools::logger::priority hInterceptPrio = datatools::logger::PRIO_ERROR;
      DT_LOG_DEBUG(logPrio, "Search helix intercepts on calorimeter blocks: ");
      vertexes_.clear();
      const geomtools::helix_3d & helix = helix_traj_.get_helix();
      std::set<int> blockTypes;
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER)->second) {
        blockTypes.insert(CALO_MAIN);
        DT_LOG_DEBUG(logPrio, "About to scan main calo blocks");
      }
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER)->second) {
        blockTypes.insert(CALO_XCALO);
        DT_LOG_DEBUG(logPrio, "About to scan X-calo blocks");
      }
      using snemo::geometry::vertex_info;
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        snemo::geometry::vertex_info_list caloVertexes;
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        DT_LOG_DEBUG(logPrio, "   from " << vertex_info::from_to_label(iFrom));
        vertex_info::from_bit_type from_bit = vertex_info::FROM_FIRST_BIT;
        // Default configured for 'first':
        geomtools::vector_3d beginPoint     = helix_traj_.get_last();
        geomtools::vector_3d beginDirection = helix_traj_.get_last_direction();
        geomtools::vector_3d endPoint       = helix_traj_.get_first();
        geomtools::vector_3d endDirection   = -helix_traj_.get_first_direction();
        if (iFrom == vertex_info::FROM_LAST) {
          from_bit = vertex_info::FROM_LAST_BIT;
          std::swap(beginPoint, endPoint);
          std::swap(beginDirection, endDirection);
        }
        
        /* 
         *    from_bit == FROM_LAST
         *
         *           Begin    End
         *             o-------o-----> endDirection
         *                -   Last
         *              - 
         *            -
         *           -
         *          o First
         *         /
         *        L
         *     beginDirection
         *
         */          
        beginPoint = endPoint - _finder_step_ * endDirection;
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        geomtools::vector_2d beginPoint_Xy(beginPoint.x(), beginPoint.y());
        geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
        geomtools::vector_2d refPoint_Xy = 0.5 * (beginPoint_Xy + endPoint_Xy);
        double distRef2End = (endPoint - refPoint).mag();
        double distRef2End_Xy = (endPoint_Xy - refPoint_Xy).mag();
        const geomtools::vector_3d direction = endDirection;
        DT_LOG_DEBUG(logPrio, "End point (world)    : " << geomtools::to_xyz(endPoint) );
        DT_LOG_DEBUG(logPrio, "Ref point (world)    : " << geomtools::to_xyz(refPoint) );
        DT_LOG_DEBUG(logPrio, "XY-Ref point (world) : " << geomtools::to_xy(refPoint_Xy) );
        DT_LOG_DEBUG(logPrio, "Direction (world)    : " << geomtools::to_xyz(direction) );
        DT_LOG_DEBUG(logPrio, "Distance ref->end    : " << distRef2End / CLHEP::mm << " mm");
        DT_LOG_DEBUG(logPrio, "XY-distance ref->end : " << distRef2End_Xy / CLHEP::mm << " mm");

        for (int iBlockType : blockTypes) {
          DT_LOG_DEBUG(logPrio, "Scanning block type=" << iBlockType << "...");
          const std::vector<geomtools::geom_id> * blockGidsPtr = nullptr;
          snemo::geometry::vertex_info::category_type vtxType
            = snemo::geometry::vertex_info::CATEGORY_UNDEF;
          if (iBlockType == CALO_MAIN) {
            blockGidsPtr = &_caloBlockGids_;  // Main walls
            vtxType = snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER;
            DT_LOG_DEBUG(logPrio, "to main calo blocks");
            // Optimization for main walls:
            if (refPoint.x() > 0.0 and direction.x() <= 0.0) {
              DT_LOG_DEBUG(logPrio, "French ref point and italian direction cannot intercept the calo wall");
              continue;
            }
            if (refPoint.x() < 0.0 and direction.x() >= 0.0) {
              DT_LOG_DEBUG(logPrio, "Italian ref point and french direction cannot intercept the calo wall");
              continue;
            }
          } else if (iBlockType == CALO_XCALO) {
            blockGidsPtr = &_xcaloBlockGids_; // X-calo walls
            vtxType = snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER;
            DT_LOG_DEBUG(logPrio, "to X-calo blocks");
          }    
          DT_LOG_DEBUG(logPrio, "Number of blocks to be scanned=" << blockGidsPtr->size());
 
          // Optimization for calo main blocks:
          // We try to range the number of block to be test using a first approximation of the track impact; if any,
          // on the main wall. We then use only limited ranges of column and row IDs to search for a more precise impact
          // and identify the hit calo block.
          int16_t mainMinRow = 0;
          int16_t mainMaxRow = 12;
          int16_t mainMinCol = 0;
          int16_t mainMaxCol = 19;

          // Calo main wall extrapolation:
          bool searchMainCaloIntercept = true;
          bool useCaloWallLineExtrapolation = _use_linear_interpolation_;
          bool useCaloWallHelixExtrapolation = _use_helix_interpolation_;
          bool caloWallLineExtrapolationSuccess = false;
          bool caloWallHelixExtrapolationSuccess = false;
          if (iBlockType == CALO_MAIN and _caloWallType_ != geomtools::geom_id::INVALID_TYPE) {
            // Select the calo main wall:
            geomtools::geom_id caloWallGid(_caloWallType_, _module_id_, track_side_);
            DT_LOG_DEBUG(logPrio, "Searching helix intercept on main calo wall #" << caloWallGid << "...");
            const geomtools::geom_info & caloWallGinfo
              = geoManager().get_mapping().get_geom_info(caloWallGid);
            const geomtools::logical_volume & caloWallLog = caloWallGinfo.get_logical();
            const geomtools::i_shape_3d & caloWallShape = caloWallLog.get_shape();    
            DT_LOG_DEBUG(logPrio, "    Calo wall shape : '" << caloWallShape.get_shape_name() << "'");
            const geomtools::placement & caloWallPlacement = caloWallGinfo.get_world_placement();
            geomtools::vector_3d caloWallRefPoint;
            caloWallPlacement.mother_to_child(refPoint, caloWallRefPoint);
            geomtools::vector_3d caloWallDirection;
            caloWallPlacement.mother_to_child_direction(direction, caloWallDirection);
            
            // First attempt for a linear extrapolation:
            if (useCaloWallLineExtrapolation) {
              geomtools::face_intercept_info caloWallLineFii;
              bool success = caloWallShape.find_intercept(caloWallRefPoint,
                                                          caloWallDirection,
                                                          caloWallLineFii,
                                                          _intercept_tolerance_);          
              if (success) {
                const geomtools::vector_3d & caloWallImpact = caloWallLineFii.get_impact();
                DT_LOG_DEBUG(logPrio, "Found a line intercept on the main wall #"
                             << caloWallGid << " at " << caloWallImpact);
                geomtools::vector_3d caloWallWorldImpact;
                caloWallPlacement.child_to_mother(caloWallImpact, caloWallWorldImpact);
                DT_LOG_DEBUG(logPrio, "impact on wall=" << caloWallImpact << " (child)");
                DT_LOG_DEBUG(logPrio, "impact on wall=" << caloWallWorldImpact << " (world)");
                double yImpact = caloWallWorldImpact.y();
                double zImpact = caloWallWorldImpact.z();
                double yMin  = _main_calo_y_first_ - 0.5 * _main_calo_y_step_;
                double yStep = _main_calo_y_step_;
                double zMin  = _main_calo_z_first_ - 0.5 * _main_calo_z_step_;
                double zStep = _main_calo_z_step_;
                int16_t colIndex = (int16_t) ((yImpact - yMin) / yStep);
                int16_t rowIndex = (int16_t) ((zImpact - zMin) / zStep);
                DT_LOG_DEBUG(logPrio, "rowIndex=" << rowIndex << "   colIndex=" << colIndex);
                // We build the effective range of col/row IDs for searching impact on calo blocks:
                mainMinRow = std::max(mainMinRow, (int16_t) (rowIndex - 1));
                mainMaxRow = std::min(mainMaxRow, (int16_t) (rowIndex + 1));
                mainMinCol = std::max(mainMinCol, (int16_t) (colIndex - 1));
                mainMaxCol = std::min(mainMaxCol, (int16_t) (colIndex + 1));
                caloWallLineExtrapolationSuccess = true;
              } else {
                DT_LOG_DEBUG(logPrio, "No line intercept on the main calo wall #" << caloWallGid);
              }
            }
            
            // Attempt for a helix extrapolation:
            if (not caloWallLineExtrapolationSuccess and useCaloWallHelixExtrapolation) {
              snemo::geometry::helix_intercept hIntercept(helix,
                                                          caloWallShape,
                                                          caloWallPlacement,
                                                          _finder_step_,
                                                          _intercept_tolerance_,
                                                          hInterceptPrio);  
              snemo::geometry::helix_intercept::extrapolation_info caloWallEi;
              bool success = hIntercept.find_intercept(caloWallEi, from_bit);
              if (success) {
                DT_LOG_DEBUG(logPrio, "Found helix intercept on main calo wall #" << caloWallGid
                             << " at distance = " << caloWallEi.extrapolated_length / CLHEP::cm << " cm");
                const geomtools::vector_3d & caloWallWorldImpact = caloWallEi.fii.get_impact();
                DT_LOG_DEBUG(logPrio, "impact on calo wall=" << caloWallWorldImpact << " (world)");
                double yImpact = caloWallWorldImpact.y();
                double zImpact = caloWallWorldImpact.z();
                double yMin  = _main_calo_y_first_ - 0.5 * _main_calo_y_step_;
                double yStep = _main_calo_y_step_;
                double zMin  = _main_calo_z_first_ - 0.5 * _main_calo_z_step_;
                double zStep = _main_calo_z_step_;
                DT_LOG_DEBUG(logPrio, "yMin=" << yMin);
                DT_LOG_DEBUG(logPrio, "zMin=" << zMin);
                DT_LOG_DEBUG(logPrio, "yImpact=" << yImpact << "   zImpact=" << zImpact);
                int16_t colIndex = (int16_t) ((yImpact - yMin) / yStep);
                int16_t rowIndex = (int16_t) ((zImpact - zMin) / zStep);
                DT_LOG_DEBUG(logPrio, "rowIndex=" << rowIndex << "   colIndex=" << colIndex);
                // We build the effective range of col/row IDs for searching impact on calo blocks:
                mainMinRow = std::max(mainMinRow, (int16_t) (rowIndex - 1));
                mainMaxRow = std::min(mainMaxRow, (int16_t) (rowIndex + 1));
                mainMinCol = std::max(mainMinCol, (int16_t) (colIndex - 1));
                mainMaxCol = std::min(mainMaxCol, (int16_t) (colIndex + 1));
                caloWallHelixExtrapolationSuccess = true;
              } else {
                DT_LOG_DEBUG(logPrio, "No helix intercept on the main calo wall #" << caloWallGid);
              }
            }           
            if (caloWallLineExtrapolationSuccess or caloWallHelixExtrapolationSuccess) {
              searchMainCaloIntercept = true;
            }            
          } // if (iBlockType == CALO_MAIN...
          
          DT_LOG_DEBUG(logPrio, "mainMinRow=" << mainMinRow);
          DT_LOG_DEBUG(logPrio, "mainMaxRow=" << mainMaxRow);
          DT_LOG_DEBUG(logPrio, "mainMinCol=" << mainMinCol);
          DT_LOG_DEBUG(logPrio, "mainMaxCol=" << mainMaxCol);
          uint16_t blockCount = 0;
          bool process = true;
          if (not searchMainCaloIntercept and iBlockType == CALO_MAIN) {
            // No need to scan all main calo blocks:
            DT_LOG_DEBUG(logPrio, "No need to scan all main calo blocks");
            process = false;
          }
          if (not process) {
            DT_LOG_DEBUG(logPrio, "Skip scanning process for blocks of type=" << iBlockType);
          } else {
            DT_LOG_DEBUG(logPrio, "Start scanning process for blocks of type=" << iBlockType);
            for (uint32_t iBlock = 0; iBlock < blockGidsPtr->size(); iBlock++) {
              const geomtools::geom_id & blockGid = (*blockGidsPtr)[iBlock];
              if (blockGid.get(0) != _module_id_ or blockGid.get(1) != track_side_ ) {
                // Reject blocks in other modules and sides:
                continue;
              }
              if (iBlockType == CALO_MAIN) {
                // Optimization: reject blocks not in a confidence zone of the main wall:
                int16_t colId = blockGid.get(2);
                int16_t rowId = blockGid.get(3);
                if (colId < mainMinCol or colId > mainMaxCol) continue;
                if (rowId < mainMinRow or rowId > mainMaxRow) continue;
              }
              blockCount++;
              const geomtools::geom_info & blockGinfo = geoManager().get_mapping().get_geom_info(blockGid);
              const geomtools::logical_volume & blockLog = blockGinfo.get_logical();
              const geomtools::i_shape_3d & blockShape = blockLog.get_shape();
              const geomtools::placement & blockPlacement = blockGinfo.get_world_placement();
              const geomtools::i_shape_3d * effectiveBlockShape = &blockShape;
              if (iBlockType == CALO_MAIN) {
                DT_LOG_DEBUG(logPrio, "Main calo ID=" << blockGid);
                effectiveBlockShape = &*_effectiveCaloBlockBoxPtr_;
              } else if (iBlockType == CALO_XCALO) {
                DT_LOG_DEBUG(logPrio, "X-calo ID=" << blockGid);
                effectiveBlockShape = &*_effectiveXcaloBlockBoxPtr_;
              }
              bool caloBlockLineExtrapolationSuccess = false;
              bool caloBlockHelixExtrapolationSuccess = false;             
              if (_use_linear_interpolation_ and
                  ( ( (iBlockType == CALO_MAIN) and caloWallLineExtrapolationSuccess)
                    or (iBlockType == CALO_XCALO) ) ) {
                DT_LOG_DEBUG(logPrio, "Searching linear intercept from helix end on block #" << blockGid
                             << " from " << vertex_info::from_to_label(iFrom) << " ...");
                geomtools::vector_3d blockRefPoint;
                blockPlacement.mother_to_child(refPoint, blockRefPoint);
                geomtools::vector_3d blockDirection;
                blockPlacement.mother_to_child_direction(direction, blockDirection);
                DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(blockRefPoint) );
                DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(blockDirection) );
                DT_LOG_DEBUG(logPrio, "Block shape : '" << blockShape.get_shape_name() << "'");
                geomtools::face_intercept_info blockFii;
                bool success = effectiveBlockShape->find_intercept(blockRefPoint,
                                                                   blockDirection,
                                                                   blockFii,
                                                                   _intercept_tolerance_);
                if (success) {
                  DT_LOG_DEBUG(logPrio, "Found helix intercept (linear) on calo block #" << blockGid);
                  geomtools::vector_3d blockImpact = blockFii.get_impact();
                  // 3D-extrapolation:
                  geomtools::vector_3d blockWorldImpact;
                  blockPlacement.child_to_mother(blockImpact, blockWorldImpact);
                  blockFii.set_impact(blockWorldImpact);
                  double distRef2Impact    = (blockWorldImpact - refPoint).mag();
                  double blockExtrapolationDist = distRef2Impact - distRef2End;
                  // XY-extrapolation:
                  geomtools::vector_2d blockWorldImpact_Xy(blockWorldImpact.x(), blockWorldImpact.y());
                  double blockDistRef2Impact_Xy  = (blockWorldImpact_Xy - refPoint_Xy).mag();
                  double blockExtrapolationDist_Xy = blockDistRef2Impact_Xy - distRef2End_Xy;
                  DT_LOG_DEBUG(logPrio, "blockExtrapolationDist    = "
                               << blockExtrapolationDist / CLHEP::mm << "  mm");
                  DT_LOG_DEBUG(logPrio, "blockExtrapolationDist_Xy = "
                               << blockExtrapolationDist_Xy / CLHEP::mm << "  mm");
                  vertex_info blockVtxInfo;
                  blockVtxInfo.category = vtxType;
                  blockVtxInfo.from = iFrom;
                  blockVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                  blockVtxInfo.gid = blockGid;
                  blockVtxInfo.face_intercept = blockFii;
                  if (blockExtrapolationDist > 0.0) {
                    blockVtxInfo.distance    = blockExtrapolationDist;
                    blockVtxInfo.distance_xy = blockExtrapolationDist_Xy;
                  } else {
                    blockVtxInfo.distance    = 0.0;
                    blockVtxInfo.distance_xy = 0.0;
                  }
                  blockVtxInfo.tolerance = _intercept_tolerance_;
                  if (datatools::logger::is_debug(logPrio)) {
                    blockVtxInfo.print(std::cerr, "[debug] ");
                  }
                  if (blockVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                    //caloVertexes[iFrom].push_back(blockVtxInfo);
                    caloVertexes.push_back(blockVtxInfo);
                    caloBlockLineExtrapolationSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Line intercept is to far from the calo block");
                  }
                } else {
                  DT_LOG_DEBUG(logPrio, "No line intercept on calo block #" << blockGid);
                }
              } // if (caloWallLineExtrapolationSuccess)
              
              if (_use_helix_interpolation_ and
                  ( ( (iBlockType == CALO_MAIN)
                     and caloWallHelixExtrapolationSuccess 
                     and not caloBlockLineExtrapolationSuccess )
                    or (iBlockType == CALO_XCALO) ) ) {
                DT_LOG_DEBUG(logPrio, "Searching helix intercept on block #" << blockGid
                             << " from " << vertex_info::from_to_label(iFrom) << " ...");
                snemo::geometry::helix_intercept hBlockIntercept(helix,
                                                                 *effectiveBlockShape,
                                                                 blockPlacement,
                                                                 _finder_step_,
                                                                 _intercept_tolerance_,
                                                                 hInterceptPrio);
                DT_LOG_DEBUG(logPrio, "Block shape : '" << blockShape.get_shape_name() << "'");
                snemo::geometry::helix_intercept::extrapolation_info blockEi;
                bool success = hBlockIntercept.find_intercept(blockEi, from_bit);
                if (success) {
                  DT_LOG_DEBUG(logPrio, "Found helix intercept on calo block #" << blockGid
                               << " at distance = " << blockEi.extrapolated_length / CLHEP::cm << " cm");
                  vertex_info blockVtxInfo;
                  blockVtxInfo.category = vtxType;
                  blockVtxInfo.from = iFrom;
                  blockVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_HELIX; 
                  blockVtxInfo.gid = blockGid;
                  blockVtxInfo.face_intercept = blockEi.fii;
                  blockVtxInfo.distance = blockEi.extrapolated_length;
                  blockVtxInfo.distance_xy = blockEi.extrapolated_xy_length;
                  blockVtxInfo.tolerance = _intercept_tolerance_;
                  if (datatools::logger::is_debug(logPrio)) {
                    blockVtxInfo.print(std::cerr, "[debug] ");
                  }
                  if (blockVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                    // caloVertexes[iFrom].push_back(blockVtxInfo);
                    caloVertexes.push_back(blockVtxInfo);
                    caloBlockHelixExtrapolationSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Helix intercept is to far from the calo block");
                  }
                } else {
                  DT_LOG_DEBUG(logPrio, "No helix intercept on calo block #" << blockGid);
                } // success
              } // if (caloWallHelixExtrapolationSuccess
              DT_LOG_DEBUG(logPrio, "caloBlockLineExtrapolationSuccess  = " << std::boolalpha << caloBlockLineExtrapolationSuccess);
              DT_LOG_DEBUG(logPrio, "caloBlockHelixExtrapolationSuccess = " << std::boolalpha << caloBlockHelixExtrapolationSuccess);
            } // for (uint32_t iBlock ...
          } // process
          DT_LOG_DEBUG(logPrio, "Number of scanned blocks=" << blockCount);
        } // for (int iBlockType...

        _post_process_calo_vertex_(caloVertexes);
        for (unsigned int i = 0; i < caloVertexes.size(); i++) {
          vertexes_.push_back(caloVertexes[i]);
        }
 
        // // Find the first intercept on a calorimeter block:
        // int closestIndex = -1;
        // double closestDist = datatools::invalid_real();
        // // for (unsigned int i = 0; i < caloVertexes[iFrom].size(); i++) {
        // //   const vertex_info & caloVtxInfo = caloVertexes[iFrom][i];
        // for (unsigned int i = 0; i < caloVertexes.size(); i++) {
        //   const vertex_info & caloVtxInfo = caloVertexes[i];
        //   if (closestIndex == -1 or caloVtxInfo.distance < closestDist) {
        //     closestIndex = i;
        //     closestDist = caloVtxInfo.distance;
        //   } 
        // }
        // if (closestIndex >= 0) {
        //   //  vertexes_.push_back(caloVertexes[iFrom][closestIndex]);
        //   vertexes_.push_back(caloVertexes[closestIndex]);
        // }

        // caloVertexes[iFrom].clear();
      } // for (iFrom...)
      return;
    } // helix_trajectory_calo_intercept
  
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
      // static const int CALO_MAIN  = 0;
      // static const int CALO_XCALO = 1;
      std::set<int> blockTypes;
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER)->second) {
        blockTypes.insert(CALO_MAIN);
        DT_LOG_DEBUG(logPrio, "About to scan main calo blocks");
      }
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER)->second) {
        blockTypes.insert(CALO_XCALO);
        DT_LOG_DEBUG(logPrio, "About to scan X-calo blocks");
      }
      // snemo::geometry::vertex_info_list caloVertexes[2];
      using snemo::geometry::vertex_info;
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        snemo::geometry::vertex_info_list caloVertexes;
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        // Default configured for 'last'
        geomtools::vector_3d beginPoint = line.get_first();
        geomtools::vector_3d endPoint   = line.get_last();
        if (iFrom == vertex_info::FROM_FIRST) {
          std::swap(beginPoint, endPoint);
        }
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        geomtools::vector_2d beginPoint_Xy(beginPoint.x(), beginPoint.y());
        geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
        geomtools::vector_2d refPoint_Xy = 0.5 * (beginPoint_Xy + endPoint_Xy);
        double distRef2End = (endPoint - refPoint).mag();
        double distRef2End_Xy = (endPoint_Xy - refPoint_Xy).mag();
        const geomtools::vector_3d direction = (endPoint - beginPoint).unit();
        DT_LOG_DEBUG(logPrio, "Ref point (world)    : " << geomtools::to_xyz(refPoint) );
        DT_LOG_DEBUG(logPrio, "XY-Ref point (world) : " << geomtools::to_xy(refPoint_Xy) );
        DT_LOG_DEBUG(logPrio, "Direction (world)    : " << geomtools::to_xyz(direction) );
        DT_LOG_DEBUG(logPrio, "Distance ref->end    : " << distRef2End / CLHEP::mm << " mm");
        DT_LOG_DEBUG(logPrio, "XY-distance ref->end : " << distRef2End_Xy / CLHEP::mm << " mm");  
        for (int iBlockType : blockTypes) {
          DT_LOG_DEBUG(logPrio, "Scanning block type=" << iBlockType << "...");
          const std::vector<geomtools::geom_id> * blockGidsPtr = nullptr;
          snemo::geometry::vertex_info::category_type vtxType
            = snemo::geometry::vertex_info::CATEGORY_UNDEF;
          if (iBlockType == CALO_MAIN) {
            blockGidsPtr = &_caloBlockGids_;  // Main walls
            vtxType = snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER;
            // Optimization for main walls:
            if (refPoint.x() > 0.0 and direction.x() <= 0.0) {
              DT_LOG_DEBUG(logPrio, "French ref point and italian direction cannot intercept the source plane");
              continue;
            }
            if (refPoint.x() < 0.0 and direction.x() >= 0.0) {
              DT_LOG_DEBUG(logPrio, "Italian ref point and french direction cannot intercept the source plane");
              continue;
            }
          } else if (iBlockType == CALO_XCALO) {
            blockGidsPtr = &_xcaloBlockGids_; // X-calo walls
            vtxType = snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER;
          }    
          DT_LOG_DEBUG(logPrio, "Number of blocks to be scanned=" << blockGidsPtr->size());

          // Optimization for calo main blocks:
          // We try to range the number of block to be test using a first approximation of the track impact; if any,
          // on the main wall. We then use only limited ranges of column and row IDs to search for a more precise impact
          // and identify the hit calo block.
          int16_t mainMinRow = 0;
          int16_t mainMaxRow = 12;
          int16_t mainMinCol = 0;
          int16_t mainMaxCol = 19;
          bool searchMainCaloIntercept = true;
          if (iBlockType == CALO_MAIN and _caloWallType_ != geomtools::geom_id::INVALID_TYPE) {
            // Select the calo main wall:
            geomtools::geom_id caloWallGid(_caloWallType_, _module_id_, track_side_);
            DT_LOG_DEBUG(logPrio, "Searching line intercept on main calo wall #" << caloWallGid << "...");
            const geomtools::geom_info & caloWallGinfo = geoManager().get_mapping().get_geom_info(caloWallGid);
            const geomtools::logical_volume & caloWallLog   = caloWallGinfo.get_logical();
            const geomtools::i_shape_3d & caloWallShape     = caloWallLog.get_shape();    
            const geomtools::placement  & caloWallPlacement = caloWallGinfo.get_world_placement();
            geomtools::vector_3d caloWallRefPoint;
            caloWallPlacement.mother_to_child(refPoint, caloWallRefPoint);
            geomtools::vector_3d caloWallDirection;
            caloWallPlacement.mother_to_child_direction(direction, caloWallDirection);
            // Check if the track hit the submodule:
            geomtools::face_intercept_info caloWallFii;
            // Linear extrapolation:
            bool success = caloWallShape.find_intercept(caloWallRefPoint,
                                                        caloWallDirection,
                                                        caloWallFii,
                                                        _intercept_tolerance_);          
            if (success) {
              const geomtools::vector_3d & caloWallImpact = caloWallFii.get_impact();
              DT_LOG_DEBUG(logPrio, "Found an intercept on the main wall #" << caloWallGid << " at " << caloWallImpact);
              geomtools::vector_3d caloWallWorldImpact;
              caloWallPlacement.child_to_mother(caloWallImpact, caloWallWorldImpact);
              DT_LOG_DEBUG(logPrio, "impact on wall=" << caloWallImpact << " (child)");
              DT_LOG_DEBUG(logPrio, "impact on wall=" << caloWallWorldImpact << " (world)");
              double yImpact = caloWallWorldImpact.y();
              double zImpact = caloWallWorldImpact.z();
              double yMin  = _main_calo_y_first_ - 0.5 * _main_calo_y_step_;
              double yStep = _main_calo_y_step_;
              double zMin  = _main_calo_z_first_ - 0.5 * _main_calo_z_step_;
              double zStep = _main_calo_z_step_;
              DT_LOG_DEBUG(logPrio, "yMin=" << yMin);
              DT_LOG_DEBUG(logPrio, "zMin=" << zMin);
              DT_LOG_DEBUG(logPrio, "yImpact=" << yImpact << "   zImpact=" << zImpact);
              int16_t colIndex = (int16_t) ((yImpact - yMin) / yStep);
              int16_t rowIndex = (int16_t) ((zImpact - zMin) / zStep);
              DT_LOG_DEBUG(logPrio, "rowIndex=" << rowIndex << "   colIndex=" << colIndex);
              // We build the effective range of col/row IDs for searching impact on calo blocks:
              mainMinRow = std::max(mainMinRow, (int16_t) (rowIndex - 1));
              mainMaxRow = std::min(mainMaxRow, (int16_t) (rowIndex + 1));
              mainMinCol = std::max(mainMinCol, (int16_t) (colIndex - 1));
              mainMaxCol = std::min(mainMaxCol, (int16_t) (colIndex + 1));
            } else {
              DT_LOG_DEBUG(logPrio, "No line intercept on the main wall #" << caloWallGid);
            }
            searchMainCaloIntercept = success;
          }
          DT_LOG_DEBUG(logPrio, "mainMinRow=" << mainMinRow);
          DT_LOG_DEBUG(logPrio, "mainMaxRow=" << mainMaxRow);
          DT_LOG_DEBUG(logPrio, "mainMinCol=" << mainMinCol);
          DT_LOG_DEBUG(logPrio, "mainMaxCol=" << mainMaxCol);
          uint16_t blockCount = 0;
          bool process = true;
          if (not searchMainCaloIntercept and iBlockType == CALO_MAIN) {
            // No need to scan all main calo blocks
            DT_LOG_DEBUG(logPrio, "No need to scan all main calo blocks");
            process = false;
          }
          if (not process) {
            DT_LOG_DEBUG(logPrio, "Skip scanning process for blocks of type=" << iBlockType);
          } else {
            for (uint32_t iBlock = 0; iBlock < blockGidsPtr->size(); iBlock++) {
              const geomtools::geom_id & blockGid = (*blockGidsPtr)[iBlock];
              if (blockGid.get(0) != _module_id_ or blockGid.get(1) != track_side_ ) {
                // Reject blocks in other modules and sides:
                continue;
              }
              if (iBlockType == CALO_MAIN) {
                // Reject blocks not in a confident zone of the main wall:
                int16_t colId = blockGid.get(2);
                int16_t rowId = blockGid.get(3);
                if (colId < mainMinCol or colId > mainMaxCol) continue;
                if (rowId < mainMinRow or rowId > mainMaxRow) continue;
              }
              blockCount++;
              DT_LOG_DEBUG(logPrio, "Searching line intercept on block #" << blockGid << "...");
              const geomtools::geom_info  & blockGinfo     = geoManager().get_mapping().get_geom_info(blockGid);
              const geomtools::logical_volume & blockLog   = blockGinfo.get_logical();
              const geomtools::i_shape_3d & blockShape     = blockLog.get_shape();
              const geomtools::placement & blockPlacement  = blockGinfo.get_world_placement();
              const geomtools::i_shape_3d * effectiveBlockShape =  &blockShape;
              if (iBlockType == CALO_MAIN) {
                effectiveBlockShape = &*_effectiveCaloBlockBoxPtr_;
              } else if (iBlockType == CALO_XCALO) {
                effectiveBlockShape = &*_effectiveXcaloBlockBoxPtr_;
              }
              geomtools::vector_3d blockRefPoint;
              blockPlacement.mother_to_child(refPoint, blockRefPoint);
              geomtools::vector_3d blockDirection;
              blockPlacement.mother_to_child_direction(direction, blockDirection);
              // DT_LOG_DEBUG(logPrio, "    Block GID   : " << blockGid );
              DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(blockRefPoint) );
              DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(blockDirection) );
              DT_LOG_DEBUG(logPrio, "Block shape : '" << blockShape.get_shape_name() << "'");
              geomtools::face_intercept_info blockFii;
              bool success = effectiveBlockShape->find_intercept(blockRefPoint, blockDirection, blockFii, _intercept_tolerance_);
              if (success) {
                DT_LOG_DEBUG(logPrio, "Found line intercept on calo block #" << blockGid);
                geomtools::vector_3d blockImpact = blockFii.get_impact();
                // 3D-extrapolation:
                geomtools::vector_3d blockWorldImpact;
                blockPlacement.child_to_mother(blockImpact, blockWorldImpact);
                blockFii.set_impact(blockWorldImpact);
                double distRef2Impact    = (blockWorldImpact - refPoint).mag();
                double blockExtrapolationDist = distRef2Impact - distRef2End;
                // XY-extrapolation:
                geomtools::vector_2d blockWorldImpact_Xy(blockWorldImpact.x(), blockWorldImpact.y());
                double blockDistRef2Impact_Xy  = (blockWorldImpact_Xy - refPoint_Xy).mag();
                double blockExtrapolationDist_Xy = blockDistRef2Impact_Xy - distRef2End_Xy;
                DT_LOG_DEBUG(logPrio, "blockExtrapolationDist    = " << blockExtrapolationDist / CLHEP::mm << "  mm");
                DT_LOG_DEBUG(logPrio, "blockExtrapolationDist_Xy = " << blockExtrapolationDist_Xy / CLHEP::mm << "  mm");
                vertex_info blockVtxInfo;
                blockVtxInfo.category = vtxType;
                blockVtxInfo.from = iFrom;
                blockVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                blockVtxInfo.gid = blockGid;
                blockVtxInfo.face_intercept = blockFii;
                if (blockExtrapolationDist > 0.0) {
                  blockVtxInfo.distance    = blockExtrapolationDist;
                  blockVtxInfo.distance_xy = blockExtrapolationDist_Xy;
                } else {
                  blockVtxInfo.distance    = 0.0;
                  blockVtxInfo.distance_xy = 0.0;
                }
                blockVtxInfo.tolerance = _intercept_tolerance_;
                if (datatools::logger::is_debug(logPrio)) {
                  blockVtxInfo.print(std::cerr, "[debug] ");
                }
                if (blockVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                  // caloVertexes[iFrom].push_back(blockVtxInfo);
                  caloVertexes.push_back(blockVtxInfo);
                } else {
                  DT_LOG_DEBUG(logPrio, "Line intercept is to far from the calo block");
                }
              } else {
                DT_LOG_DEBUG(logPrio, "No line intercept on calo block #" << blockGid);
              }
            } // for (uint32_t iBlock = 0...)
          } // process
          DT_LOG_DEBUG(logPrio, "Number of scanned blocks=" << blockCount);
        } // for (int iBlockType : blockTypes)  

        _post_process_calo_vertex_(caloVertexes);
        for (unsigned int i = 0; i < caloVertexes.size(); i++) {
          vertexes_.push_back(caloVertexes[i]);
        }

        // // Find the first intercept on a calorimeter block:
        // int closestIndex = -1;
        // double closestDist = datatools::invalid_real();
        // // for (unsigned int i = 0; i < caloVertexes[iFrom].size(); i++) {
        // //   const vertex_info & caloVtxInfo = caloVertexes[iFrom][i];
        // for (unsigned int i = 0; i < caloVertexes.size(); i++) {
        //   const vertex_info & caloVtxInfo = caloVertexes[i];
        //   if (closestIndex == -1 or caloVtxInfo.distance < closestDist) {
        //     closestIndex = i;
        //     closestDist = caloVtxInfo.distance;
        //   } 
        // }
        // if (closestIndex >= 0) {
        //   // vertexes_.push_back(caloVertexes[iFrom][closestIndex]);
        //   vertexes_.push_back(caloVertexes[closestIndex]);
        // }
        // // caloVertexes[iFrom].clear();
      }
      return;
    } // line_trajectory_calo_intercept

  
    void vertex_extrapolation_driver::line_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                       const snemo::datamodel::line_trajectory_pattern & line_traj_,
                                                                       uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      // logPrio = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logPrio, "Search line intercepts on source elements from mask = " << from_mask_);
      vertexes_.clear();
      const geomtools::line_3d & line = line_traj_.get_segment();
      using snemo::geometry::vertex_info;
      bool use_foils = false;
      bool use_calib_src = false;
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL)->second) {
        use_foils = true;
        DT_LOG_DEBUG(logPrio, "About to scan source foils and pads");
      }
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE)->second) {
        use_calib_src = true;
        DT_LOG_DEBUG(logPrio, "About to scan calibration source");
      }
      DT_LOG_DEBUG(logPrio, "Loop on track's line end points");
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        std::vector<vertex_info> srcVertexes;
        DT_LOG_DEBUG(logPrio, "From : " << iFrom);
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        // Default configured for 'last'
        geomtools::vector_3d beginPoint = line.get_first();
        geomtools::vector_3d endPoint   = line.get_last();
        if (iFrom == vertex_info::FROM_FIRST) {
          std::swap(beginPoint, endPoint);
        }
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        geomtools::vector_2d beginPoint_Xy(beginPoint.x(), beginPoint.y());
        geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
        geomtools::vector_2d refPoint_Xy = 0.5 * (beginPoint_Xy + endPoint_Xy);
        double distRef2End = (endPoint - refPoint).mag();
        double distRef2End_Xy = (endPoint_Xy - refPoint_Xy).mag();
        const geomtools::vector_3d direction = (endPoint - beginPoint).unit();
        DT_LOG_DEBUG(logPrio, "Ref point (world)    : " << geomtools::to_xyz(refPoint) );
        DT_LOG_DEBUG(logPrio, "XY-Ref point (world) : " << geomtools::to_xy(refPoint_Xy) );
        DT_LOG_DEBUG(logPrio, "Direction (world)    : " << geomtools::to_xyz(direction) );
        DT_LOG_DEBUG(logPrio, "Distance ref->end    : " << distRef2End / CLHEP::mm << " mm");
        DT_LOG_DEBUG(logPrio, "XY-distance ref->end : " << distRef2End_Xy / CLHEP::mm << " mm");
        // Optimization:
        {
          if (refPoint.x() > 0.0 and direction.x() >= 0.0) {
            DT_LOG_DEBUG(logPrio, "French ref point and french direction cannot intercept the source plane");
            continue;
          }
          if (refPoint.x() < 0.0 and direction.x() <= 0.0) {
            DT_LOG_DEBUG(logPrio, "Italian ref point and italian direction cannot intercept the source plane");
            continue;
          }
        }
        int16_t minStripId = _sourceStripMinId_;
        int16_t maxStripId = _sourceStripMaxId_;
        bool sourceSubmoduleInterceptSuccess = true;
        
        // Optimization for strip scanning:
        if (use_foils or use_calib_src) {
          DT_LOG_DEBUG(logPrio, "Searching line intercept on source submodule #" << _sourceSubmoduleGid_ << "...");
          const geomtools::geom_info & srcSubmodGinfo      = geoManager().get_mapping().get_geom_info(_sourceSubmoduleGid_);
          const geomtools::logical_volume & srcSubmodLog   = srcSubmodGinfo.get_logical();
          const geomtools::i_shape_3d & srcSubmodShape     = srcSubmodLog.get_shape();         
          const geomtools::placement & srcSubmodPlacement = srcSubmodGinfo.get_world_placement();
          DT_LOG_DEBUG(logPrio, "Source submodule shape = " << srcSubmodShape.get_shape_name());
          DT_LOG_DEBUG(logPrio, "Source submodule world placement = " << srcSubmodPlacement);
          
          geomtools::vector_3d srcSubmodRefPoint;
          srcSubmodPlacement.mother_to_child(refPoint, srcSubmodRefPoint);
          geomtools::vector_3d srcSubmodDirection;
          srcSubmodPlacement.mother_to_child_direction(direction, srcSubmodDirection);
          // Check if the track hit the submodule:
          geomtools::face_intercept_info srcSubmodFii;
          bool success = srcSubmodShape.find_intercept(srcSubmodRefPoint,
                                                       srcSubmodDirection,
                                                       srcSubmodFii,
                                                       _intercept_tolerance_);          
          if (success) {
            DT_LOG_DEBUG(logPrio, "Found an intercept on the source submodule #" << _sourceSubmoduleGid_ << " at " << srcSubmodFii.get_impact());
            const geomtools::vector_3d & impact = srcSubmodFii.get_impact();
            double yImpact = impact.y();
            int32_t minId = 100000;
            int32_t maxId = -100000;
            for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
              const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
              const geomtools::geom_info & sourceStripGinfo = geoManager().get_mapping().get_geom_info(sourceStripGid);
              const geomtools::placement & sourceStripPlacement = sourceStripGinfo.get_world_placement();
              double yStrip = sourceStripPlacement.get_translation().y();
              if (std::abs(yStrip - yImpact) < _max_source_extrapolation_xy_length_) {
                int32_t sourceStripId = (int32_t) sourceStripGid.get(1);
                if (sourceStripId > maxId) {
                  maxId = sourceStripId;
                }
                if (sourceStripId < minId) {
                  minId = sourceStripId;
                } 
              }
            }
            minStripId = std::max(minStripId, (int16_t) (minId - 1));
            maxStripId = std::min(maxStripId, (int16_t) (maxId + 1));
          } else {
            DT_LOG_DEBUG(logPrio, "No intercept on the source submodule #" << _sourceSubmoduleGid_);
          }
          sourceSubmoduleInterceptSuccess = success;
        }
        DT_LOG_DEBUG(logPrio, "minStripId=" << minStripId);
        DT_LOG_DEBUG(logPrio, "maxStripId=" << maxStripId);

        // Scan strips/pads...
        if (use_foils and sourceSubmoduleInterceptSuccess) {
          DT_LOG_DEBUG(logPrio, "Scanning strips...");
          for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
            const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
            DT_LOG_DEBUG(logPrio, "  Source strip GID : " << sourceStripGid);
            auto sourceStripId = sourceStripGid.get(1);
            if (sourceStripGid.get(0) != _module_id_) { 
              continue;
            }
            if ((int32_t) sourceStripId < minStripId or (int32_t) sourceStripId > maxStripId) { 
              continue;
            } 
            DT_LOG_DEBUG(logPrio, "Searching line intercept on strip #" << sourceStripId);
            const geomtools::geom_info & sourceStripGinfo     = geoManager().get_mapping().get_geom_info(sourceStripGid);
            const geomtools::logical_volume & sourceStripLog  = sourceStripGinfo.get_logical();
            const geomtools::i_shape_3d & sourceStripShape    = sourceStripLog.get_shape();
            const geomtools::placement & sourceStripPlacement = sourceStripGinfo.get_world_placement();
            geomtools::vector_3d srcStripRefPoint;
            sourceStripPlacement.mother_to_child(refPoint, srcStripRefPoint);
            geomtools::vector_3d srcStripDirection;
            sourceStripPlacement.mother_to_child_direction(direction, srcStripDirection);
            DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(srcStripRefPoint) );
            DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(srcStripDirection) );
            DT_LOG_DEBUG(logPrio, "Source strip shape  : '" << sourceStripShape.get_shape_name() << "'");
            DT_LOG_DEBUG(logPrio, "Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
            geomtools::face_intercept_info srcStripFii;
            bool success = sourceStripShape.find_intercept(srcStripRefPoint,
                                                           srcStripDirection,
                                                           srcStripFii,
                                                           _intercept_tolerance_);
            if (! success) {
              DT_LOG_DEBUG(logPrio, "No intersection with the strip #" << sourceStripId << "! No chance to intersect a pad.");
              // No intersection with the strip volume: no chance to intersect a pad ! We give up.
              continue;
            }
            DT_LOG_DEBUG(logPrio, "Found line intercept on strip #" << sourceStripId);
            geomtools::vector_3d srcStripImpact = srcStripFii.get_impact();
            geomtools::vector_3d srcStripWorldImpact;
            sourceStripPlacement.child_to_mother(srcStripImpact, srcStripWorldImpact);
            srcStripFii.set_impact(srcStripWorldImpact);
            // Search intercept on daughter pad volumes:
            for (uint32_t sourcePadGidIndex = 0; sourcePadGidIndex < _sourcePadGids_.size(); sourcePadGidIndex++) {
              const geomtools::geom_id & sourcePadGid = _sourcePadGids_[sourcePadGidIndex];
              if (sourcePadGid.get(1) != sourceStripId) {
                /// Reject pads in other strips:
                continue;
              }
              uint32_t sourcePadId = sourcePadGid.get(2);
              DT_LOG_DEBUG(logPrio, "Searching line intercept on pad #" << sourcePadGid << " in strip #" << sourceStripGid);
              const geomtools::geom_info  & sourcePadGinfo     = geoManager().get_mapping().get_geom_info(sourcePadGid);
              const geomtools::logical_volume & sourcePadLog   = sourcePadGinfo.get_logical();
              const geomtools::i_shape_3d & sourcePadShape     = sourcePadLog.get_shape();
              const geomtools::placement  & sourcePadPlacement = sourcePadGinfo.get_world_placement();
              geomtools::vector_3d srcPadRefPoint;
              sourcePadPlacement.mother_to_child(refPoint, srcPadRefPoint);
              geomtools::vector_3d srcPadDirection;
              sourcePadPlacement.mother_to_child_direction(direction, srcPadDirection);
              DT_LOG_DEBUG(logPrio, "Ref point        : " << geomtools::to_xyz(srcPadRefPoint) );
              DT_LOG_DEBUG(logPrio, "Direction        : " << geomtools::to_xyz(srcPadDirection) );
              DT_LOG_DEBUG(logPrio, "Source pad shape : '" << sourcePadShape.get_shape_name() << "'");

              // Attempt to find an intercept on a source pad:
              vertex_info padVtxInfo;
              bool sourcePadInterceptSuccess = false;
              const bool useSourcePadLineExtrapolation = true;
              if (useSourcePadLineExtrapolation) {
                geomtools::face_intercept_info srcPadFii;
                success = sourcePadShape.find_intercept(srcPadRefPoint,
                                                        srcPadDirection,
                                                        srcPadFii,
                                                        _intercept_tolerance_);
                if (success) {
                  geomtools::vector_3d srcPadImpact = srcPadFii.get_impact();
                  // 3D-extrapolation:
                  geomtools::vector_3d srcPadWorldImpact;
                  sourcePadPlacement.child_to_mother(srcPadImpact, srcPadWorldImpact);
                  DT_LOG_DEBUG(logPrio, "Found line intercept on pad #"
                               << sourcePadId << " in strip #" << sourceStripId << " at " << geomtools::to_xyz(srcPadWorldImpact));
                  srcPadFii.set_impact(srcPadWorldImpact);
                  double srcPadDistRef2Impact = (srcPadWorldImpact - refPoint).mag();
                  double srcPadExtrapolationDist = srcPadDistRef2Impact - distRef2End;
                  DT_LOG_DEBUG(logPrio, "srcPadExtrapolationDist    = " << srcPadExtrapolationDist / CLHEP::mm << "  mm");
                  // XY-extrapolation:
                  geomtools::vector_2d srcPadWorldImpact_Xy(srcPadWorldImpact.x(), srcPadWorldImpact.y());
                  double srcPadDistRef2Impact_Xy = (srcPadWorldImpact_Xy - refPoint_Xy).mag();
                  double srcPadExtrapolationDist_Xy = srcPadDistRef2Impact_Xy - distRef2End_Xy;
                  DT_LOG_DEBUG(logPrio, "srcPadExtrapolationDist_Xy = " << srcPadExtrapolationDist_Xy / CLHEP::mm << "  mm");
                  // Result:
                  padVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                  padVtxInfo.from = iFrom;
                  padVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_HELIX;
                  padVtxInfo.gid = sourcePadGid;
                  padVtxInfo.face_intercept = srcPadFii;
                  if (srcPadExtrapolationDist > 0.0) {
                    padVtxInfo.distance    = srcPadExtrapolationDist;
                    padVtxInfo.distance_xy = srcPadExtrapolationDist_Xy;
                  } else {
                    padVtxInfo.distance = 0.0;
                    padVtxInfo.distance_xy = 0.0;
                  }
                  padVtxInfo.tolerance = _intercept_tolerance_;
                  sourcePadInterceptSuccess = true;
                  if (datatools::logger::is_debug(logPrio)) {
                    padVtxInfo.print(std::cerr, "[debug] ");
                  }
                  if (padVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                    sourcePadInterceptSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Line intercept is to far from source pad #" << sourcePadGid);
                  } // extrapolation distance check
                } else {
                  DT_LOG_DEBUG(logPrio, "No line intercept on source pad #" << sourcePadGid);
                }
              }
              
              if (not sourcePadInterceptSuccess) {
                DT_LOG_DEBUG(logPrio, "Give up with pad #" << sourcePadGid << " in strip #" << sourceStripGid);
                continue;
              }

              bool sourcePadBulkInterceptSuccess = false;
              bool hasCandidatePadBulk = false;
              if (_sourcePadBulkGids_.size() > 0) {
                // Search intercept on daughter pad bulk volumes:
                for (uint32_t sourcePadBulkGidIndex = 0; sourcePadBulkGidIndex < _sourcePadBulkGids_.size(); sourcePadBulkGidIndex++) {
                  const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[sourcePadBulkGidIndex];
                  DT_LOG_DEBUG(logPrio, "Scanning the pad bulk #" << sourcePadBulkGid.get(2) << " in strip #" << sourcePadBulkGid.get(1));
                  if (sourcePadBulkGid.get(1) != sourceStripId or sourcePadBulkGid.get(2) != sourcePadId ) {
                    // Reject pad bulks in other strips and pads:
                    continue;
                  }
                  hasCandidatePadBulk = true; 
                  DT_LOG_DEBUG(logPrio, "Searching line intercept on pad bulk #" << sourcePadId << " in strip #" << sourceStripId);
                  const geomtools::geom_info  & sourcePadBulkGinfo  = geoManager().get_mapping().get_geom_info(sourcePadBulkGid);
                  const geomtools::logical_volume & sourcePadBulkLog   = sourcePadBulkGinfo.get_logical();
                  const geomtools::i_shape_3d & sourcePadBulkShape     = sourcePadBulkLog.get_shape();
                  const geomtools::placement  & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
                  geomtools::vector_3d srcPadBulkRefPoint;
                  sourcePadBulkPlacement.mother_to_child(refPoint, srcPadBulkRefPoint);
                  geomtools::vector_3d srcPadBulkDirection;
                  sourcePadBulkPlacement.mother_to_child_direction(direction, srcPadBulkDirection);
                  DT_LOG_DEBUG(logPrio, "Ref point : " << geomtools::to_xyz(srcPadBulkRefPoint) );
                  DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(srcPadBulkDirection) );
                  DT_LOG_DEBUG(logPrio, "Source pad bulk shape : '" << sourcePadBulkShape.get_shape_name() << "'");
                  geomtools::face_intercept_info srcPadBulkFii;
                  success = sourcePadBulkShape.find_intercept(srcPadBulkRefPoint,
                                                              srcPadBulkDirection,
                                                              srcPadBulkFii,
                                                              _intercept_tolerance_);
                  if (! success) {
                    DT_LOG_DEBUG(logPrio, "Give up strip/pad = " << sourceStripId << '/' << sourcePadId);
                    continue;
                  }
                  geomtools::vector_3d srcPadBulkImpact = srcPadBulkFii.get_impact();
                  // 3D-extrapolation:
                  geomtools::vector_3d srcPadBulkWorldImpact;
                  sourcePadBulkPlacement.child_to_mother(srcPadBulkImpact, srcPadBulkWorldImpact);
                  DT_LOG_DEBUG(logPrio, "Found line intercept on pad bulk #" << sourcePadGid
                               << " at " << geomtools::to_xyz(srcPadBulkWorldImpact));
                  srcPadBulkFii.set_impact(srcPadBulkWorldImpact);
                  double srcPadBulkDistRef2Impact    = (srcPadBulkWorldImpact - refPoint).mag();
                  double srcPadBulkExtrapolationDist = srcPadBulkDistRef2Impact - distRef2End;
                  // XY extrapolation:
                  geomtools::vector_2d srcPadBulkImpact_Xy(srcPadBulkWorldImpact.x(), srcPadBulkWorldImpact.y());
                  double srcPadBulkDistRef2Impact_Xy    = (srcPadBulkImpact_Xy - refPoint_Xy).mag();
                  double srcPadBulkExtrapolationDist_Xy = srcPadBulkDistRef2Impact_Xy - distRef2End_Xy;
                  // Result:
                  vertex_info padBulkVtxInfo;
                  padBulkVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                  padBulkVtxInfo.from = iFrom;
                  padBulkVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                  padBulkVtxInfo.gid = sourcePadBulkGid;
                  padBulkVtxInfo.face_intercept = srcPadBulkFii;
                  if (srcPadBulkExtrapolationDist > 0.0) {
                    padBulkVtxInfo.distance    = srcPadBulkDistRef2Impact;
                    padBulkVtxInfo.distance_xy = srcPadBulkExtrapolationDist_Xy;
                  } else {
                    padBulkVtxInfo.distance    = 0.0;
                    padBulkVtxInfo.distance_xy = 0.0;
                  }
                  DT_LOG_DEBUG(logPrio, "srcPadBulkExtrapolationDist    = " << srcPadBulkExtrapolationDist / CLHEP::mm << "  mm");
                  DT_LOG_DEBUG(logPrio, "srcPadBulkExtrapolationDist_Xy = " << srcPadBulkExtrapolationDist_Xy / CLHEP::mm << "  mm");
                  padBulkVtxInfo.tolerance = _intercept_tolerance_;
                  if (padBulkVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                    DT_LOG_DEBUG(logPrio, "add padBulkVtxInfo");
                    srcVertexes.push_back(padBulkVtxInfo);
                    sourcePadBulkInterceptSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Line intercept is to far from the pad bulk");
                  }
                } // for (uint32_t k = 0; k < _sourcePadBulkGids_.size()...
              } else {
                DT_LOG_DEBUG(logPrio, "No source Pad Bulk (SNRS) to be scanned");
              } // scan source pad bulk

              if (sourcePadInterceptSuccess) { 
                if (not sourcePadBulkInterceptSuccess) {
                  if (hasCandidatePadBulk) {
                    // A bulk was expected but missed, so we are in a gap:
                    padVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP;               
                  }
                  // Validate pad intercept if no candidate pad bulk
                  DT_LOG_DEBUG(logPrio, "add padVtxInfo");
                  srcVertexes.push_back(padVtxInfo);
                }
              }
              
            } // for (uint32_t j = 0; j < _sourcePadGids_.size()...   
          } // for (uint32_t iStrip = 0;...
        } // if (use_foils and sourceSubmoduleInterceptSuccess)

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
            DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(srcCalibrationSpotRefPoint) );
            DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(srcCalibrationSpotDirection) );
            DT_LOG_DEBUG(logPrio, "Calibration spot shape : '" << sourceCalibrationSpotShape.get_shape_name() << "'");
            geomtools::face_intercept_info srcCalibrationSpotFii;
            bool success = sourceCalibrationSpotShape.find_intercept(srcCalibrationSpotRefPoint,
                                                                     srcCalibrationSpotDirection,
                                                                     srcCalibrationSpotFii,
                                                                     _intercept_tolerance_);
            if (! success) {
              DT_LOG_DEBUG(logPrio, "Give up calibration source");
              continue;
            }
            DT_LOG_DEBUG(logPrio, "Found line intercept on calibration source spot #" << sourceCalibrationSpotGid);
            geomtools::vector_3d srcCalibrationSpotImpact = srcCalibrationSpotFii.get_impact();
            geomtools::vector_3d srcCalibrationSpotWorldImpact;
            sourceCalibrationSpotPlacement.child_to_mother(srcCalibrationSpotImpact, srcCalibrationSpotWorldImpact);
            srcCalibrationSpotFii.set_impact(srcCalibrationSpotWorldImpact);
            double srcCalibrationSpotDistRef2Impact = (srcCalibrationSpotWorldImpact - refPoint).mag();
            double srcCalibrationSpotExtrapolationDist = srcCalibrationSpotDistRef2Impact - distRef2End;
            vertex_info calibrationSpotVtxInfo;
            calibrationSpotVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE;
            calibrationSpotVtxInfo.from = iFrom;
            calibrationSpotVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
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
              DT_LOG_DEBUG(logPrio, "Line intercept is to far from the caibration source spot");
            }
          }
        } // if (use_calib_src)

        _post_process_source_vertex_(srcVertexes);  
        for (unsigned int i = 0; i < srcVertexes.size(); i++) {
          vertexes_.push_back(srcVertexes[i]);
        }
        //  int closestIndex = -1;
        // double closestDist = datatools::invalid_real();
        // for (unsigned int i = 0; i < srcVertexes.size(); i++) {
        //   const vertex_info & vtxInfo = srcVertexes[i];
        //   if (closestIndex == -1 or vtxInfo.distance < closestDist) {
        //     closestIndex = i;
        //     closestDist = vtxInfo.distance;
        //   }
        // }
        // if (closestIndex >= 0) {
        //   vertexes_.push_back(srcVertexes[closestIndex]);
        // }
        // srcVertexes.clear();
      } // for (int iFrom=...) 
      return;
    } // line_trajectory_source_intercept

    
    void vertex_extrapolation_driver::helix_trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                        const snemo::datamodel::helix_trajectory_pattern & helix_traj_,
                                                                        uint32_t from_mask_) const
    {
      datatools::logger::priority logPrio = logPriority_;
      DT_LOG_DEBUG(logPrio, "Skip search of helix intercepts on source elements from mask = " << from_mask_);     
      datatools::logger::priority hInterceptPrio = datatools::logger::PRIO_ERROR;
      // logPrio = datatools::logger::PRIO_DEBUG;
      DT_LOG_DEBUG(logPrio, "Search helix intercepts on source elements from mask = " << from_mask_);
      vertexes_.clear();
      const geomtools::helix_3d & helix = helix_traj_.get_helix();
      using snemo::geometry::vertex_info;
      bool use_foils = false;
      bool use_calib_src = false;
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL)->second) {
        use_foils = true;
      }
      if (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE)->second) {
        use_calib_src = true;
      }
      DT_LOG_DEBUG(logPrio, "Loop on track's helix end points");
      for (int iFrom = vertex_info::FROM_FIRST; iFrom <= vertex_info::FROM_LAST; iFrom++) {
        std::vector<vertex_info> srcVertexes;
        // Select the end point:
        if ((iFrom == vertex_info::FROM_FIRST) && not (from_mask_ & vertex_info::FROM_FIRST_BIT)) continue;
        if ((iFrom == vertex_info::FROM_LAST)  && not (from_mask_ & vertex_info::FROM_LAST_BIT)) continue;
        DT_LOG_DEBUG(logPrio, "from " << vertex_info::from_to_label(iFrom));
        vertex_info::from_bit_type from_bit = vertex_info::FROM_FIRST_BIT;
        // Default configured for 'first':
        geomtools::vector_3d beginPoint     = helix_traj_.get_last();
        geomtools::vector_3d beginDirection = helix_traj_.get_last_direction();
        geomtools::vector_3d endPoint       = helix_traj_.get_first();
        geomtools::vector_3d endDirection   = -helix_traj_.get_first_direction();
        if (iFrom == vertex_info::FROM_LAST) {
          from_bit = vertex_info::FROM_LAST_BIT;
          std::swap(beginPoint, endPoint);
          std::swap(beginDirection, endDirection);
        }
        
        /* 
         *    from_bit == FROM_LAST
         *
         *           Begin    End
         *             o-------o-----> endDirection
         *                -   Last
         *              - 
         *            -
         *           -
         *          o First
         *         /
         *        L
         *     beginDirection
         *
         */          
        beginPoint = endPoint - _finder_step_ * endDirection;
        geomtools::vector_3d refPoint = 0.5 * (beginPoint + endPoint);
        geomtools::vector_2d beginPoint_Xy(beginPoint.x(), beginPoint.y());
        geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
        geomtools::vector_2d refPoint_Xy = 0.5 * (beginPoint_Xy + endPoint_Xy);
        double distRef2End = (endPoint - refPoint).mag();
        double distRef2End_Xy = (endPoint_Xy - refPoint_Xy).mag();
        const geomtools::vector_3d direction = endDirection;
        DT_LOG_DEBUG(logPrio, "End point (world)    : " << geomtools::to_xyz(endPoint) );
        DT_LOG_DEBUG(logPrio, "Ref point (world)    : " << geomtools::to_xyz(refPoint) );
        DT_LOG_DEBUG(logPrio, "XY-Ref point (world) : " << geomtools::to_xy(refPoint_Xy) );
        DT_LOG_DEBUG(logPrio, "Direction (world)    : " << geomtools::to_xyz(direction) );
        DT_LOG_DEBUG(logPrio, "Distance ref->end    : " << distRef2End / CLHEP::mm << " mm");
        DT_LOG_DEBUG(logPrio, "XY-distance ref->end : " << distRef2End_Xy / CLHEP::mm << " mm");
        // Optimization:
        {
          if (refPoint.x() > 0.0 and direction.x() >= 0.0) {
            DT_LOG_DEBUG(logPrio, "French ref point and french direction cannot intercept the source plane");
            continue;
          }
          if (refPoint.x() < 0.0 and direction.x() <= 0.0) {
            DT_LOG_DEBUG(logPrio, "Italian ref point and italian direction cannot intercept the source plane");
            continue;
          }
        }
        int16_t minStripId = _sourceStripMinId_;
        int16_t maxStripId = _sourceStripMaxId_;
        bool sourceSubmoduleInterceptSuccess = true;
        
        // Optimization for strip scanning:
        if (use_foils or use_calib_src) {
          DT_LOG_DEBUG(logPrio, "Searching line intercept on source submodule #" << _sourceSubmoduleGid_ << "...");
          const geomtools::geom_info & srcSubmodGinfo      = geoManager().get_mapping().get_geom_info(_sourceSubmoduleGid_);
          const geomtools::logical_volume & srcSubmodLog   = srcSubmodGinfo.get_logical();
          const geomtools::i_shape_3d & srcSubmodShape     = srcSubmodLog.get_shape();         
          const geomtools::placement & srcSubmodPlacement = srcSubmodGinfo.get_world_placement();
          DT_LOG_DEBUG(logPrio, "Source submodule shape = " << srcSubmodShape.get_shape_name());
          DT_LOG_DEBUG(logPrio, "Source submodule world placement = " << srcSubmodPlacement);

          bool useSourceSubmoduleLineExtrapolation = _use_linear_interpolation_;
          bool useSourceSubmoduleHelixExtrapolation = _use_helix_interpolation_;
          bool sourceSubmoduleLineExtrapolationSuccess = false;
          bool sourceSubmoduleHelixExtrapolationSuccess = false;
          
          if (useSourceSubmoduleLineExtrapolation) {
            geomtools::vector_3d srcSubmodRefPoint;
            srcSubmodPlacement.mother_to_child(refPoint, srcSubmodRefPoint);
            geomtools::vector_3d srcSubmodDirection;
            srcSubmodPlacement.mother_to_child_direction(direction, srcSubmodDirection);
            // Check if the track hit the submodule:
            geomtools::face_intercept_info srcSubmodFii;
            bool success = srcSubmodShape.find_intercept(srcSubmodRefPoint,
                                                         srcSubmodDirection,
                                                         srcSubmodFii,
                                                         _intercept_tolerance_);          
            if (success) {
              DT_LOG_DEBUG(logPrio, "Found an intercept on the source submodule #"
                           << _sourceSubmoduleGid_ << " at " << srcSubmodFii.get_impact());
              const geomtools::vector_3d & impact = srcSubmodFii.get_impact();
              double yImpact = impact.y();
              int32_t minId = 100000;
              int32_t maxId = -100000;
              for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
                const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
                const geomtools::geom_info & sourceStripGinfo = geoManager().get_mapping().get_geom_info(sourceStripGid);
                const geomtools::placement & sourceStripPlacement = sourceStripGinfo.get_world_placement();
                double yStrip = sourceStripPlacement.get_translation().y();
                if (std::abs(yStrip - yImpact) < _max_source_extrapolation_xy_length_) {
                  int32_t sourceStripId = (int32_t) sourceStripGid.get(1);
                  if (sourceStripId > maxId) {
                    maxId = sourceStripId;
                  }
                  if (sourceStripId < minId) {
                    minId = sourceStripId;
                  } 
                }
              }
              minStripId = std::max(minStripId, (int16_t) (minId - 1));
              maxStripId = std::min(maxStripId, (int16_t) (maxId + 1));
              sourceSubmoduleLineExtrapolationSuccess = true;
            } else {
              DT_LOG_DEBUG(logPrio, "No line intercept on the source submodule #" << _sourceSubmoduleGid_);
            }
          } // if (sourceSubmoduleLineExtrapolationSuccess) 

          if (useSourceSubmoduleHelixExtrapolation and not sourceSubmoduleLineExtrapolationSuccess) {
            snemo::geometry::helix_intercept hIntercept(helix,
                                                        srcSubmodShape,
                                                        srcSubmodPlacement,
                                                        _finder_step_,
                                                        _intercept_tolerance_,
                                                        hInterceptPrio);
            snemo::geometry::helix_intercept::extrapolation_info srcSubmodEi;
            bool success = hIntercept.find_intercept(srcSubmodEi, from_bit);
            if (success) {
              const geomtools::vector_3d & srcSubmodWorldImpact = srcSubmodEi.fii.get_impact();
              DT_LOG_DEBUG(logPrio, "Found an helix intercept on the source submodule #"
                           << _sourceSubmoduleGid_ << " at " << srcSubmodWorldImpact << " (world)");
              // geomtools::vector_3d srcSubmodImpact;
              // srcSubmodPlacement.mother_to_child(srcSubmodWorldImpact, srcSubmodImpact);
              // DT_LOG_DEBUG(logPrio, "impact on source submodule=" << srcSubmodWorldImpact << " (child)");
              double yImpactWorld = srcSubmodWorldImpact.y();
              int32_t minId = 100000;
              int32_t maxId = -100000;
              // Find candidate source strips 
              for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
                const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
                const geomtools::geom_info & sourceStripGinfo = geoManager().get_mapping().get_geom_info(sourceStripGid);
                const geomtools::placement & sourceStripPlacement = sourceStripGinfo.get_world_placement();
                double yStripWorld = sourceStripPlacement.get_translation().y();
                if (std::abs(yStripWorld - yImpactWorld) < _max_source_extrapolation_xy_length_) {
                  int32_t sourceStripId = (int32_t) sourceStripGid.get(1);
                  if (sourceStripId > maxId) {
                    maxId = sourceStripId;
                  }
                  if (sourceStripId < minId) {
                    minId = sourceStripId;
                  } 
                }
              }
              minStripId = std::max(minStripId, (int16_t) (minId - 1));
              maxStripId = std::min(maxStripId, (int16_t) (maxId + 1));
            } else {
              DT_LOG_DEBUG(logPrio, "No helix intercept on the source submodule #" << _sourceSubmoduleGid_);
            }
          } // if (sourceSubmoduleHelixExtrapolationSuccess) 
          
          if (sourceSubmoduleLineExtrapolationSuccess or sourceSubmoduleHelixExtrapolationSuccess) {
            sourceSubmoduleInterceptSuccess = true;
          }
        }
        DT_LOG_DEBUG(logPrio, "minStripId=" << minStripId);
        DT_LOG_DEBUG(logPrio, "maxStripId=" << maxStripId);

        // Scan strips/pads...
        if (use_foils and sourceSubmoduleInterceptSuccess) {
          DT_LOG_DEBUG(logPrio, "Scanning strips...");
          for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
            const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
            DT_LOG_DEBUG(logPrio, "Source strip GID : " << sourceStripGid);
            uint32_t sourceStripId = sourceStripGid.get(1);
            if (sourceStripGid.get(0) != _module_id_) { 
              continue;
            }
            if ((int32_t) sourceStripId < minStripId or (int32_t) sourceStripId > maxStripId) { 
              continue;
            }
            
            DT_LOG_DEBUG(logPrio, "Searching helix intercept on strip #" << sourceStripId);
            const geomtools::geom_info  & sourceStripGinfo     = geoManager().get_mapping().get_geom_info(sourceStripGid);
            const geomtools::logical_volume & sourceStripLog   = sourceStripGinfo.get_logical();
            const geomtools::i_shape_3d & sourceStripShape     = sourceStripLog.get_shape();
            const geomtools::placement  & sourceStripPlacement = sourceStripGinfo.get_world_placement();
            DT_LOG_DEBUG(logPrio, "Source strip shape  : '" << sourceStripShape.get_shape_name() << "'");
            DT_LOG_DEBUG(logPrio, "Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
            
            bool sourceStripLineExtrapolationSuccess = false;
            bool sourceStripHelixExtrapolationSuccess = false;
            
            if (_use_linear_interpolation_ and sourceSubmoduleInterceptSuccess) {
              DT_LOG_DEBUG(logPrio, "Searching linear intercept from helix end on strip #" << sourceStripId
                           << " from " << vertex_info::from_to_label(iFrom) << " ...");
              geomtools::vector_3d stripRefPoint;
              sourceStripPlacement.mother_to_child(refPoint, stripRefPoint);
              geomtools::vector_3d stripDirection;
              sourceStripPlacement.mother_to_child_direction(direction, stripDirection);
              DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(stripRefPoint) );
              DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(stripDirection) );
              geomtools::face_intercept_info srcStripFii;
              bool success = sourceStripShape.find_intercept(stripRefPoint, stripDirection, srcStripFii, _intercept_tolerance_);
              if (success) {
                DT_LOG_DEBUG(logPrio, "Found line intercept on strip #" << sourceStripId);
                geomtools::vector_3d srcStripImpact = srcStripFii.get_impact();
                geomtools::vector_3d srcStripWorldImpact;
                sourceStripPlacement.child_to_mother(srcStripImpact, srcStripWorldImpact);
                srcStripFii.set_impact(srcStripWorldImpact);
                sourceStripLineExtrapolationSuccess = true;
                // XXX Add vtx info
              }
            }
            
            if (_use_helix_interpolation_ and sourceSubmoduleInterceptSuccess and not sourceStripLineExtrapolationSuccess) {
              DT_LOG_DEBUG(logPrio, "Searching helix intercept from helix end on strip #" << sourceStripId
                           << " from " << vertex_info::from_to_label(iFrom) << " ...");
              snemo::geometry::helix_intercept::extrapolation_info srcStripEi;
              snemo::geometry::helix_intercept hIntercept(helix,
                                                          sourceStripShape,
                                                          sourceStripPlacement,
                                                          _finder_step_,
                                                          _intercept_tolerance_,
                                                          hInterceptPrio);
              bool success = hIntercept.find_intercept(srcStripEi, from_bit);
              if (success) {
                sourceStripHelixExtrapolationSuccess = true;
                // XXX Add vtx info
              }
            }
            
            if (not sourceStripLineExtrapolationSuccess and not sourceStripHelixExtrapolationSuccess) {
              // No intersection with the strip volume: no chance to intersect a pad ! We give up.
              continue;         
            }
            
            DT_LOG_DEBUG(logPrio, "Found helix intercept on strip #" << sourceStripId);

            // Search intercept on daughter pad volumes:
            for (uint32_t sourcePadGidIndex = 0; sourcePadGidIndex < _sourcePadGids_.size(); sourcePadGidIndex++) {
              const geomtools::geom_id & sourcePadGid = _sourcePadGids_[sourcePadGidIndex];
              if (sourcePadGid.get(1) != sourceStripId) {
                // Reject pads in other strips:
                continue;
              }
              uint32_t sourcePadId = sourcePadGid.get(2);
              const geomtools::geom_info  & sourcePadGinfo     = geoManager().get_mapping().get_geom_info(sourcePadGid);
              const geomtools::logical_volume & sourcePadLog   = sourcePadGinfo.get_logical();
              const geomtools::i_shape_3d & sourcePadShape     = sourcePadLog.get_shape();
              const geomtools::placement  & sourcePadPlacement = sourcePadGinfo.get_world_placement();
              DT_LOG_DEBUG(logPrio, "  Source pad " << sourcePadId << " shape : '" << sourcePadShape.get_shape_name() << "'");
 
              // Attempt to find an intercept on a source pad:
              vertex_info padVtxInfo;
              bool sourcePadInterceptSuccess = false;
              bool sourcePadLineExtrapolationSuccess = false;
              bool sourcePadHelixExtrapolationSuccess = false;
              if (_use_linear_interpolation_) {
                DT_LOG_DEBUG(logPrio, "Searching line intercept from helix trajectory on pad #"
                             << sourcePadGid << " in strip #" << sourceStripGid);
                geomtools::vector_3d srcPadRefPoint;
                sourcePadPlacement.mother_to_child(refPoint, srcPadRefPoint);
                geomtools::vector_3d srcPadDirection;
                sourcePadPlacement.mother_to_child_direction(direction, srcPadDirection);
                // Check if the track hits the source pad:
                geomtools::face_intercept_info srcPadFii;
                bool success = sourcePadShape.find_intercept(srcPadRefPoint,
                                                             srcPadDirection,
                                                             srcPadFii,
                                                             _intercept_tolerance_);
                if (success) {
                  const geomtools::vector_3d & srcPadImpact = srcPadFii.get_impact();
                  DT_LOG_DEBUG(logPrio, "Found a line intercept on source pad #"
                               << sourcePadGid << " at " << srcPadImpact);
                  // 3D-extrapolation:
                  geomtools::vector_3d srcPadImpactWorld;
                  sourcePadPlacement.child_to_mother(srcPadImpact, srcPadImpactWorld);
                  srcPadFii.set_impact(srcPadImpactWorld);
                  double distRef2Impact    = (srcPadImpactWorld - refPoint).mag();
                  double extrapolationDist = distRef2Impact - distRef2End;
                  DT_LOG_DEBUG(logPrio, "extrapolationDist = " << extrapolationDist / CLHEP::mm << "  mm");
                  // XY-extrapolation:
                  geomtools::vector_2d srcPadImpactWorld_Xy(srcPadImpactWorld.x(), srcPadImpactWorld.y());
                  double distRef2Impact_Xy  = (srcPadImpactWorld_Xy - refPoint_Xy).mag();
                  double extrapolationDist_Xy = distRef2Impact_Xy - distRef2End_Xy;
                  DT_LOG_DEBUG(logPrio, "extrapolationDist_Xy = " << extrapolationDist_Xy / CLHEP::mm << "  mm");
                  padVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                  padVtxInfo.from = iFrom;
                  padVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                  padVtxInfo.gid = sourcePadGid;
                  DT_LOG_DEBUG(logPrio, "sourcePadGid = " << sourcePadGid);
                  padVtxInfo.face_intercept = srcPadFii;
                  if (extrapolationDist > 0.0) {
                    padVtxInfo.distance    = extrapolationDist;
                    padVtxInfo.distance_xy = extrapolationDist_Xy;
                  } else {
                    padVtxInfo.distance    = 0.0;
                    padVtxInfo.distance_xy = 0.0;
                  }
                  padVtxInfo.tolerance = _intercept_tolerance_;
                  if (datatools::logger::is_debug(logPrio)) {
                    DT_LOG_DEBUG(logPrio, "Line intercept is :");
                    padVtxInfo.print(std::cerr, "[debug] ");
                  }
                  if (padVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                    // srcVertexes.push_back(padVtxInfo);
                    DT_LOG_DEBUG(logPrio, "Tag source pad line extrapolation success");
                    sourcePadLineExtrapolationSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Line intercept is to far from source pad #" << sourcePadGid);
                  } // extrapolation distance check
                } else {
                  DT_LOG_DEBUG(logPrio, "No line intercept on source pad #" << sourcePadGid);
                } // if (success)
              } // if (useSourcePadLineExtrapolation)
              
              if (_use_helix_interpolation_ and not sourcePadLineExtrapolationSuccess ) {            
                DT_LOG_DEBUG(logPrio, "Searching helix intercept on source pad #" << sourcePadGid << " in strip #" << sourceStripGid);            
                snemo::geometry::helix_intercept::extrapolation_info srcPadEi;
                snemo::geometry::helix_intercept hIntercept(helix,
                                                            sourcePadShape,
                                                            sourcePadPlacement,
                                                            _finder_step_,
                                                            _intercept_tolerance_,
                                                            hInterceptPrio);
                bool success = hIntercept.find_intercept(srcPadEi, from_bit);
                if (success) {
                  DT_LOG_DEBUG(logPrio, "Found helix intercept on source pad #" << sourcePadGid
                               << " at " << geomtools::to_xyz(srcPadEi.fii.get_impact()));
                  padVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                  padVtxInfo.from = iFrom;
                  padVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_HELIX;
                  padVtxInfo.gid = sourcePadGid;
                  padVtxInfo.face_intercept = srcPadEi.fii;
                  padVtxInfo.distance = srcPadEi.extrapolated_length;
                  padVtxInfo.distance_xy = srcPadEi.extrapolated_xy_length;
                  padVtxInfo.tolerance = _intercept_tolerance_;
                  if (datatools::logger::is_debug(logPrio)) {
                    padVtxInfo.print(std::cerr, "[debug] ");
                  }
                  if (padVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                    // srcVertexes.push_back(padVtxInfo);
                    sourcePadHelixExtrapolationSuccess = true;
                  } else {
                    DT_LOG_DEBUG(logPrio, "Helix intercept is to far from the source pad #" << sourcePadGid);
                  }
                } else {
                  DT_LOG_DEBUG(logPrio, "No helix intercept on source pad #" << sourcePadGid);
                }
              } // if (useSourcePadHelixExtrapolation)

              if (sourcePadLineExtrapolationSuccess or sourcePadHelixExtrapolationSuccess) {
                DT_LOG_DEBUG(logPrio, "Source pad intercept success");
                sourcePadInterceptSuccess = true;
              }
 
              if (not sourcePadInterceptSuccess) {
                DT_LOG_DEBUG(logPrio, "Give up source pad #" << sourcePadGid);
                continue;
              } 

              bool sourcePadBulkInterceptSuccess = false;
              bool hasCandidatePadBulk = false;
              if (_sourcePadBulkGids_.size() > 0) {
                DT_LOG_DEBUG(logPrio, "About to scan source pad bulks...");
                // Search intercept on daughter pad bulk volumes:
                for (uint32_t sourcePadBulkGidIndex = 0; sourcePadBulkGidIndex < _sourcePadBulkGids_.size(); sourcePadBulkGidIndex++) {
                  const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[sourcePadBulkGidIndex];
                  if (sourcePadBulkGid.get(1) != sourceStripId or sourcePadBulkGid.get(2) != sourcePadId ) {
                    // Reject pad bulks in other strips and pads:
                    continue;
                  }
                  hasCandidatePadBulk = true;
                  DT_LOG_DEBUG(logPrio, "Searching helix intercept on pad bulk #" << sourcePadBulkGid << " in pad #" << sourcePadGid);
                  const geomtools::geom_info & sourcePadBulkGinfo = geoManager().get_mapping().get_geom_info(sourcePadBulkGid);
                  const geomtools::logical_volume & sourcePadBulkLog = sourcePadBulkGinfo.get_logical();
                  const geomtools::i_shape_3d & sourcePadBulkShape = sourcePadBulkLog.get_shape();
                  const geomtools::placement & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
                  DT_LOG_DEBUG(logPrio, "Source pad bulk shape : '" << sourcePadBulkShape.get_shape_name() << "'");
 
                  bool sourcePadBulkLineExtrapolationSuccess = false;
                  bool sourcePadBulkHelixExtrapolationSuccess = false;
 
                  if (_use_linear_interpolation_) {
                    DT_LOG_DEBUG(logPrio, "Searching line intercept from helix trajectory on source pad bulk #"
                                 << sourcePadBulkGid << " in pad #" << sourcePadGid);
                    geomtools::vector_3d srcPadBulkRefPoint;
                    sourcePadPlacement.mother_to_child(refPoint, srcPadBulkRefPoint);
                    geomtools::vector_3d srcPadBulkDirection;
                    sourcePadPlacement.mother_to_child_direction(direction, srcPadBulkDirection);
                    // Check if the track hits the source pad:
                    geomtools::face_intercept_info srcPadBulkFii;
                    bool success = sourcePadBulkShape.find_intercept(srcPadBulkRefPoint,
                                                                     srcPadBulkDirection,
                                                                     srcPadBulkFii,
                                                                     _intercept_tolerance_);
                    if (success) {
                      const geomtools::vector_3d & srcPadBulkImpact = srcPadBulkFii.get_impact();
                      DT_LOG_DEBUG(logPrio, "Found a line intercept on source pad #"
                                   << sourcePadBulkGid << " at " << srcPadBulkImpact);
                      // 3D-extrapolation:
                      geomtools::vector_3d srcPadBulkImpactWorld;
                      sourcePadPlacement.child_to_mother(srcPadBulkImpact, srcPadBulkImpactWorld);
                      srcPadBulkFii.set_impact(srcPadBulkImpactWorld);
                      double distRef2Impact    = (srcPadBulkImpactWorld - refPoint).mag();
                      double extrapolationDist = distRef2Impact - distRef2End;
                      DT_LOG_DEBUG(logPrio, "extrapolationDist    = " << extrapolationDist / CLHEP::mm << "  mm");
                      // XY-extrapolation:
                      geomtools::vector_2d srcPadBulkImpactWorld_Xy(srcPadBulkImpactWorld.x(), srcPadBulkImpactWorld.y());
                      double distRef2Impact_Xy  = (srcPadBulkImpactWorld_Xy - refPoint_Xy).mag();
                      double extrapolationDist_Xy = distRef2Impact_Xy - distRef2End_Xy;
                      DT_LOG_DEBUG(logPrio, "extrapolationDist_Xy = " << extrapolationDist_Xy / CLHEP::mm << "  mm");
                      vertex_info padBulkVtxInfo;
                      padBulkVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                      padBulkVtxInfo.from = iFrom;
                      padBulkVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                      padBulkVtxInfo.gid = sourcePadBulkGid;
                      padBulkVtxInfo.face_intercept = srcPadBulkFii;
                      if (extrapolationDist > 0.0) {
                        padBulkVtxInfo.distance    = extrapolationDist;
                        padBulkVtxInfo.distance_xy = extrapolationDist_Xy;
                      } else {
                        padBulkVtxInfo.distance    = 0.0;
                        padBulkVtxInfo.distance_xy = 0.0;
                      }
                      padBulkVtxInfo.tolerance = _intercept_tolerance_;
                      if (datatools::logger::is_debug(logPrio)) {
                        padBulkVtxInfo.print(std::cerr, "[debug] ");
                      }
                      if (padBulkVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                        srcVertexes.push_back(padBulkVtxInfo);
                        sourcePadBulkLineExtrapolationSuccess = true;
                      } else {
                        DT_LOG_DEBUG(logPrio, "Line intercept is to far from the source pad bulk");
                      }
                    } else {
                      DT_LOG_DEBUG(logPrio, "No line intercept on source pad bulk #" << sourcePadBulkGid);
                    } //  if (success)
                  } // if (_use_linear_interpolation_)
              
                  if (_use_helix_interpolation_ and not sourcePadBulkLineExtrapolationSuccess ) {            
                    DT_LOG_DEBUG(logPrio, "Searching helix intercept on source pad bulk #"
                                 << sourcePadBulkGid << " in pad #" << sourcePadGid);            
                    snemo::geometry::helix_intercept::extrapolation_info srcPadBulkEi;
                    snemo::geometry::helix_intercept hIntercept(helix,
                                                                sourcePadBulkShape,
                                                                sourcePadBulkPlacement,
                                                                _finder_step_,
                                                                _intercept_tolerance_,
                                                                hInterceptPrio);
                    bool success = hIntercept.find_intercept(srcPadBulkEi, from_bit);
                    if (success) {
                      DT_LOG_DEBUG(logPrio, "Found helix intercept on source pad #" << sourcePadBulkGid
                                   << " at " << geomtools::to_xyz(srcPadBulkEi.fii.get_impact()));
                      vertex_info padBulkVtxInfo;
                      padBulkVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                      padBulkVtxInfo.from = iFrom;
                      padBulkVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_HELIX;
                      padBulkVtxInfo.gid = sourcePadBulkGid;
                      padBulkVtxInfo.face_intercept = srcPadBulkEi.fii;
                      padBulkVtxInfo.distance = srcPadBulkEi.extrapolated_length;
                      padBulkVtxInfo.distance_xy = srcPadBulkEi.extrapolated_xy_length;
                      padBulkVtxInfo.tolerance = _intercept_tolerance_;
                      if (datatools::logger::is_debug(logPrio)) {
                        padBulkVtxInfo.print(std::cerr, "[debug] ");
                      }
                      if (padBulkVtxInfo.distance_xy <= _max_calo_extrapolation_xy_length_) {
                        srcVertexes.push_back(padBulkVtxInfo);
                        sourcePadBulkHelixExtrapolationSuccess = true;
                      } else {
                        DT_LOG_DEBUG(logPrio, "Helix intercept is to far from the source pad bulk");
                      }
                    } else {
                      DT_LOG_DEBUG(logPrio, "No helix intercept on source pad bulk #" << sourcePadBulkGid);
                    }
                  } // if (useSourcePadBulkHelixExtrapolation)

                  if (sourcePadBulkLineExtrapolationSuccess or sourcePadBulkHelixExtrapolationSuccess) {
                    sourcePadBulkInterceptSuccess = true;
                  }
                  
                  if (not sourcePadBulkInterceptSuccess) {
                    DT_LOG_DEBUG(logPrio, "Give up source pad bulk #" << sourcePadBulkGid);
                  }
                  
                } // for (uint32_t sourcePadBulkGidIndex ...
              } // if (_sourcePadBulkGids_.size() > 0)

              if (sourcePadInterceptSuccess) { 
                if (not sourcePadBulkInterceptSuccess) {
                  if (hasCandidatePadBulk) {
                    // A bulk was expected but missed, so we are in a gap:
                    padVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP;               
                  }
                  // Validate pad intercept if no candidate pad bulk
                  DT_LOG_DEBUG(logPrio, "add padVtxInfo");
                  srcVertexes.push_back(padVtxInfo);
                }
              }
              
            } // for (uint32_t sourcePadGidIndex...
          } // for (uint32_t sourcePadGidIndex...
        } // if (use_foils)
        
        if (use_calib_src) {
          // Calibration spot:
          for (uint32_t iSpot = 0; iSpot < _sourceCalibrationSpotGids_.size(); iSpot++) {
            const geomtools::geom_id & sourceCalibrationSpotGid = _sourceCalibrationSpotGids_[iSpot];
            DT_LOG_DEBUG(logPrio, "Searching helix intercept on calibration source spot #" << sourceCalibrationSpotGid);
            const geomtools::geom_info  & sourceCalibrationSpotGinfo  = geoManager().get_mapping().get_geom_info(sourceCalibrationSpotGid);
            const geomtools::logical_volume & sourceCalibrationSpotLog   = sourceCalibrationSpotGinfo.get_logical();
            const geomtools::i_shape_3d & sourceCalibrationSpotShape     = sourceCalibrationSpotLog.get_shape();
            const geomtools::placement  & sourceCalibrationSpotPlacement = sourceCalibrationSpotGinfo.get_world_placement();
            DT_LOG_DEBUG(logPrio, "Calibration spot shape : '" << sourceCalibrationSpotShape.get_shape_name() << "'");

            bool calibrationSpotLineExtrapolationSuccess = false;
            // bool calibrationSpotHelixExtrapolationSuccess = false;
            
            if (_use_linear_interpolation_) {
              geomtools::vector_3d calibSpotRefPoint;
              sourceCalibrationSpotPlacement.mother_to_child(refPoint, calibSpotRefPoint);
              geomtools::vector_3d calibSpotDirection;
              sourceCalibrationSpotPlacement.mother_to_child_direction(direction, calibSpotDirection);
              DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(calibSpotRefPoint) );
              DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(calibSpotDirection) );
              DT_LOG_DEBUG(logPrio, "Calib. spot strip shape  : '" << sourceCalibrationSpotShape.get_shape_name() << "'");
              DT_LOG_DEBUG(logPrio, "Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
              geomtools::face_intercept_info calibSpotFii;
              bool success = sourceCalibrationSpotShape.find_intercept(calibSpotRefPoint,
                                                                       calibSpotDirection,
                                                                       calibSpotFii,
                                                                       _intercept_tolerance_);
              if (success) {
                geomtools::vector_3d calibSpotImpact = calibSpotFii.get_impact();
                geomtools::vector_3d calibSpotWorldImpact;
                sourceCalibrationSpotPlacement.child_to_mother(calibSpotImpact, calibSpotWorldImpact);
                calibSpotFii.set_impact(calibSpotWorldImpact);
                double calibSpotDistRef2Impact    = (calibSpotWorldImpact - refPoint).mag();
                double calibSpotExtrapolationDist = calibSpotDistRef2Impact - distRef2End;
                // XY extrapolation:
                geomtools::vector_2d calibSpotImpact_Xy(calibSpotWorldImpact.x(), calibSpotWorldImpact.y());
                double calibSpotDistRef2Impact_Xy    = (calibSpotImpact_Xy - refPoint_Xy).mag();
                double calibSpotExtrapolationDist_Xy = calibSpotDistRef2Impact_Xy - distRef2End_Xy;
                // Result:
                vertex_info calibSpotVtxInfo;
                calibSpotVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
                calibSpotVtxInfo.from = iFrom;
                calibSpotVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_LINE;
                calibSpotVtxInfo.gid = sourceCalibrationSpotGid;
                calibSpotVtxInfo.face_intercept = calibSpotFii;
                if (calibSpotExtrapolationDist > 0.0) {
                  calibSpotVtxInfo.distance    = calibSpotDistRef2Impact;
                  calibSpotVtxInfo.distance_xy = calibSpotExtrapolationDist_Xy;
                } else {
                  calibSpotVtxInfo.distance    = 0.0;
                  calibSpotVtxInfo.distance_xy = 0.0;
                }
                DT_LOG_DEBUG(logPrio, "calibSpotExtrapolationDist    = " << calibSpotExtrapolationDist / CLHEP::mm << "  mm");
                DT_LOG_DEBUG(logPrio, "calibSpotExtrapolationDist_Xy = " << calibSpotExtrapolationDist_Xy / CLHEP::mm << "  mm");
                calibSpotVtxInfo.tolerance = _intercept_tolerance_;
                if (calibSpotVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  DT_LOG_DEBUG(logPrio, "add calibSpotVtxInfo");
                  srcVertexes.push_back(calibSpotVtxInfo);
                  calibrationSpotLineExtrapolationSuccess = true;
                } else {
                  DT_LOG_DEBUG(logPrio, "Line intercept is to far from the calibration spot");
                } // extrapolation distance check
              } // if (success)
            } // if (useCalibrationSpotLineExtrapolation) 

            if (_use_helix_interpolation_ and not calibrationSpotLineExtrapolationSuccess) {
              snemo::geometry::helix_intercept hCalibrationSpotIntercept(helix,
                                                                         sourceCalibrationSpotShape,
                                                                         sourceCalibrationSpotPlacement,
                                                                         _finder_step_,
                                                                         _intercept_tolerance_,
                                                                         logPrio);
              snemo::geometry::helix_intercept::extrapolation_info srcCalibrationSpotEi;
              bool success = hCalibrationSpotIntercept.find_intercept(srcCalibrationSpotEi, from_bit);
              if (success) {
                DT_LOG_DEBUG(logPrio, "Found helix intercept on calibration source spot #" << sourceCalibrationSpotGid);
                geomtools::vector_3d srcCalibrationSpotImpact = srcCalibrationSpotEi.fii.get_impact();
                geomtools::vector_3d srcCalibrationSpotWorldImpact;
                sourceCalibrationSpotPlacement.child_to_mother(srcCalibrationSpotImpact, srcCalibrationSpotWorldImpact);
                srcCalibrationSpotEi.fii.set_impact(srcCalibrationSpotWorldImpact);
                vertex_info calibrationSpotVtxInfo;
                calibrationSpotVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE;
                calibrationSpotVtxInfo.from = iFrom;
                calibrationSpotVtxInfo.extrapolation_mode = vertex_info::EXTRAPOLATION_HELIX;
                calibrationSpotVtxInfo.gid = sourceCalibrationSpotGid;
                calibrationSpotVtxInfo.face_intercept = srcCalibrationSpotEi.fii;
                calibrationSpotVtxInfo.distance = srcCalibrationSpotEi.extrapolated_length;
                calibrationSpotVtxInfo.distance_xy = srcCalibrationSpotEi.extrapolated_xy_length;
                calibrationSpotVtxInfo.tolerance = _intercept_tolerance_;
                if (calibrationSpotVtxInfo.distance_xy <= _max_source_extrapolation_xy_length_) {
                  srcVertexes.push_back(calibrationSpotVtxInfo);
                  // calibrationSpotHelixExtrapolationSuccess = true;
                } else {
                  DT_LOG_DEBUG(logPrio, "Helix intercept is to far from the calibration source spot");
                } // extrapolation distance check
              } // if (success)
            } // if (useCalibrationSpotHelixExtrapolation...
          } // for (uint32_t iStrip
        } // if (use_calib_src)

        _post_process_source_vertex_(srcVertexes);  
        for (unsigned int i = 0; i < srcVertexes.size(); i++) {
          vertexes_.push_back(srcVertexes[i]);
        }
        
        // int closestIndex = -1;
        // double closestDist = datatools::invalid_real();
        // for (unsigned int i = 0; i < srcVertexes.size(); i++) {
        //   const vertex_info & vtxInfo = srcVertexes[i];
        //   if (closestIndex == -1 or vtxInfo.distance < closestDist) {
        //     closestIndex = i;
        //     closestDist = vtxInfo.distance;
        //   }
        // }
        // if (closestIndex >= 0) {
        //   vertexes_.push_back(srcVertexes[closestIndex]);
        // }
        // srcVertexes.clear();
        
      } // for (int iFrom=...) 
      return;
    } // helix_trajectory_source_intercept

    
    void vertex_extrapolation_driver::_measure_vertices_(const snemo::datamodel::tracker_trajectory & trajectory_,
                                                         snemo::datamodel::VertexHdlCollection & vertices_)
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
      const snedm::base_trajectory_pattern & a_track_pattern = trajectory_.get_pattern();
      const std::string & a_pattern_id = a_track_pattern.get_pattern_id();

      // Extrapolated vertices:
      snemo::geometry::vertex_info_list vertice_infos;
  
      DT_LOG_DEBUG(logPrio, "Pattern ID : '" << a_pattern_id << "'");

      // ----- Start of line pattern handling
      if (a_pattern_id == snedm::line_trajectory_pattern::pattern_id()) {
        DT_LOG_DEBUG(logPrio, "Line pattern handling...");
        const auto & ltp = dynamic_cast<const snedm::line_trajectory_pattern &>(a_track_pattern);
        bool find_on_source = true; 
        bool find_on_calo   = true;
        if ((_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP)->second == false)) {
          find_on_source = false;
        }
        if ((_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER)->second == false)) {
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
    
      } // ----- end of line pattern handling

      // ---- start of helix pattern handling
      if (a_pattern_id == snedm::helix_trajectory_pattern::pattern_id()) {
        DT_LOG_DEBUG(logPrio, "Helix pattern handling...");
        const auto & htp = dynamic_cast<const snedm::helix_trajectory_pattern &>(a_track_pattern);
        bool find_on_source = true;
        bool find_on_calo   = true;
        if ((_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP)->second == false)) {
          find_on_source = false;
        }
        if ((_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER)->second == false)
            and (_use_vertices_.find(snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER)->second == false)) {
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
      }
      // ----- end of helix pattern

      // Save new vertices in the Falaise datamodel:
      for (const auto & vtxInfo : vertice_infos) {
        DT_LOG_DEBUG(logPrio, "Publish vertex information:");
        if (datatools::logger::is_debug(logPrio)) {
          vtxInfo.print(std::cerr, "[debug] ");
        }
        // Convert the geometry vertex model to the official vertex datamodel:
        snedm::VertexHdl hNewVertex = datatools::make_handle<snedm::Vertex>();
        hNewVertex->set_hit_id(vertices_.size());
        hNewVertex->set_geom_id(vtxInfo.gid);
        DT_LOG_DEBUG(logPrio, "Vertex geometry ID=" << hNewVertex->get_geom_id());
        hNewVertex->set_category(snedm::convert(vtxInfo.category));
        hNewVertex->set_extrapolation(snedm::convert(vtxInfo.extrapolation_mode));
        if (vtxInfo.from == snemo::geometry::vertex_info::FROM_FIRST) {
          hNewVertex->set_from(snedm::VERTEX_FROM_FIRST);
        } else if (vtxInfo.from == snemo::geometry::vertex_info::FROM_LAST) {
          hNewVertex->set_from(snedm::VERTEX_FROM_LAST);
        }
        if (datatools::is_valid(vtxInfo.distance)) {
          hNewVertex->set_distance(vtxInfo.distance);
        }
        if (datatools::is_valid(vtxInfo.distance_xy)) {
          hNewVertex->set_distance_xy(vtxInfo.distance_xy);
        }
        if (vtxInfo.best) {
          hNewVertex->set_best(true);
        }
        if (vtxInfo.edge) {
          hNewVertex->set_edge(true);
        }
        hNewVertex->get_spot().set_blur_dimension(geomtools::blur_spot::dimension_three);
        hNewVertex->get_spot().set_position(vtxInfo.face_intercept.get_impact());
        // Errors are set in all 3 dimensions wrt the extrapolation tolerance along
        // the trajectory path (typically normal to the impact surface). 
        hNewVertex->get_spot().set_errors(vtxInfo.tolerance);
        vertices_.push_back(hNewVertex);
        DT_LOG_DEBUG(logPrio, "New vertex:");
        hNewVertex->print_tree(std::cerr);
      }

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
      return;
    } // _measure_vertices_

    
    void vertex_extrapolation_driver::_check_vertices_(const snemo::datamodel::tracker_trajectory & trajectory_,
                                                       std::map<snemo::geometry::vertex_info::category_type, bool> & use_vertices_)
    {
      if (!trajectory_.has_cluster()) {
        return;
      }
      // Reset values of booleans
      namespace snedm = snemo::datamodel;
      use_vertices_.clear();
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL] = false;
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER] = false;
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER] = false;
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_GAMMA_VETO] = false;
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE] = false;
      use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP] = false;

      const snedm::tracker_cluster & a_cluster = trajectory_.get_cluster();
      const snedm::TrackerHitHdlCollection & the_hits = a_cluster.hits();
      for (const datatools::handle<snedm::calibrated_tracker_hit> & a_hit : the_hits) {
        const geomtools::geom_id & a_gid = a_hit->get_geom_id();

        // Extract layer
        const snemo::geometry::gg_locator & gg_locator = geoLocator_->geigerLocator();
        const uint32_t layer = gg_locator.getLayerAddress(a_gid);
        if (layer < 2) {
          // Extrapolate vertex to the foil or calib. sources if the first GG layers are fired
          use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL] = true;
          use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE] = true;
          use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP] = true;
        }

        const uint32_t side = gg_locator.getSideAddress(a_gid);
        if (layer >= gg_locator.numberOfLayers(side) - 2) {
          use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER] = true;
        }

        const uint32_t row = gg_locator.getRowAddress(a_gid);
        if (row <= 1 || row >= gg_locator.numberOfRows(side) - 1) {
          use_vertices_[snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER] = true;
        }
      }
      return;
    } // _check_vertices_

 
    void vertex_extrapolation_driver::_post_process_source_vertex_(snemo::geometry::vertex_info_list & src_vertexes_) const
    {
      DT_LOG_DEBUG(logPriority_, "Post processing of " << src_vertexes_.size() << " source vertexes");
      // std::cerr << "\n\n *************** PP src vtx ******** \n\n";
                                                                                                         
      if (src_vertexes_.size() == 0) return;
      snemo::geometry::vertex_info_list workVertexes;

      // Default keep all vertexes extrapolated on source foil region:
      for (int i = 0; i < (int) src_vertexes_.size(); i++) {
        workVertexes.push_back(src_vertexes_[i]);
      }
      
      src_vertexes_ = workVertexes;
      return;
    }

    void vertex_extrapolation_driver::_post_process_calo_vertex_(snemo::geometry::vertex_info_list & calo_vertexes_) const
    {
      DT_LOG_DEBUG(logPriority_, "Post processing of " << calo_vertexes_.size() << " calo vertexes");
      if (calo_vertexes_.size() == 0) return;
      snemo::geometry::vertex_info_list workVertexes; 
      // Determine the closest vertex from the end of the track:
      int closestIndex = -1;
      double closestDist = datatools::invalid_real();
      for (int i = 0; i < (int) calo_vertexes_.size(); i++) {
        const snemo::geometry::vertex_info & vtxInfo = calo_vertexes_[i];
        if (closestIndex == -1 or vtxInfo.distance < closestDist) {
          closestIndex = i;
          closestDist = vtxInfo.distance;
        }
      }
      bool findEdge = true;
      // Add the vertex with shorter extrapolation distance:
      workVertexes.push_back(calo_vertexes_[closestIndex]);
      {
        snemo::geometry::vertex_info & vtxInfo = workVertexes.back();
        vtxInfo.best = true;

        if (findEdge) {
          // Attempt to identify an 'edge' vertex:
          const geomtools::geom_info & blockGinfo  = geoManager().get_mapping().get_geom_info(vtxInfo.gid);
          const geomtools::logical_volume & blockLog = blockGinfo.get_logical();
          const geomtools::i_shape_3d & blockShape = blockLog.get_shape();
          const geomtools::placement & blockPlacement = blockGinfo.get_world_placement();
          const geomtools::i_shape_3d * effectiveBlockShape = &blockShape;
          if (vtxInfo.category == snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER) {
            effectiveBlockShape = &*_effectiveCaloBlockBoxPtr2_;
          } else if (vtxInfo.category == snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER) {
            effectiveBlockShape = &*_effectiveXcaloBlockBoxPtr2_;
          }
          const geomtools::vector_3d & worldVtx = vtxInfo.face_intercept.get_impact();
          geomtools::vector_3d shapeVtx;
          blockPlacement.mother_to_child(worldVtx, shapeVtx);
          geomtools::face_identifier faceId
            = effectiveBlockShape->on_surface(shapeVtx,
                                              vtxInfo.face_intercept.get_face_id(),
                                              _intercept_tolerance_);
          // bool onSurface = effectiveBlockShape->check_surface(shapeVtx, _intercept_tolerance_);
          if (not faceId.is_valid()) { // onSurface) {
            DT_LOG_DEBUG(logPriority_, "Found an edge vertex");
            vtxInfo.edge = true;
          }
        }
      }
      
      // 2022-12-09 FM: Do not work for now because it breaks the calo block association algorithm
      // // Keep those with closestDist +/- tolerance XXX
      // for (int i = 0; i < (int) calo_vertexes_.size(); i++) {
      //   const snemo::geometry::vertex_info & vtxInfo = calo_vertexes_[i];
      //   if (std::abs(vtxInfo.distance - closestDist) <= 1.5 * _intercept_tolerance_) {
      //     workVertexes.push_back(vtxInfo);
      //     if (i == closestIndex) {
      //       workVertexes.back().best = true;
      //     }
      //   }
      // }
      
      calo_vertexes_ = workVertexes;
      return;
    }
      
    // static
    void vertex_extrapolation_driver::init_ocd(datatools::object_configuration_description & ocd_)
    {
      // Prefix "VED" stands for "Vertex Extrapolation Driver" :
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "VED.");

      {
        // // Description of the 'VED.use_linear_extrapolation' configuration property :
        // datatools::configuration_property_description &cpd = ocd_.add_property_info();
        // cpd.set_name_pattern("VED.use_linear_extrapolation")
        //   .set_from("snemo::reconstruction::vertex_extrapolation_driver")
        //   .set_terse_description("Use a linear extrapolation to determine vertex position")
        //   .set_traits(datatools::TYPE_BOOLEAN)
        //   .set_mandatory(false)
        //   .set_default_value_boolean(false)
        //   .set_long_description("Feature to be implemented")
        //   .add_example(
        //                "Set the default value::                      \n"
        //                "                                             \n"
        //                "  VED.use_linear_extrapolation : boolean = false \n"
        //                "                                             \n");
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
