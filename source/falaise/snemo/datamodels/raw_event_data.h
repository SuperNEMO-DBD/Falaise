// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/raw_event_data_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-13
 *
 * Description:
 *
 *  The Raw Event Data (RED) digitized datamodel.
 *  It contains a collection of a calorimeter digitized hit and a collection of a tracker digitized hit.
 *
 * History:
 *
 */
#ifndef FALAISE_SNEMO_DATAMODELS_RAW_EVENT_DATA_H
#define FALAISE_SNEMO_DATAMODELS_RAW_EVENT_DATA_H 1

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
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO raw event data (RED)
    class raw_event_data : public datatools::i_serializable,
                           public datatools::i_tree_dumpable,
                           public datatools::i_clear
    {
    public:

      /// Default constructor
      raw_event_data() = default;

      /// Destructor
      virtual ~raw_event_data() = default;

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

      //! Add the origin trigger IDs
      void add_origin_trigger_id(int32_t tid_);

      //! Return the reference of the collection of the calorimeter digitized hit
      const std::vector<calorimeter_digitized_hit> & get_calorimeter_hits() const;

      void reset_origin_trigger_id();

      calorimeter_digitized_hit & add_calorimeter_hit();

      std::vector<calorimeter_digitized_hit> & grab_calorimeter_hits();

      const std::vector<tracker_digitized_hit> & get_tracker_hits() const;

      tracker_digitized_hit & add_tracker_hit();

      std::vector<tracker_digitized_hit> & grab_tracker_hits();

      const datatools::properties & get_auxiliaries() const;

      datatools::properties & grab_auxiliaries();

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::raw_trigger_data rawEventData
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Raw Event Data:");
      /// poptions.put("indent", ">>> ");
      /// rawEventData.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

    private:

      int32_t _run_id_ = INVALID_RUN_ID; ///< Run ID
      int32_t _event_id_ = INVALID_EVENT_ID; ///< Event ID
      int64_t _reference_timestamp_; ///< Reference timestamp in the run timestamp frame
      std::vector<calorimeter_digitized_hit> _calorimeter_digitized_hits_; ///< Collection of calorimeter digitized hits
      std::vector<tracker_digitized_hit> _tracker_digitized_hits_; ///< Collection of tracker digitized hits
      std::set<int32_t> _origin_trigger_ids_; ///< Set of RTD trigger IDs used to build this event
      datatools::properties _auxiliaries_;

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_RAW_EVENT_DATA_H
