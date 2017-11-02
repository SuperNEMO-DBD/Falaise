/** \file falaise/snemo/datamodels/topology_1e_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/angle_measurement.h>
#include <falaise/snemo/datamodels/energy_measurement.h>
#include <falaise/snemo/datamodels/topology_1e_pattern.h>
#include <falaise/snemo/datamodels/vertex_measurement.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_1e_pattern,
                                                  "snemo::datamodel::topology_1e_pattern")

// static
const std::string& topology_1e_pattern::pattern_id() {
  static const std::string _id("1e");
  return _id;
}

std::string topology_1e_pattern::get_pattern_id() const {
  return topology_1e_pattern::pattern_id();
}

topology_1e_pattern::topology_1e_pattern() : base_topology_pattern() { return; }

topology_1e_pattern::~topology_1e_pattern() { return; }

bool topology_1e_pattern::has_electron_track() const { return has_particle_track("e1"); }

const snemo::datamodel::particle_track& topology_1e_pattern::get_electron_track() const {
  return get_particle_track("e1");
}

bool topology_1e_pattern::has_electron_angle() const {
  return has_measurement_as<snemo::datamodel::angle_measurement>("angle_e1");
}

double topology_1e_pattern::get_electron_angle() const {
  DT_THROW_IF(!has_electron_angle(), std::logic_error, "No electron angle measurement stored !");
  return get_measurement_as<snemo::datamodel::angle_measurement>("angle_e1").get_angle();
}

bool topology_1e_pattern::has_electron_energy() const {
  return has_measurement_as<snemo::datamodel::energy_measurement>("energy_e1");
}

double topology_1e_pattern::get_electron_energy() const {
  DT_THROW_IF(!has_electron_energy(), std::logic_error, "No electron energy measurement stored !");
  return get_measurement_as<snemo::datamodel::energy_measurement>("energy_e1").get_energy();
}

double topology_1e_pattern::get_electron_track_length() const {
  double length = datatools::invalid_real();
  if (has_electron_track()) {
    const snemo::datamodel::particle_track& the_electron = get_electron_track();
    if (the_electron.has_trajectory()) {
      const snemo::datamodel::tracker_trajectory& a_trajectory = the_electron.get_trajectory();
      const snemo::datamodel::base_trajectory_pattern& a_track_pattern = a_trajectory.get_pattern();
      length = a_track_pattern.get_shape().get_length();
    }
  }
  return length;
}

bool topology_1e_pattern::has_electron_vertex_location() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1");
}

std::string topology_1e_pattern::get_electron_vertex_location() const {
  DT_THROW_IF(!has_electron_vertex_location(), std::logic_error,
              "No electron vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1").get_location();
}

bool topology_1e_pattern::has_electron_vertex_position() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1");
}

double topology_1e_pattern::get_electron_vertex_position_x() const {
  DT_THROW_IF(!has_electron_vertex_position(), std::logic_error,
              "No electron vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1")
      .get_vertex_position_x();
}

double topology_1e_pattern::get_electron_vertex_position_y() const {
  DT_THROW_IF(!has_electron_vertex_position(), std::logic_error,
              "No electron vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1")
      .get_vertex_position_y();
}

double topology_1e_pattern::get_electron_vertex_position_z() const {
  DT_THROW_IF(!has_electron_vertex_position(), std::logic_error,
              "No electron vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1")
      .get_vertex_position_z();
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
