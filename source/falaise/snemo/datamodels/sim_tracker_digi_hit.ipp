/// \file falaise/datamodels/sim_tracker_digi_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template<class Archive>
    void sim_tracker_digi_hit::serialize(Archive & ar, const unsigned int /* version */)
    {
      ar & boost::serialization::make_nvp("geomtools__base_hit",
                                          boost::serialization::base_object<geomtools::base_hit>(*this));
      if (has_anode_t0()) {
        ar & boost::serialization::make_nvp("anode_t0", _anode_t0_);
      }
      if (has_anode_t1()) {
        ar & boost::serialization::make_nvp("anode_t1", _anode_t1_);
      }
      if (has_anode_t2()) {
        ar & boost::serialization::make_nvp("anode_t2", _anode_t2_);
      }
      if (has_anode_t3()) {
        ar & boost::serialization::make_nvp("anode_t3", _anode_t3_);
      }
      if (has_anode_t4()) {
        ar & boost::serialization::make_nvp("anode_t4", _anode_t4_);
      }
      if (has_cathode_t5()) {
        ar & boost::serialization::make_nvp("cathode_t5", _cathode_t5_);
      }
      if (has_cathode_t6()) {
        ar & boost::serialization::make_nvp("cathode_t6", _cathode_t6_);
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
