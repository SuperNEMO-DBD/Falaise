/// \file falaise/datamodels/sim_calo_digi_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/mctools:
#include <bayeux/mctools/digitization/sampled_signal.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void sim_calo_digi_hit::serialize(Archive& ar, const unsigned int /* version */) {
  ar& boost::serialization::make_nvp(
      "mctools__digitization__sampled_signal",
      boost::serialization::base_object<mctools::digitization::sampled_signal>(*this));
  // ar & boost::serialization::make_nvp("xxx", _xxx_);
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
