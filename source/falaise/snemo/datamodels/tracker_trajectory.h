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
#include <falaise/snemo/datamodels/track_fitting_utils.h>

namespace snemo {

  namespace datamodel {

    /// A trajectory of Geiger calibrated hits referenced by handles
    class tracker_trajectory
      : public geomtools::base_hit
    {
    public:
      
      /// Default constructor
      tracker_trajectory() = default;

      // Destructor
      virtual ~tracker_trajectory() override = default;

      // Copy constructor
      tracker_trajectory(const tracker_trajectory &) = default;

      // Copy assignment
      tracker_trajectory & operator=(const tracker_trajectory &) = default;

      // Move constructor
      tracker_trajectory(tracker_trajectory &&);

      // Move assignment
      tracker_trajectory & operator=(tracker_trajectory &&) = default;
     
      /// Check if there is a valid trajectory ID
      bool has_id() const;

      /// Get the trajectory ID
      int get_id() const;

      /// Set the trajectory ID
      void set_id(int32_t);

      /// Check if the cluster is present
      bool has_cluster() const;

      /// Attach a cluster by handle
      void set_cluster_handle(const TrackerClusterHdl & cluster);

      /// Return a mutable reference on the cluster
      tracker_cluster & get_cluster();

      /// Return a non mutable reference on the cluster
      const tracker_cluster & get_cluster() const;

      /// Return a mutable reference on the cluster handle
      const TrackerClusterHdl & get_cluster_handle() const;

      /// Check if the pattern is present
      bool has_pattern() const;

      /// Attach a pattern by handle
      void set_pattern_handle(const TrajectoryPatternHdl & pattern);

      /// Return a mutable reference on the pattern
      TrajectoryPattern & get_pattern();

      /// Return a non mutable reference on the pattern
      const TrajectoryPattern & get_pattern() const;
      
      /// Return a mutable reference on the fit infos structure     
      track_fit_infos & get_fit_infos();
  
      /// Return anon  mutable reference on the fit infos structure     
      const track_fit_infos & get_fit_infos() const;

      /// Empty the contents of the tracker trajectory
      virtual void clear() override;

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;

    private:
      /// Detach the cluster
      void detach_cluster();
      /// Detach the pattern
      void detach_pattern();

      TrackerClusterHdl cluster_;     ///< Handle to the fitted cluster
      TrajectoryPatternHdl pattern_;  ///< Handle to a trajectory fitted pattern
      track_fit_infos fit_infos_;     ///< Fit informations

      /// Collection of handles of calibrated tracker hit
      typedef TrackerHitHdlCollection orphans_collection_type;
      orphans_collection_type orphans_;  ///< Collection of orphan Geiger hit handles (retained only
      ///< for serialization back-compatibility)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

    /// Handle on tracker trajectory
    using TrackerTrajectory = tracker_trajectory;
    using TrackerTrajectoryCollection = std::vector<TrackerTrajectory>;

    using TrackerTrajectoryHdl = datatools::handle<TrackerTrajectory>;
    using TrackerTrajectoryHdlCollection = std::vector<TrackerTrajectoryHdl>;

  }  // end of namespace datamodel

}  // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::tracker_trajectory, 1)

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
