// \file falaise/snemo/reconstruction/alpha_finder_driver.cc

// Ourselves:
#include <ChargedParticleTracking/alpha_finder_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/property_set.h>
#include <falaise/quantity.h>

#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
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

const geomtools::manager &alpha_finder_driver::geoManager() const {
  DT_THROW_IF(geoManager_ == nullptr, std::logic_error, "No geometry manager is setup !");
  return *geoManager_;
}

/// Initialize the driver through configuration properties
alpha_finder_driver::alpha_finder_driver(const falaise::property_set &ps,
                                         const geomtools::manager *gm)
    : alpha_finder_driver::alpha_finder_driver() {
  logPriority_ =
      datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
  geoManager_ = gm;
  auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
  geoLocator_ = snemo::geometry::getSNemoLocator(geoManager(), locator_plugin_name);
  minDelayedTime_ = ps.get<falaise::time_t>("minimal_delayed_time", {15., "microsecond"})();
  minXYSearchDistance_ =
      ps.get<falaise::length_t>("minimal_cluster_xy_search_distance", {21., "cm"})();
  minZSearchDistance_ =
      ps.get<falaise::length_t>("minimal_cluster_z_search_distance", {30., "cm"})();
  minVertexDistance_ = ps.get<falaise::length_t>("minimal_vertex_distance", {30., "cm"})();
}

void alpha_finder_driver::process(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  this->_find_delayed_unfitted_cluster_(tracker_trajectory_data_, particle_track_data_);
  this->_find_delayed_unclustered_hit_(tracker_trajectory_data_, particle_track_data_);
}

void alpha_finder_driver::_find_delayed_unfitted_cluster_(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  namespace snedm = snemo::datamodel;

  // Check if the solution exist
  if (!tracker_trajectory_data_.has_solutions()) {
    return;
  }

  // The default solution is chosen
  const snedm::tracker_trajectory_solution &a_solution =
      tracker_trajectory_data_.get_default_solution();
  // Get unfitted cluster first
  if (!a_solution.has_unfitted_clusters()) {
    return;
  }
  const snedm::TrackerClusterHdlCollection &the_unfitted_clusters =
      a_solution.get_unfitted_clusters();
  // Loop on all the unfitted cluster
  for (const datatools::handle<snedm::tracker_cluster>& a_delayed_cluster : the_unfitted_clusters) {
    // The unfitted cluster has to be delayed
    if (!a_delayed_cluster->is_delayed()) {
      return;
    }

    const snedm::TrackerHitHdlCollection &delayed_gg_hits = a_delayed_cluster->hits();

    geomtools::vector_3d associated_vertex = geomtools::invalid_vector_3d();

    this->_find_short_track_(delayed_gg_hits, a_solution, particle_track_data_);

    // Add tracker cluster handle to trajectory
    if (particle_track_data_.hasParticles()) {
      snedm::ParticleHdlCollection &particles = particle_track_data_.particles();
      datatools::handle<snedm::particle_track> &a_particle = particles.back();
      const datatools::properties &aux = a_particle->get_auxiliaries();
      if (aux.has_flag(alpha_finder_driver::short_alpha_key()) && a_particle->has_trajectory()) {
        snedm::tracker_trajectory &a_trajectory = a_particle->get_trajectory();
        a_trajectory.set_cluster_handle(a_delayed_cluster);
      }
    }
  }  // end of delayed cluster
}

void alpha_finder_driver::_find_delayed_unclustered_hit_(
    const snemo::datamodel::tracker_trajectory_data &tracker_trajectory_data_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  namespace snedm = snemo::datamodel;

  // Check if the solution exist
  if (!tracker_trajectory_data_.has_solutions()) {
    return;
  }
  // The default solution is chosen
  const snedm::tracker_trajectory_solution &a_solution =
      tracker_trajectory_data_.get_default_solution();
  // Get associated clustering solution (if any)
  if (!a_solution.has_clustering_solution()) {
    return;
  }
  const snedm::tracker_clustering_solution &a_clustering_solution =
      a_solution.get_clustering_solution();
  if (!a_clustering_solution.has_unclustered_hits()) {
    return;
  }
  const snedm::TrackerHitHdlCollection &unclustered_gg_hits =
      a_clustering_solution.get_unclustered_hits();

  this->_find_short_track_(unclustered_gg_hits, a_solution, particle_track_data_, false);
}

void alpha_finder_driver::_find_short_track_(
    const snemo::datamodel::TrackerHitHdlCollection &hits_,
    const snemo::datamodel::tracker_trajectory_solution &solution_,
    snemo::datamodel::particle_track_data &particle_track_data_, const bool hits_from_cluster) {
  namespace snedm = snemo::datamodel;

  geomtools::vector_3d associated_vertex = geomtools::invalid_vector_3d();
  bool has_associated_alpha = false;
  // Loop on all the geiger hits of the unfitted cluster
  for (auto ihit = hits_.begin(); ihit != hits_.end(); ++ihit) {
    const snedm::calibrated_tracker_hit &a_delayed_gg_hit = ihit->get();

    // set a default distance for closest vertex to check against
    double closest_vertex_distance = minVertexDistance_;

    if (!a_delayed_gg_hit.is_delayed() || a_delayed_gg_hit.get_delayed_time() < minDelayedTime_) {
      continue;
    }

    if (!hits_from_cluster) {
      // Reset values
      geomtools::invalidate(associated_vertex);
      has_associated_alpha = false;
    }

    // Get prompt trajectories
    if (!solution_.has_trajectories()) {
      return;
    }
    const snedm::TrackerTrajectoryHdlCollection &the_trajectories = solution_.get_trajectories();
    // Loop on all the trajectories
    for (const datatools::handle<snedm::tracker_trajectory> &a_trajectory : the_trajectories) {
      // Look into properties to find the default trajectory. Here,
      // default means the one with the best chi2. This flag is set by the
      // 'fitting' module.
      if (!a_trajectory->get_auxiliaries().has_flag("default")) {
        continue;
      }

      if (!a_trajectory->has_cluster()) {
        continue;
      }

      const snedm::tracker_cluster &a_prompt_cluster = a_trajectory->get_cluster();
      if (a_prompt_cluster.is_delayed()) {
        continue;
      }

      const snedm::TrackerHitHdlCollection &prompt_gg_hits = a_prompt_cluster.hits();
      for (const datatools::handle<snedm::calibrated_tracker_hit> &a_prompt_gg_hit :
           prompt_gg_hits) {
        const geomtools::vector_2d a_prompt_position(a_prompt_gg_hit->get_x(),
                                                     a_prompt_gg_hit->get_y());
        const geomtools::vector_2d a_delayed_position(a_delayed_gg_hit.get_x(),
                                                      a_delayed_gg_hit.get_y());
        const double distance_xy = (a_delayed_position - a_prompt_position).mag();

        bool has_minimal_xy_search = false;
        if (distance_xy < minXYSearchDistance_) {
          has_minimal_xy_search = true;
        }

        const double z_delayed = a_delayed_gg_hit.get_z();
        const double sigma_z_delayed = a_delayed_gg_hit.get_sigma_z();
        const double z_prompt = a_prompt_gg_hit->get_z();
        const double sigma_z_prompt = a_prompt_gg_hit->get_sigma_z();
        const double distance_z = std::abs(z_delayed - z_prompt);
        const double sigma = sigma_z_delayed + sigma_z_prompt;
        bool has_minimal_z_search = false;
        if ((distance_z - minZSearchDistance_) < sigma) {
          has_minimal_z_search = true;
        }

        if (has_minimal_xy_search && has_minimal_z_search) {
          has_associated_alpha = true;
          break;
        }
      }  // end of prompt gg hits

      // Look for trajectories extremities
      if (has_associated_alpha) {
        geomtools::vector_3d first = geomtools::invalid_vector_3d();
        geomtools::vector_3d last = geomtools::invalid_vector_3d();
        const snedm::base_trajectory_pattern &a_pattern = a_trajectory->get_pattern();
        const std::string &a_pattern_id = a_pattern.get_pattern_id();
        if (a_pattern_id == snedm::line_trajectory_pattern::pattern_id()) {
          auto ltp = dynamic_cast<const snedm::line_trajectory_pattern &>(a_pattern);
          first = ltp.get_segment().get_first();
          last = ltp.get_segment().get_last();
        } else if (a_pattern_id == snedm::helix_trajectory_pattern::pattern_id()) {
          auto htp = dynamic_cast<const snedm::helix_trajectory_pattern &>(a_pattern);
          first = htp.get_helix().get_first();
          last = htp.get_helix().get_last();
        }
        const geomtools::vector_3d a_delayed_position(
            a_delayed_gg_hit.get_x(), a_delayed_gg_hit.get_y(), a_delayed_gg_hit.get_z());
        if (geomtools::is_valid(first)) {
          const double distance = (first - a_delayed_position).mag();
          if (distance < minVertexDistance_ && distance < closest_vertex_distance) {
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
          if (distance < minVertexDistance_ && distance < closest_vertex_distance) {
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
      }
      // If hits come from unclustered hits then add a new particle
      // Create a new cluster with only one delayed geiger hits and associate it
      // to the particle track trajectory
      auto a_cluster = datatools::make_handle<snedm::tracker_cluster>();
      a_cluster->make_delayed();
      auto &hits = a_cluster->hits();
      hits.push_back(*ihit);

      // Build a new particle track
      this->_build_alpha_particle_track_(hits, associated_vertex, particle_track_data_);

      // Add special tracker cluster handle to trajectory
      snedm::ParticleHdlCollection &particles = particle_track_data_.particles();
      snedm::particle_track &a_particle = particles.back().grab();
      const datatools::properties &aux = a_particle.get_auxiliaries();
      if (aux.has_flag(alpha_finder_driver::short_alpha_key()) && a_particle.has_trajectory()) {
        snedm::tracker_trajectory &a_trajectory = a_particle.get_trajectory();
        a_trajectory.set_cluster_handle(a_cluster);
      }
    }
  }  // end of delayed hits
}

void alpha_finder_driver::_fit_short_track_(const snemo::datamodel::TrackerHitHdlCollection &hits_,
                                            const geomtools::vector_3d &first_vertex_,
                                            geomtools::vector_3d &last_vertex_) {
  namespace snedm = snemo::datamodel;

  double max_distance = 0.0 * CLHEP::cm;
  // Loop on all the delayed geiger hits to compute distance between hit
  // and associated vertex
  for (const datatools::handle<snedm::calibrated_tracker_hit> &a_hit : hits_) {
    const geomtools::vector_3d a_hit_position(a_hit->get_x(), a_hit->get_y(), a_hit->get_z());
    const double distance = (first_vertex_ - a_hit_position).mag();
    if (distance > max_distance) {
      last_vertex_ = a_hit_position;
      max_distance = distance;
    }
  }
}

void alpha_finder_driver::_build_alpha_particle_track_(
    const snemo::datamodel::TrackerHitHdlCollection &hits_,
    const geomtools::vector_3d &first_vertex_,
    snemo::datamodel::particle_track_data &particle_track_data_) {
  namespace snedm = snemo::datamodel;

  // Add short alpha particle track
  auto a_short_alpha = datatools::make_handle<snedm::particle_track>();
  a_short_alpha->set_track_id(particle_track_data_.numberOfParticles());
  a_short_alpha->set_charge(snedm::particle_track::UNDEFINED);

  a_short_alpha->grab_auxiliaries().store_flag(alpha_finder_driver::short_alpha_key());

  particle_track_data_.insertParticle(a_short_alpha);

  // "Fit" short alpha trajectory
  geomtools::vector_3d last_vertex = geomtools::invalid_vector_3d();
  const geomtools::vector_3d &first_vertex = first_vertex_;
  this->_fit_short_track_(hits_, first_vertex, last_vertex);
  if (!geomtools::is_valid(last_vertex)) {
    return;
  }

  // Create new 'tracker_trajectory' handle:
  auto a_trajectory = datatools::make_handle<snedm::tracker_trajectory>();
  // Set trajectory geom_id using the first geiger hit of the associated
  // cluster
  geoManager().get_id_mgr().make_id("tracker_submodule", a_trajectory->grab_geom_id());
  geoManager().get_id_mgr().extract(hits_.front().get().get_geom_id(),
                                    a_trajectory->grab_geom_id());
  // Set trajectory pattern
  auto a_line = new snedm::line_trajectory_pattern;
  a_line->get_segment().set_first(first_vertex);
  a_line->get_segment().set_last(last_vertex);
  a_trajectory->set_pattern_handle(a_line);
  a_short_alpha->set_trajectory_handle(a_trajectory);

  // Set a alpha delayed time based on Geiger delayed times and store as
  // properties of line trajectory (like it is done so far in trackfit)
  double delay_time;
  datatools::plus_infinity(delay_time);  // double x = datatools::plus_infinity();
  {
    for (const datatools::handle<snedm::calibrated_tracker_hit> &a_hit : hits_) {
      delay_time = std::min(delay_time, a_hit->get_delayed_time());
    }
  }
  // Here we should spread the time since we do not know where exactly the
  // alpha passes through the cell.
  a_trajectory->grab_auxiliaries().store_real("t0", delay_time);

  // Determine vertices origin : previous process should have determine a
  // 'first' vertex associated to the prompt track and a 'last' vertex
  // associated to the center of a Geiger cells. The latter will be set to a
  // 'VERTEX_ON_WIRE'; the former will be set given its relative position
  // wrt to detector geometry.
  snedm::particle_track::vertex_collection_type &vertices = a_short_alpha->get_vertices();
  {
    // Vertex on wire
    auto spot = datatools::make_handle<geomtools::blur_spot>();
    spot->set_hit_id(vertices.size());
    spot->grab_auxiliaries().update(snedm::particle_track::vertex_type_key(),
                                    snedm::particle_track::vertex_on_wire_label());
    spot->set_blur_dimension(geomtools::blur_spot::dimension_three);
    spot->set_position(last_vertex);
    vertices.push_back(spot);
  }

  {
    // Check first vertex origin
    const geomtools::geom_id &gid = a_trajectory->get_geom_id();
    const geomtools::id_mgr &id_mgr = geoManager().get_id_mgr();
    if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side")) {
      DT_LOG_ERROR(logPriority_,
                   "Trajectory geom_id " << gid << " has no 'module' or 'side' address!");
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

    const double epsilon = 1e-5 * CLHEP::mm;
    std::string vertex_label;
    if (std::abs(first_vertex.x()) < epsilon) {
      vertex_label = snedm::particle_track::vertex_on_source_foil_label();
    } else if (std::abs(first_vertex.x() - xcalo_bd[0]) < epsilon ||
               std::abs(first_vertex.x() - xcalo_bd[1]) < epsilon) {
      vertex_label = snedm::particle_track::vertex_on_main_calorimeter_label();
    } else if (std::abs(first_vertex.y() - ycalo_bd[0]) < epsilon ||
               std::abs(first_vertex.y() - ycalo_bd[1]) < epsilon) {
      vertex_label = snedm::particle_track::vertex_on_x_calorimeter_label();
    } else if (std::abs(first_vertex.z() - zcalo_bd[0]) < epsilon ||
               std::abs(first_vertex.z() - zcalo_bd[1]) < epsilon) {
      vertex_label = snedm::particle_track::vertex_on_gamma_veto_label();
    } else {
      vertex_label = snedm::particle_track::vertex_on_wire_label();
    }
    auto spot = datatools::make_handle<geomtools::blur_spot>();
    spot->set_hit_id(vertices.size());
    spot->grab_auxiliaries().update(snedm::particle_track::vertex_type_key(), vertex_label);
    spot->set_blur_dimension(geomtools::blur_spot::dimension_three);
    spot->set_position(first_vertex);
    vertices.push_back(spot);
  }
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
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::alpha_finder_driver,
                               "snemo::reconstruction::alpha_finder_driver")

// end of falaise/snemo/reconstruction/alpha_finder_driver.cc
