/// \file falaise/snemo/datamodels/topology_1e_pattern.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2015-11-09
 * Last modified: 2015-11-09
 *
 * Description: The 1 electron topology pattern class
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E_PATTERN_H 1

// This project:
#include <falaise/snemo/datamodels/base_topology_pattern.h>

namespace snemo {

namespace datamodel {

/// \brief The 1e class of reconstructed topology
class topology_1e_pattern : public base_topology_pattern {
 public:
  /// Static function to return pattern identifier of the pattern
  static const std::string& pattern_id();

  /// Return pattern identifier of the pattern
  virtual std::string get_pattern_id() const;

  /// Constructor
  topology_1e_pattern();

  /// Destructor
  virtual ~topology_1e_pattern();

  /// Check electron track availability
  bool has_electron_track() const;

  /// Return electron track
  const snemo::datamodel::particle_track& get_electron_track() const;

  /// Check angle measurement availability
  bool has_electron_angle() const;

  /// Return electron angle
  double get_electron_angle() const;

  /// Check electron energy validity
  bool has_electron_energy() const;

  /// Return electron energy
  double get_electron_energy() const;

  /// Get electron track length
  double get_electron_track_length() const;

  /// Check electron vertex location validity
  bool has_electron_vertex_location() const;

  /// Return electron vertex location
  std::string get_electron_vertex_location() const;

  /// Check electron vertex position validity
  bool has_electron_vertex_position() const;

  /// Return electron vertex position in X
  double get_electron_vertex_position_x() const;

  /// Return electron vertex position in Y
  double get_electron_vertex_position_y() const;

  /// Return electron vertex position in Z
  double get_electron_vertex_position_z() const;

 private:
  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_1e_pattern,
                        "snemo::datamodel::topology_1e_pattern")

#endif  // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1E_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
