// falaise/snemo/datamodels/tracker_trajectory_solution.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// This project:
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

namespace datamodel {

// serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory_solution,
                                                  "snemo::datamodel::tracker_trajectory_solution")

bool tracker_trajectory_solution::has_solution_id() const { return id_ >= 0; }

int tracker_trajectory_solution::get_solution_id() const { return id_; }

void tracker_trajectory_solution::set_solution_id(int32_t id) {
  if (id >= 0) {
    id_ = id;
  } else {
    invalidate_solution_id();
  }
}

void tracker_trajectory_solution::invalidate_solution_id() { id_ = -1; }

/*** Reference clustering solution ***/

tracker_clustering_solution& tracker_trajectory_solution::grab_clustering_solution() {
  return solutions_.grab();
}

void tracker_trajectory_solution::set_clustering_solution(const TrackerClusteringSolutionHdl& s) {
  solutions_ = s;
}

const tracker_clustering_solution& tracker_trajectory_solution::get_clustering_solution() const {
  return solutions_.get();
}

bool tracker_trajectory_solution::has_clustering_solution() const { return solutions_.has_data(); }

void tracker_trajectory_solution::invalidate_clustering_solution() { solutions_.reset(); }

bool tracker_trajectory_solution::has_unfitted_clusters() const {
  return !unfitted_.empty();
}

TrackerClusterHdlCollection& tracker_trajectory_solution::grab_unfitted_clusters() {
  return unfitted_;
}

const TrackerClusterHdlCollection& tracker_trajectory_solution::get_unfitted_clusters() const {
  return unfitted_;
}

void tracker_trajectory_solution::invalidate_unfitted_clusters() { unfitted_.clear(); }

datatools::properties& tracker_trajectory_solution::get_auxiliaries() { return _auxiliaries_; }

const datatools::properties& tracker_trajectory_solution::get_auxiliaries() const {
  return _auxiliaries_;
}

bool tracker_trajectory_solution::has_trajectories() const { return !trajectories_.empty(); }

TrackerTrajectoryHdlCollection& tracker_trajectory_solution::grab_trajectories() {
  return trajectories_;
}

const TrackerTrajectoryHdlCollection& tracker_trajectory_solution::get_trajectories() const {
  return trajectories_;
}

void tracker_trajectory_solution::invalidate_trajectories() { trajectories_.clear(); }

void tracker_trajectory_solution::reset() { this->clear(); }

void tracker_trajectory_solution::clear() {
  invalidate_solution_id();
  invalidate_clustering_solution();
  invalidate_trajectories();
  invalidate_unfitted_clusters();
  _auxiliaries_.clear();
}

void tracker_trajectory_solution::tree_dump(std::ostream& out, const std::string& title,
                                            const std::string& indent, bool is_last) const {
  if (!title.empty()) {
    out << indent << title << std::endl;
  }

  // make_tag
  // make_skip_tag
  // make_last_tag
  // make_last_skip_tag
  // make_inherit_tag
  // make_inherit_skip_tag
  std::string tag = indent + datatools::i_tree_dumpable::tags::item();
  std::string skip_tag = indent + datatools::i_tree_dumpable::tags::skip_item();
  std::string last_tag = indent + datatools::i_tree_dumpable::tags::last_item();
  std::string last_skip_tag = indent + datatools::i_tree_dumpable::tags::last_skip_item();
  // Last two are functions of indent and is_last;

  out << indent << datatools::i_tree_dumpable::tag << "Solution ID  : " << id_ << std::endl;

  out << indent << datatools::i_tree_dumpable::tag << "Reference clustering solution : ";
  if (has_clustering_solution()) {
    out << solutions_->get_solution_id();
  } else {
    out << "<none>";
  }
  out << std::endl;

  out << indent << datatools::i_tree_dumpable::tag << "Trajectories : " << trajectories_.size()
       << std::endl;
  for (size_t i = 0; i < trajectories_.size(); i++) {
    const datatools::handle<tracker_trajectory>& htraj = trajectories_.at(i);
    const tracker_trajectory& traj = htraj.get();
    out << indent << datatools::i_tree_dumpable::skip_tag;
    std::ostringstream indent2_oss;
    indent2_oss << indent << datatools::i_tree_dumpable::skip_tag;
    size_t j = i;
    j++;
    if (j == trajectories_.size()) {
      out << datatools::i_tree_dumpable::last_tag;
      indent2_oss << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
      indent2_oss << datatools::i_tree_dumpable::skip_tag;
    }
    out << "Trajectory #" << i << " : " << std::endl;
    traj.tree_dump(out, "", indent2_oss.str());
  }

  out << indent << datatools::i_tree_dumpable::tag
       << "Unfitted clusters : " << unfitted_.size() << std::endl;

  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last) << "Auxiliaries : ";
  if (_auxiliaries_.empty()) {
    out << "<empty>";
  }
  out << std::endl;
  {
    std::ostringstream indent_oss;
    indent_oss << indent;
    indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(is_last);
    _auxiliaries_.tree_dump(out, "", indent_oss.str());
  }
}

}  // end of namespace datamodel

}  // end of namespace snemo
