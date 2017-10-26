/** \file falaise/snemo/datamodels/base_topology_measurement.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/base_topology_measurement.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_topology_measurement,
                                                  "snemo::datamodel::base_topology_measurement")

base_topology_measurement::base_topology_measurement() { return; }

base_topology_measurement::~base_topology_measurement() { return; }

const datatools::properties& base_topology_measurement::get_auxiliaries() const {
  return _auxiliaries_;
}

datatools::properties& base_topology_measurement::grab_auxiliaries() { return _auxiliaries_; }

void base_topology_measurement::tree_dump(std::ostream& out_, const std::string& title_,
                                          const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Auxiliaries : ";
  if (_auxiliaries_.empty()) {
    out_ << "<empty>";
  }
  out_ << std::endl;
  {
    std::ostringstream indent_oss;
    indent_oss << indent;
    indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    _auxiliaries_.tree_dump(out_, "", indent_oss.str());
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
