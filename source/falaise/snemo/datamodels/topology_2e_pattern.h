/// \file falaise/snemo/datamodels/topology_2e_pattern.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2015-05-19
 * Last modified: 2015-10-16
 *
 * Description: The 2 electrons topology pattern class
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2E_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2E_PATTERN_H 1

// This project:
#include <falaise/snemo/datamodels/base_topology_pattern.h>

namespace snemo {

namespace datamodel {

/// \brief The 2 electrons class of reconstructed topology
class topology_2e_pattern : public base_topology_pattern {
 public:
  /// Static function to return pattern identifier of the pattern
  static const std::string& pattern_id();

  /// Return pattern identifier of the pattern
  virtual std::string get_pattern_id() const;

  /// Constructor
  topology_2e_pattern();

  /// Destructor
  virtual ~topology_2e_pattern();

  /// Check electron minimal energy validity
  bool has_electron_minimal_energy() const;

  /// Get electron minimal energy
  double get_electron_minimal_energy() const;

  /// Check electron maximal energy validity
  bool has_electron_maximal_energy() const;

  /// Get electron maximal energy
  double get_electron_maximal_energy() const;

  /// Check electrons energy
  bool has_electrons_energy() const;

  /// Get electrons energy sum
  double get_electrons_energy_sum() const;

  /// Get electrons energy difference
  double get_electrons_energy_difference() const;

  /// Get the name of the minimal energy electron
  std::string get_minimal_energy_electron_name() const;

  /// Get the name of the maximal energy electron
  std::string get_maximal_energy_electron_name() const;

  /// Check electrons TOF internal probability validity
  bool has_electrons_internal_probability() const;

  /// Get electrons TOF internal probability
  double get_electrons_internal_probability() const;

  /// Check electrons TOF external probability validity
  bool has_electrons_external_probability() const;

  /// Get electrons TOF external probability
  double get_electrons_external_probability() const;

  /// Check angle between electrons validity
  bool has_electrons_angle() const;

  /// Get angle between electrons
  double get_electrons_angle() const;

  /// Check common vertices probability between electrons validity
  bool has_electrons_vertices_probability() const;

  /// Get common vertices probability between electrons
  double get_electrons_vertices_probability() const;

  /// Check common vertices distance between electrons validity
  bool has_electrons_vertices_distance() const;

  /// Get common vertices distance in X between electrons
  double get_electrons_vertices_distance_x() const;

  /// Get common vertices distance in Y between electrons
  double get_electrons_vertices_distance_y() const;

  /// Get common vertices distance in Z between electrons
  double get_electrons_vertices_distance_z() const;

  /// Check common vertices location between electrons
  bool has_electrons_vertex_location() const;

  /// Get common vertices location between electrons
  std::string get_electrons_vertex_location() const;

  /// Check electrons vertex position validity
  bool has_electrons_vertex_position() const;

  /// Return electrons vertex position in X
  double get_electrons_vertex_position_x() const;

  /// Return electrons vertex position in Y
  double get_electrons_vertex_position_y() const;

  /// Return electrons vertex position in Z
  double get_electrons_vertex_position_z() const;

 private:
  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_2e_pattern,
                        "snemo::datamodel::topology_2e_pattern")

#endif  // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2E_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
