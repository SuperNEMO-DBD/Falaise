// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/raw_tracker_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_RAW_TRACKER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_RAW_TRACKER_HIT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/raw_tracker_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

namespace datamodel {

/// Serialization method
template <class Archive>
void raw_tracker_hit::serialize(Archive& ar, const unsigned int /* version */) {
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
  ar& boost::serialization::make_nvp("trigger_id", _trigger_id_);
  ar& boost::serialization::make_nvp("channel_address", _channel_address_);
  ar& boost::serialization::make_nvp("data_description", _data_description_);
  if (has_anode_t0()) {
    ar& boost::serialization::make_nvp("anode_t0", _anode_t0_);
    if (has_anode_t1()) {
      ar& boost::serialization::make_nvp("anode_t1", _anode_t1_);
    }
    if (has_anode_t2()) {
      ar& boost::serialization::make_nvp("anode_t2", _anode_t2_);
    }
    if (has_anode_t3()) {
      ar& boost::serialization::make_nvp("anode_t3", _anode_t3_);
    }
    if (has_anode_t4()) {
      ar& boost::serialization::make_nvp("anode_t4", _anode_t4_);
    }
    if (has_cathode_t5()) {
      ar& boost::serialization::make_nvp("cathode_t5", _cathode_t5_);
    }
    if (has_cathode_t6()) {
      ar& boost::serialization::make_nvp("cathode_t6", _cathode_t6_);
    }
  }
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_RAW_TRACKER_HIT_IPP
