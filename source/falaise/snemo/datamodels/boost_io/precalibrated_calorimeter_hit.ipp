// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_calorimeter_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_IPP 1

// Ourselves
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>

// Third party
// - Boost
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template <class Archive>
    void precalibrated_calorimeter_hit::serialize(Archive& ar, const unsigned int /* version */) {
      ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      // ar& boost::serialization::make_nvp("energy", energy_);
      // ar& boost::serialization::make_nvp("sigma_energy", sigma_energy_);
      // ar& boost::serialization::make_nvp("time", time_);
      // ar& boost::serialization::make_nvp("sigma_time", sigma_time_);
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_IPP
