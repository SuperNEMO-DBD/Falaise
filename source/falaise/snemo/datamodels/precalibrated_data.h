// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_data.h
/* Author(s) :    Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2024-03-13
 *
 * License:
 *
 * Description:
 *
 *   SuperNEMO precalibrated data model
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
    class precalibrated_data 
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {
    public:
      
      /// Return the const collection of calorimeter hits
      const PreCalibratedCalorimeterHitHdlCollection & calorimeter_hits() const;

      /// Return the mutable collection of calorimeter hits
      PreCalibratedCalorimeterHitHdlCollection & calorimeter_hits();

      /// Return the const collection of tracker hits
      const PreCalibratedTrackerHitHdlCollection & tracker_hits() const;

      /// Return the mutable collection of tracker hits
      PreCalibratedTrackerHitHdlCollection & tracker_hits();

      /// Return the const properties container
      const datatools::properties & get_properties() const;
     
      /// Return the mutable properties container
      datatools::properties & grab_properties();
      
      /// Clear attributes
      void clear() override;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snemo::datamodel::unified_digitized_data udd
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Unified Digitized Data:");
      /// poptions.put("indent", ">>> ");
      /// poptions.put("list_hits", true);
      /// poptions.put("list_properties", true);
      /// udd.print_tree(std::clog, poptions);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:
      PreCalibratedCalorimeterHitHdlCollection calorimeter_hits_; //!< Collection of precalibrated calorimeter hits
      PreCalibratedTrackerHitHdlCollection tracker_hits_; //!< Collection of precalibrated tracker hits
      datatools::properties _properties_; //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::precalibrated_data, "snemo::datamodel::precalibrated_data")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::precalibrated_data, 1)

#endif // FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_H
