// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/event_header.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-07
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Description:
 *
 *   Event header
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODEL_EVENT_HEADER_H
#define FALAISE_SNEMO_DATAMODEL_EVENT_HEADER_H 1

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/event_id.h>
#include <datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/timestamp.h>

namespace snemo {

  namespace datamodel {

    /// \brief A event header class to be embedded as a bank in a 'datatools::things' event record.
    class event_header : public datatools::i_serializable,
                         public datatools::i_clear,
                         public datatools::i_tree_dumpable
    {
    public:

      /// \brief Generation type of the event record
      enum generation_type {
        GENERATION_INVALID   = -1, /// Invalide generation type
        GENERATION_REAL      =  0, /// Real event from the detector's DAQ
        GENERATION_SIMULATED =  1  /// Simulated event from the MC application
      };

      /// Return the default label/name of a 'event header' bank
      static const std::string & event_header_label();

      /// Return the event ID
      const datatools::event_id & get_id() const;

      /// Return the mutable event ID
      datatools::event_id & grab_id();

      /// Set the event ID
      void set_id(const datatools::event_id &);

      /// Return the constant list of properties
      const datatools::properties & get_properties() const;

      /// Return the mutable list of properties
      datatools::properties & grab_properties();

      /// Set the list of properties
      void set_properties(const datatools::properties &);

      /// Return the timestamp
      const snemo::datamodel::timestamp & get_timestamp() const;

      /// Return the mutable timestamp
      snemo::datamodel::timestamp & grab_timestamp();

      /// Set the timestamp
      void set_timestamp(const snemo::datamodel::timestamp &);

      /// Return the generation
      generation_type get_generation() const;

      /// Set the generation
      void set_generation(generation_type);

      /// Check if event record is real (collected by the experiment DAQ)
      bool is_real() const;

      /// Check if event record is simulated
      bool is_simulated() const;

      /// Constructor
      event_header();

      /// Destructor
      virtual ~event_header();

      /// Clear the event header internal data
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

      /// Basic print
      void dump() const;

    private:

      datatools::event_id         _id_;         //!< Run/Event ID
      generation_type             _generation_; //!< Generation flag
      snemo::datamodel::timestamp _timestamp_;  //!< Reference time of the event
      datatools::properties       _properties_; //!< Dictionary of properties

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::event_header, "snemo::datamodel::event_header")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::event_header, 1)

#endif // FALAISE_SNEMO_DATAMODEL_EVENT_HEADER_H
