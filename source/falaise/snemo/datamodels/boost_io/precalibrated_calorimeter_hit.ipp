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
      ar& boost::serialization::make_nvp("amplitude", _amplitude_);
      ar& boost::serialization::make_nvp("sigma_amplitude", _sigma_amplitude_);
      ar& boost::serialization::make_nvp("charge", _charge_);
      ar& boost::serialization::make_nvp("sigma_charge", _sigma_charge_);
      ar& boost::serialization::make_nvp("time", _time_);
      ar& boost::serialization::make_nvp("sigma_time", _sigma_time_);
      ar& boost::serialization::make_nvp("baseline", _baseline_);
      ar& boost::serialization::make_nvp("sigma_baseline", _sigma_baseline_);
      ar& boost::serialization::make_nvp("rising_time", _rising_time_);
      ar& boost::serialization::make_nvp("sigma_rising_time", _sigma_rising_time_);
      ar& boost::serialization::make_nvp("falling_time", _falling_time_);
      ar& boost::serialization::make_nvp("sigma_falling_time", _sigma_falling_time_);
      ar& boost::serialization::make_nvp("width", _width_);
      ar& boost::serialization::make_nvp("sigma_width", _sigma_width_);

    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_IPP
