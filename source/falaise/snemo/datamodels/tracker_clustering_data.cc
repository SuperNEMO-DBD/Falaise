/// \file falaise/snemo/datamodels/tracker_clustering_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

namespace datamodel {

bool tracker_clustering_data::empty() const { return _solutions_.empty(); }

size_t tracker_clustering_data::size() const { return _solutions_.size(); }

const tracker_clustering_solution& tracker_clustering_data::at(size_t index) const {
  return *(_solutions_.at(index));
}

void tracker_clustering_data::push_back(const TrackerClusteringSolutionHdl& solution,
                                        bool isDefault) {
  DT_THROW_IF(!solution, std::logic_error, "Cannot store a null handle !");

  for (const auto& addr : _solutions_) {
    DT_THROW_IF(&(*addr) == &(*solution), std::logic_error, "Duplicated solutions is not allowed!");
  }
  _solutions_.push_back(solution);
  if (isDefault || _solutions_.size() == 1) {
    _default_solution_ = solution;
  }
}

bool tracker_clustering_data::has_default() const { return _default_solution_; }

tracker_clustering_solution& tracker_clustering_data::get_default() {
  DT_THROW_IF(empty(), std::logic_error, "No default solution is available !");
  return *_default_solution_;
}

const tracker_clustering_solution& tracker_clustering_data::get_default() const {
  DT_THROW_IF(empty(), std::logic_error, "No default solution is available !");
  return *_default_solution_;
}

void tracker_clustering_data::set_default(size_t index_) {
  _default_solution_ = _solutions_.at(index_);
}

TrackerClusteringSolutionHdlCollection& tracker_clustering_data::solutions() { return _solutions_; }

const TrackerClusteringSolutionHdlCollection& tracker_clustering_data::solutions() const {
  return _solutions_;
}

void tracker_clustering_data::clear() {
  _solutions_.clear();
  _default_solution_ = TrackerClusteringSolutionHdl{};
  _auxiliaries_.clear();
}

void tracker_clustering_data::tree_dump(std::ostream& out_, const std::string& title_,
                                        const std::string& indent_, bool /*inherit_*/) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag << "Solution(s) : " << _solutions_.size()
       << std::endl;
  for (size_t i = 0; i < size(); i++) {
    std::ostringstream indent2;
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent << datatools::i_tree_dumpable::skip_tag;
    if (i == size() - 1) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "Solution #" << i << " : " << std::endl;
    at(i).tree_dump(out_, "", indent2.str());
  }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Default solution : " << (!_default_solution_ ? "No" : "Yes") << std::endl;
}

// Serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_data,
                                                  "snemo::datamodel::tracker_clustering_data")

}  // end of namespace datamodel

}  // end of namespace snemo
