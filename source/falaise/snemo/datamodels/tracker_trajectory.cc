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

bool tracker_trajectory::has_trajectory_id() const { return has_hit_id(); }

int tracker_trajectory::get_trajectory_id() const { return get_hit_id(); }

void tracker_trajectory::set_trajectory_id(int trajectory_id_) {
  set_hit_id(trajectory_id_);
}

void tracker_trajectory::invalidate_trajectory_id() {
  invalidate_hit_id();
}

bool tracker_trajectory::has_cluster() const { return _cluster_.has_data(); }

void tracker_trajectory::detach_cluster() {
  _cluster_.reset();
}

tracker_trajectory::handle_cluster& tracker_trajectory::get_cluster_handle() { return _cluster_; }

const tracker_trajectory::handle_cluster& tracker_trajectory::get_cluster_handle() const {
  return _cluster_;
}

void tracker_trajectory::set_cluster_handle(const handle_cluster& cluster_handle_) {
  _cluster_ = cluster_handle_;
}

tracker_cluster& tracker_trajectory::get_cluster() { return *_cluster_; }

const tracker_cluster& tracker_trajectory::get_cluster() const { return *_cluster_; }

bool tracker_trajectory::has_pattern() const { return _pattern_.has_data(); }

void tracker_trajectory::set_pattern_handle(const handle_pattern& pattern_handle_) {
  _pattern_ = pattern_handle_;
}

void tracker_trajectory::detach_pattern() {
  _pattern_.reset();
}

tracker_trajectory::handle_pattern& tracker_trajectory::get_pattern_handle() { return _pattern_; }

const tracker_trajectory::handle_pattern& tracker_trajectory::get_pattern_handle() const {
  return _pattern_;
}

base_trajectory_pattern& tracker_trajectory::get_pattern() { return *_pattern_; }

const base_trajectory_pattern& tracker_trajectory::get_pattern() const { return *_pattern_; }

void tracker_trajectory::reset() {
  this->clear();
}

void tracker_trajectory::clear() {
  detach_pattern();
  _orphans_.clear();
  detach_cluster();
  base_hit::clear();
}

void tracker_trajectory::tree_dump(std::ostream& out_, const std::string& title_,
                                   const std::string& indent_, bool inherit_) const {
  base_hit::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Cluster : ";
  if (has_cluster()) {
    out_ << _cluster_->get_cluster_id();
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Pattern : ";
  if (!has_pattern()) {
    out_ << "<none>";
  } else {
    out_ << "'" << _pattern_->get_pattern_id() << "'";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Trajectory ID  : " << get_trajectory_id() << std::endl;

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
