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

void base_topology_measurement::tree_dump(std::ostream& out_, const std::string& title_,
                                          const std::string& indent_, bool /*inherit_*/) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
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
