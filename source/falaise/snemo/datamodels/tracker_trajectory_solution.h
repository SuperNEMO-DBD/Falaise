/// \file falaise/snemo/datamodels/tracker_trajectory_solution.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-02-28
 *
 * Description: A trajectory solution
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H 1

// Standard library:
#include <set>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/i_clear.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

namespace snemo {

  namespace datamodel {

    class tracker_cluster;

    /// \brief A collection of tracker clusters , solution of a trajectory algorithm
    class tracker_trajectory_solution
      : public datatools::i_serializable,
        public datatools::i_tree_dumpable,
        public datatools::i_clear
    {
    public:
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
      void set_clustering_solution(const TrackerClusteringSolutionHdl& s);

      /// Reset the reference clustering solution
      void invalidate_clustering_solution();

      /// Return a mutable reference on the reference clustering solution
      tracker_clustering_solution& grab_clustering_solution();

      /// Return a non mutable reference on the reference clustering solution
      const tracker_clustering_solution& get_clustering_solution() const;

      /// Check if there is trajectories
      bool has_trajectories() const;

      /// Return a mutable reference on the container of trajectories
      TrackerTrajectoryHdlCollection& grab_trajectories();

      /// Return a non mutable reference on the container of trajectories
      const TrackerTrajectoryHdlCollection& get_trajectories() const;

      /// Reset the trajectories
      void invalidate_trajectories();

      /// Check if there is unfitted clusters
      bool has_unfitted_clusters() const;

      /// Return a mutable reference on the container of handles on unfitted clusters
      TrackerClusterHdlCollection& grab_unfitted_clusters();

      /// Return a non mutable reference on the container of handles on unfitted clusters
      const TrackerClusterHdlCollection& get_unfitted_clusters() const;

      /// Reset the unfitted clusters
      void invalidate_unfitted_clusters();

      bool has_best_trajectories() const;

      uint32_t number_of_best_trajectories() const;
      
      const std::set<int32_t> & get_best_trajectories() const;

      std::set<int32_t> & grab_best_trajectories();

      /// Empty the contents of the tracker trajectories solution
      virtual void clear() override;

      /// Smart print
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;

      /*** Auxiliaries ***/
      // These are marked as deprecated to warn/fail compilation if they are used
      // NOT removed because the EventBrowser uses them, and is very tightly coupled
      // so cannot be removed just yet...
      /// Return a mutable reference on the container of auxiliary properties
      ///\deprecated properties should not be used in core data types
      datatools::properties& get_auxiliaries() __attribute__((deprecated));

      /// Return a non mutable reference on the container of auxiliary properties
      ///\deprecated properties should not be used in core data types
      const datatools::properties& get_auxiliaries() const __attribute__((deprecated));

    private:
      int32_t id_ = -1;                              //!< Unique solution ID
      TrackerClusteringSolutionHdl solutions_;       //!< The reference clustering solution
      TrackerTrajectoryHdlCollection trajectories_;  //!< Trajectories associated to clusters
      TrackerClusterHdlCollection unfitted_;         //!< Unfitted clusters
      datatools::properties _auxiliaries_;           //!< List of auxiliary properties
      std::set<int32_t> best_trajectories_;          //!< Indexes of best trajectories (added in Falaise 5.0, class version 1)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    /// Handle on tracker trajectory solution
    // typedef datatools::handle<tracker_trajectory_solution> handle_type;
    using TrackerTrajectorySolution = tracker_trajectory_solution;
    using TrackerTrajectorySolutionCollection = std::vector<TrackerTrajectorySolution>;

    using TrackerTrajectorySolutionHdl = datatools::handle<TrackerTrajectorySolution>;
    using TrackerTrajectorySolutionHdlCollection = std::vector<TrackerTrajectorySolutionHdl>;

  }  // end of namespace datamodel

}  // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::tracker_trajectory_solution, 1)

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
