/// \file falaise/snemo/datamodels/energy_measurement.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2015-10-24
 * Last modified: 2015-10-24
 *
 * Description: The class for energy measurement
 */

#ifndef FALAISE_SNEMO_DATAMODEL_ENERGY_MEASUREMENT_H
#define FALAISE_SNEMO_DATAMODEL_ENERGY_MEASUREMENT_H 1

// This project:
#include <falaise/snemo/datamodels/base_topology_measurement.h>

namespace snemo {

namespace datamodel {

/// \brief The energy measurement
class energy_measurement : public base_topology_measurement {
 public:
  /// Constructor
  energy_measurement();

  /// Destructor
  ~energy_measurement();

  /// Check energy validity
  bool has_energy() const;

  /// Set energy value
  void set_energy(double energy_);

  /// Get a non-mutable reference to energy
  const double& get_energy() const;

  /// Get a mutable reference to energy
  double& grab_energy();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 private:
  double _energy_;  //!< The energy value

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::energy_measurement,
                        "snemo::datamodel::energy_measurement")

#endif  // FALAISE_SNEMO_DATAMODEL_ENERGY_MEASUREMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
