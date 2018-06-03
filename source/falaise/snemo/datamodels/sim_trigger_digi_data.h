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
    class sim_trigger_digi_data : public datatools::i_serializable,
				  public datatools::i_tree_dumpable
    {

    public:

      /// Constructor
      sim_trigger_digi_data();

      /// Destructor
      virtual ~sim_trigger_digi_data();

      /// Set the Trigger ID
      void set_trigger_id(const uint64_t tid_);

      /// Return the Trigger ID
      uint64_t get_trigger_id() const;

      /// Set the calo L1 decision at 25 ns
      void set_L1_decision_25ns(const bool L1_dec_);

      /// Return the calo L1 decision at 25 ns
      bool get_L1_decision_25ns() const;

      /// Set the calo L1 CT at 25 ns
      void set_L1_decision_clocktick_25ns(const uint32_t L1_ct_);

      /// Return the calo L1 CT at 25 ns
      uint32_t get_L1_decision_clocktick_25ns() const;

      /// Set the calo Zoning Word S0 at 25 ns
      void set_calo_zoning_word_S0_25ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the calo Zoning Word S0 at 25 ns
      std::bitset<snemo::electronics::constants::NZONES> get_calo_zoning_word_S0_25ns() const;

      /// Set the calo Zoning Word S1 at 25 ns
      void set_calo_zoning_word_S1_25ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the calo Zoning Word S1 at 25 ns
      std::bitset<snemo::electronics::constants::NZONES> get_calo_zoning_word_S1_25ns() const;

      /// Set the calo High Threshold Multiplicity S0 at 25 ns
      void set_calo_HTM_S0_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity S0 at 25 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_S0_25ns() const;

      /// Set the calo High Threshold Multiplicity S1 at 25 ns
      void set_calo_HTM_S1_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity S1 at 25 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_S1_25ns() const;

      /// Set the calo High Threshold Multiplicity gveto at 25 ns
      void set_calo_HTM_gveto_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity gveto at 25 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_gveto_25ns() const;

      /// Set the calo Low Threshold Only S0 at 25 ns
      void set_calo_LTO_S0_25ns(const bool LTO_);

      /// Return the calo Low Threshold Only S0 at 25 ns
      bool get_calo_LTO_S0_25ns() const;

      /// Set the calo Low Threshold Only S1 at 25 ns
      void set_calo_LTO_S1_25ns(const bool LTO_);

      /// Return the calo Low Threshold Only S1 at 25 ns
      bool get_calo_LTO_S1_25ns() const;

      /// Set the calo Low Threshold Only gveto at 25 ns
      void set_calo_LTO_gveto_25ns(const bool LTO_);

      /// Return the calo Low Threshold Only gveto at 25 ns
      bool get_calo_LTO_gveto_25ns() const;

      /// Set the calo Single Side coincidence at 25 ns
      void set_calo_SS_coinc_25ns(const bool SS_);

      /// Return the calo Single Side coincidence at 25 ns
      bool get_calo_SS_coinc_25ns() const;

      /// Set the calo Total Threshold Multiplicity at 25 ns
      void set_calo_TTM_25ns(const bool SS_);

      /// Return the calo Total Threshold Multiplicity at 25 ns
      bool get_calo_TTM_25ns() const;

      /// Set the calo External info at 25 ns
      void set_calo_XT_info_25ns(const std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> & bitset_);

      /// Return the calo External info at 25 ns
      std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> get_calo_XT_info_25ns() const;

      /// Set the calo L1 decision at 1600 ns
      void set_L2_decision_1600ns(const bool L2_dec_);

      /// Return the calo L1 decision at 1600 ns
      bool get_L2_decision_1600ns() const;

      /// Set the calo L1 CT at 1600 ns
      void set_L2_decision_clocktick_1600ns(const uint32_t L2_ct_);

      /// Return the calo L1 CT at 1600 ns
      uint32_t get_L2_decision_clocktick_1600ns() const;

      /// Set the calo L2 trigger mode at 1600 ns
      void set_L2_decision_trigger_mode(snemo::electronics::constants::L2_trigger_mode & L2_);

      /// Return the calo L2 trigger mode at 1600 ns
      snemo::electronics::constants::L2_trigger_mode get_L2_decision_trigger_mode() const;

      /// Set the calo Zoning Word S0 at 1600 ns
      void set_calo_zoning_word_S0_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the calo Zoning Word S0 at 1600 ns
      std::bitset<snemo::electronics::constants::NZONES> get_calo_zoning_word_S0_1600ns() const;

      /// Set the calo Zoning Word S1 at 1600 ns
      void set_calo_zoning_word_S1_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the calo Zoning Word S1 at 1600 ns
      std::bitset<snemo::electronics::constants::NZONES> get_calo_zoning_word_S1_1600ns() const;

      /// Set the calo High Threshold Multiplicity S0 at 1600 ns
      void set_calo_HTM_S0_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity S0 at 1600 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_S0_1600ns() const;

      /// Set the calo High Threshold Multiplicity S1 at 1600 ns
      void set_calo_HTM_S1_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity S1 at 1600 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_S1_1600ns() const;

      /// Set the calo High Threshold Multiplicity gveto at 1600 ns
      void set_calo_HTM_gveto_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_);

      /// Return the calo High Threshold Multiplicity gveto at 1600 ns
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> get_calo_HTM_gveto_1600ns() const;

      /// Set the calo Low Threshold Only S0 at 1600 ns
      void set_calo_LTO_S0_1600ns(const bool LTO_);

      /// Return the calo Low Threshold Only S0 at 1600 ns
      bool get_calo_LTO_S0_1600ns() const;

      /// Set the calo Low Threshold Only S1 at 1600 ns
      void set_calo_LTO_S1_1600ns(const bool LTO_);

      /// Return the calo Low Threshold Only S1 at 1600 ns
      bool get_calo_LTO_S1_1600ns() const;

      /// Set the calo Low Threshold Only gveto at 1600 ns
      void set_calo_LTO_gveto_1600ns(const bool LTO_);

      /// Return the calo Low Threshold Only gveto at 1600 ns
      bool get_calo_LTO_gveto_1600ns() const;

      /// Set the calo Single Side coincidence at 1600 ns
      void set_calo_SS_coinc_1600ns(const bool SS_);

      /// Return the calo Single Side coincidence at 1600 ns
      bool get_calo_SS_coinc_1600ns() const;

      /// Set the calo Total Threshold Multiplicity at 1600 ns
      void set_calo_TTM_1600ns(const bool SS_);

      /// Return the calo Total Threshold Multiplicity at 1600 ns
      bool get_calo_TTM_1600ns() const;

      /// Set the calo External info at 1600 ns
      void set_calo_XT_info_1600ns(const std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> & bitset_);

      /// Return the calo External info at 1600 ns
      std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> get_calo_XT_info_1600ns() const;

      /// Set the tracker finale data per zone at 1600 ns
      void set_tracker_finale_data_1600ns(const std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE> tab_[snemo::electronics::constants::NSIDES][snemo::electronics::constants::NZONES]);

      /// Get the tracker finale data per zone at 1600 ns
      void get_tracker_finale_data_1600ns(std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE> tab_[snemo::electronics::constants::NSIDES][snemo::electronics::constants::NZONES]);

      /// Set the coincidence Zoning Word S0 at 1600 ns
      void set_coincidence_ZW_S0_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the coincidence Zoning Word S0 at 1600 ns
      std::bitset<snemo::electronics::constants::NZONES> get_coincidence_ZW_S0_1600ns() const;

      /// Set the coincidence Zoning Word S1 at 1600 ns
      void set_coincidence_ZW_S1_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_);

      /// Return the coincidence Zoning Word S1 at 1600 ns
      std::bitset<snemo::electronics::constants::NZONES> get_coincidence_ZW_S1_1600ns() const;

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out = std::clog,
			     const std::string & a_title = "",
			     const std::string & a_indent = "",
			     bool a_inherit = false) const;

    private:

      uint64_t _trigger_id_; // The trigger ID 1600ns

      // L1 datas :
      bool _L1_decision_25ns_;                //!< Calo L1 decision 25ns
      uint32_t _L1_decision_clocktick_25ns_;  //!< Calo L1 clocktick decision 25ns
      std::bitset<snemo::electronics::constants::NZONES> _calo_zoning_word_S0_25ns_;  //!< Calo Zoning Word Side 0
      std::bitset<snemo::electronics::constants::NZONES> _calo_zoning_word_S1_25ns_;  //!< Calo Zoning Word Side 1
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_S0_25ns_; //!< Calorimeter High Threshold Multiplicity Side 0
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_S1_25ns_; //!< Calorimeter High Threshold Multiplicity Side 1
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_gveto_25ns_; //!< Calorimeter High Threshold Multiplicity Gamma veto
      bool _calo_LTO_S0_25ns_;     //!< Low Threshold Only Side 0
      bool _calo_LTO_S1_25ns_;     //!< Low Threshold Only Side 1
      bool _calo_LTO_gveto_25ns_;  //!< Low Threshold Only Gamma veto
      bool _calo_SS_coinc_25ns_;   //!< Single Side Coincidence
      bool _calo_TTM_25ns_;        //!< Total Threshold Multiplicity bit
      std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> _calo_XT_info_25ns_;  //!< External information

      // L2 datas :
      bool _L2_decision_1600ns_;                //!< L2 trigger decision 1600ns
      uint32_t _L2_decision_clocktick_1600ns_;  //!< L2 trigger clocktick decision 1600ns
      snemo::electronics::constants::L2_trigger_mode _L2_decision_trigger_mode_;  //!< L2 trigger decision mode
      std::bitset<snemo::electronics::constants::NZONES> _calo_zoning_word_S0_1600ns_;  //!< Calo Zoning Word Side 0
      std::bitset<snemo::electronics::constants::NZONES> _calo_zoning_word_S1_1600ns_;  //!< Calo Zoning Word Side 1
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_S0_1600ns_;  //!< Calorimeter High Threshold Multiplicity Side 0
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_S1_1600ns_;  //!< Calorimeter High Threshold Multiplicity Side 1
      std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> _calo_HTM_gveto_1600ns_; //!< Calorimeter High Threshold Multiplicity Gamma veto
      bool _calo_LTO_S0_1600ns_;     //!< Low Threshold Only Side 0
      bool _calo_LTO_S1_1600ns_;     //!< Low Threshold Only Side 1
      bool _calo_LTO_gveto_1600ns_;  //!< Low Threshold Only Gamma veto
      bool _calo_SS_coinc_1600ns_;   //!< Single Side Coincidence
      bool _calo_TTM_1600ns_;        //!< Total Threshold Multiplicity bit
      std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> _calo_XT_info_1600ns_;  //!< External information

      std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE> _tracker_finale_data_1600ns_[snemo::electronics::constants::NSIDES][snemo::electronics::constants::NZONES];  //!< Tracker Finale data bitset for each zone (2 sides, 10 zones / side)
      std::bitset<snemo::electronics::constants::NZONES> _coincidence_ZW_S0_1600ns_; //!< Coincidence calo / tracker Zoning Word Side 0
      std::bitset<snemo::electronics::constants::NZONES> _coincidence_ZW_S1_1600ns_; //!< Coincidence calo / tracker Zoning Word Side 1

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
