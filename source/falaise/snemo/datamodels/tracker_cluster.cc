/// \file falaise/snemo/datamodels/tracker_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_cluster,
                                                  "snemo::datamodel::tracker_cluster")

// static
const std::string& tracker_cluster::delayed_cluster_flag() {
  static const std::string _flag("delayed");
  return _flag;
}

bool tracker_cluster::is_delayed() const {
  return get_auxiliaries().has_flag(tracker_cluster::delayed_cluster_flag());
}

bool tracker_cluster::is_prompt() const { return !is_delayed(); }

void tracker_cluster::make_delayed() {
  grab_auxiliaries().update_flag(tracker_cluster::delayed_cluster_flag());
}

void tracker_cluster::make_prompt() {
  grab_auxiliaries().unset_flag(tracker_cluster::delayed_cluster_flag());
}

bool tracker_cluster::has_cluster_id() const { return has_hit_id(); }

int tracker_cluster::get_cluster_id() const { return get_hit_id(); }

void tracker_cluster::set_cluster_id(int32_t cluster_id_) {
  set_hit_id(cluster_id_);
}

void tracker_cluster::invalidate_cluster_id() {
  invalidate_hit_id();
}

calibrated_tracker_hit::collection_type& tracker_cluster::get_hits() { return _hits_; }

const calibrated_tracker_hit::collection_type& tracker_cluster::get_hits() const { return _hits_; }

void tracker_cluster::reset() {
  this->clear();
}

void tracker_cluster::clear() {
  _hits_.clear();
  base_hit::clear();
}

unsigned int tracker_cluster::get_number_of_hits() const { return _hits_.size(); }

const calibrated_tracker_hit& tracker_cluster::get_hit(int i_) const {
  DT_THROW_IF(i_ < 0 || i_ >= (int)_hits_.size(), std::range_error,
              "Invalid clustered hit index (" << i_ << ") !");
  return *(_hits_[i_]);
}

void tracker_cluster::tree_dump(std::ostream& out_, const std::string& title_,
                                const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  base_hit::tree_dump(out_, title_, indent, true);

  out_ << indent << datatools::i_tree_dumpable::tag << "Hit(s) : " << _hits_.size() << std::endl;
  for (int i = 0; i < (int)_hits_.size(); i++) {
    const calibrated_tracker_hit& gg_calib_hit = _hits_.at(i).get();
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    if (i + 1 == (int)_hits_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Hit #" << i << " : Id=" << gg_calib_hit.get_hit_id()
         << " GID=" << gg_calib_hit.get_geom_id() << std::endl;
  }
  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Cluster ID  : " << get_cluster_id() << std::endl;

  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo
