// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_data.h
/* Author(s) :    Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-03
 *
 * License:
 *
 * Description:
 *
 *   SuperNEMO preprecalibrated data model
 *
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_H
#define FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_H 1

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
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about precalibrated hits
    class precalibrated_data : public datatools::i_serializable,
                               public datatools::i_tree_dumpable,
                               public datatools::i_clear {
    public:
      /// Return the const collection of calorimeter hits
      const PreCalibCalorimeterHitHdlCollection& calorimeter_hits() const;

      /// Return the mutable collection of calorimeter hits
      PreCalibCalorimeterHitHdlCollection& calorimeter_hits();

      /// Return the const collection of tracker hits
      const PreCalibTrackerHitHdlCollection& tracker_hits() const;

      /// Return the mutable collection of tracker hits
      PreCalibTrackerHitHdlCollection& tracker_hits();

      /// Clear attributes
      virtual void clear() override;

      /// Smart print
      virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                             const std::string& indent = "", bool is_last = false) const override;

    private:
      PreCalibCalorimeterHitHdlCollection calorimeter_hits_;  //!< Collection of precalibrated calorimeter hits
      PreCalibTrackerHitHdlCollection tracker_hits_;          //!< Collection of precalibrated tracker hits

      datatools::properties
      _properties_;  //!< Auxiliary properties (only retained for serialization compat)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  }  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::precalibrated_data, "snemo::datamodel::precalibrated_data")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::precalibrated_data, 1)

#endif  // FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_H
