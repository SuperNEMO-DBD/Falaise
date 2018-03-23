/// \file falaise/snemo/datamodels/sim_readout_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 *            : Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
 * Creation date: 2016-12-06
 * Last modified: 2016-12-08
 *
 * Description: The SuperNEMO simulated digitized readout data model
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

namespace snemo {

namespace datamodel {

/// \brief The data structure that hosts information about the digitized readout data
class sim_readout_digi_data : public datatools::i_serializable, public datatools::i_tree_dumpable {
 public:
  /// The collection of calorimeter digi hits
  typedef std::vector<sim_calo_digi_hit> calo_digi_hit_collection_type;

  /// The collection of tracker digi hits
  typedef std::vector<sim_tracker_digi_hit> tracker_digi_hit_collection_type;

  /// Constructor
  sim_readout_digi_data();

  /// Destructor
  virtual ~sim_readout_digi_data();

  /// Check validity
  bool is_valid() const;

  /// Reset
  void reset();

  /// Return the const collection of calorimeter digi hits
  const calo_digi_hit_collection_type& get_calo_digi_hits() const;

  /// Return the mutable collection of calorimeter digi hits
  calo_digi_hit_collection_type& grab_calo_digi_hits();

  /// Return the const collection of tracker digi hits
  const tracker_digi_hit_collection_type& get_tracker_digi_hits() const;

  /// Return the mutable collection of tracker digi hits
  tracker_digi_hit_collection_type& grab_tracker_digi_hits();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  calo_digi_hit_collection_type _calo_digi_hits_;        //!< Simulated calorimeter digitized hits
  tracker_digi_hit_collection_type _tracker_digi_hits_;  //!< Simulated tracker digitized hits

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
