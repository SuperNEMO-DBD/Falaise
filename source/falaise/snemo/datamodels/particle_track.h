/// \file falaise/snemo/datamodels/particle_track.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *              Xavier  Garrido  <garrido@lal.in2p3.fr>
 * Creation date: 2012-04-17
 * Last modified: 2014-06-03
 *
 * Description: A particle track represents the physical interpretation of a
 * trajectory fitted from a cluster of tracker hits or from a gamma tracking
 * algorithm
 */

#ifndef FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_H
#define FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/geomtools:
#include <geomtools/blur_spot.h>
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO particle track
    class particle_track : public geomtools::base_hit
    {
    public:

      /// Electric charge enumeration
      enum charge_type
        {
          INVALID   = -4,
          invalid   = INVALID,
          UNDEFINED = -3,
          undefined = UNDEFINED,
          NEUTRAL   = 0,
          neutral   = NEUTRAL,
          POSITIVE  = +1,
          positive  = POSITIVE,
          NEGATIVE  = -1,
          negative  = NEGATIVE
        };


      /// Handle on particle track
      typedef datatools::handle<particle_track> handle_type;

      /// Handle on vertex spot
      typedef datatools::handle<geomtools::blur_spot> handle_spot;

      /// Collection of vertex spots
      typedef std::vector<handle_spot> vertex_collection_type;

      /// Default constructor
      particle_track();

      /// Destructor
      virtual ~particle_track();

      /// Check if there is a valid track ID
      bool has_track_id() const;

      /// Get the track ID
      int get_track_id() const;

      /// Set the track ID
      void set_track_id(int32_t track_id_);

      /// Invalidate the track ID
      void invalidate_track_id();

      //// Set particle charge
      void set_charge(int8_t charge_);

      /// Get particle charge
      int8_t get_charge() const;

      /// Get negative charged particle
      bool has_negative_charge() const;

      /// Check if the trajectory is present
      bool has_trajectory() const;

      /// Detach the trajectory
      void detach_trajectory ();

      /// Attach a trajectory by handle
      void set_trajectory_handle(const tracker_trajectory::handle_type & trajectory_handle_);

      /// Return a mutable reference on the trajectory handle
      tracker_trajectory::handle_type & grab_trajectory_handle();

      /// Return a non mutable reference on the trajectory handle
      const tracker_trajectory::handle_type & get_trajectory_handle() const;

      /// Return a mutable reference on the trajectory
      tracker_trajectory & grab_trajectory();

      /// Return a non mutable reference on the trajectory
      const tracker_trajectory & get_trajectory() const;

      /// Check if there are some vertices along the fitted trajectory
      bool has_vertices() const;

      /// Reset the collection of vertices
      void reset_vertices();

      /// Return a mutable reference on the collection of vertices (handles)
      vertex_collection_type & grab_vertices();

      /// Return a non mutable reference on the collection of vertices (handles)
      const vertex_collection_type & get_vertices() const;

      /// Check if there are some associated calorimeter
      bool has_associated_calorimeters() const;

      /// Reset the collection of associated calorimeters
      void reset_associated_calorimeters();

      /// Return a mutable reference on the collection of associated calorimeters (handles)
      calibrated_calorimeter_hit::collection_type & grab_associated_calorimeters();

      /// Return a non mutable reference on the collection of associated calorimeters (handles)
      const calibrated_calorimeter_hit::collection_type & get_associated_calorimeters() const;

      /// Empty the contents of the particle track
      void clear();

      /// Reset the particle track (see clear)
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & out     = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:
      int8_t                                      _charge_from_source_;      ///!< Particle charge
      tracker_trajectory::handle_type             _trajectory_;              ///!< Handle to the fitted trajectory
      vertex_collection_type                      _vertices_;                ///!< Collection of vertices
      calibrated_calorimeter_hit::collection_type _associated_calorimeters_; ///!< Collection of associated calorimeter hits

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
