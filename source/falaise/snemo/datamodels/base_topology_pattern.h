/// \file falaise/snemo/datamodels/base_topology_pattern.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2015-09-19
 * Last modified: 2015-11-11
 *
 * Description: The base class of topology patterns
 */

#ifndef FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_H 1

// Standard library:
#include <map>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/datamodels/base_topology_measurement.h>
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

namespace datamodel {

/// \brief The base class of reconstructed topology
class base_topology_pattern : public datatools::i_serializable, public datatools::i_tree_dumpable {
 public:
  /// Return topology pattern id
  virtual std::string get_pattern_id() const = 0;

  /// Typedef to particle track dictionary
  typedef std::map<std::string, snemo::datamodel::particle_track::handle_type>
      particle_track_dict_type;

  /// Typedef to base topology pattern handle
  typedef datatools::handle<snemo::datamodel::base_topology_pattern> handle_type;

  /// Typedef to measurement handle
  typedef datatools::handle<base_topology_measurement> handle_measurement;

  /// Typedef to measurement dictionary
  typedef std::map<std::string, handle_measurement> measurement_dict_type;

  /// Constructor
  base_topology_pattern();

  /// Destructor
  virtual ~base_topology_pattern();

  /// Get a mutable reference to particle track dictionary
  particle_track_dict_type &grab_particle_track_dictionary();

  /// Get a non-mutable reference to particle track dictionary
  const particle_track_dict_type &get_particle_track_dictionary() const;

  /// Check if a particle track exists
  bool has_particle_track(const std::string &) const;

  /// Get a given particle track
  const snemo::datamodel::particle_track &get_particle_track(const std::string &) const;

  /// Check if a measurement is available
  bool has_measurement(const std::string &) const;

  /// Get a given measurement
  const snemo::datamodel::base_topology_measurement &get_measurement(const std::string &) const;

  /// Check measurement data type
  template <class T>
  bool has_measurement_as(const std::string &label_) const {
    DT_THROW_IF(!has_measurement(label_), std::logic_error,
                "Topology pattern does not hold any '" << label_ << "' measurement !");
    const std::type_info &ti = typeid(T);
    const std::type_info &tf = typeid(get_measurement(label_));
    return ti == tf;
  }

  /// Get a non-mutable measurement of a given type
  template <class T>
  const T &get_measurement_as(const std::string &label_) const {
    DT_THROW_IF(!has_measurement_as<T>(label_), std::logic_error,
                "Invalid request on measurement data type !");
    return dynamic_cast<const T &>(get_measurement(label_));
  }

  /// Get a mutable reference to measurement dictionary
  measurement_dict_type &grab_measurement_dictionary();

  /// Get a non-mutable reference to measurement dictionary
  const measurement_dict_type &get_measurement_dictionary() const;

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

 private:
  particle_track_dict_type _tracks_;  //!< Particle track dictionary
  measurement_dict_type _meas_;       //!< Measurement dictionary

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
