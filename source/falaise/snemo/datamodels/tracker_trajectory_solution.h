/** \file falaise/snemo/datamodels/tracker_trajectory_solution.h */
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-02-28
 *
 * Description: A trajectory solution
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H 1

// Third party:
// - Boost
#include <boost/cstdint.hpp>
// - Bayeux/datatools
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/handle.h>
#include <datatools/properties.h>

// This project
#include <falaise/snemo/datamodels/tracker_trajectory.h>

namespace snemo {

  namespace datamodel {

    class tracker_clustering_solution;
    class tracker_cluster;

    /// \brief A collection of tracker clusters , solution of a trajectory algorithm
    class tracker_trajectory_solution : DATATOOLS_SERIALIZABLE_CLASS,
                                        public datatools::i_tree_dumpable,
                                        public datatools::i_clear
    {
    public:

      /// Handle on tracker cluster
      typedef tracker_trajectory::handle_type trajectory_handle_type;

      /// Collection of handles on tracker clusters
      typedef std::vector<trajectory_handle_type> trajectory_col_type;

      /// Handle on tracker cluster solution
      typedef datatools::handle<tracker_clustering_solution> handle_clustering_solution_type;

      /// Handle on tracker trajectory solution
      typedef datatools::handle<tracker_trajectory_solution> handle_type;

      /// Handle on tracker cluster
      typedef datatools::handle<tracker_cluster> cluster_handle_type;

      /// Collection of handles on tracker clusters
      typedef std::vector<cluster_handle_type> cluster_col_type;

      /// Default constructor
      tracker_trajectory_solution();

      /// Default constructor
      virtual ~tracker_trajectory_solution();

      /// Reset the tracker cluster solution
      void reset();

      /*** Solution ID ***/

      /// Check if there is a valid solution ID
      bool has_solution_id() const;

      /// Get the solution ID
      int get_solution_id() const;

      /// Set the solution ID
      void set_solution_id(int32_t);

      /// Invalidate the solution ID
      void invalidate_solution_id();

      /*** Reference clustering solution ***/

      /// Check if there is a valid reference clustering solution
      bool has_clustering_solution() const;

      /// Set the reference clustering solution
      void set_clustering_solution(const handle_clustering_solution_type & clustering_solution_);

      /// Reset the reference clustering solution
      void invalidate_clustering_solution();

      /// Return a mutable reference on the reference clustering solution
      tracker_clustering_solution & grab_clustering_solution();

      /// Return a non mutable reference on the reference clustering solution
      const tracker_clustering_solution & get_clustering_solution() const;

      /*** Auxiliaries ***/

      /// Return a mutable reference on the container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Return a non mutable reference on the container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /*** Trajectories ***/

      /// Return a mutable reference on the container of trajectories
      trajectory_col_type & grab_trajectories();

      /// Return a non mutable reference on the container of trajectories
      const trajectory_col_type & get_trajectories() const;

      /// Reset the trajectories
      void invalidate_trajectories();

      /*** Unfitted clusters ***/

      /// Return a mutable reference on the container of handles on unfitted clusters
      cluster_col_type & grab_unfitted_clusters();

      /// Return a non mutable reference on the container of handles on unfitted clusters
      const cluster_col_type & get_unfitted_clusters() const;

      /// Reset the unfitted clusters
      void invalidate_unfitted_clusters();

      /*** Utilities ***/

      /// Empty the contents of the tracker trajectories solution
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:

      int32_t                         _solution_id_;         /// Unique solution ID
      handle_clustering_solution_type _clustering_solution_; /// The reference clustering solution
      trajectory_col_type             _trajectories_;        /// Collection of handles on trajectories associated to clusters
      cluster_col_type                _unfitted_clusters_;   /// Collection of handles on unfitted clusters
      datatools::properties           _auxiliaries_;         /// List of auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H

// end of falaise/snemo/datamodels/tracker_trajectory_solution.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
