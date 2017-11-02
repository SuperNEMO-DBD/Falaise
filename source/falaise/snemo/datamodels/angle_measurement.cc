/** \file falaise/snemo/datamodels/angle_measurement.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/angle_measurement.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(angle_measurement,
                                                  "snemo::datamodel::angle_measurement")

angle_measurement::angle_measurement() {
  datatools::invalidate(_angle_);
  return;
}

angle_measurement::~angle_measurement() { return; }

bool angle_measurement::has_angle() const { return datatools::is_valid(_angle_); }

void angle_measurement::set_angle(double angle_) {
  _angle_ = angle_;
  return;
}

const double& angle_measurement::get_angle() const { return _angle_; }

double& angle_measurement::grab_angle() { return _angle_; }

void angle_measurement::tree_dump(std::ostream& out_, const std::string& title_,
                                  const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) indent = indent_;
  base_topology_measurement::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Angle: ";
  if (has_angle()) {
    out_ << _angle_ / CLHEP::degree << "°" << std::endl;
  } else {
    out_ << "<no value>" << std::endl;
  }
  return;
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
