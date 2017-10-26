/** \file falaise/snemo/datamodels/tof_measurement.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/tof_measurement.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tof_measurement,
                                                  "snemo::datamodel::tof_measurement")

tof_measurement::tof_measurement() { return; }

tof_measurement::~tof_measurement() { return; }

bool tof_measurement::has_internal_probabilities() const {
  return !_internal_probabilities_.empty();
}

const tof_measurement::probability_type& tof_measurement::get_internal_probabilities() const {
  return _internal_probabilities_;
}

tof_measurement::probability_type& tof_measurement::grab_internal_probabilities() {
  return _internal_probabilities_;
}

bool tof_measurement::has_external_probabilities() const {
  return !_external_probabilities_.empty();
}

const tof_measurement::probability_type& tof_measurement::get_external_probabilities() const {
  return _external_probabilities_;
}

tof_measurement::probability_type& tof_measurement::grab_external_probabilities() {
  return _external_probabilities_;
}

void tof_measurement::tree_dump(std::ostream& out_, const std::string& title_,
                                const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) indent = indent_;
  base_topology_measurement::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::tag << "Internal probabilities: ";
  if (has_internal_probabilities()) {
    out_ << _internal_probabilities_.size() << std::endl;
  } else {
    out_ << "<no value>" << std::endl;
  }
  for (size_t i = 0; i < _internal_probabilities_.size(); i++) {
    const double pint = _internal_probabilities_.at(i);
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    if (i + 1 == _internal_probabilities_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Value #" << i << " = " << pint / CLHEP::perCent << "%" << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "External probabilities: ";
  if (has_external_probabilities()) {
    out_ << _external_probabilities_.size() << std::endl;
  } else {
    out_ << "<no value>" << std::endl;
  }
  for (size_t i = 0; i < _external_probabilities_.size(); i++) {
    const double pext = _external_probabilities_.at(i);
    out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    if (i + 1 == _external_probabilities_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Value #" << i << " = " << pext / CLHEP::perCent << "%" << std::endl;
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
