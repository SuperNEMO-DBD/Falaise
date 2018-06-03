/// \file falaise/digitization/datamodels/sim_trigger_digi_data.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/bitset.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.ipp>

// This project:

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void sim_trigger_digi_data::serialize(Archive& ar, const unsigned int /* version */)
    {
      ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar & boost::serialization::make_nvp("trigger_id"                  , _trigger_id_);
      ar & boost::serialization::make_nvp("L1_decision_25ns"            , _L1_decision_25ns_);
      ar & boost::serialization::make_nvp("L1_decision_clocktick_25ns"  , _L1_decision_clocktick_25ns_);
      ar & boost::serialization::make_nvp("calo_zoning_word_S0_25ns"    , _calo_zoning_word_S0_25ns_);
      ar & boost::serialization::make_nvp("alo_zoning_word_S1_25ns"     , _calo_zoning_word_S1_25ns_);
      ar & boost::serialization::make_nvp("calo_HTM_S0_25ns"            , _calo_HTM_S0_25ns_);
      ar & boost::serialization::make_nvp("calo_HTM_S1_25ns"            , _calo_HTM_S1_25ns_);
      ar & boost::serialization::make_nvp("calo_HTM_gveto_25ns"         , _calo_HTM_gveto_25ns_);
      ar & boost::serialization::make_nvp("calo_LTO_S0_25ns"            , _calo_LTO_S0_25ns_);
      ar & boost::serialization::make_nvp("calo_LTO_S1_25ns"            , _calo_LTO_S1_25ns_);
      ar & boost::serialization::make_nvp("calo_LTO_gveto_25ns"         , _calo_LTO_gveto_25ns_);
      ar & boost::serialization::make_nvp("calo_SS_coinc_25ns"          , _calo_SS_coinc_25ns_);
      ar & boost::serialization::make_nvp("calo_TTM_25ns"               , _calo_TTM_25ns_);
      ar & boost::serialization::make_nvp("calo_XT_info_25ns"           , _calo_XT_info_25ns_);
      ar & boost::serialization::make_nvp("L2_decision_1600ns"          , _L2_decision_1600ns_);
      ar & boost::serialization::make_nvp("L2_decision_clocktick_1600ns", _L2_decision_clocktick_1600ns_);
      ar & boost::serialization::make_nvp("L2_decision_trigger_mode"    , _L2_decision_trigger_mode_);
      ar & boost::serialization::make_nvp("calo_zoning_word_S0_1600ns"  , _calo_zoning_word_S0_1600ns_);
      ar & boost::serialization::make_nvp("calo_zoning_word_S1_1600ns"  , _calo_zoning_word_S1_1600ns_);
      ar & boost::serialization::make_nvp("calo_HTM_S0_1600ns"          , _calo_HTM_S0_1600ns_);
      ar & boost::serialization::make_nvp("calo_HTM_S1_1600ns"          , _calo_HTM_S1_1600ns_);
      ar & boost::serialization::make_nvp("calo_HTM_gveto_1600ns"       , _calo_HTM_gveto_1600ns_);
      ar & boost::serialization::make_nvp("calo_LTO_S0_1600ns"          , _calo_LTO_S0_1600ns_);
      ar & boost::serialization::make_nvp("calo_LTO_S1_1600ns"          , _calo_LTO_S1_1600ns_);
      ar & boost::serialization::make_nvp("calo_LTO_gveto_1600ns"       , _calo_LTO_gveto_1600ns_);
      ar & boost::serialization::make_nvp("calo_SS_coinc_1600ns"        , _calo_SS_coinc_1600ns_);
      ar & boost::serialization::make_nvp("calo_TTM_1600ns"             , _calo_TTM_1600ns_);
      ar & boost::serialization::make_nvp("calo_XT_info_1600ns"         , _calo_XT_info_1600ns_);
      ar & boost::serialization::make_nvp("tracker_finale_data_1600ns"  , _tracker_finale_data_1600ns_);
      ar & boost::serialization::make_nvp("coincidence_ZW_S0_1600ns"    , _coincidence_ZW_S0_1600ns_);
      ar & boost::serialization::make_nvp("coincidence_ZW_S1_1600ns"    , _coincidence_ZW_S1_1600ns_);

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
