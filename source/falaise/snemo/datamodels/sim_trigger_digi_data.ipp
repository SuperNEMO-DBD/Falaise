/// \file falaise/digitization/datamodels/sim_trigger_digi_data.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_TRIGGER_DIGI_DATA_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.ipp>

// This project:

namespace snemo {

namespace datamodel {

template <class Archive>
void sim_trigger_digi_data::serialize(Archive& ar, const unsigned int /* version */) {
  ar& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
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
