/// \file falaise/snemo/datamodels/tracker_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace {
const std::string& delayed_cluster_flag() {
  static const std::string _flag("delayed");
  return _flag;
}
}  // namespace

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_cluster,
                                                  "snemo::datamodel::tracker_cluster")

bool tracker_cluster::is_delayed() const {
  return get_auxiliaries().has_flag(delayed_cluster_flag());
}

bool tracker_cluster::is_prompt() const { return !is_delayed(); }

void tracker_cluster::make_delayed() { grab_auxiliaries().update_flag(delayed_cluster_flag()); }

void tracker_cluster::make_prompt() { grab_auxiliaries().unset_flag(delayed_cluster_flag()); }

int tracker_cluster::get_cluster_id() const { return get_hit_id(); }

void tracker_cluster::set_cluster_id(int32_t id) { set_hit_id(id); }

TrackerHitHdlCollection& tracker_cluster::hits() { return hits_; }

const TrackerHitHdlCollection& tracker_cluster::hits() const { return hits_; }

void tracker_cluster::clear() {
  hits_.clear();
  base_hit::clear();
}

size_t tracker_cluster::size() const { return hits_.size(); }

const calibrated_tracker_hit& tracker_cluster::at(size_t index) const {
  return *(hits_.at(index));
}

void tracker_cluster::tree_dump(std::ostream& out, const std::string& title,
                                const std::string& indent, bool is_last) const {
  base_hit::tree_dump(out, title, indent, true);

  out << indent << datatools::i_tree_dumpable::tag << "Hits[" << hits_.size() << "]:" << std::endl;
  for (size_t i = 0; i < hits_.size(); ++i) {
    //const calibrated_tracker_hit& gg_calib_hit = hits_.at(i).get();
    out << indent << datatools::i_tree_dumpable::skip_tag;
    if (i + 1 == hits_.size()) {
      out << datatools::i_tree_dumpable::last_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
    }
    out << "Hit[" << i << "] : (Id : " << hits_[i]->get_hit_id()
         << ", GID : " << hits_[i]->get_geom_id() << ")" << std::endl;
  }
  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last)
       << "Cluster ID  : " << get_cluster_id() << std::endl;
}

}  // end of namespace datamodel

}  // end of namespace snemo
