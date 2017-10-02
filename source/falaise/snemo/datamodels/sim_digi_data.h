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
#include <datatools/handle.h>

// This project:
#include <falaise/snemo/datamodels/sim_digi_event_data.h>

namespace snemo {

namespace datamodel {

/// \brief The data structure that hosts information about the digitized information
/// (trigger, readout and hits) associated to a simulated event.
class sim_digi_data : public datatools::i_serializable, public datatools::i_tree_dumpable {
 public:
  /// Handle to a simulated digitize event
  typedef datatools::handle<sim_digi_event_data> sim_digi_event_handle_type;

  /// Collection of handles on trigger event
  typedef std::vector<sim_digi_event_handle_type> sim_digi_event_collection_type;

  /// Constructor
  sim_digi_data();

  /// Destructor
  virtual ~sim_digi_data();

  /// Check validity
  bool is_valid() const;

  /// Reset
  void reset();

  /// Return the const collection of trigger digi events
  const sim_digi_event_collection_type& get_digi_events() const;

  /// Return the mutable collection of trigger digi events
  sim_digi_event_collection_type& grab_digi_events();

  /// Return the const container of auxiliary properties
  const datatools::properties& get_auxiliaries() const;

  /// Return the mutable container of auxiliary properties
  datatools::properties& grab_auxiliaries();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  datatools::properties _auxiliaries_;  //!< Auxiliary properties

  sim_digi_event_collection_type
      _digi_events_;  //!< Simulated digitized events (trigger + readout datas)

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
