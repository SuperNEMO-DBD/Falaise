/// \file falaise/snemo/reconstruction/source_vertex_extrapolator.cc

// Ourselves:
#include <ChargedParticleTracking/source_vertex_extrapolator.h>

// Standard library:
#include <set>
#include <map>
#include <cmath>

// This project (Falaise):
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/helix_intercept.h>

namespace snemo {

  namespace reconstruction {

    source_vertex_extrapolator::source_vertex_extrapolator(const geomtools::manager & gm_,
							   const config_type & config_)
    {
      _geoManager_ = &gm_;
      _config_ = config_;
      _initialize_();
      return;
    }

    const source_vertex_extrapolator::config_type & source_vertex_extrapolator::config() const
    {
      return _config_;
    }

    void source_vertex_extrapolator::_initialize_()
    {
      DT_LOG_DEBUG(_config_.verbosity, "Module ID = " << _config_.module_id);
      DT_LOG_DEBUG(_config_.verbosity, "Finder step = " << _config_.finder_step / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_config_.verbosity, "Intercept tolerance = " << _config_.intercept_tolerance / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_config_.verbosity, "Effective source horizontal tolerance = " << _config_.effective_source_horizontal_tolerance / CLHEP::cm << " cm");
      DT_LOG_DEBUG(_config_.verbosity, "Effective source vertical tolerance   = " << _config_.effective_source_vertical_tolerance / CLHEP::cm << " cm");
      DT_LOG_DEBUG(_config_.verbosity, "Max source extrapolation XY-length    = " << _config_.max_source_extrapolation_horizontal_length / CLHEP::cm << " cm");
      DT_LOG_DEBUG(_config_.verbosity, "Max source extrapolation Z-length     = " << _config_.max_source_extrapolation_vertical_length / CLHEP::cm << " cm");
      DT_LOG_DEBUG(_config_.verbosity, "Vertex extrapolation on calibration source Y extend factor (>=0 mm) = " << _config_.calibration_source_extend_horizontal / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_config_.verbosity, "Vertex extrapolation on calibration source Z extend factor (>=0 mm) = " << _config_.calibration_source_extend_vertical / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_config_.verbosity, "Cone angle = "
		   << _config_.cone_angle / CLHEP::degree << "°");
      DT_LOG_DEBUG(_config_.verbosity, "Cone number of rays = "
		   << _config_.cone_nrays);
      DT_LOG_DEBUG(_config_.verbosity, "Cone number of layers = "
		   << _config_.cone_nlayers);

      _geoLocator_ = snemo::geometry::getSNemoLocator(*_geoManager_, _config_.locator_plugin_name);
      _geoMapping_ = &_geoManager_->get_mapping();
      _geoIdMgr_ = &_geoManager_->get_id_mgr();
      if (datatools::logger::is_debug(_config_.verbosity)) {
        _geoIdMgr_->tree_dump(std::cerr, "Geometry ID manager: ", "[debug] ");
        _geoIdMgr_->print_list_of_categories("print", {}, std::cerr);
      }

      // Identify source submodule:
      _sourceSubmoduleType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_submodule")) {
        _sourceSubmoduleType_ = _geoIdMgr_->get_category_info("source_submodule").get_type();
        DT_LOG_DEBUG(_config_.verbosity, "Source submodule type is " << _sourceSubmoduleType_);
        geomtools::geom_id sourceSubmoduleGidPattern(_sourceSubmoduleType_, _config_.module_id);
        std::vector<geomtools::geom_id> sourceSubmoduleGids;
        _geoMapping_->compute_matching_geom_id(sourceSubmoduleGidPattern, sourceSubmoduleGids);
        DT_THROW_IF(sourceSubmoduleGids.size() != 1, std::logic_error, "Should have found 1 unique source submodule!");
        _sourceSubmoduleGid_ = sourceSubmoduleGids[0];
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found source submodule GID=" << _sourceSubmoduleGid_ << " source submodule");
    
      // Identify source strip block assembly:
      _sourceStripBlockAssemblyType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_strip_block_assembly")) {
        _sourceStripBlockAssemblyType_ = _geoIdMgr_->get_category_info("source_strip_block_assembly").get_type();
        DT_LOG_DEBUG(_config_.verbosity, "Source  strip block assembly type is " << _sourceStripBlockAssemblyType_);
        geomtools::geom_id sourceStripBlockAssemblyGidPattern(_sourceStripBlockAssemblyType_, _config_.module_id);
        std::vector<geomtools::geom_id> sourceStripBlockAssemblyGids;
        _geoMapping_->compute_matching_geom_id(sourceStripBlockAssemblyGidPattern, sourceStripBlockAssemblyGids);
        DT_THROW_IF(sourceStripBlockAssemblyGids.size() != 1, std::logic_error, "Should have found 1 unique source strip block assembly!");
        _sourceStripBlockAssemblyGid_ = sourceStripBlockAssemblyGids[0];
	DT_LOG_DEBUG(_config_.verbosity, "Found source strip block assembly GID=" << _sourceStripBlockAssemblyGid_);
	const geomtools::geom_info & srcAssemblyGinfo    = _geoMapping_->get_geom_info(_sourceStripBlockAssemblyGid_);
	const geomtools::logical_volume & srcAssemblyLog = srcAssemblyGinfo.get_logical();
	const geomtools::i_shape_3d & srcAssemblyShape   = srcAssemblyLog.get_shape();         
	DT_LOG_DEBUG(_config_.verbosity, "Source submodule assembly shape = " << srcAssemblyShape.get_shape_name());
	const geomtools::box & tempBox = dynamic_cast<const geomtools::box &>(srcAssemblyShape);
	_sourceStripBlockAssemblyPlacement_ = &srcAssemblyGinfo.get_world_placement();
	_sourceStripBlockAssemblyBoxPtr_    = std::make_unique<geomtools::box>(tempBox.get_x(), 
									       tempBox.get_y(),
									       tempBox.get_z());
	_sourcePlaneBoxPtr_    = std::make_unique<geomtools::box>(2.0 * CLHEP::mm, 
								  tempBox.get_y() + 2 * _config_.effective_source_horizontal_tolerance,
								  tempBox.get_z() + 2 * _config_.effective_source_vertical_tolerance);
	DT_LOG_DEBUG(_config_.verbosity, "Source submodule assembly world placement = " << *_sourceStripBlockAssemblyPlacement_);
      }

      // Identify source strips:
      _sourceStripType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_strip")) {
        _sourceStripType_ = _geoIdMgr_->get_category_info("source_strip").get_type();
        DT_LOG_DEBUG(_config_.verbosity, "Source strip type is " << _sourceStripType_);
        geomtools::geom_id sourceStripGidPattern(_sourceStripType_,
                                                 _config_.module_id,
                                                 geomtools::geom_id::ANY_ADDRESS);
        _geoMapping_->compute_matching_geom_id(sourceStripGidPattern, _sourceStripGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourceStripGids_.size() << " source strips");
      if (_sourceStripGids_.size() == 0) {
        // 2024-03-05 FM: add guard against malformed geometry (no source strip)
        DT_THROW(std::logic_error, "No source strip was found in the geometry setup!");
      }
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
      DT_LOG_DEBUG(_config_.verbosity, "Min source strip ID=" << _sourceStripMinId_);
      DT_LOG_DEBUG(_config_.verbosity, "Max source strip ID=" << _sourceStripMaxId_);
      // Identify source strip gaps:
      _sourceStripGapType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_strip_gap")) {
        _sourceStripGapType_ = _geoIdMgr_->get_category_info("source_strip_gap").get_type();
        DT_LOG_DEBUG(_config_.verbosity, "Source strip gap type is " << _sourceStripGapType_);
        geomtools::geom_id sourceStripGapGidPattern(_sourceStripGapType_,
                                                    _config_.module_id,
                                                    geomtools::geom_id::ANY_ADDRESS);
        _geoMapping_->compute_matching_geom_id(sourceStripGapGidPattern, _sourceStripGapGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourceStripGapGids_.size() << " source strip gaps");
      if (_sourceStripGapGids_.size()) {
        DT_LOG_DEBUG(_config_.verbosity, "Extracting geometry infos about source strip gaps...");
        {
          // Edge gaps (gap=0|36):
          const geomtools::geom_info & srcStripEdgeGapGinfo = _geoManager_->get_mapping().get_geom_info(_sourceStripGapGids_[0]);
          const geomtools::logical_volume & srcStripEdgeGapLog  = srcStripEdgeGapGinfo.get_logical();
          const geomtools::i_shape_3d & srcStripEdgeGapShape    = srcStripEdgeGapLog.get_shape();    
          const geomtools::placement & srcStripEdgeGapPlacement = srcStripEdgeGapGinfo.get_world_placement();
          _sourceStripEdgeGapX_ = srcStripEdgeGapPlacement.get_translation().x();
          _sourceStripEdgeGapZ_ = srcStripEdgeGapPlacement.get_translation().z();
          const geomtools::box & srcStripEdgeGapBox = dynamic_cast<const geomtools::box &>(srcStripEdgeGapShape);
          _sourceStripEdgeGapBoxPtr_ = std::make_unique<geomtools::box>(srcStripEdgeGapBox.get_x(), // 4.0 * CLHEP::mm,
                                                                        srcStripEdgeGapBox.get_y(),
                                                                        srcStripEdgeGapBox.get_z());
        }
        {
          // Gaps (gap=1-35):
          const geomtools::geom_info & srcStripGapGinfo = _geoManager_->get_mapping().get_geom_info(_sourceStripGapGids_[1]);
          const geomtools::logical_volume & srcStripGapLog  = srcStripGapGinfo.get_logical();
          const geomtools::i_shape_3d & srcStripGapShape    = srcStripGapLog.get_shape();    
          const geomtools::placement & srcStripGapPlacement = srcStripGapGinfo.get_world_placement();
          _sourceStripGapX_ = srcStripGapPlacement.get_translation().x();
          _sourceStripGapZ_ = srcStripGapPlacement.get_translation().z();
          const geomtools::box & srcStripGapBox = dynamic_cast<const geomtools::box &>(srcStripGapShape);
          _sourceStripGapBoxPtr_ = std::make_unique<geomtools::box>(srcStripGapBox.get_x(),
                                                                    srcStripGapBox.get_y(),
                                                                    srcStripGapBox.get_z());
        }
      }
       
      // Identify source pads:
      _sourcePadType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_pad")) {
        _sourcePadType_ = _geoIdMgr_->get_category_info("source_pad").get_type();
        DT_LOG_DEBUG(_config_.verbosity, "Source pad type is " << _sourcePadType_);
        geomtools::geom_id sourcePadGidPattern(_sourcePadType_,
                                               _config_.module_id,
                                               geomtools::geom_id::ANY_ADDRESS,
                                               geomtools::geom_id::ANY_ADDRESS);
        _geoMapping_->compute_matching_geom_id(sourcePadGidPattern, _sourcePadGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourcePadGids_.size() << " source pads");

      // Identify source pad bulks (deformed pads from SNRS):
      _sourcePadBulkType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_pad_bulk")) {
        // Only detected for the SNRS deformed source strips:
        _sourcePadBulkType_ = _geoIdMgr_->get_category_info("source_pad_bulk").get_type();
        geomtools::geom_id sourcePadBulkGidPattern(_sourcePadBulkType_,
                                                   _config_.module_id,
                                                   geomtools::geom_id::ANY_ADDRESS,
                                                   geomtools::geom_id::ANY_ADDRESS);
        _geoMapping_->compute_matching_geom_id(sourcePadBulkGidPattern, _sourcePadBulkGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourcePadBulkGids_.size() << " source pad bulks");

      if (_sourcePadBulkGids_.size()) {
        _use_deformed_source_strips_ = true;
        DT_LOG_DEBUG(_config_.verbosity, "Using deformed source strips.");
      } else {
        DT_LOG_DEBUG(_config_.verbosity, "Using flat source strips.");
      }
      
      // Identify calibration sources:
      _sourceCalibrationSpotType_ = geomtools::geom_id::INVALID_TYPE;
      _sourceCalibrationCarrierType_ = geomtools::geom_id::INVALID_TYPE;
      
      if (_geoIdMgr_->has_category_info("source_calibration_carrier")) {
        _sourceCalibrationCarrierType_ = _geoIdMgr_->get_category_info("source_calibration_carrier").get_type();
        geomtools::geom_id sourceCalibrarionCarrierGidPattern(_sourceCalibrationCarrierType_,
                                                              _config_.module_id,
                                                              geomtools::geom_id::ANY_ADDRESS,  // track
                                                              geomtools::geom_id::ANY_ADDRESS); // position
        _geoMapping_->compute_matching_geom_id(sourceCalibrarionCarrierGidPattern, _sourceCalibrationCarrierGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Source calibration carrier type = " << _sourceCalibrationCarrierType_);
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourceCalibrationCarrierGids_.size() << " calibration source carriers");

      if (_geoIdMgr_->has_category_info("source_calibration_spot")) {
        _sourceCalibrationSpotType_ = _geoIdMgr_->get_category_info("source_calibration_spot").get_type();
        geomtools::geom_id sourceCalibrarionSpotGidPattern(_sourceCalibrationSpotType_,
                                                           _config_.module_id,
                                                           geomtools::geom_id::ANY_ADDRESS,  // track
                                                           geomtools::geom_id::ANY_ADDRESS); // position
        _geoMapping_->compute_matching_geom_id(sourceCalibrarionSpotGidPattern, _sourceCalibrationSpotGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Source calibration spot type = " << _sourceCalibrationSpotType_);
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourceCalibrationSpotGids_.size() << " calibration source spots");
     
      _sourceCalibTrackMinId_ = +100000;
      _sourceCalibTrackMaxId_ = -100000;
      static const int32_t trackNumIdx = 1;
      for (const auto & carrierGid : _sourceCalibrationCarrierGids_) {
        int32_t trackId = (int32_t) carrierGid.get(trackNumIdx);
        if (trackId > _sourceCalibTrackMaxId_) {
          _sourceCalibTrackMaxId_ = trackId;
        }
        if (trackId < _sourceCalibTrackMinId_) {
          _sourceCalibTrackMinId_ = trackId;
        }
      }
      DT_LOG_DEBUG(_config_.verbosity, "Min source calib track ID=" << _sourceCalibTrackMinId_);
      DT_LOG_DEBUG(_config_.verbosity, "Max source calib track ID=" << _sourceCalibTrackMaxId_);
      // Identify calibration source tracks:
      _sourceCalibTrackType_ = geomtools::geom_id::INVALID_TYPE;
      if (_geoIdMgr_->has_category_info("source_calibration_track")) {
        _sourceCalibTrackType_ = _geoIdMgr_->get_category_info("source_calibration_track").get_type();
        geomtools::geom_id sourceCalibTrackGidPattern(_sourceCalibTrackType_,
                                                      _config_.module_id,
                                                      geomtools::geom_id::ANY_ADDRESS);
        _geoMapping_->compute_matching_geom_id(sourceCalibTrackGidPattern, _sourceCalibTrackGids_);
      }
      DT_LOG_DEBUG(_config_.verbosity, "Found " << _sourceCalibTrackGids_.size() << " source calibration tracks");
      if (_sourceCalibTrackGids_.size()) {
        DT_LOG_DEBUG(_config_.verbosity, "Extracting geometry infos about source calibration tracks...");
        const geomtools::geom_info & srcCalibTrackGinfo
	  = _geoManager_->get_mapping().get_geom_info(_sourceCalibTrackGids_[0]);
        const geomtools::logical_volume & srcCalibTrackLog = srcCalibTrackGinfo.get_logical();
        const geomtools::i_shape_3d & srcCalibTrackShape = srcCalibTrackLog.get_shape();    
        const geomtools::placement & srcCalibTrackPlacement = srcCalibTrackGinfo.get_world_placement();
        _sourceCalibTrackX_ = srcCalibTrackPlacement.get_translation().x();
        _sourceCalibTrackZ_ = srcCalibTrackPlacement.get_translation().z();
        const geomtools::box & srcCalibTrackBox = dynamic_cast<const geomtools::box &>(srcCalibTrackShape);
        _sourceCalibTrackHeight_ = srcCalibTrackBox.get_z();
        double effectiveTrackBoxThickness = 4.25 * CLHEP::mm;
        DT_LOG_DEBUG(_config_.verbosity, "effectiveTrackBoxThickness = " << effectiveTrackBoxThickness / CLHEP::mm << " mm");
        
        _sourceCalibTrackBoxPtr_ = std::make_unique<geomtools::box>(effectiveTrackBoxThickness,
                                                                    srcCalibTrackBox.get_y(),
                                                                    _sourceCalibTrackHeight_);
        if (_sourceCalibrationCarrierGids_.size()) {
          const geomtools::geom_info & srcCalibCarrierGinfo
	    = _geoManager_->get_mapping().get_geom_info(_sourceCalibrationCarrierGids_[0]);
          const geomtools::logical_volume & srcCalibCarrierLog = srcCalibCarrierGinfo.get_logical();
          const geomtools::i_shape_3d & srcCalibCarrierShape = srcCalibCarrierLog.get_shape();    
          const geomtools::box & srcCalibCarrierBox = dynamic_cast<const geomtools::box &>(srcCalibCarrierShape);
          _sourceCalibrationCarrierBoxPtr_ = dynamic_cast<const geomtools::box*>(&srcCalibCarrierShape);
          _sourceCalibrationCarrierEffectiveBoxPtr_ = std::make_unique<geomtools::box>();
          // Const cast workaround because of a bug in Bayeux:
          const_cast<geomtools::box &>(srcCalibCarrierBox).compute_inflated(*_sourceCalibrationCarrierEffectiveBoxPtr_,
                                                                            _config_.calibration_source_extend_vertical,
                                                                            _config_.calibration_source_extend_horizontal,
                                                                            0. * CLHEP::mm);
	  DT_LOG_DEBUG(_config_.verbosity, "Using inflated shape for calibration source carrier:");
	  if (datatools::logger::is_debug(_config_.verbosity)) {
	    _sourceCalibrationCarrierBoxPtr_->tree_dump(std::cerr, "Original shape:", "[debug] ");
	    _sourceCalibrationCarrierEffectiveBoxPtr_->tree_dump(std::cerr, "Inflated shape:", "[debug] ");
	  }
        }
      }
   
      return;
    }
 
    void source_vertex_extrapolator::trajectory_source_intercept(snemo::geometry::vertex_info_list & vertexes_,
                                                                 const snemo::datamodel::base_trajectory_pattern & traj_,
                                                                 int from_) const
    {
      datatools::logger::priority logPrio = _config_.verbosity;
      DT_LOG_DEBUG(logPrio, "=====> New trajectory end");
      DT_LOG_DEBUG(logPrio, "Trajectory pattern : " << std::quoted(traj_.get_pattern_id()));
      DT_LOG_DEBUG(logPrio, "From : " << std::quoted(snemo::geometry::vertex_info::from_to_label(from_)));
      DT_THROW_IF(from_ != snemo::geometry::vertex_info::FROM_FIRST and from_ != snemo::geometry::vertex_info::FROM_LAST,
		  std::logic_error,
		  "Should have found 1 unique source strip block assembly!");
      vertexes_.clear();
      
      // Default configured for 'last':
      geomtools::vector_3d beginPoint = traj_.get_first();
      geomtools::vector_3d endPoint = traj_.get_last();
      DT_LOG_DEBUG(logPrio, "First point = " << beginPoint / CLHEP::mm << " mm");
      DT_LOG_DEBUG(logPrio, "Last point  = " << endPoint / CLHEP::mm << " mm");
      geomtools::vector_3d beginDirection = traj_.get_first_direction();
      geomtools::vector_3d endDirection = traj_.get_last_direction();
      DT_LOG_DEBUG(logPrio, "First direction = " << beginDirection);
      DT_LOG_DEBUG(logPrio, "Last direction  = " << endDirection);
      if (from_ == snemo::geometry::vertex_info::FROM_FIRST) {
	endPoint = traj_.get_first();
	endDirection = -traj_.get_first_direction();
      }
      const geomtools::vector_3d direction = endDirection; 
      DT_LOG_DEBUG(logPrio, "End point (world)    : " << geomtools::to_xyz(endPoint) );
      DT_LOG_DEBUG(logPrio, "Direction (world)    : " << geomtools::to_xyz(direction) );

      std::vector<geomtools::vector_3d> coneDirections;
      // Reference extrapolated vertex:
      coneDirections.push_back(direction);

      double deltaAlpha = 2 * M_PI / _config_.cone_nrays;
      for (auto iDir = 0u; iDir < _config_.cone_nrays; iDir++) {
	for (auto iLayer = 0u; iLayer < _config_.cone_nlayers; iLayer++) {
	  double alpha = iDir * deltaAlpha;
	  if (iLayer % 2) {
	    alpha += 0.5 * deltaAlpha;
	  }
	  double beta = _config_.cone_angle / (iLayer + 1);
	  geomtools::vector_3d dirAB;
	  geomtools::set_r_theta_phi(dirAB, 1.0, beta, alpha);
	  geomtools::placement pl;
	  pl.set_orientation_zyz(direction.phi(), direction.theta(), 0.0);
	  geomtools::vector_3d worldDirAB;
	  pl.child_to_mother_direction(dirAB, worldDirAB);
	  coneDirections.push_back(worldDirAB);
	}
      }

      snemo::geometry::vertex_info_list vertexes;
      for (auto coneRayId = 0u; coneRayId < coneDirections.size(); coneRayId++) {
	DT_LOG_DEBUG(logPrio, "==========> Cone ray #" << coneRayId);
	const auto & coneDir = coneDirections[coneRayId];
	// Optimization:
	geomtools::vector_3d backPoint = endPoint - _config_.finder_step * coneDir;
	geomtools::vector_3d refPoint = 0.5 * (backPoint + endPoint);
	{
	  if (refPoint.x() > 0.0 and direction.x() >= 0.0) {
	    DT_LOG_DEBUG(logPrio, "French reference point and french direction cannot intercept the source plane");
	    continue;
	  }
	  if (refPoint.x() < 0.0 and direction.x() <= 0.0) {
	    DT_LOG_DEBUG(logPrio, "Italian reference point and italian direction cannot intercept the source plane");
	    continue;
	  }
	}
	intercept_input intInput;
	intInput.from = from_;
	intInput.traj_end_pos = endPoint;
	intInput.traj_end_dir = coneDir;
	intInput.cone_axis = (coneRayId == 0); // Only the first cone ray corresponds to the cone axis
	intInput.reference_plane = (coneRayId == 0); // Store the vertex extrapolated on the reference plane for the cone axis only
	_trajectory_source_intercept_(vertexes, intInput);
	// Edge for source pad (bulk) ?
	if (coneRayId == 0 and vertexes.size() > 0) {
	  // If first reference is valid, no need to scan other rays:
	  const auto & lastVtx = vertexes.back();
	  if (lastVtx.reference and not lastVtx.edge) {
	    break;
	  }
	}
      }

      DT_LOG_DEBUG(logPrio, "Number of cone extrapolated vertexes before post-processing = " << vertexes.size());

      // Post processing of candidate vertexes:
      if (_config_.debug_no_post_processing) {
	DT_LOG_DEBUG(logPrio, "No post-processing of candidate vertexes...");
	for (const auto & vtxInfo : vertexes) {
	  vertexes_.push_back(vtxInfo);
	}
      } else {
	DT_LOG_DEBUG(logPrio, "Post-processing of candidate vertexes...");
	std::set<geomtools::geom_id> vtxGids;
	std::map<geomtools::geom_id, vertex_working_data> vtxWkDatas;
	for (const auto & vtxInfo : vertexes) {
	  const auto & gid = vtxInfo.gid;
	  if (not vtxGids.count(gid)) {
	    vertex_working_data vwd;
	    vwd.gid = gid;
	    // Pick up the first vertex info:
	    vwd.vtx = vtxInfo;
	    vtxWkDatas.emplace(gid, vwd);
	    vtxGids.insert(gid);
	  }
	}
     
	for (auto & vwdItem : vtxWkDatas) {
	  const auto & gid = vwdItem.first;
	  vertex_working_data & vwd = vwdItem.second;
	  for (const auto & vtxInfo : vertexes_) {
	    if (vtxInfo.gid != gid) continue;
	    // Compute the average impact position:
	    vwd.n++;
	    double x = vtxInfo.face_intercept.get_impact().x();
	    double y = vtxInfo.face_intercept.get_impact().y();
	    double z = vtxInfo.face_intercept.get_impact().z();
	    vwd.xbar += x;
	    vwd.ybar += y;
	    vwd.zbar += z;
	    vwd.x2bar += x * x;
	    vwd.y2bar += y * y;
	    vwd.z2bar += z * z;
	    if (vtxInfo.edge) {
	      vwd.nbEdges++;
	    }
	  }	
	}
      
	for (auto & vwdItem : vtxWkDatas) {
	  // const auto & gid = vwdItem.first;
	  vertex_working_data & vwd = vwdItem.second;
	  if (vwd.n > 1) {
	    vwd.xbar /= vwd.n;
	    vwd.ybar /= vwd.n;
	    vwd.zbar /= vwd.n;
	    vwd.x2bar /= vwd.n;
	    vwd.y2bar /= vwd.n;
	    vwd.z2bar /= vwd.n;
	    geomtools::vector_3d impact(vwd.xbar, vwd.ybar, vwd.zbar);
	    geomtools::vector_2d impact2(vwd.xbar, vwd.ybar);
	    geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
	    vwd.vtx.face_intercept.set_impact(impact);
	    double dist = (impact - endPoint).mag();
	    double dist_Xy = (impact2 - endPoint_Xy).mag();
	    vwd.vtx.distance = dist;
	    vwd.vtx.distance_xy = dist_Xy;
	    vwd.vtx.edge = (vwd.n == vwd.nbEdges);
	    vwd.vtx.xerr = std::sqrt(vwd.x2bar + vwd.xbar * vwd.xbar);
	    vwd.vtx.yerr = std::sqrt(vwd.y2bar + vwd.ybar * vwd.ybar);
	    vwd.vtx.zerr = std::sqrt(vwd.z2bar + vwd.zbar * vwd.zbar);
	  }
	}
      
	for (auto & vwdItem : vtxWkDatas) {
	  vertex_working_data & vwd = vwdItem.second;
	  vertexes_.push_back(vwd.vtx);
	}
      }
      
      DT_LOG_DEBUG(logPrio, "Vertexes #" << vertexes_.size());
      return;
    }
       
    void source_vertex_extrapolator::_trajectory_source_intercept_(snemo::geometry::vertex_info_list & vertexes_,
								   const intercept_input & input_) const
    {
      datatools::logger::priority logPrio = _config_.verbosity;
      const geomtools::vector_3d & endPoint = input_.traj_end_pos;
      const geomtools::vector_3d & direction = input_.traj_end_dir; 
      geomtools::vector_3d backPoint = endPoint - _config_.finder_step * direction;
      geomtools::vector_3d refPoint = 0.5 * (backPoint + endPoint);
      geomtools::vector_2d endPoint_Xy(endPoint.x(), endPoint.y());
      geomtools::vector_2d refPoint_Xy(refPoint.x(), refPoint.y());
      DT_LOG_DEBUG(logPrio, "endPoint       x = " << endPoint.x()); 
      DT_LOG_DEBUG(logPrio, "refPoint       x = " << refPoint.x());
      // Move at caller level
      // {
      // 	if (refPoint.x() > 0.0 and direction.x() >= 0.0) {
      // 	  DT_LOG_DEBUG(logPrio, "French reference point and french direction cannot intercept the source plane");
      // 	  return;
      // 	}
      // 	if (refPoint.x() < 0.0 and direction.x() <= 0.0) {
      // 	  DT_LOG_DEBUG(logPrio, "Italian reference point and italian direction cannot intercept the source plane");
      // 	  return;
      // 	}
      // }
      DT_LOG_DEBUG(logPrio, "Assembly block x = " << _sourceStripBlockAssemblyBoxPtr_->get_x());
      double cumulDx = 0.0;
      double dl = std::min(5.0 * CLHEP::mm, 0.25 * _config_.finder_step);
      bool refPointOk = false;
      while (true) {	
	if (std::fabs(refPoint.x()) < 0.5 * _sourceStripBlockAssemblyBoxPtr_->get_x()) {
	  // DT_THROW(std::domain_error, "Reference point cannot be in the source strip block assembly region");
	  geomtools::vector_3d newRefPoint = refPoint - dl * direction;
	  double deltaX = std::fabs(newRefPoint.x() - refPoint.x());
	  DT_LOG_DEBUG(logPrio, "Attempt to step back ref point out of the source strip block assembly region...");
	  cumulDx += deltaX;
	  refPoint = newRefPoint;
	  refPoint_Xy.set(refPoint.x(), refPoint.y());
	  if (cumulDx > 0.5 * _sourceStripBlockAssemblyBoxPtr_->get_x()) {
	    DT_LOG_DEBUG(logPrio, "Step back limit is reached.");
	    break;
	  }
	} else {
	  refPointOk = true;
	  break;
	}
      }
      if (not refPointOk) {
	DT_LOG_WARNING(logPrio, "Invalid reference point! No vertex extrapolation is possible.");
	return;
      }
      DT_LOG_DEBUG(logPrio, "#vertexes = " << vertexes_.size());
      double distRef2End    = (endPoint - refPoint).mag();
      double distRef2End_Xy = (endPoint_Xy - refPoint_Xy).mag();
      DT_LOG_DEBUG(logPrio, "distRef2End = " << distRef2End / CLHEP::mm << " mm");
      DT_LOG_DEBUG(logPrio, "distRef2End_Xy = " << distRef2End_Xy / CLHEP::mm << " mm");

      snemo::geometry::vertex_info referencePlaneVtxInfo; // Default: invalid
      // First search for candidate volumes from some intercept on the reference source plane:
      std::vector<geomtools::geom_id> candidateGids;
      {
	DT_LOG_DEBUG(logPrio, "Searching line intercept on the reference source plane...");
	geomtools::vector_3d srcPlaneRefPoint;
	_sourceStripBlockAssemblyPlacement_->mother_to_child(refPoint, srcPlaneRefPoint);
	geomtools::vector_3d srcPlaneDirection;
	_sourceStripBlockAssemblyPlacement_->mother_to_child_direction(direction, srcPlaneDirection);
	// Check if the track hits the submodule's source plane (a virtual box of 2 mm in thickness):
	geomtools::face_intercept_info srcPlaneFii;
	bool success = _sourcePlaneBoxPtr_->find_intercept(srcPlaneRefPoint,
							   srcPlaneDirection,
							   srcPlaneFii,
							   _config_.intercept_tolerance);          
	if (success) {
	  DT_LOG_DEBUG(logPrio, "Found an intercept on the source plane at " << srcPlaneFii.get_impact());
	  const geomtools::vector_3d & impact = srcPlaneFii.get_impact();

	  if (input_.reference_plane) {
	    DT_LOG_DEBUG(logPrio, "Prepare the vertex extrapolated on the reference source plane");
	    geomtools::vector_3d referencePlaneWorldImpact;
	    _sourceStripBlockAssemblyPlacement_->child_to_mother(impact, referencePlaneWorldImpact);
	    double referencePlaneExtrapolationDist = (referencePlaneWorldImpact - endPoint).mag();
	    geomtools::vector_2d referencePlaneWorldImpact_Xy(referencePlaneWorldImpact.x(), referencePlaneWorldImpact.y());
	    double referencePlaneExtrapolationDist_Xy = (referencePlaneWorldImpact_Xy - endPoint_Xy).mag();
	    referencePlaneVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_REFERENCE_SOURCE_PLANE;
	    referencePlaneVtxInfo.from = input_.from;
	    referencePlaneVtxInfo.extrapolation_mode = snemo::geometry::vertex_info::EXTRAPOLATION_LINE;
	    referencePlaneVtxInfo.gid = _sourceSubmoduleGid_;
	    referencePlaneVtxInfo.face_intercept = srcPlaneFii;
	    referencePlaneVtxInfo.face_intercept.set_impact(referencePlaneWorldImpact);
	    referencePlaneVtxInfo.distance = referencePlaneExtrapolationDist;
	    referencePlaneVtxInfo.distance_xy = referencePlaneExtrapolationDist_Xy;
	    referencePlaneVtxInfo.tolerance = _config_.intercept_tolerance;
	    referencePlaneVtxInfo.best = false;
	    referencePlaneVtxInfo.edge = false;
	    referencePlaneVtxInfo.reference = false;
	  }

	  // Now locate candidate volumes at the source plane level that the trajectory may intersect...
	  double yImpact = impact.y();
	  double zImpact = impact.z();
	  double longitude = direction.phi();
	  double colatitude = direction.theta();
	  double latitude = 0.5 * M_PI - colatitude;
	  DT_LOG_DEBUG(logPrio, "longitude = " << longitude / CLHEP::degree << "°"
		       << " colatitude=" << colatitude / CLHEP::degree << "°"
		       << " latitude=" << latitude / CLHEP::degree << "°");
	  double dirCosLongitude = std::fabs(std::cos(longitude));
	  double dirCosLatitude = std::fabs(std::cos(latitude));
	  DT_LOG_DEBUG(logPrio, "dirCosLatitude = " << dirCosLatitude << "   dirCosLongitude = " << dirCosLongitude);
	  // Take into account the incidence angle of the trajectory direction on the surface
	  // to enlarge the tolerance in the YZ plane:
	  double ySourceTolerance = std::min(_config_.max_source_extrapolation_horizontal_length,
					     3 * _config_.effective_source_horizontal_tolerance / dirCosLongitude);
	  double zSourceTolerance = std::min(_config_.max_source_extrapolation_vertical_length,
					     3 * _config_.effective_source_vertical_tolerance / dirCosLatitude);
	  DT_LOG_DEBUG(logPrio, "yImpact = " << yImpact / CLHEP::mm << " mm");
	  DT_LOG_DEBUG(logPrio, "ySourceTolerance = " << ySourceTolerance / CLHEP::mm << " mm");
	  DT_LOG_DEBUG(logPrio, "zImpact = " << zImpact / CLHEP::mm << " mm");
	  DT_LOG_DEBUG(logPrio, "zSourceTolerance = " << zSourceTolerance / CLHEP::mm << " mm");

	  // Find candidate source strip gaps candidate:
	  DT_LOG_DEBUG(logPrio, "# source strip gaps = " << _sourceStripGapGids_.size());
	  for (uint32_t iStripGap = 0; iStripGap < _sourceStripGapGids_.size(); iStripGap++) {
	    const geomtools::geom_id & sourceStripGapGid = _sourceStripGapGids_[iStripGap];
	    DT_LOG_TRACE(logPrio, "Scanning strip gap " << sourceStripGapGid);
	    const geomtools::geom_info & sourceStripGapGinfo = _geoMapping_->get_geom_info(sourceStripGapGid);
	    const geomtools::placement & sourceStripGapPlacement = sourceStripGapGinfo.get_world_placement();
	    double yStripGap = sourceStripGapPlacement.get_translation().y();
	    DT_LOG_TRACE(logPrio, "sourceStripGapGid=" << sourceStripGapGid << " yStripGap=" << yStripGap / CLHEP::mm << " mm");
	    if (std::fabs(yStripGap - yImpact) < ySourceTolerance) {
	      candidateGids.push_back(sourceStripGapGid);
	      DT_LOG_DEBUG(logPrio, "Strip gap " << sourceStripGapGid << " selected.");
	    }
	  }

	  // Find candidate calib source tracks candidate:
	  DT_LOG_DEBUG(logPrio, "# calib source tracks = " << _sourceCalibTrackGids_.size());
	  std::set<int> calibTrackIds;
	  for (uint32_t iTrack = 0; iTrack < _sourceCalibTrackGids_.size(); iTrack++) {
	    const geomtools::geom_id & sourceCalibTrackGid = _sourceCalibTrackGids_[iTrack];
	    DT_LOG_TRACE(logPrio, "Scanning calib source track " << sourceCalibTrackGid);
	    const geomtools::geom_info & sourceCalibTrackGinfo = _geoMapping_->get_geom_info(sourceCalibTrackGid);
	    const geomtools::placement & sourceCalibTrackPlacement = sourceCalibTrackGinfo.get_world_placement();
	    double yCalibTrack = sourceCalibTrackPlacement.get_translation().y();
	    if (std::fabs(yCalibTrack - yImpact) < ySourceTolerance) {
	      candidateGids.push_back(sourceCalibTrackGid);
	      DT_LOG_DEBUG(logPrio, "Calib. source track " << sourceCalibTrackGid << " selected.");
	      calibTrackIds.insert(sourceCalibTrackGid.get(1));
	    }
	  }
	  for (auto tid : calibTrackIds) {
	    DT_LOG_DEBUG(logPrio, " - candidate calib. source track number = " << tid);
	  }

	  // Find candidate source strips:
	  DT_LOG_DEBUG(logPrio, "# source strips = " << _sourceStripGids_.size());
	  std::set<int> stripIds;
	  for (uint32_t iStrip = 0; iStrip < _sourceStripGids_.size(); iStrip++) {
	    const geomtools::geom_id & sourceStripGid = _sourceStripGids_[iStrip];
	    DT_LOG_TRACE(logPrio, "Scanning source strip " << sourceStripGid);
	    const geomtools::geom_info & sourceStripGinfo = _geoMapping_->get_geom_info(sourceStripGid);
	    const geomtools::placement & sourceStripPlacement = sourceStripGinfo.get_world_placement();
	    double yStrip = sourceStripPlacement.get_translation().y();
	    if (std::fabs(yStrip - yImpact) < ySourceTolerance) {
	      candidateGids.push_back(sourceStripGid);
	      DT_LOG_DEBUG(logPrio, "Source strip " << sourceStripGid << " selected.");
	      stripIds.insert(sourceStripGid.get(1));
	    }
	  }
	  for (auto sid : stripIds) {
	    DT_LOG_DEBUG(logPrio, " - candidate source strip number = " << sid);
	  }
	  
	  DT_LOG_DEBUG(logPrio, "# calib source carriers = " << _sourceCalibrationCarrierGids_.size());
	  for (uint32_t iCalibSrcCarrier = 0u; iCalibSrcCarrier < _sourceCalibrationCarrierGids_.size(); iCalibSrcCarrier++) {
	    const geomtools::geom_id & calibSourceCarrierGid = _sourceCalibrationCarrierGids_[iCalibSrcCarrier];
	    // DT_LOG_TRACE(logPrio, "Scanning calib. source carrier " << calibSourceCarrierGid);
	    auto tkId = calibSourceCarrierGid.get(1);
	    DT_LOG_TRACE(logPrio, "Checking carrier track number " << tkId);
	    if (calibTrackIds.count(calibSourceCarrierGid.get(1)) == 0) {
	      DT_LOG_TRACE(logPrio, "Not in any candidate calib. source track...");
	      continue;
	    } 
	    DT_LOG_DEBUG(logPrio, "Calib. source carrier " << calibSourceCarrierGid << " found in a candidate calib. track");
	    const geomtools::geom_info & calibSourceCarrierGinfo = _geoMapping_->get_geom_info(calibSourceCarrierGid);
	    const geomtools::placement & calibSourceCarrierPlacement = calibSourceCarrierGinfo.get_world_placement();
	    const geomtools::logical_volume & calibSourceCarrierLog = calibSourceCarrierGinfo.get_logical();
	    const geomtools::i_shape_3d & calibSourceCarrierShape = calibSourceCarrierLog.get_shape();
	    const geomtools::bounding_data & bb = calibSourceCarrierShape.get_bounding_data();
	    if (datatools::logger::is_trace(logPrio)) {
	      bb.tree_dump(std::cerr, "Calib. source carrier BB:", "[trace] ");
	    }
	    // Select only carrier in some vertical:
	    double zMin = calibSourceCarrierPlacement.get_translation().z() + bb.get_xmin() - zSourceTolerance;
	    double zMax = calibSourceCarrierPlacement.get_translation().z() + bb.get_xmax() + zSourceTolerance;
	    DT_LOG_DEBUG(logPrio, "calibSourceCarrierGid=" << calibSourceCarrierGid << " z-carrier=[" << zMin / CLHEP::mm << ";"
			 << zMax / CLHEP::mm  << "] mm  vs zImpact=" << zImpact / CLHEP::mm << " mm");
	    if (zImpact < zMax and zImpact > zMin) {
	      candidateGids.push_back(calibSourceCarrierGid);
	      DT_LOG_DEBUG(logPrio, "Calib. source carrier " << calibSourceCarrierGid << " selected.");
	    }	      
	  }
	  
	  DT_LOG_DEBUG(logPrio, "# source pads = " << _sourcePadGids_.size());
	  for (uint32_t iPad = 0u; iPad < _sourcePadGids_.size(); iPad++) {
	    const geomtools::geom_id & sourcePadGid = _sourcePadGids_[iPad];
	    DT_LOG_TRACE(logPrio, "Scanning source pad " << sourcePadGid);
	    if (stripIds.count(sourcePadGid.get(1)) == 0) {
	      DT_LOG_TRACE(logPrio, "Not in any candidate source strip...");
	      continue;
	    }
	    DT_LOG_TRACE(logPrio, "Source pad " << sourcePadGid << " found in a candidate source strip");
	    const geomtools::geom_info & sourcePadGinfo = _geoMapping_->get_geom_info(sourcePadGid);
	    const geomtools::placement & sourcePadPlacement = sourcePadGinfo.get_world_placement();
	    const geomtools::logical_volume & sourcePadLog = sourcePadGinfo.get_logical();
	    const geomtools::i_shape_3d & sourcePadShape = sourcePadLog.get_shape();
	    const geomtools::bounding_data & bb = sourcePadShape.get_bounding_data();
	    if (datatools::logger::is_trace(logPrio)) {
	      bb.tree_dump(std::cerr, "Source pad BB:", "[trace] ");
	    }
	    double zPadMin = sourcePadPlacement.get_translation().z() + bb.get_zmin();
	    double zPadMax = sourcePadPlacement.get_translation().z() + bb.get_zmax();
	    DT_LOG_TRACE(logPrio, "sourcePadGid=" << sourcePadGid << " z-pad=[" << zPadMin / CLHEP::mm
			 << ";" << zPadMax / CLHEP::mm  << "] mm vs zImpact=" << zImpact / CLHEP::mm << " mm");
	    if (zImpact < zPadMax and zImpact > zPadMin) {
	      candidateGids.push_back(sourcePadGid);
	      DT_LOG_DEBUG(logPrio, "Source pad " << sourcePadGid << " selected.");
	    }
	  }
	  
	  DT_LOG_DEBUG(logPrio, "# source pad bulks = " << _sourcePadBulkGids_.size());
	  for (uint32_t iPadBulk = 0u; iPadBulk < _sourcePadBulkGids_.size(); iPadBulk++) {
	    const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[iPadBulk];
	    if (stripIds.count(sourcePadBulkGid.get(1)) == 0) continue;
	    DT_LOG_TRACE(logPrio, "Scanning source pad bulk " << sourcePadBulkGid << " in a candidate strip");
	    const geomtools::geom_info & sourcePadBulkGinfo = _geoMapping_->get_geom_info(sourcePadBulkGid);
	    const geomtools::placement & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
	    const geomtools::logical_volume & sourcePadBulkLog = sourcePadBulkGinfo.get_logical();
	    const geomtools::i_shape_3d & sourcePadBulkShape = sourcePadBulkLog.get_shape();
	    const geomtools::bounding_data & bb = sourcePadBulkShape.get_bounding_data();
	    // bb.tree_dump(std::cerr, "Source pad bulk BB:", "[debug] ");
	    double zPadBulkMin = sourcePadBulkPlacement.get_translation().z() + bb.get_zmin();
	    double zPadBulkMax = sourcePadBulkPlacement.get_translation().z() + bb.get_zmax();
	    DT_LOG_TRACE(logPrio, "sourcePadBulkGid=" << sourcePadBulkGid << " z-pad_bulk=[" << zPadBulkMin / CLHEP::mm
			 << ";" << zPadBulkMax / CLHEP::mm  << "] mm  vs zImpact=" << zImpact / CLHEP::mm << " mm");
	    if (zImpact < zPadBulkMax and zImpact > zPadBulkMin) {
	      candidateGids.push_back(sourcePadBulkGid);
	      DT_LOG_DEBUG(logPrio, "Source pad bulk " << sourcePadBulkGid << " selected.");
	    }
	  }
	} // (success)
	
 	DT_LOG_DEBUG(logPrio, "Identified candidate volumes : " << candidateGids.size());
	if (datatools::logger::is_debug(logPrio)) {
	  for (auto & gid : candidateGids) {
	    auto geomType = gid.get_type();
	    std::cerr << " - Candidate : " << gid;
	    std::cerr << " (category = " << std::quoted(_geoIdMgr_->get_category_info(geomType).get_category()) << ')';
	    std::cerr << '\n';
	  }
	}
      }

      // Now explicitely scan all the candidate volumes for extrapolated vertexes:
      snemo::geometry::vertex_info_list srcVertexes;
      // std::set<geomtools::geom_id> vtxGeomIds;  
      for (const auto & volumeGid : candidateGids) {
        auto geoType = volumeGid.get_type();
        if (geoType == _sourcePadType_
            or geoType == _sourcePadBulkType_
            or geoType == _sourceCalibrationCarrierType_) {
          DT_LOG_DEBUG(logPrio, "==> Searching line intercept on candidate volume with GID=" << volumeGid);
        } else {
          continue;
        }
        const geomtools::geom_info & volumeGinfo = _geoMapping_->get_geom_info(volumeGid);
        const geomtools::logical_volume & volumeLog = volumeGinfo.get_logical();
        const geomtools::i_shape_3d * volumeShapePtr = &volumeLog.get_shape();
        if (geoType == _sourceCalibrationCarrierType_) {
          // Use enlarged shape for calib. source carrier:
          volumeShapePtr = _sourceCalibrationCarrierEffectiveBoxPtr_.get();
        }
        const geomtools::placement & volumePlacement = volumeGinfo.get_world_placement();
        geomtools::vector_3d volumeRefPoint;
        volumePlacement.mother_to_child(refPoint, volumeRefPoint);
        geomtools::vector_3d volumeDirection;
        volumePlacement.mother_to_child_direction(direction, volumeDirection);
        DT_LOG_DEBUG(logPrio, "Ref point   : " << geomtools::to_xyz(volumeRefPoint) );
        DT_LOG_DEBUG(logPrio, "Direction   : " << geomtools::to_xyz(volumeDirection) );
        DT_LOG_DEBUG(logPrio, "Volume shape  : '" << volumeShapePtr->get_shape_name() << "'");
        DT_LOG_DEBUG(logPrio, "Intercept tolerance : " << _config_.intercept_tolerance / CLHEP::mm << " mm");
        geomtools::face_intercept_info volumeFii;
        if (datatools::logger::is_trace(_config_.verbosity)) {
          volumeShapePtr->tree_dump(std::cerr, "Shape: ", "[trace] ");
        }
        bool success = volumeShapePtr->find_intercept(volumeRefPoint,
                                                      volumeDirection,
                                                      volumeFii,
                                                      _config_.intercept_tolerance);
        if (! success) {
          DT_LOG_TRACE(logPrio, "No intersection with volume " << volumeGid << "!");
          // No intersection with the volume.
          continue;
        }
        DT_LOG_DEBUG(logPrio, "Found line intercept on volume " << volumeGid);
        geomtools::vector_3d volumeImpact = volumeFii.get_impact();
        geomtools::vector_3d volumeImpactWorldImpact;
        volumePlacement.child_to_mother(volumeImpact, volumeImpactWorldImpact);
        //  Set volumeFii impact point in world coord. system frame:
        volumeFii.set_impact(volumeImpactWorldImpact);
        bool edgeSourceCalibrationCarrier = false;
        if (geoType == _sourceCalibrationCarrierType_) {
          // Trick for impact on extended source calib. carrier shape and associated "edge" extrapolated vertex:
          double x = volumeFii.get_impact().x();
          double y = volumeFii.get_impact().y();
          double z = volumeFii.get_impact().z();
          double x0 = volumePlacement.get_translation().x();
          double y0 = volumePlacement.get_translation().y();
          double z0 = volumePlacement.get_translation().z();
          double dy = y - y0;
          double dz = z - z0;
          int sy = dy > 0.0 ? +1 : -1;
          int sz = dz > 0.0 ? +1 : -1;
          // Beware ! 90° rotated box along the Y axis
          double carrierDX = _sourceCalibrationCarrierBoxPtr_->get_half_z();
          double carrierDY = _sourceCalibrationCarrierBoxPtr_->get_half_y();
          double carrierDZ = _sourceCalibrationCarrierBoxPtr_->get_half_x();
          if (x < x0 + carrierDX and x > x0 - carrierDX) {
            // Adjust the impact X coord to the edge of the shape:
            DT_LOG_DEBUG(logPrio, "Adjust the impact X coord to the edge of the carrier shape " << volumeGid);
            x = x0 + (refPoint.x() > 0.0 ? +1 : -1) * carrierDX;
            volumeFii.grab_impact().setX(x);
          }
          if (std::fabs(dy) > carrierDY) {
            // Adjust the impact Y coord to the edge of the shape:
            y = y0 + sy * carrierDY;
            edgeSourceCalibrationCarrier = true;
            volumeFii.grab_impact().setY(y);
          }
          if (std::fabs(dz) > carrierDZ) {
            // Adjust the impact Z coord to the edge of the shape:
            z = z0 + sz * carrierDZ;
            edgeSourceCalibrationCarrier = true;
            volumeFii.grab_impact().setZ(z);
          }
          // From here, if the extrapolated vertex was detected in the extended of edge part of the
          // source carrier, it is tagged as 'edge' and forced to a edge position of the carrier box shape.
	}

	// Construct an intercept on a volume:
	snemo::geometry::vertex_info volumeVtxInfo;
	double volumeDistRef2Impact = (volumeImpactWorldImpact - refPoint).mag();
	double volumeExtrapolationDist = volumeDistRef2Impact - distRef2End;
	DT_LOG_DEBUG(logPrio, "volumeExtrapolationDist    = " << volumeExtrapolationDist / CLHEP::mm << "  mm");
	// XY-extrapolation:
	geomtools::vector_2d volumeWorldImpact_Xy(volumeImpactWorldImpact.x(), volumeImpactWorldImpact.y());
	double volumeDistRef2Impact_Xy = (volumeWorldImpact_Xy - refPoint_Xy).mag();
	double volumeExtrapolationDist_Xy = volumeDistRef2Impact_Xy - distRef2End_Xy;
	DT_LOG_DEBUG(logPrio, "volumeExtrapolationDist_Xy = " << volumeExtrapolationDist_Xy / CLHEP::mm << "  mm");
	// Result:
	if (geoType == _sourceStripGapType_ or geoType == _sourceCalibTrackType_) {
	  volumeVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP;
	} else if (geoType == _sourcePadType_ or geoType == _sourcePadBulkType_) {
	  volumeVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL;
	} else if (geoType == _sourceCalibrationCarrierType_) {
	  volumeVtxInfo.category = snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE;
	} else {
	  continue;
	}
	volumeVtxInfo.from = input_.from; 
	volumeVtxInfo.extrapolation_mode = snemo::geometry::vertex_info::EXTRAPOLATION_LINE;
	volumeVtxInfo.gid = volumeGid;
	volumeVtxInfo.face_intercept = volumeFii;
	if (volumeExtrapolationDist > 0.0) {
	  volumeVtxInfo.distance    = volumeExtrapolationDist;
	  volumeVtxInfo.distance_xy = volumeExtrapolationDist_Xy;
	} else {
	  volumeVtxInfo.distance = 0.0;
	  volumeVtxInfo.distance_xy = 0.0;
	}
	volumeVtxInfo.edge = false;
	if (geoType == _sourceCalibrationCarrierType_) {
	  if (edgeSourceCalibrationCarrier) {
	    volumeVtxInfo.edge = true; // "edge" tag for adjusted vertex on extended calib. source carrier
	  }
	}

	volumeVtxInfo.tolerance = _config_.intercept_tolerance;
	if (datatools::logger::is_debug(logPrio)) {
	  DT_LOG_DEBUG(logPrio, "Line intercept is :");
	  volumeVtxInfo.print(std::cerr, "[debug] ");
	}
	DT_LOG_DEBUG(logPrio, "volumeVtxInfo.distance_xy = " << volumeVtxInfo.distance_xy / CLHEP::mm << " mm");
	// Extrapolation distance check:
	bool storeIt = false;
	if (volumeVtxInfo.distance_xy >= _config_.max_source_extrapolation_horizontal_length) {
	  DT_LOG_DEBUG(logPrio, "Line intercept is too far from volume " << volumeGid);
	} else {
	  storeIt = true;
	}
	if (storeIt) {
	  DT_LOG_DEBUG(logPrio, "Store this line intercept on volume " << volumeGid);
	  srcVertexes.push_back(volumeVtxInfo);
	  // vtxGeomIds.insert(volumeGid);
	} else {
	  DT_LOG_TRACE(logPrio, "Line intercept on volume " << volumeGid << " is duplicated");
	} 
      } // end of the scanning of candidate volume

      // Keep all vertexes but some artefact:
      // Due to the complexity of the geometry model for realistic ITEP-style source,
      // the source pad does not correspond to the effective bended source foil in that
      // case. We must target a "source pad bulk" volume whcih belongs to the ITEP source
      // pad considered as a bounding box.
      // So if a vertex was found on a "source pad" for ITEP foil, we discard it because
      // we expect a vertex on the corresponding "source pad bulk".
      DT_LOG_DEBUG(logPrio, "Number of line intercepts : " << srcVertexes.size());
      srcVertexes.erase(std::remove_if(srcVertexes.begin(),
				       srcVertexes.end(),
				       [&](auto & srcVtxInfo) -> bool {
					 bool removeIt = false;
					 const auto & gid = srcVtxInfo.gid;
					 if (gid.get_type() == _sourcePadType_) {
					   // Trick for source pads with pad bulk volume (ITEP, realistic shape):
					   geomtools::geom_id bulkGid = gid;
					   bulkGid.set_type(_sourcePadBulkType_);
					   // Remove pad volume if associated pad bulk exists:
					   if (std::find(_sourcePadBulkGids_.begin(),
							 _sourcePadBulkGids_.end(),
							 bulkGid) != _sourcePadBulkGids_.end()) {
					     DT_LOG_DEBUG(logPrio, "Remove pad volume " << gid << " because pad bulk " << bulkGid << " exists");
					     removeIt = true;
					   }
					 }
					 return removeIt;
				       }),
			srcVertexes.end());
      DT_LOG_DEBUG(logPrio, "Number of line intercepts after pad bulk trick : " << srcVertexes.size());

      auto previousNbVtx = vertexes_.size();     
      DT_LOG_DEBUG(logPrio, "Current number of stored extrapolated vertexes : " << previousNbVtx);
      {
	// Only keep the vertex with shortest extrapolation distance
	int bestIdx = -1;
	double minExtrapolationDistance = std::numeric_limits<double>::infinity();
	int edgeCarrierIdx = -1;
	double minEdgeCarrierDist = std::numeric_limits<double>::infinity();
	for (auto idx = 0u; idx < srcVertexes.size(); idx++) {
	  const auto & srcVtxInfo = srcVertexes[idx];
	  if (srcVtxInfo.category == snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE) {
	    if (srcVtxInfo.edge) {
	      // Search for the edge vertex on source calibration carrier with shorter extrapolation distance:
	      if (srcVtxInfo.distance < minEdgeCarrierDist) {
		edgeCarrierIdx = idx;
		minEdgeCarrierDist = srcVtxInfo.distance;
	      }
	      // Do not take edge carrier into account for the calculation of the min extrapolation distance:
	      continue;
	    }
	  }
	  double extrapolationDist = srcVtxInfo.distance;
	  if (extrapolationDist < minExtrapolationDistance) {
	    bestIdx = idx;
	    minExtrapolationDistance = extrapolationDist;
	  }
	}
	DT_LOG_DEBUG(logPrio, "bestIdx=" << bestIdx);
	DT_LOG_DEBUG(logPrio, "edgeCarrierIdx=" << edgeCarrierIdx);

	// Fill the reconstructed list of vertexes:
	if (input_.reference_plane and referencePlaneVtxInfo.category != snemo::geometry::vertex_info::CATEGORY_UNDEF) {
	  DT_LOG_DEBUG(logPrio, "Store the vertex extrapolated on the reference source plane");
	  // We assume that no other vertex on reference source plane exists...
	  vertexes_.push_back(referencePlaneVtxInfo);
	}
	
	bool storeIt = false;
	bool detectDuplicatedVertexes = true;
	double duplicateTolerance = _config_.duplicate_tolerance;
	if (_config_.debug_no_duplicate_detect) {
	  detectDuplicatedVertexes = false;
	}
	if (bestIdx >= 0) {
	  if (detectDuplicatedVertexes) {
	    // Detect if some duplicated vertexes exist:
	    if (std::find_if(vertexes_.begin(),
			     vertexes_.end(),
			     [&] (const snemo::geometry::vertex_info & vtx_info_) -> bool {
			       bool comp = 
				 snemo::geometry::vertex_info::equal(vtx_info_, 
								     srcVertexes[bestIdx],
								     duplicateTolerance);
			       DT_LOG_DEBUG(logPrio, "Compare stored " << vtx_info_.face_intercept.get_impact()
					    << " vs " << srcVertexes[bestIdx].face_intercept.get_impact()
					    << " -> " << std::boolalpha << comp);			     
			       return comp;
			     }) == vertexes_.end()) {
	      storeIt = true;
	    }
	  } else {
	    storeIt = true;
	  }    
	  if (storeIt) {
	    DT_LOG_DEBUG(logPrio, "Store the vertex extrapolated on volume " << srcVertexes[bestIdx].gid);
	    vertexes_.push_back(srcVertexes[bestIdx]);
	    if (input_.cone_axis) {
	      vertexes_.back().reference = true;
	    }
	  }
	}
	
	// Special trick to preserve edge impact on a source carrier 
	// which is closer to the trajectory endpoint:
	if (edgeCarrierIdx >= 0) {
	  if (minEdgeCarrierDist < minExtrapolationDistance) {
	    storeIt = false;
	    if (detectDuplicatedVertexes) {
	      // Detect if a duplicated vertex:
	      if (std::find_if(vertexes_.begin(),
			       vertexes_.end(),
			       [&] (const snemo::geometry::vertex_info & vtx_info_) -> bool {
				 bool comp = 
				   snemo::geometry::vertex_info::equal(vtx_info_, 
								       srcVertexes[edgeCarrierIdx],
								       duplicateTolerance);
				 DT_LOG_DEBUG(logPrio, "Compare stored " << vtx_info_.face_intercept.get_impact()
					      << " vs " << srcVertexes[edgeCarrierIdx].face_intercept.get_impact()
					      << " -> " << std::boolalpha << comp);			     
				 return comp;
			       }) == vertexes_.end()) {
		storeIt = true;
	      }
	    } else {
	      storeIt = true;
	    }
	    if (storeIt) {
	      DT_LOG_DEBUG(logPrio, "Store the vertex extrapolated on volume " << srcVertexes[edgeCarrierIdx].gid);
	      vertexes_.push_back(srcVertexes[edgeCarrierIdx]);
	    }
	  }
	}
      }
     
      DT_LOG_DEBUG(logPrio, "Number of new selected line intercepts : " << vertexes_.size() - previousNbVtx);
      if (datatools::logger::is_debug(logPrio)) {
	for (auto vtxId = previousNbVtx; vtxId < vertexes_.size(); vtxId++) {
	  const auto & srcVtxInfo = vertexes_[vtxId];
	  srcVtxInfo.print(std::cerr, "[debug] New vertex #" + std::to_string(vtxId) + " " );
	}
      }

      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo
