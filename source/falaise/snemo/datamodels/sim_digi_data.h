/// \file falaise/snemo/datamodels/sim_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated digitized data model
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/handle.h>

// This project:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about the digitized information
    /// (trigger, readout and hits) associated to a simulated event.
    class sim_digi_data : public datatools::i_serializable,
			  public datatools::i_tree_dumpable
    {
    public:

      /// Handle to a simulated tracker digitized hit
      typedef datatools::handle<sim_tracker_digi_hit> tracker_digi_hit_handle_type;

      /// Handle to a simulated calorimeter digitized hit
      typedef datatools::handle<sim_calo_digi_hit> calo_digi_hit_handle_type;

      /// Handle to a simulated trigger digitized data
      typedef datatools::handle<sim_trigger_digi_data> trigger_digi_data_handle_type;

      /// Collection of handles on simulated tracker digitized hit
      typedef std::vector<tracker_digi_hit_handle_type> tracker_digi_hit_collection_type;

      /// Collection of handles on simulated calorimeter digitized hit
      typedef std::vector<calo_digi_hit_handle_type> calo_digi_hit_collection_type;

      /// Collection of handles on simulated trigger digitized data
      typedef std::vector<trigger_digi_data_handle_type> trigger_digi_data_collection_type;

      /// Constructor
      sim_digi_data();

      /// Destructor
      virtual ~sim_digi_data();

      /// Check validity
      bool is_valid() const;

      /// Reset
      void reset();

      /// Return the const collection of trigger digi events
      const calo_digi_hit_collection_type & get_calo_digi_hits() const;

      /// Return the mutable collection of trigger digi events
      calo_digi_hit_collection_type & grab_calo_digi_hits();


      /// Add a simulated calo digi hit at the end of the collection and grab the reference on it
      sim_calo_digi_hit & add_calo_digi_hit();

      /// Return the const collection of trigger digi events
      const tracker_digi_hit_collection_type & get_tracker_digi_hits() const;

      /// Return the mutable collection of trigger digi events
      tracker_digi_hit_collection_type & grab_tracker_digi_hits();

      /// Add a simulated tracker digi hit at the end of the collection and grab the reference on it
      sim_tracker_digi_hit & add_tracker_digi_hit();

      /// Return the const collection of trigger digi events
      const trigger_digi_data_collection_type & get_trigger_digi_data() const;

      /// Return the mutable collection of trigger digi events
      trigger_digi_data_collection_type & grab_trigger_digi_data();

      /// Add a simulated trigger digi data at the end of the collection and grab the reference on it
      sim_trigger_digi_data & add_trigger_digi_data();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out = std::clog,
			     const std::string & a_title = "",
			     const std::string & a_indent = "",
			     bool a_inherit = false) const;

    private:

      calo_digi_hit_collection_type     _calo_digi_hits_;    //!< Calorimeter Digitized Hits
      tracker_digi_hit_collection_type	_tracker_digi_hits_; //!< Tracker Digitized Hits
      trigger_digi_data_collection_type _trigger_digi_data_;      //!< Trigger Digitized data

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  }  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::sim_digi_data, "snemo::datamodel::sim_digi_data")

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
