/// \file falaise/snemo/reconstruction/charge_computation_driver.cc

// Ourselves:
#include <ChargedParticleTracking/charge_computation_driver.h>

// Standard library:
#include <sstream>

// This project (Falaise):
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

namespace snemo {

namespace reconstruction {

const std::string &charge_computation_driver::get_id() {
  static const std::string s("CCD");
  return s;
}

charge_computation_driver::charge_computation_driver(const falaise::property_set &ps)
    : charge_computation_driver::charge_computation_driver() {
  chargeFromSource_ = ps.get<bool>("charge_from_source", true);

  auto a_direction = ps.get<std::string>("magnetic_field_direction", "+z");
  if (a_direction == "+z") {
    magneticFieldDirection_ = +1;
  } else if (a_direction == "-z") {
    magneticFieldDirection_ = -1;
  } else {
    DT_THROW_IF(true, std::logic_error,
                "Value for 'magnetic_field_direction' must be either '+z' or '-z'!");
  }
}

void charge_computation_driver::process(const snemo::datamodel::tracker_trajectory &trajectory_,
                                        snemo::datamodel::particle_track &particle_) {
  namespace snedm = snemo::datamodel;

  // Look first if trajectory pattern is an helix or not
  const snedm::base_trajectory_pattern &a_track_pattern = trajectory_.get_pattern();

  if (a_track_pattern.get_pattern_id() == snedm::line_trajectory_pattern::pattern_id()) {
    particle_.set_charge(snedm::particle_track::UNDEFINED);
    return;
  }

  // Retrieve helix trajectory
  const snedm::helix_trajectory_pattern* ptr_helix = nullptr;
  if (a_track_pattern.get_pattern_id() == snedm::helix_trajectory_pattern::pattern_id()) {
    ptr_helix = dynamic_cast<const snedm::helix_trajectory_pattern *>(&a_track_pattern);
  }
  if (ptr_helix == nullptr) {
    return;
  }

  // Retrieve starting and ending point of helix trajectory
  const geomtools::vector_3d first_point = ptr_helix->get_helix().get_first();
  const geomtools::vector_3d last_point = ptr_helix->get_helix().get_last();
  const bool is_negative = std::fabs(first_point.x()) < std::fabs(last_point.x());

  int a_charge = (is_negative ? -1 : +1);
  a_charge *= magneticFieldDirection_;
  if (chargeFromSource_) {
    a_charge *= -1;
  }

  if (a_charge < 0) {
    particle_.set_charge(snedm::particle_track::NEGATIVE);
  } else {
    particle_.set_charge(snedm::particle_track::POSITIVE);
  }
}

}  // end of namespace reconstruction

}  // end of namespace snemo
