// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/boost_io/vertex_utils.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_VERTEX_UTILS_IPP
#define FALAISE_SNEMO_DATAMODELS_VERTEX_UTILS_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/vertex_utils.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.ipp>
#include <bayeux/geomtools/blur_spot.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization
    template <class Archive>
    void vertex::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar_ & boost::serialization::make_nvp("category", _category_);
      ar_ & boost::serialization::make_nvp("from", _from_);
      ar_ & boost::serialization::make_nvp("extrapolation", _extrapolation_);
      ar_ & boost::serialization::make_nvp("spot", _spot_);
      ar_ & boost::serialization::make_nvp("distance", _distance_);
      ar_ & boost::serialization::make_nvp("distance_xy", _distance_xy_);
      ar_ & boost::serialization::make_nvp("edge", _edge_);
      ar_ & boost::serialization::make_nvp("best", _best_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_IPP
