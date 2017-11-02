/** \file falaise/snemo/datamodels/energy_measurement.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/energy_measurement.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(energy_measurement,
                                                  "snemo::datamodel::energy_measurement")

energy_measurement::energy_measurement() {
  datatools::invalidate(_energy_);
  return;
}

energy_measurement::~energy_measurement() { return; }

bool energy_measurement::has_energy() const { return datatools::is_valid(_energy_); }

void energy_measurement::set_energy(double energy_) {
  _energy_ = energy_;
  return;
}

const double& energy_measurement::get_energy() const { return _energy_; }

double& energy_measurement::grab_energy() { return _energy_; }

void energy_measurement::tree_dump(std::ostream& out_, const std::string& title_,
                                   const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) indent = indent_;
  base_topology_measurement::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Energy: ";
  if (has_energy()) {
    out_ << _energy_ / CLHEP::keV << " keV" << std::endl;
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
