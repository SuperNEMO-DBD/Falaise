/** \file falaise/snemo/datamodels/base_trajectory_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_trajectory_pattern,
                                                  "snemo::datamodel::base_trajectory_pattern")

bool base_trajectory_pattern::has_pattern_id() const { return !_pattern_id_.empty(); }

const std::string& base_trajectory_pattern::get_pattern_id() const { return _pattern_id_; }

void base_trajectory_pattern::_set_pattern_id(const std::string& pattern_id_) {
  _pattern_id_ = pattern_id_;
  return;
}

base_trajectory_pattern::base_trajectory_pattern(const std::string& pattern_id_) {
  _set_pattern_id(pattern_id_);
  return;
}

base_trajectory_pattern::~base_trajectory_pattern() { return; }

}  // end of namespace datamodel

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
