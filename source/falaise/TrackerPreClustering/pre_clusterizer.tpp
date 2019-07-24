/// \file falaise/TrackerPreClustering/pre_clusterizer.tpp

#ifndef FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_TPP
#define FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_TPP 1

// Standard library:
#include <algorithm>
#include <iostream>

// Third party:
// - Boost:
#include <boost/foreach.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

namespace TrackerPreClustering {

template <typename Hit>
int pre_clusterizer::process(const input_data<Hit> &input_data_, output_data<Hit> &output_data_) {
  using hit_type = Hit;
  using hit_collection_type =  std::vector<const hit_type*>;

  // Collection of hit per half-chamber :
  static hit_collection_type prompt_hits[2];
  prompt_hits[0].clear();
  prompt_hits[1].clear();
  static hit_collection_type delayed_hits[2];
  delayed_hits[0].clear();
  delayed_hits[1].clear();

  for (const hit_type* hitref : input_data_.hits) {
    if (hitref->is_sterile() || hitref->is_noisy()) {
      output_data_.ignored_hits.push_back(hitref);
      continue;
    }
    if (!hitref->has_xy() || !hitref->has_geom_id()) {
      return ERROR;
    }

    int side = hitref->get_side();

    if (hitref->is_prompt()) {
      if (is_processing_prompt_hits()) {
        if (side == 0 || side == 1) {
          int effective_side = is_split_chamber() ? side : 0;
          prompt_hits[effective_side].push_back(hitref);
        } else {
          output_data_.ignored_hits.push_back(hitref);
        }
      } else {
        output_data_.ignored_hits.push_back(hitref);
      }
      continue;
    }

    if (hitref->is_delayed()) {
      if (is_processing_delayed_hits()) {
        if (side == 0 || side == 1) {
          int effective_side = is_split_chamber() ? side : 0;
          delayed_hits[effective_side].push_back(hitref);
        } else {
          output_data_.ignored_hits.push_back(hitref);
        }
      } else {
        output_data_.ignored_hits.push_back(hitref);
      }
      continue;
    }
  }

  if (is_processing_prompt_hits()) {
    unsigned int max_side = is_split_chamber() ? 2 : 1;

    // For each side of the tracking chamber, we collect one unique candidate time-cluster of prompt
    // hits.
    for (unsigned int side = 0; side < max_side; side++) {
      if (prompt_hits[side].size() == 1) {
        output_data_.ignored_hits.push_back(prompt_hits[side][0]);
      }
      else if(prompt_hits[side].size() > 1) {
        hit_collection_type new_prompt_cluster(prompt_hits[side]);
        output_data_.prompt_clusters.push_back(new_prompt_cluster);
      }
    }
  }

  if (is_processing_delayed_hits()) {
    // For each side of the tracking chamber, we try to collect some candidate time-clusters of
    // delayed hits. The aggregation criterion uses a time-interval of width
    // '_delayed_hit_cluster_time_' (~10usec).
    unsigned int max_side = is_split_chamber() ? 2 : 1;

    for (unsigned int side = 0; side < max_side; side++) {
      if (delayed_hits[side].empty()) {
        continue;
      }

      // Single hits are not clustered
      if (delayed_hits[side].size() == 1) {
        output_data_.ignored_hits.push_back(delayed_hits[side][0]);
        continue;
      }

      // Otherwise, sort and window cluster
      compare_tracker_hit_ptr_by_delayed_time<hit_type> cthpbdt;
      std::sort(delayed_hits[side].begin(), delayed_hits[side].end(), cthpbdt);

     // Pick up the first time-orderer delayed hit on this side of the source foil as
      // the start of a forseen cluster :
      const hit_type* startHit = delayed_hits[side].at(0);
      hit_collection_type* currentCluster = nullptr;

      // Traverse remaining delayed hits from this side :
      for (size_t i = 1; i < delayed_hits[side].size(); i++) {
        const hit_type* currentHit = delayed_hits[side].at(i);
        // Time of current hit must be within the window of the current
        // cluster's start time
        bool isOutsideWindow = (currentHit->get_delayed_time() > (startHit->get_delayed_time() + _delayed_hit_cluster_time_));

        if (isOutsideWindow) {
          // With no existing cluster, the earlier hit is isolated
          if (currentCluster == nullptr) {
            output_data_.ignored_hits.push_back(startHit);
          }
          // Make the current hit the new start point and require a new cluster:
          startHit = currentHit;
          currentHit = nullptr;
          currentCluster = nullptr;
        } else {
          // cluster the two hits, creating a new cluster if needed
          if (currentCluster == nullptr) {
            // New time cluster
            output_data_.delayed_clusters.push_back(hit_collection_type{});
            currentCluster = &(output_data_.delayed_clusters.back());
            currentCluster->push_back(startHit);
          }
          // Record the current hit in the current time-cluster :
          currentCluster->push_back(currentHit);
        }
      }
    }
  }

  return pre_clusterizer::OK;
}

}  // end of namespace TrackerPreClustering

#endif  // FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_TPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
