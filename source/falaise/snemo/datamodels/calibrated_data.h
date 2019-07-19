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
  /// Handle to a calibrated tracker hit
  typedef datatools::handle<calibrated_tracker_hit> tracker_hit_handle_type;

  /// Handle to a calibrated calorimeter hit
  typedef datatools::handle<calibrated_calorimeter_hit> calorimeter_hit_handle_type;

  /// Collection of handles on calibrated tracker hits
  typedef std::vector<tracker_hit_handle_type> tracker_hit_collection_type;

  /// Collection of handles on calibrated calorimeter hits
  typedef std::vector<calorimeter_hit_handle_type> calorimeter_hit_collection_type;

  /// Check if there are some hits
  bool has_data() const;

  /// Check if there are some calibrated calorimeter hits
  bool has_calibrated_calorimeter_hits() const;

  /// Reset the collection of calorimeter hits
  void reset_calibrated_calorimeter_hits();

  /// Return the const collection of calorimeter hits
  const calorimeter_hit_collection_type& calibrated_calorimeter_hits() const;

  /// Return the mutable collection of calorimeter hits
  calorimeter_hit_collection_type& calibrated_calorimeter_hits();

  /// Check if there are some calibrated tracker hits
  bool has_calibrated_tracker_hits() const;

  /// Reset the collection of tracker hits
  void reset_calibrated_tracker_hits();

  /// Return the const collection of tracker hits
  const tracker_hit_collection_type& calibrated_tracker_hits() const;

  /// Return the mutable collection of tracker hits
  tracker_hit_collection_type& calibrated_tracker_hits();

  /// Return the const container of properties
  const datatools::properties& get_properties() const;

  /// Return the mutable container of properties
  datatools::properties& grab_properties();

  /// Reset
  void reset();

  /// Clear attributes
  virtual void clear();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  calorimeter_hit_collection_type
      _calibrated_calorimeter_hits_;  //!< Collection of calibrated calorimeter hits
  tracker_hit_collection_type _calibrated_tracker_hits_;  //!< Collection of calibrated tracker hits
  datatools::properties _properties_;                     //!< Auxiliary properties

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
