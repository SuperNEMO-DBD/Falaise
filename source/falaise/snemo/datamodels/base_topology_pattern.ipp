// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/base_topology_pattern.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_IPP
#define FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/base_topology_pattern.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace snemo {

namespace datamodel {

/// Serialization method
template <class Archive>
void base_topology_pattern::serialize(Archive& ar_, const unsigned int /* version_ */) {
  ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_& boost::serialization::make_nvp("particle_tracks", _tracks_);
  ar_& boost::serialization::make_nvp("measurements", _meas_);
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_PATTERN_IPP
