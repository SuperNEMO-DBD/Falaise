/// \file falaise/snemo/datamodels/sim_trigger_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated digitized trigger data model
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_H

// Standard library:
#include <bitset>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snemo/electronics/constants.h>

namespace snemo {

namespace datamodel {

/// \brief The data structure that hosts information about the digitized trigger data
class sim_trigger_digi_data : public datatools::i_serializable, public datatools::i_tree_dumpable {
 public:
  /// Constructor
  sim_trigger_digi_data();

  /// Destructor
  virtual ~sim_trigger_digi_data();

  /// Reset
  void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  // L1 datas :
  bool _L1_decision_25ns_;                //!< Calo L1 decision 25ns
  uint32_t _L1_decision_clocktick_25ns_;  //!< Calo L1 clocktick decision 25ns
  std::bitset<snemo::electronics::constants::NZONES>
      _calo_zoning_word_S0_25ns_;  //!< Calo Zoning Word Side 0
  std::bitset<snemo::electronics::constants::NZONES>
      _calo_zoning_word_S1_25ns_;  //!< Calo Zoning Word Side 1
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_S0_25ns_;  //!< Calorimeter High Threshold Multiplicity Side 0
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_S1_25ns_;  //!< Calorimeter High Threshold Multiplicity Side 1
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_gveto_25ns_;   //!< Calorimeter High Threshold Multiplicity Gamma veto
  bool _calo_LTO_S0_25ns_;     //!< Low Threshold Only Side 0
  bool _calo_LTO_S1_25ns_;     //!< Low Threshold Only Side 1
  bool _calo_LTO_GVETO_25ns_;  //!< Low Threshold Only Gamma veto
  bool _calo_SS_coinc_25ns_;   //!< Single Side Coincidence
  bool _calo_TTM_25ns_;        //!< Total Threshold Multiplicity bit
  std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE>
      _calo_XT_info_25ns_;  //!< External information

  // L2 datas :
  bool _L2_decision_1600ns_;                //!< L2 trigger decision 1600ns
  uint32_t _L2_decision_clocktick_1600ns_;  //!< L2 trigger clocktick decision 1600ns
  snemo::electronics::constants::L2_trigger_mode
      _L2_decision_trigger_mode_;  //!< L2 trigger decision mode
  std::bitset<snemo::electronics::constants::NZONES>
      _calo_zoning_word_S0_1600ns_;  //!< Calo Zoning Word Side 0
  std::bitset<snemo::electronics::constants::NZONES>
      _calo_zoning_word_S1_1600ns_;  //!< Calo Zoning Word Side 1
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_S0_1600ns_;  //!< Calorimeter High Threshold Multiplicity Side 0
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_S1_1600ns_;  //!< Calorimeter High Threshold Multiplicity Side 1
  std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE>
      _calo_HTM_gveto_1600ns_;   //!< Calorimeter High Threshold Multiplicity Gamma veto
  bool _calo_LTO_S0_1600ns_;     //!< Low Threshold Only Side 0
  bool _calo_LTO_S1_1600ns_;     //!< Low Threshold Only Side 1
  bool _calo_LTO_GVETO_1600ns_;  //!< Low Threshold Only Gamma veto
  bool _calo_SS_coinc_1600ns_;   //!< Single Side Coincidence
  bool _calo_TTM_1600ns_;        //!< Total Threshold Multiplicity bit
  std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE>
      _calo_XT_info_1600ns_;  //!< External information

  std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE>
      _tracker_finale_data_1600ns_[snemo::electronics::constants::NSIDES]
                                  [snemo::electronics::constants::NZONES];  //!< Tracker Finale data
                                                                            //!< bitset for each
                                                                            //!< zone (2 sides, 10
                                                                            //!< zones / side)
  std::bitset<snemo::electronics::constants::NZONES>
      _coincidence_ZW_S0_1600ns_;  //!< Coincidence calo / tracker Zoning Word Side 0
  std::bitset<snemo::electronics::constants::NZONES>
      _coincidence_ZW_S1_1600ns_;  //!< Coincidence calo / tracker Zoning Word Side 1

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
