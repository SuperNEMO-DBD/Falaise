// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_tracker_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void precalibrated_tracker_hit::serialize(Archive& ar_, const unsigned int /* version_ */) {
      ar_& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar& boost::serialization::make_nvp("reference_time", _reference_time_);
      ar& boost::serialization::make_nvp("anodic_drift_time", _anodic_drift_time_);
      ar& boost::serialization::make_nvp("sigma_anodic_drift_time", _sigma_anodic_drift_time_);
      ar& boost::serialization::make_nvp("bottom_cathode_drift_time", _bottom_cathode_drift_time_);
      ar& boost::serialization::make_nvp("sigma_bottom_cathode_drift_time", _sigma_bottom_cathode_drift_time_);
      ar& boost::serialization::make_nvp("top_cathode_drift_time", _top_cathode_drift_time_);
      ar& boost::serialization::make_nvp("sigma_top_cathode_drift_time", _sigma_top_cathode_drift_time_);
      ar& boost::serialization::make_nvp("is_delayed", _is_delayed_);

    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CALORIMETER_HIT_IPP
