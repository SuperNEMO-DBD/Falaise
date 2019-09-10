// falaise/snemo/datamodels/tracker_trajectory.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

// serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory,
                                                  "snemo::datamodel::tracker_trajectory")

bool tracker_trajectory::has_id() const { return has_hit_id(); }

int tracker_trajectory::get_id() const { return get_hit_id(); }

void tracker_trajectory::set_id(int id) { set_hit_id(id); }

bool tracker_trajectory::has_cluster() const { return cluster_.has_data(); }

void tracker_trajectory::detach_cluster() { cluster_.reset(); }

void tracker_trajectory::set_cluster_handle(const TrackerClusterHdl& cluster) {
  cluster_ = cluster;
}

tracker_cluster& tracker_trajectory::get_cluster() { return *cluster_; }

const tracker_cluster& tracker_trajectory::get_cluster() const { return *cluster_; }

bool tracker_trajectory::has_pattern() const { return pattern_.has_data(); }

void tracker_trajectory::set_pattern_handle(const TrajectoryPatternHdl& pattern) {
  pattern_ = pattern;
}

void tracker_trajectory::detach_pattern() { pattern_.reset(); }

TrajectoryPattern& tracker_trajectory::get_pattern() { return *pattern_; }

const TrajectoryPattern& tracker_trajectory::get_pattern() const { return *pattern_; }

void tracker_trajectory::clear() {
  detach_pattern();
  orphans_.clear();
  detach_cluster();
  base_hit::clear();
}

void tracker_trajectory::tree_dump(std::ostream& out, const std::string& title,
                                   const std::string& indent, bool is_last) const {
  base_hit::tree_dump(out, title, indent, true);

  out << indent << datatools::i_tree_dumpable::tag << "Cluster : "
      << (has_cluster() ? std::to_string(cluster_->get_cluster_id()) : "<none>")
      << std::endl;
  out << indent << datatools::i_tree_dumpable::tag << "Pattern : "
      << (has_pattern() ? pattern_->get_pattern_id() : "<none>")
      << std::endl;
  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last)
      << "Trajectory ID  : " << get_id() << std::endl;
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
