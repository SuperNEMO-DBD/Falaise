// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_trajectory_solution.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/tracker_cluster.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_clustering_solution.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_trajectory.ipp>

namespace snemo {

namespace datamodel {

/// Serialization
template <class Archive>
void tracker_trajectory_solution::serialize(Archive& ar_, const unsigned int /* version_ */) {
  ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_& boost::serialization::make_nvp("solution_id", id_);
  ar_& boost::serialization::make_nvp("clustering_solution", solutions_);
  ar_& boost::serialization::make_nvp("trajectories", trajectories_);
  ar_& boost::serialization::make_nvp("unfitted_clusters", unfitted_);
  ar_& boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_SOLUTION_IPP
