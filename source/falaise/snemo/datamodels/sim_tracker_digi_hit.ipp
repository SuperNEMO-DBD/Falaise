/// \file falaise/datamodels/sim_tracker_digi_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void sim_tracker_digi_hit::serialize(Archive& ar, const unsigned int /* version */)
    {
      ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar & boost::serialization::make_nvp("elec_id", _elec_id_);
      ar & boost::serialization::make_nvp("trigger_id", _trigger_id_);
      ar & boost::serialization::make_nvp("anode_R0", _anode_R0_);
      ar & boost::serialization::make_nvp("anode_R1", _anode_R1_);
      ar & boost::serialization::make_nvp("anode_R2", _anode_R2_);
      ar & boost::serialization::make_nvp("anode_R3", _anode_R3_);
      ar & boost::serialization::make_nvp("anode_R4", _anode_R4_);
      ar & boost::serialization::make_nvp("cathode_R5", _cathode_R5_);
      ar & boost::serialization::make_nvp("cathode_R6", _cathode_R6_);

      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
