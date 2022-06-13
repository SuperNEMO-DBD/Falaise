/// \file falaise/snemo/processing/detail/GeigerTimePartitioner.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-30
 * Last modified: 2014-02-07
 *
 * Copyright 2012-2014 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *  Tracker Pre-Clustering algorithm.
 *
 * History:
 *
 *   This set of classes have been ported from the Channel metapackage.
 */

#ifndef FALAISE_SNEMO_PROCESSING_DETAIL_GEIGERTIMEPARTITIONER_H
#define FALAISE_SNEMO_PROCESSING_DETAIL_GEIGERTIMEPARTITIONER_H

#include <algorithm>
#include <vector>

// Third party:
#include <CLHEP/Units/SystemOfUnits.h>

namespace snreco {

namespace detail {

/// \brief A pre-clusterizer of Geiger hits for the SuperNEMO detector
/** This algorithm aims to group the Geiger hits in a given SuperNEMO event record
 *  using some simple clustering criteria:
 *  - prompt hits are grouped within a prompt pre-cluster if they lie on the same side of
 *    the source foil,
 *  - delayed hits are grouped within a delayed pre-cluster if they lie on the same side of
 *    the source foil and are in a 10 us time coincidence window.
 *  Only two prompt clusters can exists (maximum: one per side of the source foil).
 *  There is no limitation on the number of delayed clusters.
 *
 *  The input of the algorithm is implemented through the template
 * TrackerPreClustering::input_data<> class which is basically a collection of Geiger hits.
 *
 *  The output of the algorithm is implemented through the template
 * TrackerPreClustering::output_data<> class which stores collections of prompt and delayed
 * clusters. By default the tracking chamber is considered as splitted in two parts.
 *
 */

template <typename T>
using GeigerHitPtrCollection = std::vector<const T*>;

template <typename T>
struct GeigerHitTimePartition {
  std::vector<const T*> ignoredHits{};
  std::vector<std::vector<const T*>> promptClusters{};
  std::vector<std::vector<const T*>> delayedClusters{};
};

class GeigerTimePartitioner {
 public:
  /// Default constructor
  GeigerTimePartitioner() = default;

  // Construct with settings
  GeigerTimePartitioner(double timeGap, bool classifyPrompt, bool classifyDelayed,
                        bool splitChamber);

  /// Destrutor
  ~GeigerTimePartitioner() = default;

  // defaults to satisfy Ro5
  GeigerTimePartitioner(const GeigerTimePartitioner&) = default;
  GeigerTimePartitioner& operator=(const GeigerTimePartitioner&) = default;
  GeigerTimePartitioner(GeigerTimePartitioner&&) = default;
  GeigerTimePartitioner& operator=(GeigerTimePartitioner&&) = default;

  /// Process the list of hits
  /// A 'Hit' class must be provided with the proper interface (see the TrackerPreClustering::gg_hit
  /// mock data model)
  template <typename Hit>
  GeigerHitTimePartition<Hit> partition(const GeigerHitPtrCollection<Hit>& in);

  /// Return the delayed hit cluster time
  double getMaxDelayedHitTimeGap() const;

  /// Check if prompt hits are processed
  bool classifiesPromptHits() const;

  /// Check if delayed hits are processed
  bool classifiesDelayedHits() const;

  /// Check the flag to split the tracking chamber
  bool isSplitChamber() const;

 private:
  double maxDelayedHitTimeGap_ =
      10.0 * CLHEP::microsecond;     ///< Delayed hit cluster time window (embedded time units)
  bool classifyPromptHits_ = true;   ///< Activation of the processing of prompt hits
  bool classifyDelayedHits_ = true;  ///< Activation of the processing of delayed hits
  /// Split the chamber in two half-chambers to classify the hits and time-clusters
  bool isSplitChamber_ = true; 
};

/// A functor for handle on tracker hits that perform a comparison by delayed time
template <class Hit>
struct DelayTimeComparator {
 public:
  /// Main comparison method(less than) : require non null handles and non-Nan delayed times
  bool operator()(const Hit* lhs, const Hit* rhs) const {
    if (!lhs->has_delayed_time() || !rhs->has_delayed_time()) {
      return false;
    }
    return lhs->get_delayed_time() < rhs->get_delayed_time();
  }
};

template <typename Hit>
GeigerHitTimePartition<Hit> GeigerTimePartitioner::partition(
    const GeigerHitPtrCollection<Hit>& input) {
  using HitCollection = GeigerHitPtrCollection<Hit>;

  // Collection of hit per half-chamber :
  HitCollection prompt_hits[2];
  HitCollection delayed_hits[2];

  GeigerHitTimePartition<Hit> oput;

  // Partition input into ignored (bad data), prompt, and delayed sets
  // Most of the complexity here comes from the optional partition by side...
  for (const Hit* hitref : input) {
    if (hitref == nullptr) {
      continue;
    }
    if (hitref->is_sterile() || hitref->is_noisy()) {
      oput.ignoredHits.push_back(hitref);
      continue;
    }

    int side = hitref->get_side();

    if (hitref->is_prompt()) {
      if (classifiesPromptHits()) {
        if (side == 0 || side == 1) {
          int effective_side = isSplitChamber() ? side : 0;
          prompt_hits[effective_side].push_back(hitref);
        } else {
          oput.ignoredHits.push_back(hitref);
        }
      } else {
        oput.ignoredHits.push_back(hitref);
      }
      continue;
    }

    if (hitref->is_delayed()) {
      if (classifiesDelayedHits()) {
        if (side == 0 || side == 1) {
          int effective_side = isSplitChamber() ? side : 0;
          delayed_hits[effective_side].push_back(hitref);
        } else {
          oput.ignoredHits.push_back(hitref);
        }
      } else {
        oput.ignoredHits.push_back(hitref);
      }
      continue;
    }
  }

  if (classifiesPromptHits()) {
    unsigned int max_side = isSplitChamber() ? 2 : 1;

    // For each side of the tracking chamber, we collect one unique candidate time-cluster of prompt
    // hits.
    for (unsigned int side = 0; side < max_side; side++) {
      if (prompt_hits[side].size() == 1) {
        oput.ignoredHits.push_back(prompt_hits[side][0]);
      } else if (prompt_hits[side].size() > 1) {
        HitCollection new_prompt_cluster(prompt_hits[side]);
        oput.promptClusters.push_back(new_prompt_cluster);
      }
    }
  }

  if (classifiesDelayedHits()) {
    // For each side of the tracking chamber, we try to collect some candidate time-clusters of
    // delayed hits. The aggregation criterion uses a time-interval of width
    // 'maxDelayedHitTimeGap_' (~10usec).
    unsigned int max_side = isSplitChamber() ? 2 : 1;

    for (unsigned int side = 0; side < max_side; side++) {
      if (delayed_hits[side].empty()) {
        continue;
      }

      // Single hits are not clustered
      if (delayed_hits[side].size() == 1) {
        oput.ignoredHits.push_back(delayed_hits[side][0]);
        continue;
      }

      // Otherwise, sort in order of delay time and window cluster
      std::sort(delayed_hits[side].begin(), delayed_hits[side].end(), DelayTimeComparator<Hit>{});

      // Pick up the first time-orderer delayed hit on this side of the source foil as
      // the start of a forseen cluster :
      const Hit* startHit = delayed_hits[side].at(0);
      HitCollection* currentCluster = nullptr;

      // Traverse remaining delayed hits from this side :
      for (size_t i = 1; i < delayed_hits[side].size(); i++) {
        const Hit* currentHit = delayed_hits[side].at(i);
        // Time of current hit must be within the window of the current
        // cluster's start time
        bool isOutsideWindow = (currentHit->get_delayed_time() >
                                (startHit->get_delayed_time() + maxDelayedHitTimeGap_));

        if (isOutsideWindow) {
          // With no existing cluster, the earlier hit is isolated
          if (currentCluster == nullptr) {
            oput.ignoredHits.push_back(startHit);
          }
          // Make the current hit the new start point and require a new cluster:
          startHit = currentHit;
          currentHit = nullptr;
          currentCluster = nullptr;
        } else {
          // cluster the two hits, creating a new cluster if needed
          if (currentCluster == nullptr) {
            // New time cluster
            oput.delayedClusters.push_back(HitCollection{});
            currentCluster = &(oput.delayedClusters.back());
            currentCluster->push_back(startHit);
          }
          // Record the current hit in the current time-cluster :
          currentCluster->push_back(currentHit);
        }
      }
    }
  }

  return oput;
}

}  // namespace detail

}  // namespace snreco

#endif  // FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
