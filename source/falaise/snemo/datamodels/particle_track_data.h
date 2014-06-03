/** \file falaise/snemo/datamodels/particle_track_data.h */
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

// Standard library
#include <vector>

// Third party:
// - Boost
#include <boost/serialization/access.hpp>
// - Bayeux/datatools
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/properties.h>

// This project
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO particle track model
    //  To be done...
    class particle_track_data : DATATOOLS_SERIALIZABLE_CLASS,
                                public datatools::i_tree_dumpable,
                                public datatools::i_clear
    {
    public:

      /// Collection of handles on particle tracks
      typedef std::vector<particle_track::handle_type> particle_collection_type;

      /// Default constructor
      particle_track_data();

      /// Destructor:
      virtual ~particle_track_data();

      /// Check if there are some particles
      bool has_particles() const;

      /// Returns the number of particle
      size_t get_number_of_particles() const;

      /// Add a particle track
      void add_particle(const particle_track::handle_type & handle_);

      /// Return a non mutable reference to a particle by index
      const particle_track & get_particle(size_t i_) const;

      /// Return a mutable reference to particles
      particle_collection_type & grab_particles();

      /// Return a non mutable reference to particles
      const particle_collection_type & get_particles() const;

      /// Reset the particle tracks
      void invalidate_particles();

      /// Return a non mutable reference to non associated calorimeters
      const calibrated_calorimeter_hit::collection_type & get_non_associated_calorimeters() const;

      /// Return a mutable reference to non associated calorimeters
      calibrated_calorimeter_hit::collection_type & grab_non_associated_calorimeters();

      /// Reset the non associated calorimeters
      void reset_non_associated_calorimeters ();

      /// Reset the internals
      void reset();

      /// Check if the object has a valid internal structure
      bool is_valid() const;

      /// Return a mutable reference on the container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return a non mutable reference on the container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Clear the object
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private :

      particle_collection_type _particles_;                                       //!< Collection of particle track handles
      calibrated_calorimeter_hit::collection_type _non_associated_calorimeters_;  //!< Collection of calorimeter hit handles
      datatools::properties _auxiliaries_;                                        //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::particle_track_data, "snemo::datamodel::particle_track_data")

#endif // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
