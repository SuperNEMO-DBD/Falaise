/// \file falaise/snemo/datamodels/particle_track.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *              Xavier  Garrido  <garrido@lal.in2p3.fr>
 *              Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-17
 * Last modified: 2024-03-05
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
#include <geomtools/base_hit.h>
#include <geomtools/blur_spot.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/vertex_utils.h>

namespace snemo {

  namespace datamodel {

    /// A simple record that informs about the association of a track vertex and a calorimeter hit
    struct calo_vertex_association_type
    {
      VertexHdl         vertex; ///< Vertex handle
      CalorimeterHitHdl hit;    ///< Calorimeter hit handle
    };

    typedef calo_vertex_association_type CaloVertexAssociation;
    typedef std::vector<CaloVertexAssociation> CaloVertexAssociationCollection;

    /// \brief SuperNEMO particle track
    class particle_track
      : public geomtools::base_hit
    {
    public:
      
      /// Electric charge enumeration
      enum charge_type {
        CHARGE_UNDEFINED = datatools::bit_mask::bit00, ///< Particle with undefined charge
        CHARGE_NEUTRAL   = datatools::bit_mask::bit01, ///< Neutral particle
        CHARGE_POSITIVE  = datatools::bit_mask::bit02, ///< Positively charged particle
        CHARGE_NEGATIVE  = datatools::bit_mask::bit03, ///< Negatively charged particle
      };

      /// Conversion to a string
      static const std::string & to_string(const charge_type);
    
      /// Conversion to a vertex charge
      static charge_type string_to_charge(const std::string &);

      /// Check if there is a valid track ID
      bool has_track_id() const;

      /// Get the track ID
      int get_track_id() const;

      /// Set the track ID
      void set_track_id(int32_t id);

      /// Invalidate the track ID
      void invalidate_track_id();

      //// Set particle charge
      void set_charge(charge_type charge);

      /// Get particle charge
      charge_type get_charge() const;

      /// Check if the trajectory is present
      bool has_trajectory() const;

      /// Detach the trajectory
      void detach_trajectory();

      /// Attach a trajectory by handle
      void set_trajectory_handle(const TrackerTrajectoryHdl & trajectory);

      /// Return a mutable reference on the trajectory handle
      TrackerTrajectoryHdl & get_trajectory_handle();

      /// Return a non mutable reference on the trajectory handle
      const TrackerTrajectoryHdl & get_trajectory_handle() const;

      /// Return a mutable reference on the trajectory
      TrackerTrajectory & get_trajectory();

      /// Return a non mutable reference on the trajectory
      const TrackerTrajectory & get_trajectory() const;

      /// Check if the particle track is associated to a given cluster (through its referenced trajectory)
      bool match_cluster(const tracker_cluster & cluster_) const;
      
      /// Check if there are some vertices along the fitted trajectory
      bool has_vertices() const;

      /// Reset the collection of vertices
      void clear_vertices();

      /// Return a mutable reference on the collection of vertices (handles)
      VertexHdlCollection & get_vertices();

      /// Return a non mutable reference on the collection of vertices (handles)
      const VertexHdlCollection & get_vertices() const;

      /// Check if there are some associated calorimeter hits
      bool has_associated_calorimeter_hits() const;

      /// Reset the collection of associated calorimeter hits
      void clear_associated_calorimeter_hits();

      /// Return a mutable reference on the collection of associated calorimeter hits (handles)
      CalorimeterHitHdlCollection & get_associated_calorimeter_hits();

      /// Return a non mutable reference on the collection of associated calorimeter hits (handles)
      const CalorimeterHitHdlCollection & get_associated_calorimeter_hits() const;

      /// Return a mutable reference on the collection of calorimeter/vertex association
      CaloVertexAssociationCollection & get_calo_vertex_associations();

      /// Return a non mutable reference on the collection of calorimeter/vertex association
      const CaloVertexAssociationCollection & get_calo_vertex_associations() const;

      /// Empty the contents of the particle track
      void clear() override;
 
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ 
                              /**/ = datatools::i_tree_dumpable::empty_options()) const override;

    private:

      charge_type charge_from_source_{CHARGE_UNDEFINED}; ///< Particle charge
      TrackerTrajectoryHdl trajectory_{}; ///< Handle to the fitted trajectory
      VertexHdlCollection vertices_{}; ///< Collection of vertices
      CalorimeterHitHdlCollection associated_calorimeters_{}; ///< Calorimeter hits associated with the particle
      CaloVertexAssociationCollection calo_vertex_associations_{}; ///< List of calorimeter/vertex association
      
      DATATOOLS_SERIALIZATION_DECLARATION()
      
    };

    /// Handle on particle track
    using Particle = particle_track;
    using ParticleHdl = datatools::handle<Particle>;

    using ParticleCollection = std::vector<Particle>;
    using ParticleHdlCollection = std::vector<ParticleHdl>;

    // Free functions
    /// Check a particle charge type
    bool particle_has(const Particle &, Particle::charge_type);

    /// Check a particle is electron
    bool particle_has_negative_charge(const Particle &);

    /// Checck a particle is positron
    bool particle_has_positive_charge(const Particle &);

    /// Check a particle is alpha
    bool particle_has_undefined_charge(const Particle &);

    /// Check a particle is gamma
    bool particle_has_neutral_charge(const Particle &);

    /// Check a particle has a vertex matching the input flags
    bool particle_has_vertex(const Particle & p_, const uint32_t vertex_flags_);

  } // end of namespace datamodel

} // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::particle_track, 1)

#endif // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
