/// \file falaise/snemo/datamodels/sim_trigger_digi_data.cc

// Third party
// - Bayeux/datatools
#include <datatools/utils.h>

// Ourselves:
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_trigger_digi_data,
						      "snemo::datamodel::sim_trigger_digi_data")

    sim_trigger_digi_data::sim_trigger_digi_data()
    {
      _trigger_id_ = -1;
      _L1_decision_25ns_ = false;
      _L1_decision_clocktick_25ns_ = snemo::electronics::constants::INVALID_CLOCKTICK;
      _calo_zoning_word_S0_25ns_.reset();
      _calo_zoning_word_S1_25ns_.reset();
      _calo_HTM_S0_25ns_.reset();
      _calo_HTM_S1_25ns_.reset();
      _calo_HTM_gveto_25ns_.reset();
      _calo_LTO_S0_25ns_ = false;
      _calo_LTO_S1_25ns_ = false;
      _calo_LTO_gveto_25ns_ = false;
      _calo_SS_coinc_25ns_ = false;
      _calo_TTM_25ns_ = false;
      _calo_XT_info_25ns_.reset();
      _L2_decision_1600ns_ = false;
      _L2_decision_clocktick_1600ns_ = snemo::electronics::constants::INVALID_CLOCKTICK;
      _L2_decision_trigger_mode_ = snemo::electronics::constants::INVALID;
      _calo_zoning_word_S0_1600ns_.reset();
      _calo_zoning_word_S1_1600ns_.reset();
      _calo_HTM_S0_1600ns_.reset();
      _calo_HTM_S1_1600ns_.reset();
      _calo_HTM_gveto_1600ns_.reset();
      _calo_LTO_S0_1600ns_ = false;
      _calo_LTO_S1_1600ns_ = false;
      _calo_LTO_gveto_1600ns_ = false;
      _calo_SS_coinc_1600ns_ = false;
      _calo_TTM_1600ns_ = false;
      _calo_XT_info_1600ns_.reset();
      for (unsigned int iside = 0; iside < snemo::electronics::constants::NSIDES; iside++) {
	for (unsigned int izone = 0; izone < snemo::electronics::constants::NZONES; izone++) {
	  _tracker_finale_data_1600ns_[iside][izone].reset();
	}
      }
      _coincidence_ZW_S0_1600ns_.reset();
      _coincidence_ZW_S1_1600ns_.reset();
      return;
    }

    sim_trigger_digi_data::~sim_trigger_digi_data()
    {
      return;
    }

    uint64_t sim_trigger_digi_data::get_trigger_id() const
    {
      return _trigger_id_;
    }

    void sim_trigger_digi_data::set_trigger_id(const uint64_t a_tid_)
    {
      _trigger_id_ = a_tid_;
      return;
    }

    void sim_trigger_digi_data::set_L1_decision_25ns(const bool L1_dec_)
    {
      _L1_decision_25ns_ = L1_dec_;
      return;
    }

    bool sim_trigger_digi_data::get_L1_decision_25ns() const
    {
      return _L1_decision_25ns_;
    }

    void sim_trigger_digi_data::set_L1_decision_clocktick_25ns(const uint32_t L1_ct_)
    {
      _L1_decision_clocktick_25ns_ = L1_ct_;
      return;
    }

    uint32_t sim_trigger_digi_data::get_L1_decision_clocktick_25ns() const
    {
      return _L1_decision_clocktick_25ns_;
    }

    void sim_trigger_digi_data::set_calo_zoning_word_S0_25ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _calo_zoning_word_S0_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_calo_zoning_word_S0_25ns() const
    {
      return _calo_zoning_word_S0_25ns_;
    }

    void sim_trigger_digi_data::set_calo_zoning_word_S1_25ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _calo_zoning_word_S1_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_calo_zoning_word_S1_25ns() const
    {
      return _calo_zoning_word_S1_25ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_S0_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_S0_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_S0_25ns() const
    {
      return _calo_HTM_S0_25ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_S1_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_S1_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_S1_25ns() const
    {
      return _calo_HTM_S1_25ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_gveto_25ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_gveto_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_gveto_25ns() const
    {
      return _calo_HTM_gveto_25ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_S0_25ns(const bool LTO_)
    {
      _calo_LTO_S0_25ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_S0_25ns() const
    {
      return _calo_LTO_S0_25ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_S1_25ns(const bool LTO_)
    {
      _calo_LTO_S1_25ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_S1_25ns() const
    {
      return _calo_LTO_S1_25ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_gveto_25ns(const bool LTO_)
    {
      _calo_LTO_gveto_25ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_gveto_25ns() const
    {
      return _calo_LTO_gveto_25ns_;
    }

    void sim_trigger_digi_data::set_calo_SS_coinc_25ns(const bool SS_)
    {
      _calo_SS_coinc_25ns_ = SS_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_SS_coinc_25ns() const
    {
      return _calo_SS_coinc_25ns_;
    }

    void sim_trigger_digi_data::set_calo_TTM_25ns(const bool SS_)
    {
      _calo_TTM_25ns_ = SS_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_TTM_25ns() const
    {
      return _calo_TTM_25ns_;
    }

    void sim_trigger_digi_data::set_calo_XT_info_25ns(const std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> & bitset_)
    {
      _calo_XT_info_25ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE>  sim_trigger_digi_data::get_calo_XT_info_25ns() const
    {
      return _calo_XT_info_25ns_;
    }

    void sim_trigger_digi_data::set_L2_decision_1600ns(const bool L2_dec_)
    {
      _L2_decision_1600ns_ = L2_dec_;
      return;
    }

    bool sim_trigger_digi_data::get_L2_decision_1600ns() const
    {
      return _L2_decision_1600ns_;
    }

    void sim_trigger_digi_data::set_L2_decision_clocktick_1600ns(const uint32_t L2_ct_)
    {
      _L2_decision_clocktick_1600ns_ = L2_ct_;
      return;
    }

    uint32_t sim_trigger_digi_data::get_L2_decision_clocktick_1600ns() const
    {
      return _L2_decision_clocktick_1600ns_;
    }

    void sim_trigger_digi_data::set_L2_decision_trigger_mode(snemo::electronics::constants::L2_trigger_mode & L2_)
    {
      _L2_decision_trigger_mode_ = L2_;
      return;
    }

    snemo::electronics::constants::L2_trigger_mode sim_trigger_digi_data::get_L2_decision_trigger_mode() const
    {
      return _L2_decision_trigger_mode_;
    }

    void sim_trigger_digi_data::set_calo_zoning_word_S0_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _calo_zoning_word_S0_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_calo_zoning_word_S0_1600ns() const
    {
      return _calo_zoning_word_S0_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_zoning_word_S1_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _calo_zoning_word_S1_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_calo_zoning_word_S1_1600ns() const
    {
      return _calo_zoning_word_S1_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_S0_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_S0_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_S0_1600ns() const
    {
      return _calo_HTM_S0_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_S1_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_S1_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_S1_1600ns() const
    {
      return _calo_HTM_S1_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_HTM_gveto_1600ns(const std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> & bitset_)
    {
      _calo_HTM_gveto_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::HTM_BITSET_SIZE> sim_trigger_digi_data::get_calo_HTM_gveto_1600ns() const
    {
      return _calo_HTM_gveto_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_S0_1600ns(const bool LTO_)
    {
      _calo_LTO_S0_1600ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_S0_1600ns() const
    {
      return _calo_LTO_S0_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_S1_1600ns(const bool LTO_)
    {
      _calo_LTO_S1_1600ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_S1_1600ns() const
    {
      return _calo_LTO_S1_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_LTO_gveto_1600ns(const bool LTO_)
    {
      _calo_LTO_gveto_1600ns_ = LTO_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_LTO_gveto_1600ns() const
    {
      return _calo_LTO_gveto_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_SS_coinc_1600ns(const bool SS_)
    {
      _calo_SS_coinc_1600ns_ = SS_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_SS_coinc_1600ns() const
    {
      return _calo_SS_coinc_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_TTM_1600ns(const bool SS_)
    {
      _calo_TTM_1600ns_ = SS_;
      return;
    }

    bool sim_trigger_digi_data::get_calo_TTM_1600ns() const
    {
      return _calo_TTM_1600ns_;
    }

    void sim_trigger_digi_data::set_calo_XT_info_1600ns(const std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE> & bitset_)
    {
      _calo_XT_info_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::XT_INFO_BITSET_SIZE>  sim_trigger_digi_data::get_calo_XT_info_1600ns() const
    {
      return _calo_XT_info_1600ns_;
    }

    void sim_trigger_digi_data::set_tracker_finale_data_1600ns(const std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE> tab_[snemo::electronics::constants::NSIDES][snemo::electronics::constants::NZONES])
    {
      for (unsigned int i = 0; i < snemo::electronics::constants::NSIDES; i++)
	{
	  for (unsigned int j = 0; j < snemo::electronics::constants::NZONES; j++)
	    {
	      _tracker_finale_data_1600ns_[i][j] = tab_[i][j];
	    }
	}

      return;
    }

    void sim_trigger_digi_data::get_tracker_finale_data_1600ns(std::bitset<snemo::electronics::constants::TRACKER_DATA_FULL_BITSET_SIZE> tab_[snemo::electronics::constants::NSIDES][snemo::electronics::constants::NZONES])
    {
      for (unsigned int i = 0; i < snemo::electronics::constants::NSIDES; i++)
	{
	  for (unsigned int j = 0; j < snemo::electronics::constants::NZONES; j++)
	    {
	      tab_[i][j] = _tracker_finale_data_1600ns_[i][j];
	    }
	}
      return;
    }

    void sim_trigger_digi_data::set_coincidence_ZW_S0_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _coincidence_ZW_S0_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_coincidence_ZW_S0_1600ns() const
    {
      return _coincidence_ZW_S0_1600ns_;
    }

    void sim_trigger_digi_data::set_coincidence_ZW_S1_1600ns(const std::bitset<snemo::electronics::constants::NZONES> & bitset_)
    {
      _coincidence_ZW_S1_1600ns_ = bitset_;
      return;
    }

    std::bitset<snemo::electronics::constants::NZONES> sim_trigger_digi_data::get_coincidence_ZW_S1_1600ns() const
    {
      return _coincidence_ZW_S1_1600ns_;
    }

    void sim_trigger_digi_data::reset()
    {
      _L1_decision_25ns_ = false;
      _L1_decision_clocktick_25ns_ = snemo::electronics::constants::INVALID_CLOCKTICK;
      _calo_zoning_word_S0_25ns_.reset();
      _calo_zoning_word_S1_25ns_.reset();
      _calo_HTM_S0_25ns_.reset();
      _calo_HTM_S1_25ns_.reset();
      _calo_HTM_gveto_25ns_.reset();
      _calo_LTO_S0_25ns_ = false;
      _calo_LTO_S1_25ns_ = false;
      _calo_LTO_gveto_25ns_ = false;
      _calo_SS_coinc_25ns_ = false;
      _calo_TTM_25ns_ = false;
      _calo_XT_info_25ns_.reset();
      _L2_decision_1600ns_ = false;
      _L2_decision_clocktick_1600ns_ = snemo::electronics::constants::INVALID_CLOCKTICK;
      _L2_decision_trigger_mode_ = snemo::electronics::constants::INVALID;
      _calo_zoning_word_S0_1600ns_.reset();
      _calo_zoning_word_S1_1600ns_.reset();
      _calo_HTM_S0_1600ns_.reset();
      _calo_HTM_S1_1600ns_.reset();
      _calo_HTM_gveto_1600ns_.reset();
      _calo_LTO_S0_1600ns_ = false;
      _calo_LTO_S1_1600ns_ = false;
      _calo_LTO_gveto_1600ns_ = false;
      _calo_SS_coinc_1600ns_ = false;
      _calo_TTM_1600ns_ = false;
      _calo_XT_info_1600ns_.reset();

      for (unsigned int iside = 0; iside < snemo::electronics::constants::NSIDES; iside++)
	{
	  for (unsigned int izone = 0; izone < snemo::electronics::constants::NZONES; izone++)
	    {
	      _tracker_finale_data_1600ns_[iside][izone].reset();
	    }
	}
      _coincidence_ZW_S0_1600ns_.reset();
      _coincidence_ZW_S1_1600ns_.reset();

      return;
    }

    void sim_trigger_digi_data::tree_dump(std::ostream & out_,
					  const std::string & title_,
					  const std::string & indent_,
					  bool /* inherit_ */) const
    {
      if (!title_.empty()) {
	out_ << indent_ << title_ << std::endl;
      }

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
