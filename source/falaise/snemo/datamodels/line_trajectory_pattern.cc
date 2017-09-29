// falaise/snemo/datamodels/line_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(line_trajectory_pattern,
                                                  "snemo::datamodel::line_trajectory_pattern")

// static
const std::string &line_trajectory_pattern::pattern_id() {
  static const std::string _id("line");
  return _id;
}

line_trajectory_pattern::line_trajectory_pattern()
    : base_trajectory_pattern(line_trajectory_pattern::pattern_id()) {
  return;
}

line_trajectory_pattern::~line_trajectory_pattern() { return; }

geomtools::line_3d &line_trajectory_pattern::grab_segment() { return _segment_; }

const geomtools::line_3d &line_trajectory_pattern::get_segment() const { return _segment_; }

// virtual
const geomtools::i_shape_1d &line_trajectory_pattern::get_shape() const {
  return dynamic_cast<const geomtools::i_shape_1d &>(_segment_);
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
