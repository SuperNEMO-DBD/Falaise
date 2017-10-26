/// \file falaise/snemo/datamodels/base_topology_measurement.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The base class of topology measurement
 */

#ifndef FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_H
#define FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

namespace snemo {

namespace datamodel {

/// \brief The base class of reconstructed topology
class base_topology_measurement : public datatools::i_serializable,
                                  public datatools::i_tree_dumpable {
 public:
  /// Constructor
  base_topology_measurement();

  /// Destructor
  virtual ~base_topology_measurement();

  /// Return the const container of auxiliaries
  const datatools::properties& get_auxiliaries() const;

  /// Return the mutable container of auxiliaries
  datatools::properties& grab_auxiliaries();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 private:
  datatools::properties _auxiliaries_;  //!< Auxiliary properties

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
