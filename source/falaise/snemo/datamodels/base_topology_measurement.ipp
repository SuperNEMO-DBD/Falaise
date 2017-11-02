// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/base_topology_measurement.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_IPP
#define FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/base_topology_measurement.h>

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
void base_topology_measurement::serialize(Archive& ar_, const unsigned int /* version_ */) {
  ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_BASE_TOPOLOGY_MEASUREMENT_IPP
