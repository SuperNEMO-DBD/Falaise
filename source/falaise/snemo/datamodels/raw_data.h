/// \file falaise/snemo/datamodels/raw_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-02-04
 *
 * Description: The SuperNEMO raw data model
 */

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_DATA_H
#define FALAISE_SNEMO_DATAMODEL_RAW_DATA_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

// This project :
#include <falaise/snemo/datamodels/raw_trigger_infos.h>
#include <falaise/snemo/datamodels/raw_calorimeter_hit.h>
#include <falaise/snemo/datamodels/raw_tracker_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about the raw event (trigger and hits)
    class raw_data : public datatools::i_serializable,
                     public datatools::i_tree_dumpable
    {

    public:

      /// The collection of raw calorimeter hits
      typedef std::vector<raw_calorimeter_hit> raw_calorimeter_hit_col;

      /// The collection of raw tracker hits
      typedef std::vector<raw_tracker_hit>     raw_tracker_hit_col;

      /// Return the const trigger data structure
      const raw_trigger_infos & get_raw_trigger_infos() const;

      /// Return the mutable trigger data structure
      raw_trigger_infos & grab_raw_trigger_infos();

      /// Return the const collection of raw calorimeter hits
      const raw_calorimeter_hit_col & get_raw_calorimeter_hits() const;

      /// Return the mutable collection of raw calorimeter hits
      raw_calorimeter_hit_col & grab_raw_calorimeter_hits();

      /// Return the const collection of raw tracker hits
      const raw_tracker_hit_col & get_raw_tracker_hits() const;

      /// Return the mutable collection of raw tracker hits
      raw_tracker_hit_col & grab_raw_tracker_hits();

      /// Return the const container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return the mutable container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Constructor
      raw_data();

      /// Destructor
      virtual ~raw_data();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit               = false) const;

    private:

      raw_trigger_infos       _raw_trigger_infos_;    //!< Trigger raw informations
      raw_calorimeter_hit_col _raw_calorimeter_hits_; //!< Collection of raw calorimeter hits
      raw_tracker_hit_col     _raw_tracker_hits_;     //!< Collection of raw tracker hits
      datatools::properties   _auxiliaries_;          //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::raw_data, 0)


#endif // FALAISE_SNEMO_DATAMODEL_RAW_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
