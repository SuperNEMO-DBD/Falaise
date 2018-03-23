/// \file falaise/snemo/datamodels/sim_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated digitized event data
 * which contains a simulated trigger digi data and a simulated
 * readout digi data.
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_DIGI_EVENT_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_DIGI_EVENT_DATA_H

// Standard library:
#include <iostream>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/sim_readout_digi_data.h>
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>

namespace snemo {

namespace datamodel {

/// \brief The data structure that hosts information about the digitized information
/// (trigger, readout and hits) associated to a simulated event.
class sim_digi_event_data : public datatools::i_serializable, public datatools::i_tree_dumpable {
 public:
  /// Constructor
  sim_digi_event_data();

  /// Destructor
  virtual ~sim_digi_event_data();

  /// Check validity
  bool is_valid() const;

  /// Reset
  void reset();

  /// Return the const collection of trigger digi events
  const sim_trigger_digi_data& get_trigger_data() const;

  /// Return the mutable collection of trigger digi events
  sim_trigger_digi_data& grab_trigger_data();

  /// Return the const collection of calorimeter digi hits
  const sim_readout_digi_data& get_readout_data() const;

  /// Return the mutable collection of calorimeter digi hits
  sim_readout_digi_data& grab_readout_data();

  /// Return the const container of auxiliary properties
  const datatools::properties& get_auxiliaries() const;

  /// Return the mutable container of auxiliary properties
  datatools::properties& grab_auxiliaries();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  datatools::properties _auxiliaries_;  //!< Auxiliary properties

  // Datas :
  sim_trigger_digi_data _trigger_digi_data_;  //!< Trigger digitized data structure
  sim_readout_digi_data _readout_digi_data_;  //!< Readout digitized data structure

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::sim_digi_event_data,
                        "snemo::datamodel::sim_digi_event_data")

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_DIGI_EVENT_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
