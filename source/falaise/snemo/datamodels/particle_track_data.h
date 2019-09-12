/// \file falaise/snemo/datamodels/particle_track_data.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
                Xavier Garrido   <garrido@lal.in2p3.fr>
 * Creation date: 2012-04-18
 * Last modified: 2014-06-03
 *
 * Description:  SuperNEMO Particle Track data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_DATA_H
#define FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_DATA_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_clear.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

namespace datamodel {

/// \brief SuperNEMO particle track model
//  Just a composition of a vector of particle_tracks and a vector of isolated calorimeter hits
//  TODO: split, or implement as pure dumb struct
class particle_track_data : public datatools::i_serializable,
                            public datatools::i_tree_dumpable,
                            public datatools::i_clear {
 public:
  /// Check if there are some particles
  bool hasParticles() const;

  /// Returns the number of particle
  size_t numberOfParticles() const;

  /// Add a particle track
  void insertParticle(const ParticleHdl& particle);

  /// Return a mutable reference to particles
  ParticleHdlCollection& particles();

  /// Return a non mutable reference to particles
  const ParticleHdlCollection& particles() const;

  /// Reset the particle tracks
  void clearParticles();

  /// Retrieve particles given their charge
  /// TODO: Refactor into free function/functor
  ParticleHdlCollection getParticlesByCharge(const uint32_t charge_types) const;

  /// Check if there are some non associated calorimeters
  bool hasIsolatedCalorimeters() const;

  /// Return a non mutable reference to non associated calorimeters
  const CalorimeterHitHdlCollection& isolatedCalorimeters() const;

  /// Return a mutable reference to non associated calorimeters
  CalorimeterHitHdlCollection& isolatedCalorimeters();

  /// Reset the non associated calorimeters
  void clearIsolatedCalorimeters();

  /// Clear the object
  virtual void clear();

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool is_last = false) const;

 private:
  ParticleHdlCollection particles_;                    //!< Collection of particle track handles
  CalorimeterHitHdlCollection isolated_calorimeters_;  //!< Collection of calorimeter hit handles

  datatools::properties
      _auxiliaries_;  //!< Auxiliary properties (retained for serialization back compatibility)

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::particle_track_data,
                        "snemo::datamodel::particle_track_data")

#endif  // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
