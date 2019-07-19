// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/helix_trajectory_pattern.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_IPP
#define FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/helix_3d.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/base_trajectory_pattern.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void helix_trajectory_pattern::serialize(Archive& ar, const unsigned int /* version */) {
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_trajectory_pattern);
  ar& boost::serialization::make_nvp("helix", _helix_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_IPP
