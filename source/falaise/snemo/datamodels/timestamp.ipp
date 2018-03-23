// -*- mode: c++ ; -*-
/// \file falaise/snemo/datatmodels/timestamp.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_TIMESTAMP_IPP
#define FALAISE_SNEMO_DATAMODEL_TIMESTAMP_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/timestamp.h>

// Third party:
// - Boost:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void timestamp::serialize(Archive& ar_, const unsigned int version_) {
  if (version_ > 0) {
    ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  ar_& boost::serialization::make_nvp("seconds", _seconds_);
  ar_& boost::serialization::make_nvp("picoseconds", _picoseconds_);
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_TIMESTAMP_IPP
