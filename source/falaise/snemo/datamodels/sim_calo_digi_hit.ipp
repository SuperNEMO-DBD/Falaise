/// \file falaise/datamodels/sim_calo_digi_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/mctools:
#include <bayeux/geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void sim_calo_digi_hit::serialize(Archive& ar, const unsigned int /* version */)
    {
      ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar & boost::serialization::make_nvp("elec_id", _elec_id_);
      ar & boost::serialization::make_nvp("trigger_id", _trigger_id_);
      ar & boost::serialization::make_nvp("is_LTO", _is_LTO_);
      ar & boost::serialization::make_nvp("is_LT", _is_LT_);
      ar & boost::serialization::make_nvp("is_HT", _is_HT_);
      ar & boost::serialization::make_nvp("LT_CT_25", _LT_CT_25_);
      ar & boost::serialization::make_nvp("HT_CT_25", _HT_CT_25_);
      ar & boost::serialization::make_nvp("timestamp", _timestamp_);
      ar & boost::serialization::make_nvp("baseline", _baseline_);
      ar & boost::serialization::make_nvp("peak",   _peak_);
      ar & boost::serialization::make_nvp("charge", _charge_);
      ar & boost::serialization::make_nvp("charge_overflow", _charge_overflow_);
      ar & boost::serialization::make_nvp("rising_cell",    _rising_cell_);
      ar & boost::serialization::make_nvp("rising_offset",  _rising_offset_);
      ar & boost::serialization::make_nvp("falling_cell",   _falling_cell_);
      ar & boost::serialization::make_nvp("falling_offset", _falling_offset_);
      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
