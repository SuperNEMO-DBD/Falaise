/// \file falaise/snemo/datamodels/tracker_trajectory.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: A trajectory fitted from a cluster of tracker hits
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace snemo {

  namespace datamodel {


    /// A trajectory of Geiger calibrated hits referenced by handles
    class tracker_trajectory : public geomtools::base_hit
    {
    public:

      /// Handle on tracker trajectory
      typedef datatools::handle<tracker_trajectory> handle_type;

      /// Handle on tracker cluster
      typedef tracker_cluster::handle_type handle_cluster;

      /// Collection of handles on tracker clusters
      typedef std::vector<handle_cluster> clusters_collection_type;

      /// Handle on trajectory pattern
      typedef datatools::handle<base_trajectory_pattern> handle_pattern;

      /// Collection of handles of calibrated tracker hit
      typedef calibrated_tracker_hit::collection_type orphans_collection_type;

      /// Default constructor
      tracker_trajectory();

      /// Destructor
      virtual ~tracker_trajectory();

      /// Check if there is a valid trajectory ID
      bool has_trajectory_id() const;

      /// Get the trajectory ID
      int get_trajectory_id() const;

      /// Set the trajectory ID
      void set_trajectory_id(int32_t);

      /// Invalidate the trajectory ID
      void invalidate_trajectory_id();

      /// Check if the cluster is present
      bool has_cluster() const;

      /// Detach the cluster
      void detach_cluster();

      /// Attach a cluster by handle
      void set_cluster_handle(const handle_cluster & cluster_handle_);

      /// Return a mutable reference on the cluster handle
      handle_cluster & grab_cluster_handle();

      /// Return a non mutable reference on the cluster handle
      const handle_cluster & get_cluster_handle() const;

      /// Return a mutable reference on the cluster
      tracker_cluster & grab_cluster();

      /// Return a non mutable reference on the cluster
      const tracker_cluster & get_cluster() const;

      /// Check if there are some orphan hits along the fitted trajectory
      bool has_orphans() const;

      /// Reset the collection of orphan hits
      void reset_orphans();

      /// Return a mutable reference on the collection of orphan hits (handles)
      orphans_collection_type & grab_orphans();

      /// Return a non mutable reference on the collection of orphan hits (handles)
      const orphans_collection_type & get_orphans() const;

      /// Check if the pattern is present
      bool has_pattern() const;

      /// Detach the pattern
      void detach_pattern();

      /// Attach a pattern by handle
      void set_pattern_handle(const handle_pattern & pattern_handle_);

      /// Return a mutable reference on the pattern handle
      handle_pattern & grab_pattern_handle();

      /// Return a non mutable reference on the pattern handle
      const handle_pattern & get_pattern_handle() const;

      /// Return a mutable reference on the pattern
      base_trajectory_pattern & grab_pattern();

      /// Return a non mutable reference on the pattern
      const base_trajectory_pattern & get_pattern() const;

      /// Reset the tracker trajectory (see clear)
      void reset();

      /// Empty the contents of the tracker trajectory
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out     = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:

      handle_cluster          _cluster_; /// Handle to the fitted cluster
      orphans_collection_type _orphans_; /// Collection of orphan Geiger hit handles
      handle_pattern          _pattern_; /// Handle to a trajectory fitted pattern

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
