/** \file falaise/snemo/datamodels/topology_2e_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/angle_measurement.h>
#include <falaise/snemo/datamodels/energy_measurement.h>
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/tof_measurement.h>
#include <falaise/snemo/datamodels/topology_2e_pattern.h>
#include <falaise/snemo/datamodels/vertex_measurement.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_2e_pattern,
                                                  "snemo::datamodel::topology_2e_pattern")

// static
const std::string& topology_2e_pattern::pattern_id() {
  static const std::string _id("2e");
  return _id;
}

std::string topology_2e_pattern::get_pattern_id() const {
  return topology_2e_pattern::pattern_id();
}

topology_2e_pattern::topology_2e_pattern() : base_topology_pattern() { return; }

topology_2e_pattern::~topology_2e_pattern() { return; }

bool topology_2e_pattern::has_electrons_energy() const {
  return has_measurement_as<snemo::datamodel::energy_measurement>("energy_e1") &&
         has_measurement_as<snemo::datamodel::energy_measurement>("energy_e2");
}

bool topology_2e_pattern::has_electron_minimal_energy() const { return has_electrons_energy(); }

double topology_2e_pattern::get_electron_minimal_energy() const {
  DT_THROW_IF(!has_electron_minimal_energy(), std::logic_error,
              "No electron minimal energy measurement stored !");
  return std::min(
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e1").get_energy(),
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e2").get_energy());
}

bool topology_2e_pattern::has_electron_maximal_energy() const { return has_electrons_energy(); }

double topology_2e_pattern::get_electron_maximal_energy() const {
  DT_THROW_IF(!has_electron_maximal_energy(), std::logic_error,
              "No electron maximal energy measurement stored !");
  return std::max(
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e1").get_energy(),
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e2").get_energy());
}

double topology_2e_pattern::get_electrons_energy_sum() const {
  DT_THROW_IF(!has_electrons_energy(), std::logic_error, "No electron energy measurement stored !");
  return get_electron_minimal_energy() + get_electron_maximal_energy();
}

double topology_2e_pattern::get_electrons_energy_difference() const {
  DT_THROW_IF(!has_electrons_energy(), std::logic_error, "No electron energy measurement stored !");
  return get_electron_maximal_energy() - get_electron_minimal_energy();
}

std::string topology_2e_pattern::get_minimal_energy_electron_name() const {
  if (get_measurement_as<snemo::datamodel::energy_measurement>("energy_e1").get_energy() <
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e2").get_energy())
    return "e1";
  else
    return "e2";
}

std::string topology_2e_pattern::get_maximal_energy_electron_name() const {
  if (get_measurement_as<snemo::datamodel::energy_measurement>("energy_e1").get_energy() <
      get_measurement_as<snemo::datamodel::energy_measurement>("energy_e2").get_energy())
    return "e2";
  else
    return "e1";
}

bool topology_2e_pattern::has_electrons_internal_probability() const {
  return has_measurement_as<snemo::datamodel::tof_measurement>("tof_e1_e2");
}

double topology_2e_pattern::get_electrons_internal_probability() const {
  DT_THROW_IF(!has_electrons_internal_probability(), std::logic_error,
              "No electrons TOF measurement stored !");
  return get_measurement_as<snemo::datamodel::tof_measurement>("tof_e1_e2")
      .get_internal_probabilities()
      .front();
}

bool topology_2e_pattern::has_electrons_external_probability() const {
  return has_measurement_as<snemo::datamodel::tof_measurement>("tof_e1_e2");
}

double topology_2e_pattern::get_electrons_external_probability() const {
  DT_THROW_IF(!has_electrons_external_probability(), std::logic_error,
              "No electrons TOF measurement stored !");
  return get_measurement_as<snemo::datamodel::tof_measurement>("tof_e1_e2")
      .get_external_probabilities()
      .front();
}

bool topology_2e_pattern::has_electrons_angle() const {
  return has_measurement_as<snemo::datamodel::angle_measurement>("angle_e1_e2");
}

double topology_2e_pattern::get_electrons_angle() const {
  DT_THROW_IF(!has_electrons_external_probability(), std::logic_error,
              "No electrons angle measurement stored !");
  return get_measurement_as<snemo::datamodel::angle_measurement>("angle_e1_e2").get_angle();
}

bool topology_2e_pattern::has_electrons_vertices_probability() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2");
}

double topology_2e_pattern::get_electrons_vertices_probability() const {
  DT_THROW_IF(!has_electrons_vertices_probability(), std::logic_error,
              "No common electrons vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2").get_probability();
}

bool topology_2e_pattern::has_electrons_vertices_distance() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2");
}

double topology_2e_pattern::get_electrons_vertices_distance_x() const {
  DT_THROW_IF(!has_electrons_vertices_distance(), std::logic_error,
              "No common electrons vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
      .get_vertices_distance_x();
}

double topology_2e_pattern::get_electrons_vertices_distance_y() const {
  DT_THROW_IF(!has_electrons_vertices_distance(), std::logic_error,
              "No common electrons vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
      .get_vertices_distance_y();
}

double topology_2e_pattern::get_electrons_vertices_distance_z() const {
  DT_THROW_IF(!has_electrons_vertices_distance(), std::logic_error,
              "No common electrons vertices measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
      .get_vertices_distance_z();
}

bool topology_2e_pattern::has_electrons_vertex_location() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2");
}

std::string topology_2e_pattern::get_electrons_vertex_location() const {
  DT_THROW_IF(!has_electrons_vertex_location(), std::logic_error,
              "No common electrons vertices measurement stored !");
  return dynamic_cast<const snemo::datamodel::vertex_measurement&>(get_measurement("vertex_e1_e2"))
      .get_location();
}

bool topology_2e_pattern::has_electrons_vertex_position() const {
  return has_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2");
}

double topology_2e_pattern::get_electrons_vertex_position_x() const {
  DT_THROW_IF(!has_electrons_vertex_position(), std::logic_error,
              "No electrons vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
      .get_vertex_position_x();
}

double topology_2e_pattern::get_electrons_vertex_position_y() const {
  DT_THROW_IF(!has_electrons_vertex_position(), std::logic_error,
              "No electrons vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
      .get_vertex_position_y();
}

double topology_2e_pattern::get_electrons_vertex_position_z() const {
  DT_THROW_IF(!has_electrons_vertex_position(), std::logic_error,
              "No electrons vertex measurement stored !");
  return get_measurement_as<snemo::datamodel::vertex_measurement>("vertex_e1_e2")
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
