// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_data.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_IPP
#define FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/precalibrated_calorimeter_hit.ipp>
#include <falaise/snemo/datamodels/boost_io/precalibrated_tracker_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void precalibrated_data::serialize(Archive& ar_, const unsigned int version) {
      if (version > 0) {
	ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
      ar_ & boost::serialization::make_nvp("precalibrated_calorimeter_hits", calorimeter_hits_);
      ar_ & boost::serialization::make_nvp("precalibrated_tracker_hits", tracker_hits_);
      ar_ & boost::serialization::make_nvp("properties", _properties_);
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_PRECALIBRATED_DATA_IPP
