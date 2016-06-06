/// \file falaise/snemo/datamodels/tracker_clustering_solution.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-05
 * Last modified: 2014-01-27
 *
 * Description: A possible solution for the clustering of Geiger calibrated hits
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_SOLUTION_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_SOLUTION_H 1

// Standard library:
#include <vector>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace snemo {

  namespace datamodel {

    /// \brief A collection of tracker clusters
    /**
     * A tracker clustering solution stores the possible result computed by a
     * clustering algorithm from an original collection of tracker calibrated hits.
     *
     * Original collection of Geiger hits (handled by a 'calibrated_data' object):
     *
     * +------------------------+
     * |                   o    |
     * | oo              oo     |
     * |   ooo      ooooo       |
     * |     ooooooo            |
     * |          o      o      |
     * |          oo        o   |
     * |           oo           |
     * |             o          |
     * +------------------------+
     *
     * A possible collection of clusters built from the source:
     *
     * +------------------------+
     * |                   1    |  Legend: 1 : hits belonging to cluster #1
     * | 11              11     |          0 : hits belonging to cluster #2
     * |   111      11111       |          U : unclustered hits
     * |     1111111            |
     * |          0      U      |
     * |          00        U   |
     * |           00           |
     * |             0          |
     * +------------------------+
     *
     * Another possible collection of clusters built from the source:
     *
     * +------------------------+
     * |                   0    |  Legend: 0 : hits belonging to cluster #0
     * | 11              00     |          1 : hits belonging to cluster #1
     * |   111      00000       |          U : unclustered hits
     * |     1111100            |
     * |          1      U      |
     * |          11        U   |
     * |           11           |
     * |             1          |
     * +------------------------+
     *
     * Yet another possible collection of clusters built from the source:
     *
     * +------------------------+
     * |                   0    |  Legend: 0 : hits belonging to cluster #0
     * | 11              00     |          1 : hits belonging to cluster #1
     * |   111      00000       |          2 : hits belonging to cluster #2
     * |     1111100            |          U : unclustered hits
     * |          2      U      |
     * |          22        U   |
     * |           22           |
     * |             2          |
     * +------------------------+
     *
     */
    class tracker_clustering_solution : public datatools::i_serializable
    {

    public:

      /// Handle on tracker cluster
      typedef tracker_cluster::handle_type cluster_handle_type;

      /// Collection of handles on tracker clusters
      typedef std::vector<cluster_handle_type> cluster_col_type;

      /// Handle on tracker cluster solution
      typedef datatools::handle<tracker_clustering_solution> handle_type;

      /// Collection of calibrated hit handles
      typedef calibrated_tracker_hit::collection_type hit_collection_type;

      /// Dictionary of hit/cluster belonging
      typedef std::map<int32_t, cluster_col_type> hit_belonging_col_type;

      /// Default constructor
      tracker_clustering_solution();

      /// Default constructor
      virtual ~tracker_clustering_solution();

      /// Check if there is a valid solution ID
      bool has_solution_id() const;

      /// Get the solution ID
      int get_solution_id() const;

      /// Set the solution ID
      void set_solution_id(int32_t);

      /// Invalidate the solution ID
      void invalidate_solution_id();

      /// Return a mutable reference on the container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Return a non mutable reference on the container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return a mutable reference on the container of clusters
      cluster_col_type & grab_clusters();

      /// Return a non mutable reference on the container of clusters
      const cluster_col_type & get_clusters() const;

      // /// Return a mutable reference on the container of delayed clusters
      // cluster_col_type & grab_delayed_clusters() const;

      // /// Return a non mutable reference on the container of delayed clusters
      // const cluster_col_type & get_delayed_clusters() const;

      /// Check if there is some unclustered hits
      bool has_unclustered_hits() const;

      /// Return a mutable reference on the container of handles on unclustered calibrated tracker hits
      hit_collection_type & grab_unclustered_hits();

      /// Return a non mutable reference on the container of handles on unclustered calibrated tracker hits
      const hit_collection_type & get_unclustered_hits() const;

      /// Empty the contents of the tracker cluster solution
      void clear();

      /// Reset the tracker cluster solution(see clear)
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

      /// Compute hit belonging
      void compute_hit_belonging();

      /// Test if hit with given ID belongs to a cluster with a given ID
      bool hit_belongs_to_cluster(int32_t hit_id, int32_t cluster_id_) const;

      /// Test if hit belongs to a cluster
      bool hit_belongs_to_cluster(const calibrated_tracker_hit & hit_,
                                  const tracker_cluster & cluster_) const;

      /// Test if a given hit is clusterized
      bool hit_is_clustered(const calibrated_tracker_hit & hit_) const;

      /// Test if a hit with given ID is clusterized
      bool hit_is_clustered(int32_t hit_id_) const;

      /// Test if a given hit belongs to several clusters
      bool hit_belongs_to_several_clusters(const calibrated_tracker_hit & hit_) const;

      /// Test if a given hit with given ID belongs to several clusters
      bool hit_belongs_to_several_clusters(int32_t hit_id_) const;

      /// Returns the hit belonging informations
      const hit_belonging_col_type & get_hit_belonging() const;

      /// Clear the hit belonging informations
      void reset_hit_belonging();

      /// Test if some hit belonging information is available
      bool has_hit_belonging() const;

      /// Compute the hit belonging informations from a given clustering solution
      static void compute_hit_belonging_from_solution(const tracker_clustering_solution & tcs_,
                                                      hit_belonging_col_type &hbc_);

      /// Merge two clustering solutions in a single one (only if both are built from different sets of hits)
      static int merge_two_solutions_in_ones(const tracker_clustering_solution & source0_,
                                             const tracker_clustering_solution & source1_,
                                             tracker_clustering_solution & target_);

      /// Copy one clustering solution in another one
      static int copy_one_solution_in_one(const tracker_clustering_solution & source_,
                                          tracker_clustering_solution & target_);


    private:

      int32_t                      _solution_id_;      //!< Unique solution ID
      cluster_col_type             _clusters_;         //!< Collection of handles on prompt Geiger hits clusters
      hit_collection_type          _unclustered_hits_; //!< Collection of handles on unclustered Geiger hits
      datatools::properties        _auxiliaries_;      //!< List of auxiliary properties

      // Non persistent information :
      hit_belonging_col_type       _hit_belonging_;    //!< List of clusters for each clustered hits

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

// Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::tracker_clustering_solution, 0)

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_SOLUTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
