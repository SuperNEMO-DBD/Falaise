// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_trajectory.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/base_trajectory_pattern.ipp>
#include <falaise/snemo/datamodels/boost_io/calibrated_tracker_hit.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_cluster.ipp>

namespace snemo {

namespace datamodel {

/// Serialization
template <class Archive>
void tracker_trajectory::serialize(Archive& ar, const unsigned int /* version */) {
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
  ar& boost::serialization::make_nvp("cluster", _cluster_);
  ar& boost::serialization::make_nvp("orphans", _orphans_);
  ar& boost::serialization::make_nvp("pattern", _pattern_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP
