/// \file falaise/snemo/datamodels/topology_1e1a_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The 1e1a class of trajectory patterns
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E1A_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E1A_PATTERN_H 1

// Standard library:
#include <string>

// This project:
#include <falaise/snemo/datamodels/topology_1e_pattern.h>

namespace snemo {

  namespace datamodel {

    /// \brief The 1 electron - 1 alpha class of reconstructed topology
    class topology_1e1a_pattern : public topology_1e_pattern
    {
    public:

      /// Static function to return pattern identifier of the pattern
      static const std::string & pattern_id();

      /// Return pattern identifier of the pattern
      virtual std::string get_pattern_id() const;

      /// Constructor
      topology_1e1a_pattern();

      /// Destructor
      virtual ~topology_1e1a_pattern();

      /// Check alpha track availability
      bool has_alpha_track() const;

      /// Return alpha track
      const snemo::datamodel::particle_track & get_alpha_track() const;

      /// Check angle measurement availability
      bool has_alpha_angle() const;

      /// Return alpha angle
      double get_alpha_angle() const;

      /// Check angle measurement availability
      bool has_electron_alpha_angle() const;

      /// Return electron-alpha angle
      double get_electron_alpha_angle() const;

      /// Check common vertices probability between electron and alpha validity
      bool has_electron_alpha_vertices_probability() const;

      /// Get common vertices probability between electrons
      double get_electron_alpha_vertices_probability() const;

      /// Check common vertices distance between electron and alpha validity
      bool has_electron_alpha_vertices_distance() const;

      /// Get common vertices distance in X between electron and alpha
      double get_electron_alpha_vertices_distance_x() const;

      /// Get common vertices distance in Y between electron and alpha
      double get_electron_alpha_vertices_distance_y() const;

      /// Get common vertices distance in Z between electron and alpha
      double get_electron_alpha_vertices_distance_z() const;

      /// Check common vertices location between electron and alpha
      bool has_electron_alpha_vertex_location() const;

      /// Get common vertices location between electrons
      std::string get_electron_alpha_vertex_location() const;

      /// Check common vertices position
      bool has_electron_alpha_vertex_position() const;

      /// Get common vertices position between electrons
      double get_electron_alpha_vertex_position_x() const;

      /// Get common vertices position between electrons
      double get_electron_alpha_vertex_position_y() const;

      /// Get common vertices position between electrons
      double get_electron_alpha_vertex_position_z() const;

      /// Get alpha delayed time
      double get_alpha_delayed_time() const;

      /// Get alpha track length
      double get_alpha_track_length() const;

    private:

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_1e1a_pattern,
                        "snemo::datamodel::topology_1e1a_pattern")

#endif // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E1A_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
