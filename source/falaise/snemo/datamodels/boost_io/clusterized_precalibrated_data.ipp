// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/clusterized_precalibrated_data.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_IPP
#define FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/clusterized_precalibrated_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
// - This project:
#include <falaise/snemo/datamodels/boost_io/precalibrated_cluster.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization
    template <class Archive>
    void clusterized_precalibrated_data::serialize(Archive& ar_, const unsigned int /* version_ */) {
      ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_& boost::serialization::make_nvp("clusters", _clusters_);
      ar_& boost::serialization::make_nvp("unclusterized_calorimeter_hits", _unclusterized_calorimeter_hits_);
      ar_& boost::serialization::make_nvp("unclusterized_tracker_hits", _unclusterized_tracker_hits_);
      ar_& boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_IPP
