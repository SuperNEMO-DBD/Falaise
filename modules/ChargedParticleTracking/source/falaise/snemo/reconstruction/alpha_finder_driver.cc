// \file falaise/snemo/reconstruction/alpha_finder_driver.cc

// Ourselves:
#include <falaise/snemo/reconstruction/alpha_finder_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

namespace reconstruction {

const std::string &alpha_finder_driver::short_alpha_key() {
  static const std::string s("short_alpha");
  return s;
}

const std::string &alpha_finder_driver::get_id() {
  static const std::string s("AFD");
  return s;
}

void alpha_finder_driver::set_initialized(const bool initialized_) {
  _initialized_ = initialized_;
  return;
}

bool alpha_finder_driver::is_initialized() const { return _initialized_; }

void alpha_finder_driver::set_logging_priority(const datatools::logger::priority priority_) {
  _logging_priority_ = priority_;
  return;
}

datatools::logger::priority alpha_finder_driver::get_logging_priority() const {
  return _logging_priority_;
}

bool alpha_finder_driver::has_geometry_manager() const { return _geometry_manager_ != 0; }

void alpha_finder_driver::set_geometry_manager(const geomtools::manager &gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");
  _geometry_manager_ = &gmgr_;
  return;
}

const geomtools::manager &alpha_finder_driver::get_geometry_manager() const {
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
  return *_geometry_manager_;
}

/// Constructor
alpha_finder_driver::alpha_finder_driver() {
  _set_defaults();
  return;
}

/// Destructor
alpha_finder_driver::~alpha_finder_driver() {
  if (is_initialized()) {
    reset();
  }
  return;
}

/// Initialize the driver through configuration properties
void alpha_finder_driver::initialize(const datatools::properties &setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");

  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "Missing geometry manager !");
  DT_THROW_IF(!get_geometry_manager().is_initialized(), std::logic_error,
              "Geometry manager is not initialized !");

  // Logging priority
  datatools::logger::priority lp = datatools::logger::extract_logging_configuration(setup_);
  DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
              "Invalid logging priority level for geometry manager !");
  set_logging_priority(lp);

  // Get geometry locator plugin
  const geomtools::manager &geo_mgr = get_geometry_manager();
  std::string locator_plugin_name;
  if (setup_.has_key("locator_plugin_name")) {
    locator_plugin_name = setup_.fetch_string("locator_plugin_name");
  } else {
    // If no locator plugin name is set, then search for the first one
    const geomtools::manager::plugins_dict_type &plugins = geo_mgr.get_plugins();
    for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
         ip != plugins.end(); ip++) {
      const std::string &plugin_name = ip->first;
      if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
        locator_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  DT_THROW_IF(!geo_mgr.has_plugin(locator_plugin_name) ||
                  !geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name),
              std::logic_error, "Found no locator plugin named '" << locator_plugin_name << "'");
  _locator_plugin_ = &geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);

  // Minimal anode time to consider Geiger hit as delayed
  if (setup_.has_key("minimal_delayed_time")) {
    _minimal_delayed_time_ = setup_.fetch_real("minimal_delayed_time");
    if (!setup_.has_explicit_unit("minimal_delayed_time")) {
      _minimal_delayed_time_ *= CLHEP::microsecond;
    }
  }

  // Minimal distance in XY coordinate between Geiger hits
  if (setup_.has_key("minimal_cluster_xy_search_distance")) {
    _minimal_cluster_xy_search_distance_ = setup_.fetch_real("minimal_cluster_xy_search_distance");
    if (!setup_.has_explicit_unit("minimal_cluster_xy_search_distance")) {
      _minimal_cluster_xy_search_distance_ *= CLHEP::cm;
    }
  }

  // Minimal distance in Z coordinate between Geiger hits
  if (setup_.has_key("minimal_cluster_z_search_distance")) {
    _minimal_cluster_z_search_distance_ = setup_.fetch_real("minimal_cluster_z_search_distance");
    if (!setup_.has_explicit_unit("minimal_cluster_z_search_distance")) {
      _minimal_cluster_z_search_distance_ *= CLHEP::cm;
    }
  }

  // Minimal distance between vertex of the prompt track and delayed GG hit
  if (setup_.has_key("minimal_vertex_distance")) {
    _minimal_vertex_distance_ = setup_.fetch_real("minimal_vertex_distance");
    if (!setup_.has_explicit_unit("minimal_vertex_distance")) {
      _minimal_vertex_distance_ *= CLHEP::cm;
    }
  }

  set_initialized(true);
  return;
}

/// Reset the driver
void alpha_finder_driver::reset() {
  _set_defaults();
  return;
}

void alpha_finder_driver::_set_defaults() {
  _initialized_ = false;
  _logging_priority_ = datatools::logger::PRIO_WARNING;

  _minimal_delayed_time_ = 15 * CLHEP::microsecond;
  _minimal_cluster_xy_search_distance_ = 21 * CLHEP::cm;
  _minimal_cluster_z_search_distance_ = 30 * CLHEP::cm;
  _minimal_vertex_distance_ = 30 * CLHEP::cm;
  return;
}

void alpha_finder_driver::process(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  DT_THROW_IF(!is_initialized(), std::logic_error, "Driver is not initialized !");

  this->_find_delayed_unfitted_cluster_(tracker_trajectory_data_, particle_track_data_);
  this->_find_delayed_unclustered_hit_(tracker_trajectory_data_, particle_track_data_);

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

void alpha_finder_driver::_find_delayed_unfitted_cluster_(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  // Check if the solution exist
  if (!tracker_trajectory_data_.has_solutions()) {
    DT_LOG_DEBUG(get_logging_priority(), "No solutions have been found !");
    return;
  }

  // The default solution is chosen
  const snemo::datamodel::tracker_trajectory_solution &a_solution =
      tracker_trajectory_data_.get_default_solution();
  // Get unfitted cluster first
  if (!a_solution.has_unfitted_clusters()) {
    DT_LOG_DEBUG(get_logging_priority(), "No unfitted clusters has been found !");
    return;
  }
  const snemo::datamodel::tracker_trajectory_solution::cluster_col_type &the_unfitted_clusters =
      a_solution.get_unfitted_clusters();
  // Loop on all the unfitted cluster
  for (snemo::datamodel::tracker_trajectory_solution::cluster_col_type::const_iterator iclus =
           the_unfitted_clusters.begin();
       iclus != the_unfitted_clusters.end(); ++iclus) {
    const snemo::datamodel::tracker_cluster &a_delayed_cluster = iclus->get();

    // The unfitted cluster has to be delayed
    if (!a_delayed_cluster.is_delayed()) {
      DT_LOG_DEBUG(get_logging_priority(), "The unfitted cluster is not delayed !");
      return;
    }
    const snemo::datamodel::calibrated_tracker_hit::collection_type &delayed_gg_hits =
        a_delayed_cluster.get_hits();

    geomtools::vector_3d associated_vertex;
    geomtools::invalidate(associated_vertex);
    this->_find_short_track_(delayed_gg_hits, a_solution, particle_track_data_);

    // Add tracker cluster handle to trajectory
    if (particle_track_data_.has_particles()) {
      snemo::datamodel::particle_track_data::particle_collection_type &particles =
          particle_track_data_.grab_particles();
      snemo::datamodel::particle_track &a_particle = particles.back().grab();
      const datatools::properties &aux = a_particle.get_auxiliaries();
      if (aux.has_flag(alpha_finder_driver::short_alpha_key()) && a_particle.has_trajectory()) {
        snemo::datamodel::tracker_trajectory &a_trajectory = a_particle.grab_trajectory();
        a_trajectory.set_cluster_handle(*iclus);
      }
    }
  }  // end of delayed cluster

  DT_LOG_TRACE(get_logging_priority(), "Exiting...");
  return;
}

void alpha_finder_driver::_find_delayed_unclustered_hit_(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  // Check if the solution exist
  if (!tracker_trajectory_data_.has_solutions()) {
    DT_LOG_DEBUG(get_logging_priority(), "No solutions have been found !");
    return;
  }
  // The default solution is chosen
  const snemo::datamodel::tracker_trajectory_solution &a_solution =
      tracker_trajectory_data_.get_default_solution();
  // Get associated clustering solution (if any)
  if (!a_solution.has_clustering_solution()) {
    DT_LOG_DEBUG(get_logging_priority(), "No clustering solution has been found !");
    return;
  }
  const snemo::datamodel::tracker_clustering_solution &a_clustering_solution =
      a_solution.get_clustering_solution();
  if (!a_clustering_solution.has_unclustered_hits()) {
    DT_LOG_DEBUG(get_logging_priority(), "No unclustered hits !");
    return;
  }
  const snemo::datamodel::calibrated_tracker_hit::collection_type &unclustered_gg_hits =
      a_clustering_solution.get_unclustered_hits();

  this->_find_short_track_(unclustered_gg_hits, a_solution, particle_track_data_, false);

  DT_LOG_TRACE(get_logging_priority(), "Exiting...");
  return;
}

void alpha_finder_driver::_find_short_track_(
    const snemo::datamodel::calibrated_tracker_hit::collection_type &hits_,
    const snemo::datamodel::tracker_trajectory_solution &solution_,
    snemo::datamodel::particle_track_data &particle_track_data_, const bool hits_from_cluster) {
  geomtools::vector_3d associated_vertex;
  geomtools::invalidate(associated_vertex);
  bool has_associated_alpha = false;
  // Loop on all the geiger hits of the unfitted cluster
  for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator ihit =
           hits_.begin();
       ihit != hits_.end(); ++ihit) {
    const snemo::datamodel::calibrated_tracker_hit &a_delayed_gg_hit = ihit->get();

    // set a default distance for closest vertex to check against
    double closest_vertex_distance = _minimal_vertex_distance_;

    if (!a_delayed_gg_hit.is_delayed() ||
        a_delayed_gg_hit.get_delayed_time() < _minimal_delayed_time_) {
      DT_LOG_TRACE(get_logging_priority(),
                   "Geiger is prompt or delayed time is too low ("
                       << a_delayed_gg_hit.get_delayed_time() / CLHEP::microsecond << "<"
                       << _minimal_delayed_time_ / CLHEP::microsecond << " us)");
      continue;
    }

    if (!hits_from_cluster) {
      // Reset values
      geomtools::invalidate(associated_vertex);
      has_associated_alpha = false;
    }

    // Get prompt trajectories
    if (!solution_.has_trajectories()) {
      DT_LOG_DEBUG(get_logging_priority(), "No prompt trajectories have been found !");
      return;
    }
    const snemo::datamodel::tracker_trajectory_solution::trajectory_col_type &the_trajectories =
        solution_.get_trajectories();
    // Loop on all the trajectories
    for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::const_iterator itraj =
             the_trajectories.begin();
         itraj != the_trajectories.end(); ++itraj) {
      const snemo::datamodel::tracker_trajectory &a_trajectory = itraj->get();
      // Look into properties to find the default trajectory. Here,
      // default means the one with the best chi2. This flag is set by the
      // 'fitting' module.
      if (!a_trajectory.get_auxiliaries().has_flag("default")) continue;

      if (!a_trajectory.has_cluster()) {
        DT_LOG_DEBUG(get_logging_priority(), "No associated clusters have been found !");
        continue;
      }
      const snemo::datamodel::tracker_cluster &a_prompt_cluster = a_trajectory.get_cluster();
      if (a_prompt_cluster.is_delayed()) {
        DT_LOG_DEBUG(get_logging_priority(), "The cluster is not prompt !");
        continue;
      }
      const snemo::datamodel::calibrated_tracker_hit::collection_type &prompt_gg_hits =
          a_prompt_cluster.get_hits();
      for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator ipromptgg =
               prompt_gg_hits.begin();
           ipromptgg != prompt_gg_hits.end(); ++ipromptgg) {
        const snemo::datamodel::calibrated_tracker_hit &a_prompt_gg_hit = ipromptgg->get();
        const geomtools::vector_2d a_prompt_position(a_prompt_gg_hit.get_x(),
                                                     a_prompt_gg_hit.get_y());
        const geomtools::vector_2d a_delayed_position(a_delayed_gg_hit.get_x(),
                                                      a_delayed_gg_hit.get_y());
        const double distance_xy = (a_delayed_position - a_prompt_position).mag();
        DT_LOG_DEBUG(get_logging_priority(), "Distance between delayed hit "
                                                 << a_delayed_gg_hit.get_geom_id()
                                                 << " and prompt hit "
                                                 << a_prompt_gg_hit.get_geom_id() << " = "
                                                 << distance_xy / CLHEP::cm << " cm");
        bool has_minimal_xy_search = false;
        if (distance_xy < _minimal_cluster_xy_search_distance_) {
          has_minimal_xy_search = true;
        }

        const double z_delayed = a_delayed_gg_hit.get_z();
        const double sigma_z_delayed = a_delayed_gg_hit.get_sigma_z();
        const double z_prompt = a_prompt_gg_hit.get_z();
        const double sigma_z_prompt = a_prompt_gg_hit.get_sigma_z();
        const double distance_z = std::abs(z_delayed - z_prompt);
        const double sigma = sigma_z_delayed + sigma_z_prompt;
        DT_LOG_DEBUG(get_logging_priority(),
                     "Distance between delayed hit "
                         << a_delayed_gg_hit.get_geom_id() << " and prompt hit "
                         << a_prompt_gg_hit.get_geom_id() << " z = " << distance_z / CLHEP::cm
                         << " cm - sigma = " << sigma / CLHEP::cm << " cm");
        bool has_minimal_z_search = false;
        if ((distance_z - _minimal_cluster_z_search_distance_) < sigma) {
          has_minimal_z_search = true;
        }

        if (has_minimal_xy_search && has_minimal_z_search) {
          DT_LOG_DEBUG(get_logging_priority(), "Find a GG candidate close enough to delayed hit");
          has_associated_alpha = true;
          break;
        }
      }  // end of prompt gg hits

      // Look for trajectories extremities
      if (has_associated_alpha) {
        DT_LOG_DEBUG(get_logging_priority(), "Look for trajectory extremities");
        geomtools::vector_3d first;
        geomtools::vector_3d last;
        geomtools::invalidate(first);
        geomtools::invalidate(last);
        const snemo::datamodel::base_trajectory_pattern &a_pattern = a_trajectory.get_pattern();
        const std::string &a_pattern_id = a_pattern.get_pattern_id();
        if (a_pattern_id == snemo::datamodel::line_trajectory_pattern::pattern_id()) {
          const snemo::datamodel::line_trajectory_pattern &ltp =
              dynamic_cast<const snemo::datamodel::line_trajectory_pattern &>(a_pattern);
          first = ltp.get_segment().get_first();
          last = ltp.get_segment().get_last();
        } else if (a_pattern_id == snemo::datamodel::helix_trajectory_pattern::pattern_id()) {
          const snemo::datamodel::helix_trajectory_pattern &htp =
              dynamic_cast<const snemo::datamodel::helix_trajectory_pattern &>(a_pattern);
          first = htp.get_helix().get_first();
          last = htp.get_helix().get_last();
        }
        const geomtools::vector_3d a_delayed_position(
            a_delayed_gg_hit.get_x(), a_delayed_gg_hit.get_y(), a_delayed_gg_hit.get_z());
        if (geomtools::is_valid(first)) {
          const double distance = (first - a_delayed_position).mag();
          if (distance < _minimal_vertex_distance_ && distance < closest_vertex_distance) {
            // set a new value for the closest vertex
            closest_vertex_distance = distance;
            associated_vertex = first;
          }
        }
        if (geomtools::is_valid(last)) {
          const double distance = (last - a_delayed_position).mag();
          // previous check was against the minimal vertex distance, but
          // we want to check against the already asigned distance to see
          // if this vertex is closer
          if (distance < _minimal_vertex_distance_ && distance < closest_vertex_distance) {
            closest_vertex_distance = distance;
            associated_vertex = last;
          }
        }
      }
    }  // end of trajectories

    if (has_associated_alpha && geomtools::is_valid(associated_vertex)) {
      if (hits_from_cluster) {
        // Build a new particle track
        this->_build_alpha_particle_track_(hits_, associated_vertex, particle_track_data_);
        // If hits come from unfitted tracker cluster no more processing
        break;
      } else {
        // If hits come from unclustered hits then add a new particle
        // Create a new cluster with only one delayed geiger hits and associate it
        // to the particle track trajectory
        snemo::datamodel::tracker_cluster::handle_type a_cluster(
            new snemo::datamodel::tracker_cluster);
        a_cluster.grab().make_delayed();
        snemo::datamodel::calibrated_tracker_hit::collection_type &hits =
            a_cluster.grab().grab_hits();
        hits.push_back(*ihit);

        // Build a new particle track
        this->_build_alpha_particle_track_(hits, associated_vertex, particle_track_data_);

        // Add special tracker cluster handle to trajectory
        snemo::datamodel::particle_track_data::particle_collection_type &particles =
            particle_track_data_.grab_particles();
        snemo::datamodel::particle_track &a_particle = particles.back().grab();
        const datatools::properties &aux = a_particle.get_auxiliaries();
        if (aux.has_flag(alpha_finder_driver::short_alpha_key()) && a_particle.has_trajectory()) {
          snemo::datamodel::tracker_trajectory &a_trajectory = a_particle.grab_trajectory();
          a_trajectory.set_cluster_handle(a_cluster);
        }
      }
    }

  }  // end of delayed hits
  return;
}

void alpha_finder_driver::_fit_short_track_(
    const snemo::datamodel::calibrated_tracker_hit::collection_type &hits_,
    const geomtools::vector_3d &first_vertex_, geomtools::vector_3d &last_vertex_) {
  double max_distance = 0.0 * CLHEP::cm;
  // Loop on all the delayed geiger hits to compute distance between hit
  // and associated vertex
  for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator ihit =
           hits_.begin();
       ihit != hits_.end(); ++ihit) {
    const snemo::datamodel::calibrated_tracker_hit &a_hit = ihit->get();
    const geomtools::vector_3d a_hit_position(a_hit.get_x(), a_hit.get_y(), a_hit.get_z());
    const double distance = (first_vertex_ - a_hit_position).mag();
    if (distance > max_distance) {
      last_vertex_ = a_hit_position;
      max_distance = distance;
    }
  }  // end of delayed geiger hits
  return;
}

void alpha_finder_driver::_build_alpha_particle_track_(
    const snemo::datamodel::calibrated_tracker_hit::collection_type &hits_,
    const geomtools::vector_3d &first_vertex_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  // Add short alpha particle track
  snemo::datamodel::particle_track::handle_type hPT(new snemo::datamodel::particle_track);
  snemo::datamodel::particle_track &a_short_alpha = hPT.grab();
  a_short_alpha.set_track_id(particle_track_data_.get_number_of_particles());
  a_short_alpha.set_charge(snemo::datamodel::particle_track::undefined);
  a_short_alpha.grab_auxiliaries().store_flag(alpha_finder_driver::short_alpha_key());
  particle_track_data_.add_particle(hPT);

  // "Fit" short alpha trajectory
  geomtools::vector_3d last_vertex;
  geomtools::invalidate(last_vertex);
  const geomtools::vector_3d &first_vertex = first_vertex_;
  this->_fit_short_track_(hits_, first_vertex, last_vertex);
  if (!geomtools::is_valid(last_vertex)) {
    DT_LOG_DEBUG(get_logging_priority(), "Fit of short alpha track fails !");
    return;
  }

  // Create new 'tracker_trajectory' handle:
  snemo::datamodel::tracker_trajectory::handle_type htrajectory(
      new snemo::datamodel::tracker_trajectory);
  snemo::datamodel::tracker_trajectory &a_trajectory = htrajectory.grab();
  // Set trajectory geom_id using the first geiger hit of the associated
  // cluster
  get_geometry_manager().get_id_mgr().make_id("tracker_submodule", a_trajectory.grab_geom_id());
  get_geometry_manager().get_id_mgr().extract(hits_.front().get().get_geom_id(),
                                              a_trajectory.grab_geom_id());
  // Set trajectory pattern
  snemo::datamodel::line_trajectory_pattern *a_line = new snemo::datamodel::line_trajectory_pattern;
  a_trajectory.set_pattern_handle(a_line);
  a_line->grab_segment().set_first(first_vertex);
  a_line->grab_segment().set_last(last_vertex);
  a_short_alpha.set_trajectory_handle(htrajectory);

  // Set a alpha delayed time based on Geiger delayed times and store as
  // properties of line trajectory (like it is done so far in trackfit)
  double delay_time;
  datatools::plus_infinity(delay_time);
  {
    for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator ihit =
             hits_.begin();
         ihit != hits_.end(); ++ihit) {
      const snemo::datamodel::calibrated_tracker_hit &a_hit = ihit->get();
      delay_time = std::min(delay_time, a_hit.get_delayed_time());
    }
  }
  // Here we should spread the time since we do not know where exactly the
  // alpha passes through the cell.
  a_trajectory.grab_auxiliaries().store_real("t0", delay_time);

  // Determine vertices origin : previous process should have determine a
  // 'first' vertex associated to the prompt track and a 'last' vertex
  // associated to the center of a Geiger cells. The latter will be set to a
  // 'VERTEX_ON_WIRE'; the former will be set given its relative position
  // wrt to detector geometry.
  snemo::datamodel::particle_track::vertex_collection_type &vertices =
      a_short_alpha.grab_vertices();
  {
    // Vertex on wire
    snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot);
    vertices.push_back(hBS);
    geomtools::blur_spot &spot = hBS.grab();
    spot.set_hit_id(vertices.size());
    spot.grab_auxiliaries().update(snemo::datamodel::particle_track::vertex_type_key(),
                                   snemo::datamodel::particle_track::vertex_on_wire_label());
    spot.set_blur_dimension(geomtools::blur_spot::dimension_three);
    spot.set_position(last_vertex);
  }
  {
    // Check first vertex origin
    const geomtools::geom_id &gid = a_trajectory.get_geom_id();
    const geomtools::id_mgr &id_mgr = get_geometry_manager().get_id_mgr();
    if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side")) {
      DT_LOG_ERROR(get_logging_priority(),
                   "Trajectory geom_id " << gid << " has no 'module' or 'side' address!");
      return;
    }
    DT_LOG_TRACE(get_logging_priority(), "Trajectory geom_id = " << gid);
    const int side = id_mgr.get(gid, "side");
    // Set the calorimeter locators :
    const snemo::geometry::calo_locator &calo_locator = _locator_plugin_->get_calo_locator();
    const snemo::geometry::xcalo_locator &xcalo_locator = _locator_plugin_->get_xcalo_locator();
    const snemo::geometry::gveto_locator &gveto_locator = _locator_plugin_->get_gveto_locator();
    // TODO: Add source strip locator...

    const double xcalo_bd[2] = {calo_locator.get_wall_window_x(snemo::geometry::utils::SIDE_BACK),
                                calo_locator.get_wall_window_x(snemo::geometry::utils::SIDE_FRONT)};
    const double ycalo_bd[2] = {
        xcalo_locator.get_wall_window_y(side, snemo::geometry::xcalo_locator::WALL_LEFT),
        xcalo_locator.get_wall_window_y(side, snemo::geometry::xcalo_locator::WALL_RIGHT)};
    const double zcalo_bd[2] = {
        gveto_locator.get_wall_window_z(side, snemo::geometry::gveto_locator::WALL_BOTTOM),
        gveto_locator.get_wall_window_z(side, snemo::geometry::gveto_locator::WALL_TOP)};

    const double epsilon = 1e-5 * CLHEP::mm;
    std::string vertex_label;
    if (std::abs(first_vertex.x()) < epsilon) {
      DT_LOG_DEBUG(get_logging_priority(), "Alpha track has vertex on source foil");
      vertex_label = snemo::datamodel::particle_track::vertex_on_source_foil_label();
    } else if (std::abs(first_vertex.x() - xcalo_bd[0]) < epsilon ||
               std::abs(first_vertex.x() - xcalo_bd[1]) < epsilon) {
      DT_LOG_DEBUG(get_logging_priority(), "Alpha track has vertex on main wall");
      vertex_label = snemo::datamodel::particle_track::vertex_on_main_calorimeter_label();
    } else if (std::abs(first_vertex.y() - ycalo_bd[0]) < epsilon ||
               std::abs(first_vertex.y() - ycalo_bd[1]) < epsilon) {
      DT_LOG_DEBUG(get_logging_priority(), "Alpha track has vertex on X-wall");
      vertex_label = snemo::datamodel::particle_track::vertex_on_x_calorimeter_label();
    } else if (std::abs(first_vertex.z() - zcalo_bd[0]) < epsilon ||
               std::abs(first_vertex.z() - zcalo_bd[1]) < epsilon) {
      DT_LOG_DEBUG(get_logging_priority(), "Alpha track has vertex on gamma-veto");
      vertex_label = snemo::datamodel::particle_track::vertex_on_gamma_veto_label();
    } else {
      DT_LOG_DEBUG(get_logging_priority(), "Alpha track has vertex on wire");
      vertex_label = snemo::datamodel::particle_track::vertex_on_wire_label();
    }
    snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot);
    vertices.push_back(hBS);
    geomtools::blur_spot &spot = hBS.grab();
    spot.set_hit_id(vertices.size());
    spot.grab_auxiliaries().update(snemo::datamodel::particle_track::vertex_type_key(),
                                   vertex_label);
    spot.set_blur_dimension(geomtools::blur_spot::dimension_three);
    spot.set_position(first_vertex);
  }
  return;
}

// static
void alpha_finder_driver::init_ocd(datatools::object_configuration_description &ocd_) {
  // Prefix "AFD" stands for "Alpha Finder Driver" :
  datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "AFD.");

  {
    // Description of the 'AFD.minimal_delayed_time' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("AFD.minimal_delayed_time")
        .set_from("snemo::reconstruction::alpha_finder_driver")
        .set_terse_description("Set minimal delayed time to consider Geiger hit")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("time")
        .set_unit_symbol("us")
        .set_default_value_real(15.)
        .add_example(
            "Set the default value::                           \n"
            "                                                  \n"
            "  AFD.minimal_delayed_time : real as time = 15 us \n"
            "                                                  \n");
  }
  {
    // Description of the 'AFD.minimal_cluster_xy_search_distance' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("AFD.minimal_cluster_xy_search_distance")
        .set_from("snemo::reconstruction::alpha_finder_driver")
        .set_terse_description(
            "Set minimal distance in XY coordinates to associate delayed hit with prompt hit")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("cm")
        .set_default_value_real(21.)
        .add_example(
            "Set the default value::                                           \n"
            "                                                                  \n"
            "  AFD.minimal_cluster_xy_search_distance : real as length = 21 cm \n"
            "                                                                  \n");
  }
  {
    // Description of the 'AFD.minimal_cluster_z_search_distance' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("AFD.minimal_cluster_z_search_distance")
        .set_from("snemo::reconstruction::alpha_finder_driver")
        .set_terse_description(
            "Set minimal distance in Z coordinate to associate delayed hit with prompt hit")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("cm")
        .set_default_value_real(30.)
        .add_example(
            "Set the default value::                                          \n"
            "                                                                 \n"
            "  AFD.minimal_cluster_z_search_distance : real as length = 30 cm \n"
            "                                                                 \n");
  }
  {
    // Description of the 'AFD.minimal_vertex_distance' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("AFD.minimal_vertex_distance")
        .set_from("snemo::reconstruction::alpha_finder_driver")
        .set_terse_description(
            "Set minimal distance between Geiger hit and prompt track extremities")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("cm")
        .set_default_value_real(30.)
        .add_example(
            "Set the default value::                                \n"
            "                                                       \n"
            "  AFD.minimal_vertex_distance : real as length = 30 cm \n"
            "                                                       \n");
  }
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::alpha_finder_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::alpha_finder_driver");
  ocd_.set_class_description("A driver class for finding short alpha track");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation(
      "This driver extracts the short alpha tracks from delayed Geiger hits.");

  // Invoke specific OCD support :
  ::snemo::reconstruction::alpha_finder_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::alpha_finder_driver,
                               "snemo::reconstruction::alpha_finder_driver")

// end of falaise/snemo/reconstruction/alpha_finder_driver.cc
