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
      datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
  geoManager_ = gm;
  auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
  geoLocator_ = snemo::geometry::getSNemoLocator(geoManager(), locator_plugin_name);
  const geomtools::mapping & geoMapping = geoManager().get_mapping();
  const geomtools::id_mgr & geoIdMgr = geoManager().get_id_mgr();
  // const geomtools::geom_info_dict_type & geoInfos = geoMapping.get_geom_infos();

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
    // for (const geomtools::geom_id & padBulkGid : _sourcePadBulkGids_) {
    // }
  }
  
  return;
}

void vertex_extrapolation_driver::process(const snemo::datamodel::tracker_trajectory &trajectory_,
                                          snemo::datamodel::particle_track &particle_) {
  this->_measure_vertices_(trajectory_, particle_.get_vertices());
}

void vertex_extrapolation_driver::line_trajectory_source_intercept(const snemo::datamodel::line_trajectory_pattern & line_traj_,
                                                                   std::vector<vertex_info> & vertexes_) const
{
  DT_LOG_DEBUG(logPriority_, "\nSearch line intercepts: ");
  vertexes_.clear();
  std::vector<vertex_info> srcStripVertexes;
  std::vector<vertex_info> srcPadVertexes;
  std::vector<vertex_info> srcPadBulkVertexes;
  std::vector<vertex_info> srcVertexes;

  const geomtools::line_3d & line    = line_traj_.get_segment();
  geomtools::vector_3d first = line.get_first();
  geomtools::vector_3d last  = line.get_last();
  if (std::abs(first.x()) > std::abs(last.x())) {
    std::swap(first, last);
  }
  const geomtools::vector_3d direction = (first - last).unit();
  
  for (uint32_t i = 0; i < _sourceStripGids_.size(); i++) {
    const geomtools::geom_id & sourceStripGid = _sourceStripGids_[i];
    DT_LOG_DEBUG(logPriority_, "  Source strip GID : " << sourceStripGid);
    uint32_t sourceStripId = sourceStripGid.get(1);
    DT_LOG_DEBUG(logPriority_, "  \nSearching intercept on strip #" << sourceStripId);
    const geomtools::geom_info  & sourceStripGinfo     = geoManager().get_mapping().get_geom_info(sourceStripGid);
    const geomtools::logical_volume & sourceStripLog   = sourceStripGinfo.get_logical();
    const geomtools::i_shape_3d & sourceStripShape     = sourceStripLog.get_shape();
    const geomtools::placement  & sourceStripPlacement = sourceStripGinfo.get_world_placement();
    geomtools::vector_3d srcStripFirst;
    sourceStripPlacement.mother_to_child(line.get_first(), srcStripFirst);
    geomtools::vector_3d srcStripDirection; //= (srcFirst - srcLast).unit();
    sourceStripPlacement.mother_to_child_direction(direction, srcStripDirection);
    DT_LOG_DEBUG(logPriority_, "  First point : " << geomtools::to_xyz(srcStripFirst) );
    DT_LOG_DEBUG(logPriority_, "  Direction   : " << geomtools::to_xyz(srcStripDirection) );
    DT_LOG_DEBUG(logPriority_, "  Source strip shape  : '" << sourceStripShape.get_shape_name() << "'");
    DT_LOG_DEBUG(logPriority_, "  Intercept tolerance : " << _intercept_tolerance_ / CLHEP::mm << " mm");
    geomtools::face_intercept_info srcStripFii;
    bool success = sourceStripShape.find_intercept(srcStripFirst, srcStripDirection, srcStripFii, _intercept_tolerance_);
    if (! success) {
      continue;
    }
    DT_LOG_DEBUG(logPriority_, "  \nFound intercept on strip #" << sourceStripId);
    geomtools::vector_3d srcStripImpact = srcStripFii.get_impact();
    geomtools::vector_3d srcStripWorldimpact;
    sourceStripPlacement.child_to_mother(srcStripImpact, srcStripWorldimpact);
    srcStripFii.set_impact(srcStripWorldimpact);
    vertex_info vtxInfo;
    vtxInfo.gid = sourceStripGid;
    vtxInfo.face_intercept = srcStripFii;
 
    // Search intercept on daughter pad volumes:
    // bool has_pads = false;
    // bool has_pad_intercept = false;
    for (uint32_t j = 0; j < _sourcePadGids_.size(); j++) {
      const geomtools::geom_id & sourcePadGid = _sourcePadGids_[j];
      // DT_LOG_DEBUG(logPriority_, "  Source pad GID : " << sourcePadGid);
      if (sourcePadGid.get(1) != sourceStripId) {
        /// Reject pads in other strips:
        continue;
      }
      uint32_t sourcePadId = sourcePadGid.get(2);
      DT_LOG_DEBUG(logPriority_, "  Searching intercept on pad #" << sourcePadId << " in strip #" << sourceStripId);
      const geomtools::geom_info  & sourcePadGinfo     = geoManager().get_mapping().get_geom_info(sourcePadGid);
      const geomtools::logical_volume & sourcePadLog   = sourcePadGinfo.get_logical();
      const geomtools::i_shape_3d & sourcePadShape     = sourcePadLog.get_shape();
      const geomtools::placement  & sourcePadPlacement = sourcePadGinfo.get_world_placement();
      geomtools::vector_3d srcPadFirst;
      sourcePadPlacement.mother_to_child(line.get_first(), srcPadFirst);
      geomtools::vector_3d srcPadDirection;
      sourcePadPlacement.mother_to_child_direction(direction, srcPadDirection);
      DT_LOG_DEBUG(logPriority_, "  First point : " << geomtools::to_xyz(srcPadFirst) );
      DT_LOG_DEBUG(logPriority_, "  Direction   : " << geomtools::to_xyz(srcPadDirection) );
      DT_LOG_DEBUG(logPriority_, "  Source pad shape : '" << sourcePadShape.get_shape_name() << "'");
      geomtools::face_intercept_info srcPadFii;
      success = sourcePadShape.find_intercept(srcPadFirst, srcPadDirection, srcPadFii, _intercept_tolerance_);
      if (! success) {
        continue;
      }
      DT_LOG_DEBUG(logPriority_, "  Found intercept on pad #" << sourcePadId);
      geomtools::vector_3d srcPadImpact = srcPadFii.get_impact();
      geomtools::vector_3d srcPadWorldimpact;
      sourcePadPlacement.child_to_mother(srcPadImpact, srcPadWorldimpact);
      srcPadFii.set_impact(srcPadWorldimpact);
      vertex_info padVtxInfo;
      padVtxInfo.gid = sourcePadGid;
      padVtxInfo.face_intercept = srcPadFii;

      // Search intercept on daughter pad bulk volumes:
      // bool bulk_intercept = false;
      bool candidate_bulk = false;
      for (uint32_t k = 0; k < _sourcePadBulkGids_.size(); k++) {
        const geomtools::geom_id & sourcePadBulkGid = _sourcePadBulkGids_[k];
        // DT_LOG_DEBUG(logPriority_, "  Source pad bulk GID : " << sourcePadBulkGid);
        if (sourcePadBulkGid.get(1) != sourceStripId or sourcePadBulkGid.get(2) != sourcePadId ) {
          /// Reject pad bulks in other strips and pads:
          continue;
        }
        candidate_bulk = true;
        DT_LOG_DEBUG(logPriority_, "    Searching intercept on pad bulk #" << sourcePadId << " in strip #" << sourceStripId);
        const geomtools::geom_info  & sourcePadBulkGinfo  = geoManager().get_mapping().get_geom_info(sourcePadBulkGid);
        const geomtools::logical_volume & sourcePadBulkLog   = sourcePadBulkGinfo.get_logical();
        const geomtools::i_shape_3d & sourcePadBulkShape     = sourcePadBulkLog.get_shape();
        const geomtools::placement  & sourcePadBulkPlacement = sourcePadBulkGinfo.get_world_placement();
        geomtools::vector_3d srcPadBulkFirst;
        sourcePadBulkPlacement.mother_to_child(line.get_first(), srcPadBulkFirst);
        geomtools::vector_3d srcPadBulkDirection;
        sourcePadBulkPlacement.mother_to_child_direction(direction, srcPadBulkDirection);
        DT_LOG_DEBUG(logPriority_, "    First point : " << geomtools::to_xyz(srcPadBulkFirst) );
        DT_LOG_DEBUG(logPriority_, "    Direction   : " << geomtools::to_xyz(srcPadBulkDirection) );
        DT_LOG_DEBUG(logPriority_, "    Source pad bulk shape : '" << sourcePadBulkShape.get_shape_name() << "'");
        geomtools::face_intercept_info srcPadBulkFii;
        success = sourcePadBulkShape.find_intercept(srcPadBulkFirst, srcPadBulkDirection, srcPadBulkFii, _intercept_tolerance_);
        if (! success) {
          continue;
        }
        DT_LOG_DEBUG(logPriority_, "    Found intercept on pad bulk #" << sourcePadId);
        geomtools::vector_3d srcPadBulkImpact = srcPadBulkFii.get_impact();
        geomtools::vector_3d srcPadBulkWorldimpact;
        sourcePadPlacement.child_to_mother(srcPadBulkImpact, srcPadBulkWorldimpact);
        srcPadBulkFii.set_impact(srcPadBulkWorldimpact);
        vertex_info padBulkVtxInfo;
        padBulkVtxInfo.gid = sourcePadBulkGid;
        padBulkVtxInfo.face_intercept = srcPadBulkFii;
        srcVertexes.push_back(padBulkVtxInfo);
        // bulk_intercept = true;
      }
      if (! candidate_bulk) { // and ! bulk_intercept) {
        // Validate pad intercept if no candidate bulk
        srcVertexes.push_back(padVtxInfo);
      }
    }   
  }

  int closestIndex = -1;
  double closestDist = datatools::invalid_real();
  for (unsigned int i = 0; i < srcVertexes.size(); i++) {
    const vertex_info & vtxInfo = srcVertexes[i];
    double dist = (vtxInfo.face_intercept.get_impact() - first).mag();
    if (closestIndex == -1 or dist < closestDist) {
      closestIndex = i;
      closestDist = dist;
    }
  }
  if (closestIndex >= 0) {
    vertexes_.push_back(srcVertexes[closestIndex]);
  }
  
  return;
}

void vertex_extrapolation_driver::vertex_info::print(std::ostream & out_, const std::string & indent_) const
{
  out_ << indent_ << "|-- GID = " << gid << '\n';
  out_ << indent_ << "`-- Face intercept : " << '\n';
  face_intercept.print(out_, indent_ + "    ");
  return;
}
 
void vertex_extrapolation_driver::helix_trajectory_source_intercept(const snemo::datamodel::helix_trajectory_pattern & /* helix_traj_ */,
                                                                    std::vector<vertex_info> & vertexes_) const
{
  vertexes_.clear();
  return;
}
  
void vertex_extrapolation_driver::_measure_vertices_(
    const snemo::datamodel::tracker_trajectory &trajectory_,
    snemo::datamodel::particle_track::vertex_collection_type &vertices_) {
  namespace snedm = snemo::datamodel;
  // Extract the side from the geom_id of the tracker_trajectory object:
  if (!trajectory_.has_geom_id()) {
    DT_LOG_ERROR(logPriority_, "Tracker trajectory has no geom_id, cannot extrapolate vertices");
    return;
  }
  const geomtools::geom_id &gid = trajectory_.get_geom_id();
  const geomtools::id_mgr &id_mgr = geoManager().get_id_mgr();
  if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side")) {
    DT_LOG_ERROR(logPriority_,
                 "Trajectory geom_id " << gid << " has no 'module' or 'side' address");
    return;
  }
  const int side = id_mgr.get(gid, "side");

  // Set the calorimeter locators :
  const snemo::geometry::calo_locator &calo_locator = geoLocator_->caloLocator();
  const snemo::geometry::xcalo_locator &xcalo_locator = geoLocator_->xcaloLocator();
  const snemo::geometry::gveto_locator &gveto_locator = geoLocator_->gvetoLocator();
  // TODO: Add source strip locator...

  const double xcalo_bd[2] = {calo_locator.getXCoordOfWallWindow(snemo::geometry::side_t::BACK),
                              calo_locator.getXCoordOfWallWindow(snemo::geometry::side_t::FRONT)};
  const double ycalo_bd[2] = {
      xcalo_locator.getYCoordOfWallWindow(side, snemo::geometry::xcalo_wall_t::LEFT),
      xcalo_locator.getYCoordOfWallWindow(side, snemo::geometry::xcalo_wall_t::RIGHT)};
  const double zcalo_bd[2] = {
      gveto_locator.getZCoordOfWallWindow(side, snemo::geometry::gveto_wall_t::BOTTOM),
      gveto_locator.getZCoordOfWallWindow(side, snemo::geometry::gveto_wall_t::TOP)};

  // Check Geiger cell location wrt to vertex extrapolation
  this->_check_vertices_(trajectory_);

  // Look first if trajectory pattern is an helix or not:
  const snedm::base_trajectory_pattern &a_track_pattern = trajectory_.get_pattern();
  const std::string &a_pattern_id = a_track_pattern.get_pattern_id();

  // Extrapolated vertices:
  using vertex_dict_type = std::vector<std::pair<std::string, geomtools::vector_3d> >;
  vertex_dict_type vertices;

  // Add a property into 'blur_spot' auxiliaries to refer to the hit calo:
  std::string calo_category_flag;

  DT_LOG_DEBUG(logPriority_, "Pattern ID : '" << a_pattern_id << "'");

  // ----- Start of line pattern handling
  if (a_pattern_id == snedm::line_trajectory_pattern::pattern_id()) {
    const auto &ltp = dynamic_cast<const snedm::line_trajectory_pattern &>(a_track_pattern);
    const geomtools::line_3d   &a_line = ltp.get_segment();
    const geomtools::vector_3d &first = a_line.get_first();
    const geomtools::vector_3d &last = a_line.get_last();
    const geomtools::vector_3d direction = first - last;

    // Calculate intersection on each geometric object
    using vertex_list_type = std::map<geomtools::vector_3d, std::string>;
    vertex_list_type vtxlist;

    bool find_on_source = true;
    bool find_on_calo   = true;
    bool find_on_xwalls = true;
    bool find_on_gveto  = true;

    // Vertex on source strips:
    if (find_on_source) {
      
      std::vector<vertex_info> vertexes;
      line_trajectory_source_intercept(ltp, vertexes);
      DT_LOG_DEBUG(logPriority_, "vertexes = " << vertexes.size());
      for (const auto & v : vertexes) {
        DT_LOG_DEBUG(logPriority_, "Source strip vertexes:");
        if (datatools::logger::is_debug(logPriority_)) {
          v.print(std::cerr);
        }
      }
      if (vertexes.size()) {
        // Extrapolated vertex:
        const geomtools::vector_3d & a_vertex = vertexes[0].face_intercept.get_impact();
        vtxlist.insert(std::make_pair(a_vertex, snedm::particle_track::vertex_on_source_foil_label())); 
      }
      
      // // Extrapolation on flat source foils:
      // {
      //   const double x = 0.0 * CLHEP::mm;
      //   const double y = direction.y() / direction.x() * (x - first.x()) + first.y();
      //   const double z = direction.z() / direction.y() * (y - first.y()) + first.z();

      //   // Extrapolated vertex:
      //   const geomtools::vector_3d a_vertex(x, y, z);
      //   vtxlist.insert(std::make_pair(a_vertex, snedm::particle_track::vertex_on_source_foil_label()));
      // }
    }

    // Calorimeter walls:
    if (find_on_calo) {
      for (const double x : xcalo_bd) {
        const double y = direction.y() / direction.x() * (x - first.x()) + first.y();
        const double z = direction.z() / direction.y() * (y - first.y()) + first.z();

        // Extrapolated vertex
        const geomtools::vector_3d a_vertex(x, y, z);
        vtxlist.insert(
            std::make_pair(a_vertex, snedm::particle_track::vertex_on_main_calorimeter_label()));
      }
    }

    // Calorimeter on xwalls
    if (find_on_xwalls) {
      for (const double y : ycalo_bd) {
        const double z = direction.z() / direction.y() * (y - first.y()) + first.z();
        const double x = direction.x() / direction.y() * (y - first.y()) + first.x();

        // Extrapolate vertex
        const geomtools::vector_3d a_vertex(x, y, z);
        vtxlist.insert(
            std::make_pair(a_vertex, snedm::particle_track::vertex_on_x_calorimeter_label()));
      }
    }

    // Calorimeter on gveto
    if (find_on_gveto) {
      for (const double z : zcalo_bd) {
        const double y = direction.y() / direction.z() * (z - first.z()) + first.y();
        const double x = direction.x() / direction.y() * (y - first.y()) + first.x();

        // Extrapolate vertex
        const geomtools::vector_3d a_vertex(x, y, z);
        vtxlist.insert(
            std::make_pair(a_vertex, snedm::particle_track::vertex_on_gamma_veto_label()));
      }
    }

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

    // Create a mutable line object to set the new position
    auto a_mutable_line = const_cast<geomtools::line_3d *>(&a_line);
    if (_use_vertices_[jt1->second]) {
      a_mutable_line->set_first(jt1->first);
      vertices.push_back(std::make_pair(jt1->second, jt1->first));
    } else {
      vertices.push_back(
          std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_line.get_first()));
    }
    if (_use_vertices_[jt2->second]) {
      a_mutable_line->set_last(jt2->first);
      vertices.push_back(std::make_pair(jt2->second, jt2->first));
    } else {
      vertices.push_back(
          std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_line.get_last()));
    }
  }  // ----- end of line pattern handling

  // ---- start of helix pattern handling
  if (a_pattern_id == snedm::helix_trajectory_pattern::pattern_id()) {
    const auto &htp = dynamic_cast<const snedm::helix_trajectory_pattern &>(a_track_pattern);
    const geomtools::helix_3d &a_helix = htp.get_helix();
    const geomtools::vector_3d &hcenter = a_helix.get_center();
    const double hradius = a_helix.get_radius();

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

    // Gvetos
    {
      for (const double gwall : zcalo_bd) {
        const double t = a_helix.get_t_from_z(gwall);
        tparams.insert(std::make_pair(t, snedm::particle_track::vertex_on_gamma_veto_label()));
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
      if (_use_vertices_[category] && new_length < length) {
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
      if (_use_vertices_[category] && new_length < length) {
        a_mutable_helix->set_t2(new_ts.second);
        vertices.push_back(std::make_pair(category, a_mutable_helix->get_last()));
      } else {
        vertices.push_back(
            std::make_pair(snedm::particle_track::vertex_on_wire_label(), a_helix.get_last()));
      }
    }
  }
  // ----- end of helix pattern

  // Save new vertices
  for (const auto &vertice : vertices) {
    const std::string &flag = vertice.first;
    const geomtools::vector_3d &pos = vertice.second;
    // Check vertex side is on the same side as the trajectory
    if ((side == snemo::geometry::side_t::BACK && pos.x() > 0.0) ||
        (side == snemo::geometry::side_t::FRONT && pos.x() < 0.0)) {
      // Is this an error or warning?
      DT_LOG_WARNING(logPriority_, "Closest vertex is on the opposite side!");
    }
    auto spot = datatools::make_handle<geomtools::blur_spot>();
    spot->set_hit_id(vertices_.size());
    spot->grab_auxiliaries().update(snedm::particle_track::vertex_type_key(), flag);
    // Future: determine the GID of the scintillator block or source strip
    // associated to the impact vertex:
    //
    //   int module = 0;
    //   int side = 0;
    //   int strip = -1;
    //   spot.grab_geom_id().set_type(???);
    //   spot.grab_geom_id().set_addresses(module, side/strip?, wall?, column?, row?...);
    // or:
    //   spot.set_geom_id(matchind_gid?);
    //

    // For now it is dimension 3 with no errors nor rotation defined:
    spot->set_blur_dimension(geomtools::blur_spot::dimension_three);
    spot->set_position(pos);

    vertices_.push_back(spot);

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
}

void vertex_extrapolation_driver::_check_vertices_(
    const snemo::datamodel::tracker_trajectory &trajectory_) {
  if (!trajectory_.has_cluster()) {
    return;
  }
  // Reset values of booleans
  namespace snedm = snemo::datamodel;
  _use_vertices_[snedm::particle_track::vertex_on_source_foil_label()] = false;
  _use_vertices_[snedm::particle_track::vertex_on_main_calorimeter_label()] = false;
  _use_vertices_[snedm::particle_track::vertex_on_x_calorimeter_label()] = false;
  _use_vertices_[snedm::particle_track::vertex_on_gamma_veto_label()] = false;

  const snedm::tracker_cluster &a_cluster = trajectory_.get_cluster();
  const snedm::TrackerHitHdlCollection &the_hits = a_cluster.hits();
  for (const datatools::handle<snedm::calibrated_tracker_hit>& a_hit : the_hits) {
    const geomtools::geom_id &a_gid = a_hit->get_geom_id();

    // Extract layer
    const snemo::geometry::gg_locator &gg_locator = geoLocator_->geigerLocator();
    const uint32_t layer = gg_locator.getLayerAddress(a_gid);
    if (layer < 1) {
      // Extrapolate vertex to the foil if the first GG layers are fired
      _use_vertices_[snedm::particle_track::vertex_on_source_foil_label()] = true;
    }

    const uint32_t side = gg_locator.getSideAddress(a_gid);
    if (layer >= gg_locator.numberOfLayers(side) - 1) {
      _use_vertices_[snedm::particle_track::vertex_on_main_calorimeter_label()] = true;
    }

    const uint32_t row = gg_locator.getRowAddress(a_gid);
    if (row <= 1 || row >= gg_locator.numberOfRows(side) - 1) {
      _use_vertices_[snedm::particle_track::vertex_on_x_calorimeter_label()] = true;
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
