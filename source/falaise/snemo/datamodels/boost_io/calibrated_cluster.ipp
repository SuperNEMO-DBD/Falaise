// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_cluster.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_CLUSTER_IPP
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_CLUSTER_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_cluster.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {
   
    template <class Archive>
    void calibrated_cluster::serialize(Archive & ar_, const unsigned int version_)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar_ & boost::serialization::make_nvp("calorimeter_hits", _calorimeter_hits_);
      ar_ & boost::serialization::make_nvp("tracker_hits", _tracker_hits_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALIBRATED_CLUSTER_IPP
