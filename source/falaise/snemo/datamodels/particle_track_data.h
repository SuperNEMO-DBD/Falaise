/// \file falaise/snemo/datamodels/particle_track_data.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
   Xavier Garrido   <garrido@lal.in2p3.fr>
   * Creation date: 2012-04-18
   * Last modified: 2014-06-03
   *
   * Description:  SuperNEMO Particle Track data model
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
    // A composition of a vector of particle_tracks and
    // a vector of isolated calorimeter hits
    class particle_track_data
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {
      
    public:

      /// Check if there are some particles
      bool hasParticles() const;

      /// Returns the number of particle
      size_t numberOfParticles() const;

      /// Add a particle track
      void insertParticle(const ParticleHdl & particle);

      /// Return a mutable reference to particles
      ParticleHdlCollection & particles();

      /// Return a non mutable reference to particles
      const ParticleHdlCollection & particles() const;

      /// Check if the particle track belongs to the collection of particle tracks
      bool ownsParticle(const ParticleHdl & particle) const;

      /// Check if a particle track shares its cluster with other particle tracks
      bool hasAlternativeTracks(const ParticleHdl & particle) const;

      /// Return the list of particle tracks sharing the same cluster than the given particle track
      ParticleHdlCollection alternativeTracks(const ParticleHdl & particle) const;
      
      /// Reset the particle tracks
      void clearParticles();

      /// Check if there are some non associated calorimeters
      bool hasIsolatedCalorimeters() const;

      /// Return a non mutable reference to non associated calorimeters
      const CalorimeterHitHdlCollection & isolatedCalorimeters() const;

      /// Return a mutable reference to non associated calorimeters
      CalorimeterHitHdlCollection & isolatedCalorimeters();

      /// Reset the non associated calorimeters
      void clearIsolatedCalorimeters();

      /// Clear the object
      virtual void clear() override;

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ 
                              /**/ = datatools::i_tree_dumpable::empty_options()) const override;
 
    private:
      
      ParticleHdlCollection particles_; //!< Collection of particle track handles
      CalorimeterHitHdlCollection isolated_calorimeters_; //!< Collection of calorimeter hit handles

      datatools::properties _auxiliaries_; // unused, retained for serialization back compatibility

      DATATOOLS_SERIALIZATION_DECLARATION()
      
    };

    /// Retrieve particles given their charge
    ParticleHdlCollection get_particles_by_charge(const particle_track_data & ptd,
                                                  uint32_t charge_types);

  } // end of namespace datamodel
  
} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::particle_track_data,
                        "snemo::datamodel::particle_track_data")

#endif // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
