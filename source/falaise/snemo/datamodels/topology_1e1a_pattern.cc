/** \file falaise/snemo/datamodels/topology_1e1a_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/angle_measurement.h>
#include <falaise/snemo/datamodels/topology_1e1a_pattern.h>
#include <falaise/snemo/datamodels/vertex_measurement.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_1e1a_pattern,
                                                  "snemo::datamodel::topology_1e1a_pattern")

// static
const std::string& topology_1e1a_pattern::pattern_id() {
  static const std::string _id("1e1a");
  return _id;
}

std::string topology_1e1a_pattern::get_pattern_id() const {
  return topology_1e1a_pattern::pattern_id();
}

topology_1e1a_pattern::topology_1e1a_pattern() : topology_1e_pattern() { return; }

topology_1e1a_pattern::~topology_1e1a_pattern() { return; }

bool topology_1e1a_pattern::has_alpha_track() const { return has_particle_track("a1"); }

const snemo::datamodel::particle_track& topology_1e1a_pattern::get_alpha_track() const {
  return get_particle_track("a1");
}

bool topology_1e1a_pattern::has_alpha_angle() const {
  return has_measurement_as<snemo::datamodel::angle_measurement>("angle_a1");
}

double topology_1e1a_pattern::get_alpha_angle() const {
  DT_THROW_IF(!has_alpha_angle(), std::logic_error, "No alpha angle measurement stored !");
  return get_measurement_as<snemo::datamodel::angle_measurement>("angle_a1").get_angle();
}

bool topology_1e1a_pattern::has_electron_alpha_angle() const {
  return has_measurement_as<snemo::datamodel::angle_measurement>("angle_e1_a1");
}

double topology_1e1a_pattern::get_electron_alpha_angle() const {
  DT_THROW_IF(!has_electron_alpha_angle(), std::logic_error,
              "No electron-alpha angle measurement stored !");
  return get_measurement_as<snemo::datamodel::angle_measurement>("angle_e1_a1").get_angle();
}

bool topology_1e1a_pattern::has_electron_alpha_vertices_probability() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1");
}

double topology_1e1a_pattern::get_electron_alpha_vertices_probability() const {
  DT_THROW_IF(!has_electron_alpha_vertices_probability(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1").get_probability();
}

bool topology_1e1a_pattern::has_electron_alpha_vertices_distance() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1");
}

double topology_1e1a_pattern::get_electron_alpha_vertices_distance_x() const {
  DT_THROW_IF(!has_electron_alpha_vertices_distance(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1")
      .get_vertices_distance_x();
}

double topology_1e1a_pattern::get_electron_alpha_vertices_distance_y() const {
  DT_THROW_IF(!has_electron_alpha_vertices_distance(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1")
      .get_vertices_distance_y();
}

double topology_1e1a_pattern::get_electron_alpha_vertices_distance_z() const {
  DT_THROW_IF(!has_electron_alpha_vertices_distance(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1")
      .get_vertices_distance_z();
}

bool topology_1e1a_pattern::has_electron_alpha_vertex_location() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1");
}

std::string topology_1e1a_pattern::get_electron_alpha_vertex_location() const {
  DT_THROW_IF(!has_electron_alpha_vertex_location(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return dynamic_cast<const snemo::datamodel::vertex_measurement&>(get_measurement("vertex_e1_a1"))
      .get_location();
}

bool topology_1e1a_pattern::has_electron_alpha_vertex_position() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_a1");
}

double topology_1e1a_pattern::get_electron_alpha_vertex_position_x() const {
  DT_THROW_IF(!has_electron_alpha_vertex_position(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return dynamic_cast<const snemo::datamodel::vertex_measurement&>(get_measurement("vertex_e1_a1"))
      .get_vertex_position_x();
}

double topology_1e1a_pattern::get_electron_alpha_vertex_position_y() const {
  DT_THROW_IF(!has_electron_alpha_vertex_position(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return dynamic_cast<const snemo::datamodel::vertex_measurement&>(get_measurement("vertex_e1_a1"))
      .get_vertex_position_y();
}

double topology_1e1a_pattern::get_electron_alpha_vertex_position_z() const {
  DT_THROW_IF(!has_electron_alpha_vertex_position(), std::logic_error,
              "No common electron-alpha vertices measurement stored !");
  return dynamic_cast<const snemo::datamodel::vertex_measurement&>(get_measurement("vertex_e1_a1"))
      .get_vertex_position_z();
}

double topology_1e1a_pattern::get_alpha_delayed_time() const {
  double time = datatools::invalid_real();
  if (has_alpha_track()) {
    const snemo::datamodel::particle_track& the_alpha = get_alpha_track();
    if (the_alpha.has_trajectory()) {
      const snemo::datamodel::tracker_trajectory& a_trajectory = the_alpha.get_trajectory();
      const datatools::properties& aux = a_trajectory.get_auxiliaries();
      if (aux.has_key("t0")) {
        time = aux.fetch_real("t0");
      }
    }
  }
  return time;
}

double topology_1e1a_pattern::get_alpha_track_length() const {
  double length = datatools::invalid_real();
  if (has_alpha_track()) {
    const snemo::datamodel::particle_track& the_alpha = get_alpha_track();
    if (the_alpha.has_trajectory()) {
      const snemo::datamodel::tracker_trajectory& a_trajectory = the_alpha.get_trajectory();
      const snemo::datamodel::base_trajectory_pattern& a_track_pattern = a_trajectory.get_pattern();
      length = a_track_pattern.get_shape().get_length();
    }
  }
  return length;
}

}  // end of namespace datamodel

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
