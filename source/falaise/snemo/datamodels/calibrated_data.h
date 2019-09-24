// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_data.h
/* Author(s) :    Mathieu Bongrand <bongrand@lal.in2p3.fr>
 * Creation date: 2011-02-08
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Description:
 *
 *   SuperNEMO calibrated data model
 *
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_H
#define FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/i_clear.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

// This project :
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

namespace snemo {

namespace datamodel {

/// \brief The data structure that hosts information about calibrated hits
class calibrated_data : public datatools::i_serializable,
                        public datatools::i_tree_dumpable,
                        public datatools::i_clear {
 public:
  /// Return the const collection of calorimeter hits
  const CalorimeterHitHdlCollection& calorimeter_hits() const;

  /// Return the mutable collection of calorimeter hits
  CalorimeterHitHdlCollection& calorimeter_hits();

  /// Return the const collection of tracker hits
  const TrackerHitHdlCollection& tracker_hits() const;

  /// Return the mutable collection of tracker hits
  TrackerHitHdlCollection& tracker_hits();

  /// Clear attributes
  virtual void clear();

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool is_last = false) const;

 private:
  CalorimeterHitHdlCollection calorimeter_hits_;  //!< Collection of calibrated calorimeter hits
  TrackerHitHdlCollection tracker_hits_;          //!< Collection of calibrated tracker hits

  datatools::properties
      _properties_;  //!< Auxiliary properties (only retained for serialization compat)

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::calibrated_data, "snemo::datamodel::calibrated_data")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::calibrated_data, 1)

#endif  // FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_H
