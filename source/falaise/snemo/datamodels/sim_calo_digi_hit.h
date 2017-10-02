/// \file falaise/snemo/datamodels/sim_calo_digi_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated calo digitized hit
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H
#define FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/bit_mask.h>
// - Bayeux/mctools:
#include <bayeux/mctools/digitization/sampled_signal.h>

namespace snemo {

namespace datamodel {

/// \brief Simulated calo digitized hit.
class sim_calo_digi_hit : public mctools::digitization::sampled_signal {
 public:
  /// Constructor
  sim_calo_digi_hit();

  /// Destructor
  virtual ~sim_calo_digi_hit();

  /// Check validity
  bool is_valid() const;

  /// Reset
  void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
