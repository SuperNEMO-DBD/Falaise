/// \file falaise/snemo/datamodels/tof_measurement.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2014-01-27
 * Last modified: 2015-10-20
 *
 * Description: The Time-Of-Flight measurement
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOF_MEASUREMENT_H
#define FALAISE_SNEMO_DATAMODEL_TOF_MEASUREMENT_H 1

// This project:
#include <falaise/snemo/datamodels/base_topology_measurement.h>

namespace snemo {

namespace datamodel {

/// \brief The Time Of Flight measurement
class tof_measurement : public base_topology_measurement {
 public:
  /// Typedef for probability type
  typedef std::vector<double> probability_type;

  /// Constructor
  tof_measurement();

  /// Destructor
  ~tof_measurement();

  /// Check if current measure has internal probabilities
  bool has_internal_probabilities() const;

  /// Get a non-mutable reference to internal probabilities
  const probability_type& get_internal_probabilities() const;

  /// Get a mutable reference to internal probabilities
  probability_type& grab_internal_probabilities();

  /// Check if current measure has external probabilities
  bool has_external_probabilities() const;

  /// Get a non-mutable reference to external probabilities
  const probability_type& get_external_probabilities() const;

  /// Get a mutable reference to external probabilities
  probability_type& grab_external_probabilities();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 private:
  probability_type _internal_probabilities_;  //!< TOF internal probabilities
  probability_type _external_probabilities_;  //!< TOF external probabilities

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::tof_measurement, "snemo::datamodel::tof_measurement")

#endif  // FALAISE_SNEMO_DATAMODEL_TOF_MEASUREMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
