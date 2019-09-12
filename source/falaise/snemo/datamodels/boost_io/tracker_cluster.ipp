// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_cluster.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/tracker_cluster.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/calibrated_tracker_hit.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void tracker_cluster::serialize(Archive& ar_, const unsigned int /* version_ */) {
  ar_& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
  ar_& boost::serialization::make_nvp("hits", hits_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_IPP
