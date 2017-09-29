/// \file falaise/TrackerPreClustering/pre_clusterizer.h
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

#ifndef FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_H
#define FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_H 1

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>

// This project:
#include "falaise/TrackerPreClustering/interface.h"

namespace TrackerPreClustering {

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
 *  The TrackerPreClustering::pre_clusterizer object is configured through a
 * TrackerPreClustering::setup_data object.
 *
 *  The input of the algorithm is implemented through the template
 * TrackerPreClustering::input_data<> class which is basically a collection of Geiger hits.
 *
 *  The output of the algorithm is implemented through the template
 * TrackerPreClustering::output_data<> class which stores collections of prompt and delayed
 * clusters. By default the tracking chamber is considered as splitted in two parts.
 *
 */
class pre_clusterizer {
 public:
  static const int OK;
  static const int ERROR;

  /// Check the lock flag
  bool is_locked() const;

  /// Return logging threshold
  datatools::logger::priority get_logging_priority() const;

  /// Set logging threshold
  void set_logging_priority(datatools::logger::priority logging_);

  /// Default constructor
  pre_clusterizer();

  /// Destrctuor
  virtual ~pre_clusterizer();

  /// Configure and initialize the algorithm
  int initialize(const setup_data& setup_);

  /// Reset
  void reset();

  /// Process the list of hits
  /// A 'Hit' class must be provided with the proper interface (see the TrackerPreClustering::gg_hit
  /// mock data model)
  template <typename Hit>
  int process(const input_data<Hit>& input_, output_data<Hit>& output_);

  /// Return the cell size
  double get_cell_size() const;

  /// Set the cell size
  void set_cell_size(double cell_size_);

  /// Set the delayed hit cluster time
  void set_delayed_hit_cluster_time(double);

  /// Return the delayed hit cluster time
  double get_delayed_hit_cluster_time() const;

  /// Check if prompt hits are processed
  bool is_processing_prompt_hits() const;

  /// Set the flag for prompt hits processing
  void set_processing_prompt_hits(bool);

  /// Check if delayed hits are processed
  bool is_processing_delayed_hits() const;

  /// Set the flag for delayed hits processing
  void set_processing_delayed_hits(bool);

  /// Check the flag to split the tracking chamber
  bool is_split_chamber() const;

  /// Set the flag to split the tracking chamber
  void set_split_chamber(bool);

 protected:
  /// Set defualt attribute values
  void _set_defaults();

 private:
  bool _locked_;                          /// Lock flag
  datatools::logger::priority _logging_;  /// Logging flag
  double _cell_size_;                     /// Size of a cell (embedded length units)
  double _delayed_hit_cluster_time_;      /// Delayed hit cluster time window (embedded time units)
  bool _processing_prompt_hits_;          /// Activation of the processing of prompt hits
  bool _processing_delayed_hits_;         /// Activation of the processing of delayed hits
  bool _split_chamber_;  /// Split the chamber in two half-chambers to classify the hits and
                         /// time-clusters
};

/// A functor for handle on tracker hits that perform a comparison by delayed time
template <class Hit>
struct compare_tracker_hit_ptr_by_delayed_time {
 public:
  /// Main comparison method(less than) : require non null handles and non-Nan delayed times
  bool operator()(const Hit* hit_ptr_i_, const Hit* hit_ptr_j_) const {
    if (!hit_ptr_i_->has_delayed_time()) return false;
    if (!hit_ptr_j_->has_delayed_time()) return false;
    return hit_ptr_i_->get_delayed_time() < hit_ptr_j_->get_delayed_time();
  }
};

}  // end of namespace TrackerPreClustering

#include "falaise/TrackerPreClustering/pre_clusterizer.tpp"

#endif  // FALAISE_TRACKERPRECLUSTERING_PRE_CLUSTERIZER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
