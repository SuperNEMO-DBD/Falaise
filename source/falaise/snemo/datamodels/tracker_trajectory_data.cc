// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/tracker_trajectory_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

namespace snemo {

namespace datamodel {

bool tracker_trajectory_data::has_solutions() const { return get_number_of_solutions() > 0; }

size_t tracker_trajectory_data::get_number_of_solutions() const { return _solutions_.size(); }

const tracker_trajectory_solution& tracker_trajectory_data::get_solution(size_t index) const {
  return _solutions_.at(index).get();
}

void tracker_trajectory_data::add_solution(const TrackerTrajectorySolutionHdl& solution_handle_,
                                           bool default_solution_) {
  DT_THROW_IF(!solution_handle_, std::logic_error, "Cannot store a null handle !");

  for (const auto& addr : _solutions_) {
    DT_THROW_IF(&*addr == &*solution_handle_, std::logic_error,
                "Duplicated solutions is not allowed!");
  }
  _solutions_.push_back(solution_handle_);
  if (default_solution_ || _solutions_.size() == 1) {
    _default_solution_ = solution_handle_;
  }
}

void tracker_trajectory_data::invalidate_solutions() { _solutions_.clear(); }

bool tracker_trajectory_data::has_default_solution() const { return _default_solution_.has_data(); }

const tracker_trajectory_solution& tracker_trajectory_data::get_default_solution() const {
  DT_THROW_IF(!has_default_solution(), std::logic_error, "No default solution is available !");
  return *_default_solution_;
}

/// Set the default trajectory solution
void tracker_trajectory_data::set_default_solution(size_t index) {
  _default_solution_ = _solutions_.at(index);
}

void tracker_trajectory_data::invalidate_default_solution() { _default_solution_.reset(); }

TrackerTrajectorySolutionHdlCollection& tracker_trajectory_data::get_solutions() {
  return _solutions_;
}

const TrackerTrajectorySolutionHdlCollection& tracker_trajectory_data::get_solutions() const {
  return _solutions_;
}

void tracker_trajectory_data::reset() { this->clear(); }

void tracker_trajectory_data::clear() {
  invalidate_solutions();
  invalidate_default_solution();
  _auxiliaries_.clear();
}

void tracker_trajectory_data::tree_dump(std::ostream& out_, const std::string& title_,
                                        const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Solutions : " << _solutions_.size()
       << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Default solution : " << (_default_solution_ ? "Yes" : "No") << std::endl;
  if (_default_solution_) {
    std::ostringstream indent_oss;
    indent_oss << indent_;
    indent_oss << datatools::i_tree_dumpable::skip_tag;
    _default_solution_.get().tree_dump(out_, "", indent_oss.str());
  }

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Auxiliaries : ";
  if (_auxiliaries_.empty()) {
    out_ << "<empty>";
  }
  out_ << std::endl;
  {
    std::ostringstream indent_oss;
    indent_oss << indent_;
    indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    _auxiliaries_.tree_dump(out_, "", indent_oss.str());
  }
}

// serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory_data,
                                                  "snemo::datamodel::tracker_trajectory_data")

}  // end of namespace datamodel

}  // end of namespace snemo
