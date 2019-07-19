// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_data.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_IPP
#define FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/calibrated_calorimeter_hit.ipp>
#include <falaise/snemo/datamodels/boost_io/calibrated_tracker_hit.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void calibrated_data::serialize(Archive& ar, const unsigned int version) {
  if (version > 0) {
    ar& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  ar& boost::serialization::make_nvp("calibrated_calorimeter_hits", _calibrated_calorimeter_hits_);
  ar& boost::serialization::make_nvp("calibrated_tracker_hits", _calibrated_tracker_hits_);
  ar& boost::serialization::make_nvp("properties", _properties_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_CALIBRATED_DATA_IPP
