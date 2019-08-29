// falaise/snemo/datamodels/tracker_clustering_solution.cc

// Standard library:
#include <set>

// Ourselves:
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

namespace datamodel {

bool tracker_clustering_solution::has_solution_id() const { return _solution_id_ >= 0; }

int tracker_clustering_solution::get_solution_id() const { return _solution_id_; }

void tracker_clustering_solution::set_solution_id(int32_t solution_id_) {
  if (solution_id_ >= 0) {
    _solution_id_ = solution_id_;
  } else {
    invalidate_solution_id();
  }
}

void tracker_clustering_solution::invalidate_solution_id() { _solution_id_ = -1; }

datatools::properties &tracker_clustering_solution::get_auxiliaries() { return _auxiliaries_; }

const datatools::properties &tracker_clustering_solution::get_auxiliaries() const {
  return _auxiliaries_;
}

bool tracker_clustering_solution::has_unclustered_hits() const {
  return !_unclustered_hits_.empty();
}

TrackerHitHdlCollection &tracker_clustering_solution::get_unclustered_hits() {
  return _unclustered_hits_;
}

const TrackerHitHdlCollection &tracker_clustering_solution::get_unclustered_hits() const {
  return _unclustered_hits_;
}

TrackerClusterHdlCollection &tracker_clustering_solution::get_clusters() { return _clusters_; }

const TrackerClusterHdlCollection &tracker_clustering_solution::get_clusters() const {
  return _clusters_;
}

void tracker_clustering_solution::reset() { this->clear(); }

void tracker_clustering_solution::clear() {
  reset_hit_belonging();
  _clusters_.clear();
  _unclustered_hits_.clear();
  invalidate_solution_id();
  _auxiliaries_.clear();
}

bool tracker_clustering_solution::hit_belongs_to_cluster(const calibrated_tracker_hit &hit_,
                                                         const tracker_cluster &cluster_) const {
  return hit_belongs_to_cluster(hit_.get_hit_id(), cluster_.get_cluster_id());
}

bool tracker_clustering_solution::hit_is_clustered(const calibrated_tracker_hit &hit_) const {
  return hit_is_clustered(hit_.get_hit_id());
}

bool tracker_clustering_solution::hit_is_clustered(int32_t hit_id_) const {
  if (!_hit_belonging_.empty()) {
    auto found = _hit_belonging_.find(hit_id_);
    return found != _hit_belonging_.end() && !found->second.empty();
  }

  for (const auto &ihit : _unclustered_hits_) {
    if (!ihit.has_data()) {
      continue;
    }
    if (ihit->get_hit_id() == hit_id_) {
      return false;
    }
  }
  return true;
}

bool tracker_clustering_solution::hit_belongs_to_several_clusters(
    const calibrated_tracker_hit &hit_) const {
  return hit_belongs_to_several_clusters(hit_.get_hit_id());
}

bool tracker_clustering_solution::hit_belongs_to_several_clusters(int32_t hit_id_) const {
  // If available, use the' hit_belonging' collection :
  if (!_hit_belonging_.empty()) {
    auto found = _hit_belonging_.find(hit_id_);
    return found != _hit_belonging_.end() && found->second.size() > 1;
  }

  // Shortcut :
  if (_clusters_.size() < 2) {
    return false;
  }

  // Traverse all clusters to count belongings for this hit
  // TODO: Review use of has_data, since a cluster by definition should have valid data
  int cluster_counter = 0;
  for (const auto &the_cluster : _clusters_) {
    if (!the_cluster.has_data()) {
      continue;
    }

    for (const auto &the_hit : the_cluster->get_hits()) {
      if (!the_hit.has_data()) {
        continue;
      }

      if (the_hit->get_hit_id() == hit_id_) {
        cluster_counter++;
        break;
      }
    }
  }
  return cluster_counter > 1;
}

bool tracker_clustering_solution::hit_belongs_to_cluster(int32_t hit_id_,
                                                         int32_t cluster_id_) const {
  // If available, use the' hit_belonging' collection :
  if (!_hit_belonging_.empty()) {
    auto found_iter = _hit_belonging_.find(hit_id_);
    if (found_iter == _hit_belonging_.end()) {
      return false;
    }

    for (const auto &the_cluster : found_iter->second) {
      if (the_cluster.has_data()) {
        if (the_cluster->get_cluster_id() == cluster_id_) {
          return true;
        }
      }
    }
  } else {
    // Shortcut :
    if (_clusters_.empty()) {
      return false;
    }
    // Traverse all clusters to detect belongings for this hit :
    for (const auto &the_cluster : _clusters_) {
      if (the_cluster.has_data()) {
        if (the_cluster->get_cluster_id() != cluster_id_) {
          continue;
        }
      }
      // Traverse all hits in the current cluster :
      for (const auto &the_hit : the_cluster->get_hits()) {
        if (the_hit.has_data()) {
          if (the_hit->get_hit_id() == hit_id_) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

const tracker_clustering_solution::hit_belonging_col_type &
tracker_clustering_solution::get_hit_belonging() const {
  return _hit_belonging_;
}

void tracker_clustering_solution::reset_hit_belonging() { _hit_belonging_.clear(); }

bool tracker_clustering_solution::has_hit_belonging() const { return !_hit_belonging_.empty(); }

// static
void tracker_clustering_solution::compute_hit_belonging_from_solution(
    const tracker_clustering_solution &tcs_, hit_belonging_col_type &hbc_) {
  hbc_.clear();

  for (const auto &the_cluster : tcs_.get_clusters()) {
    if (!the_cluster.has_data()) {
      continue;
    }

    for (const auto &the_hit : the_cluster->get_hits()) {
      if (!the_hit.has_data()) {
        continue;
      }

      int32_t hit_id = the_hit->get_hit_id();

      if (hbc_.find(hit_id) == hbc_.end()) {
        {
          TrackerClusterHdlCollection dummy_col;
          hbc_[hit_id] = dummy_col;
        }
        hbc_[hit_id].push_back(the_cluster);
      }
    }
  }
}

void tracker_clustering_solution::compute_hit_belonging() {
  compute_hit_belonging_from_solution(*this, _hit_belonging_);
}

// static
int tracker_clustering_solution::copy_one_solution_in_one(
    const tracker_clustering_solution &source_, tracker_clustering_solution &target_) {
  // Preallocate the total number of clusters/hits from both solutions:
  auto &src_clusters = source_.get_clusters();
  auto &src_hits = source_.get_unclustered_hits();

  auto &tgt_clusters = target_.get_clusters();
  tgt_clusters.reserve(tgt_clusters.size() + src_clusters.size());

  auto &tgt_hits = target_.get_unclustered_hits();
  tgt_hits.reserve(tgt_hits.size() + src_hits.size());

  // Search for the maximum cluster Id from the target:
  int max_cluster_id = -1;

  if (!tgt_clusters.empty()) {
    auto id_comp = [](const TrackerClusterHdl &a, const TrackerClusterHdl &b) {
      return a->get_cluster_id() < b->get_cluster_id();
    };

    auto max_cluster_id_iter = std::max_element(tgt_clusters.begin(), tgt_clusters.end(), id_comp);

    max_cluster_id = (*max_cluster_id_iter)->get_cluster_id();
  }

  // copy clusters from the solution into the target
  for (size_t icluster_source = 0; icluster_source < src_clusters.size(); icluster_source++) {
    // Pickup cluster from the solution:
    const auto &a_cluster_hdl = src_clusters.at(icluster_source);
    // Create a new cluster from the old::
    auto hcl = datatools::make_handle<tracker_cluster>(*a_cluster_hdl);
    // But give it an unique Id:
    hcl->set_cluster_id(max_cluster_id + icluster_source + 1);
    tgt_clusters.push_back(hcl);
  }

  // TODO: Possible bug. as does not touch solution input parameter...
  // Extract unclustered hits from the solution: Huh? copies target to target?
  for (int iunclustered_hit = 0; iunclustered_hit < (int)target_.get_unclustered_hits().size();
       iunclustered_hit++) {
    target_.get_unclustered_hits().push_back(target_.get_unclustered_hits().at(iunclustered_hit));
  }

  return 0;
}

// A mess, so not touching for now...
// static
int tracker_clustering_solution::merge_two_solutions_in_ones(
    const tracker_clustering_solution &source0_, const tracker_clustering_solution &source1_,
    tracker_clustering_solution &target_) {
  const tracker_clustering_solution &sol0 = source0_;
  const tracker_clustering_solution &sol1 = source1_;
  // Preallocate the total number of clusters from both solutions:
  target_.get_clusters().reserve(target_.get_clusters().size() + sol0.get_clusters().size() +
                                 sol1.get_clusters().size());
  // Preallocate the total number of unclustered hits from both solutions:
  target_.get_unclustered_hits().reserve(target_.get_unclustered_hits().size() +
                                         sol0.get_unclustered_hits().size() +
                                         sol1.get_unclustered_hits().size());
  // Search for the maximum cluster Id from the target:
  int max_cluster_id = -1;
  for (auto &icluster_target : target_.get_clusters()) {
    const tracker_cluster &a_cluster = icluster_target.get();
    int cluster_id = a_cluster.get_cluster_id();
    if (cluster_id > max_cluster_id) {
      max_cluster_id = cluster_id;
    }
  }
  static const unsigned int NSOURCES = 2;
  std::set<int> check_hits[NSOURCES];
  for (int source = 0; source < (int)NSOURCES; source++) {
    const tracker_clustering_solution *psol = nullptr;
    if (source == 0) {
      psol = &sol0;
    } else {
      psol = &sol1;
    }
    const tracker_clustering_solution &rsol = *psol;
    // Extract clusters from the solution:
    for (int icluster_source = 0; icluster_source < (int)rsol.get_clusters().size();
         icluster_source++) {
      // Pickup a cluster from the solution:
      const tracker_cluster &a_cluster = rsol.get_clusters().at(icluster_source).get();
      // Create a new cluster:
      auto hcl = datatools::make_handle<tracker_cluster>();
      tracker_cluster &cl = hcl.grab();
      // Copy the original cluster into the new one:
      cl = a_cluster;
      // But give it an unique Id:
      cl.set_cluster_id(max_cluster_id + icluster_source + 1);  // target_.grab_clusters().size());
      // Record the hit Ids in the check set for this source:
      for (auto &iclustered_hit : cl.get_hits()) {
        int hit_id = iclustered_hit.get().get_hit_id();
        check_hits[source].insert(hit_id);
      }
      // Store this cluster in the solution:
      target_.get_clusters().push_back(hcl);
    }
    // Extract unclustered hits from the solution:
    for (const auto &iunclustered_hit : rsol.get_unclustered_hits()) {
      target_.get_unclustered_hits().push_back(iunclustered_hit);
      int uhit_id = iunclustered_hit.get().get_hit_id();
      check_hits[source].insert(uhit_id);
    }
  }

  for (auto ihit_id = check_hits[0].begin(); ihit_id != check_hits[0].end(); ihit_id++) {
    int hit_id = *ihit_id;
    if (check_hits[1].count(hit_id) == 1) {
      DT_THROW_IF(true, std::logic_error,
                  "Tracker hit with Id " << hit_id
                                         << " in first solution is already taken into account by "
                                            "the second clustering solution !"
                                         << "Both source clustering solutions should be related to "
                                            "independant sets of tracker hits !");
    }
  }
  return 0;
}

void tracker_clustering_solution::tree_dump(std::ostream &out_, const std::string &title_,
                                            const std::string &indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Solution ID  : " << _solution_id_
       << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Cluster(s)   : " << get_clusters().size()
       << std::endl;
  for (size_t i = 0; i < get_clusters().size(); i++) {
    const tracker_cluster &tc = get_clusters()[i].get();
    std::ostringstream indent2;
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (i == get_clusters().size() - 1) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "Cluster #" << i << " : " << std::endl;
    tc.tree_dump(out_, "", indent2.str());
  }

  {
    int hit_index = 0;
    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Unclustered hit(s) : " << _unclustered_hits_.size() << std::endl;
    for (auto i = _unclustered_hits_.begin(); i != _unclustered_hits_.end(); i++) {
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
      auto j = i;
      j++;
      if (j == _unclustered_hits_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << "Hit #" << hit_index << " : Id=" << i->get().get_hit_id()
           << " GID=" << i->get().get_geom_id();
      out_ << std::endl;
      hit_index++;
    }
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Hits belonging : " << _hit_belonging_.size() << std::endl;

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
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_solution,
                                                  "snemo::datamodel::tracker_clustering_solution")

}  // end of namespace datamodel

}  // end of namespace snemo
