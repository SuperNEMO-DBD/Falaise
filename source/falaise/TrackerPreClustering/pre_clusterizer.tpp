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
  typedef Hit hit_type;
  typedef std::vector<const hit_type *> hit_collection_type;

  // Collection of hit per half-chamber :
  static hit_collection_type prompt_hits[2];
  prompt_hits[0].clear();
  prompt_hits[1].clear();
  static hit_collection_type delayed_hits[2];
  delayed_hits[0].clear();
  delayed_hits[1].clear();

  BOOST_FOREACH (const hit_type *hitref, input_data_.hits) {
    DT_LOG_DEBUG(_logging_, "Hit ID : " << hitref->get_id());
    if (hitref->is_sterile()) {
      DT_LOG_DEBUG(_logging_, "  `-> sterile hit");
      output_data_.ignored_hits.push_back(hitref);
      continue;
    }
    if (hitref->is_noisy()) {
      DT_LOG_DEBUG(_logging_, "  `-> noisy hit");
      output_data_.ignored_hits.push_back(hitref);
      continue;
    }
    if (!hitref->has_xy()) {
      DT_LOG_DEBUG(_logging_, "  `-> no XY hit");
      return ERROR;
    }
    if (!hitref->has_geom_id()) {
      DT_LOG_DEBUG(_logging_, "  `-> no GID hit");
      return ERROR;
    }

    // int module = hitref->get_module();
    int side = hitref->get_side();
    // int layer  = hitref->get_layer();
    // int row    = hitref->get_row();

    if (hitref->is_prompt()) {
      DT_LOG_DEBUG(_logging_, "  `-> prompt hit");
      if (is_processing_prompt_hits()) {
        if (side == 0 || side == 1) {
          DT_LOG_DEBUG(_logging_, "  `-> push with prompt hit(side " << side << ")");
          int effective_side = side;
          if (!is_split_chamber()) {
            effective_side = 0;
          }
          prompt_hits[effective_side].push_back(hitref);
        } else {
          DT_LOG_DEBUG(_logging_, "  `-> ignored prompt hit");
          output_data_.ignored_hits.push_back(hitref);
        }
      } else {
        DT_LOG_DEBUG(_logging_, "  `-> ignored prompt hit");
        output_data_.ignored_hits.push_back(hitref);
      }
      continue;
    }

    if (hitref->is_delayed()) {
      DT_LOG_DEBUG(_logging_, "  `-> delayed hit");
      if (is_processing_delayed_hits()) {
        if (side == 0 || side == 1) {
          DT_LOG_DEBUG(_logging_, "   push with delayed hit(side " << side << ")");
          int effective_side = side;
          if (!is_split_chamber()) {
            effective_side = 0;
          }
          delayed_hits[effective_side].push_back(hitref);
        } else {
          DT_LOG_DEBUG(_logging_, "  `-> ignored delayed hit");
          output_data_.ignored_hits.push_back(hitref);
        }
      } else {
        DT_LOG_DEBUG(_logging_, "  `-> ignored delayed hit");
        output_data_.ignored_hits.push_back(hitref);
      }
      continue;
    }
  }

  if (is_processing_prompt_hits()) {
    DT_LOG_DEBUG(_logging_, "Processing prompt hits...");
    unsigned int max_side = 2;
    if (!is_split_chamber()) {
      max_side = 1;
    }
    // For each side of the tracking chamber, we collect one unique candidate time-cluster of prompt
    // hits.
    for (unsigned int side = 0; side < max_side; side++) {
      DT_LOG_DEBUG(_logging_, (side == max_side - 1 ? "`- " : "|- ") << "Side " << side);
      hit_collection_type *new_prompt_cluster = 0;
      if (prompt_hits[side].size() == 1) {
        DT_LOG_DEBUG(_logging_, "  `-> ignored prompt hit");
        // std::cerr << "DEVEL: TPC: ignored hit = " << prompt_hits[side][0] << std::endl;
        output_data_.ignored_hits.push_back(prompt_hits[side][0]);
        continue;
      }
      for (unsigned int i = 0; i < prompt_hits[side].size(); i++) {
        // Traverse the prompt hits in this side :
        const hit_type *hit_ref = prompt_hits[side].at(i);
        DT_LOG_DEBUG(_logging_, (side == max_side - 1 ? " " : "|")
                                    << (i == prompt_hits[side].size() - 1 ? "   `- " : "   |- ")
                                    << "Hit ID : " << hit_ref->get_id());
        if (new_prompt_cluster == 0) {
          hit_collection_type tmp;
          output_data_.prompt_clusters.push_back(tmp);
          new_prompt_cluster = &output_data_.prompt_clusters.back();
        }
        new_prompt_cluster->push_back(hit_ref);
      }
    }
  }

  if (is_processing_delayed_hits()) {
    DT_LOG_DEBUG(_logging_, "Processing delayed hits...");
    // For each side of the tracking chamber, we try to collect some candidate time-clusters of
    // delayed hits. The aggregation criterion uses a time-interval of width
    // '_delayed_hit_cluster_time_' (~10usec).
    unsigned int max_side = 2;
    if (!is_split_chamber()) {
      max_side = 1;
    }
    for (unsigned int side = 0; side < max_side; side++) {
      DT_LOG_DEBUG(_logging_, "Side " << side);
      // Sort the collection of delayed hits by delayed time :
      compare_tracker_hit_ptr_by_delayed_time<hit_type> cthpbdt;
      std::sort(delayed_hits[side].begin(), delayed_hits[side].end(), cthpbdt);
      DT_LOG_DEBUG(_logging_, "Delayed hits on side " << side << " have been time ordered.");
      if (delayed_hits[side].size() < 2) {
        if (delayed_hits[side].size() == 1) {
          DT_LOG_DEBUG(_logging_, "  -> ignored delayed hit");
          output_data_.ignored_hits.push_back(delayed_hits[side][0]);
        }
        continue;
      }
      // Pick up the first time-orderer delayed hit on this side of the source foil as
      // the start of a forseen cluster :
      const hit_type *hit_ref_1 = delayed_hits[side].at(0);
      hit_collection_type *new_cluster = 0;
      for (unsigned int i = 1; i < delayed_hits[side].size(); i++) {
        DT_LOG_DEBUG(_logging_, "Hit #1 ID : " << hit_ref_1->get_id());
        // Traverse the other delayed hits in this side :
        const hit_type *hit_ref_2 = delayed_hits[side].at(i);
        DT_LOG_DEBUG(_logging_, "Hit #2 ID : " << hit_ref_2->get_id());
        // Check if the delayed time falls in a time window of given width and starting from
        // the time reference given by the 'first' hit :
        if (hit_ref_2->get_delayed_time() >
            (hit_ref_1->get_delayed_time() + _delayed_hit_cluster_time_)) {
          // If the delay between both hits is too large:
          // If no current cluster was initiated, drop the current starting cluster hit :
          if (new_cluster == 0) {
            DT_LOG_DEBUG(_logging_, "  -> ignored hit #1");
            output_data_.ignored_hits.push_back(hit_ref_1);
          }
          // Make the current hit the new start for a cluster :
          hit_ref_1 = hit_ref_2;
          hit_ref_2 = 0;
          new_cluster = 0;
          continue;
        }
        // If the delay between both hits make them good candidate for time clustering,
        // push them all in the current collecting cluster if any, and create it if it has
        // not been done
        if (new_cluster == 0) {
          // New time cluster :
          hit_collection_type hct;
          output_data_.delayed_clusters.push_back(hct);
          new_cluster = &(output_data_.delayed_clusters.back());
          // Record the first hit of this new time-cluster :
          new_cluster->push_back(hit_ref_1);
          DT_LOG_DEBUG(_logging_, "  -> start a new delayed cluster");
        }
        // Record the current hit in the current time-cluster :
        DT_LOG_DEBUG(_logging_, "  -> add the hit #2 in the current delayed cluster");
        new_cluster->push_back(hit_ref_2);
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
