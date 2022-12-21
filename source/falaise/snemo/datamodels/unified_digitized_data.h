// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/unified_digitized_data.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-13
 *
 * Description:
 *
 *  The Unified Digitized Data (UDD) datamodel.
 *  It contains a trigger digitized hit, a collection of calorimeter digitized
 *  hits and a collection of tracker digitized hits.
 *
 * History:
 *
 */
#ifndef FALAISE_SNEMO_DATAMODELS_UNIFIED_DIGITIZED_DATA_H
#define FALAISE_SNEMO_DATAMODELS_UNIFIED_DIGITIZED_DATA_H 1

// Standard Library:
#include <iostream>
#include <set>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux:
#include <datatools/handle.h>
#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/trigger_digitized_hit.h>
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>
#include <falaise/snemo/datamodels/udd_utils.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO Unified Digitized data (UDD)
    class unified_digitized_data : public datatools::i_serializable,
                                   public datatools::i_tree_dumpable,
                                   public datatools::i_clear
    {
    public:

      /// Default constructor
      unified_digitized_data() = default;

      /// Destructor
      virtual ~unified_digitized_data() = default;

      /// Check if the data record is complete
      bool is_valid() const;

      //! Reset the data
      void invalidate();

      //! Check if run ID is set
      bool has_run_id() const;

      //! Return the run ID
      int32_t get_run_id() const;

      //! Set the run ID
      void set_run_id(const int32_t);

      //! Check if event ID is set
      bool has_event_id() const;

      //! Return the event ID
      int32_t get_event_id() const;

      //! Set the event ID
      void set_event_id(const int32_t);

      //! Check if reference timestamp is set
      bool has_reference_timestamp() const;

      //! Return the reference timestamp
      int64_t get_reference_timestamp() const;

      //! Set the reference timestamp
      void set_reference_timestamp(const int64_t &);

      //! Get the origin trigger IDs
      const std::set<int32_t> & get_origin_trigger_ids() const;

      //! Set the list of origin trigger IDs
      void set_origin_trigger_ids(const std::set<int32_t> & tids_);

      //! Add the origin trigger IDs
      void add_origin_trigger_id(int32_t tid_);

      //! Reset the origin trigger IDs
      void reset_origin_trigger_id();

      //! Return the const collection of calorimeter digitized hits
      const CalorimeterDigiHitHdlCollection & get_calorimeter_hits() const;

      //! Return the mutable collection of calorimeter digitized hits
      CalorimeterDigiHitHdlCollection & grab_calorimeter_hits();

      //! Add a calorimeter hit to the collection of calorimeter digitized hits
      calorimeter_digitized_hit & add_calorimeter_hit();

      //! Return the const collection of tracker digitized hits
      const TrackerDigiHitHdlCollection & get_tracker_hits() const;

      //! Return the mutable collection of tracker digitized hits
      TrackerDigiHitHdlCollection & grab_tracker_hits();

      //! Add a tracker hit to the collection of tracker digitized hits
      tracker_digitized_hit & add_tracker_hit();

      //! Return the const datatools auxiliaries
      const datatools::properties & get_auxiliaries() const;

      //! Return the mutable datatools auxiliaries
      datatools::properties & grab_auxiliaries();

      //! Set the datatools auxiliaries
      void set_auxiliaries(const datatools::properties & aux_);

      /// Clear attributes
      virtual void clear() override;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snemo::datamodel::unified_digitized_data udd
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Unified Digitized Data:");
      /// poptions.put("indent", ">>> ");
      /// udd.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      int32_t _run_id_ = INVALID_RUN_ID; ///< Run ID
      int32_t _event_id_ = INVALID_EVENT_ID; ///< Event ID
      int64_t _reference_timestamp_; ///< Reference timestamp in the run timestamp frame
      CalorimeterDigiHitHdlCollection _calorimeter_digitized_hits_; ///< Collection of calorimeter digitized hits
      TrackerDigiHitHdlCollection _tracker_digitized_hits_; ///< Collection of tracker digitized hits
      std::set<int32_t> _origin_trigger_ids_; ///< Set of RTD trigger IDs used to build this event
      datatools::properties _auxiliaries_;

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // namespace datamodel

} // namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::unified_digitized_data,
                        "snemo::datamodel::unified_digitized_data")

#endif // FALAISE_SNEMO_DATAMODELS_UNIFIED_DIGITIZED_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
