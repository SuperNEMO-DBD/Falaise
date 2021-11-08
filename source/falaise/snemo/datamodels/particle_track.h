/// \file falaise/snemo/datamodels/particle_track.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *              Xavier  Garrido  <garrido@lal.in2p3.fr>
 *              Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-17
 * Last modified: 2021-11-03
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

namespace snemo {

namespace datamodel {

/// \brief SuperNEMO particle track
class particle_track : public geomtools::base_hit {
 public:
  /// Electric charge enumeration
  enum charge_type {
    UNDEFINED = datatools::bit_mask::bit00,  ///< Particle with undefined charge
    NEUTRAL   = datatools::bit_mask::bit01,  ///< Neutral particle
    POSITIVE  = datatools::bit_mask::bit02,  ///< Positively charged particle
    NEGATIVE  = datatools::bit_mask::bit03,  ///< Negatively charged particle
  };

  /// Vertex flags
  enum vertex_type {
    VERTEX_NONE = 0x0,
    VERTEX_ON_SOURCE_FOIL = datatools::bit_mask::bit00,
    VERTEX_ON_MAIN_CALORIMETER = datatools::bit_mask::bit01,
    VERTEX_ON_X_CALORIMETER = datatools::bit_mask::bit02,
    VERTEX_ON_GAMMA_VETO = datatools::bit_mask::bit03,
    VERTEX_ON_WIRE = datatools::bit_mask::bit04,
    VERTEX_ON_CALIBRATION_SOURCE = datatools::bit_mask::bit05
  };

  /// Key for the vertex 'from' property
  static const std::string &vertex_from_key();

  /// Key for the vertex 'distance' property
  static const std::string &vertex_distance_key();

  /// Key for the vertex 'distance_xy' property
  static const std::string &vertex_distance_xy_key();

  /// Key for the vertex 'type' property
  static const std::string &vertex_type_key();

  /// Return the label from the vertex type
  static const std::string &vertex_type_to_label(vertex_type);

  /// Return the vertex type from the label
  static vertex_type label_to_vertex_type(const std::string &);

  /// Associated 'VERTEX_NONE' flag for auxiliary property
  static const std::string &vertex_none_label();

  /// Associated 'VERTEX_ON_SOURCE_FOIL' flag for auxiliary property
  static const std::string &vertex_on_source_foil_label();

  /// Associated 'VERTEX_ON_MAIN_CALORIMETER' flag for auxiliary property
  static const std::string &vertex_on_main_calorimeter_label();

  /// Associated 'VERTEX_ON_X_CALORIMETER' flag for auxiliary property
  static const std::string &vertex_on_x_calorimeter_label();

  /// Associated 'VERTEX_ON_GAMMA_CALORIMETER' flag for auxiliary property
  static const std::string &vertex_on_gamma_veto_label();

  /// Associated 'VERTEX_ON_WIRE' flag for auxiliary property
  static const std::string &vertex_on_wire_label();

  /// Associated 'VERTEX_ON_CALIBRATION_SOURCE' flag for auxiliary property
  static const std::string &vertex_on_calibration_source_label();

  /// Check a vertex type
  static bool vertex_is(const geomtools::blur_spot &, vertex_type);

  /// Check a vertex on source foil
  static bool vertex_is_on_source_foil(const geomtools::blur_spot &);

  /// Check a vertex on main calorimeter
  static bool vertex_is_on_main_calorimeter(const geomtools::blur_spot &);

  /// Check a vertex on X calorimeter
  static bool vertex_is_on_x_calorimeter(const geomtools::blur_spot &);

  /// Check a vertex on gamma veto
  static bool vertex_is_on_gamma_veto(const geomtools::blur_spot &);

  /// Check a vertex on wire
  static bool vertex_is_on_wire(const geomtools::blur_spot &);

  /// Check a vertex on calibration source
  static bool vertex_is_on_calibration_source(const geomtools::blur_spot &);

  /// Handle on vertex spot
  typedef datatools::handle<geomtools::blur_spot> handle_spot;

  /// Collection of vertex spots
  typedef std::vector<handle_spot> vertex_collection_type;

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
  void set_trajectory_handle(const TrackerTrajectoryHdl &trajectory);

  /// Return a mutable reference on the trajectory handle
  TrackerTrajectoryHdl &get_trajectory_handle();

  /// Return a non mutable reference on the trajectory handle
  const TrackerTrajectoryHdl &get_trajectory_handle() const;

  /// Return a mutable reference on the trajectory
  TrackerTrajectory &get_trajectory();

  /// Return a non mutable reference on the trajectory
  const TrackerTrajectory &get_trajectory() const;

  /// Check if there are some vertices along the fitted trajectory
  bool has_vertices() const;

  /// Reset the collection of vertices
  void clear_vertices();

  /// Return a mutable reference on the collection of vertices (handles)
  vertex_collection_type &get_vertices();

  /// Return a non mutable reference on the collection of vertices (handles)
  const vertex_collection_type &get_vertices() const;

  /// Check if there are some associated calorimeter hits
  bool has_associated_calorimeter_hits() const;

  /// Reset the collection of associated calorimeter hits
  void clear_associated_calorimeter_hits();

  /// Return a mutable reference on the collection of associated calorimeter hits (handles)
  CalorimeterHitHdlCollection &get_associated_calorimeter_hits();

  /// Return a non mutable reference on the collection of associated calorimeter hits (handles)
  const CalorimeterHitHdlCollection &get_associated_calorimeter_hits() const;

  /// Empty the contents of the particle track
  void clear() override;

  /// Smart print
  virtual void tree_dump(std::ostream &out = std::clog, const std::string &title = "",
                         const std::string &indent = "", bool is_last = false) const override;

 private:
  charge_type charge_from_source_{UNDEFINED};  ///< Particle charge
  TrackerTrajectoryHdl trajectory_{};          ///< Handle to the fitted trajectory
  vertex_collection_type vertices_{};          ///< Collection of vertices
  CalorimeterHitHdlCollection
      associated_calorimeters_{};  ///< Calorimeter hits associated with the Particle

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

// Check if a vertex matches a given type
inline bool vertex_matches(const Particle::handle_spot &vtx, const int32_t vertex_flags) {
  std::vector<Particle::vertex_type> vertex_types{
      Particle::VERTEX_ON_SOURCE_FOIL,
      Particle::VERTEX_ON_MAIN_CALORIMETER,
      Particle::VERTEX_ON_X_CALORIMETER,
      Particle::VERTEX_ON_GAMMA_VETO,
      Particle::VERTEX_ON_WIRE,
      Particle::VERTEX_ON_CALIBRATION_SOURCE};
  for (const auto &vtx_type : vertex_types) {
    if (((vertex_flags & vtx_type) != 0u) && Particle::vertex_is(*vtx, vtx_type)) {
      return true;
    }
  }
  return false;
}

/// Check a particle has a vertex matching the input flags
inline bool particle_has_vertex(const Particle &p, const int32_t vertex_flags) {
  for (const auto &vtx : p.get_vertices()) {
    if (vertex_matches(vtx, vertex_flags)) {
      return true;
    }
  }
  return false;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
