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
      return;
    }

    sim_trigger_digi_data::~sim_trigger_digi_data()
    {
      return;
    }

    bool sim_trigger_digi_data::is_valid() const
    {
      return true;
    }
    
    void sim_trigger_digi_data::invalidate()
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
      _calo_LTO_GVETO_25ns_ = false;
      _calo_SS_coinc_25ns_ = false;
      _calo_TTM_25ns_ = false;
      _calo_XT_info_25ns_.reset();
      _L2_decision_1600ns_ = false;
      _L2_decision_clocktick_1600ns_= snemo::electronics::constants::INVALID_CLOCKTICK;
      _L2_decision_trigger_mode_ = snemo::electronics::constants::INVALID;
      _calo_zoning_word_S0_1600ns_.reset();
      _calo_zoning_word_S1_1600ns_.reset();
      _calo_HTM_S0_1600ns_.reset();
      _calo_HTM_S1_1600ns_.reset();
      _calo_HTM_gveto_1600ns_.reset();
      _calo_LTO_S0_1600ns_ = false;
      _calo_LTO_S1_1600ns_ = false;
      _calo_LTO_GVETO_1600ns_ = false;
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

    void sim_trigger_digi_data::reset()
    {
      return;
    }

    void sim_trigger_digi_data::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      // Validity:
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Valid: " << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
