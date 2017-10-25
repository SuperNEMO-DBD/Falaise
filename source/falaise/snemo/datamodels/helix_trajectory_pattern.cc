// falaise/snemo/datamodels/helix_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(helix_trajectory_pattern,
                                                  "snemo::datamodel::helix_trajectory_pattern")

// static
const std::string &helix_trajectory_pattern::pattern_id() {
  static const std::string _id("helix");
  return _id;
}

helix_trajectory_pattern::helix_trajectory_pattern()
    : base_trajectory_pattern(helix_trajectory_pattern::pattern_id()) {
  return;
}

helix_trajectory_pattern::~helix_trajectory_pattern() { return; }

geomtools::helix_3d &helix_trajectory_pattern::grab_helix() { return _helix_; }

const geomtools::helix_3d &helix_trajectory_pattern::get_helix() const { return _helix_; }

// virtual
const geomtools::i_shape_1d &helix_trajectory_pattern::get_shape() const {
  return dynamic_cast<const geomtools::i_shape_1d &>(_helix_);
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
