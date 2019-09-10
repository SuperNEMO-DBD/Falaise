// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_tracker_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

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
void calibrated_tracker_hit::serialize(Archive& ar_, const unsigned int version_) {
  ar_& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
  if (version_ == 0 && Archive::is_loading::value) {
    traits_ = 0x0;
    ar_& boost::serialization::make_nvp("r", r_);
    ar_& boost::serialization::make_nvp("sigma_r", sigma_r_);
    ar_& boost::serialization::make_nvp("z", z_);
    ar_& boost::serialization::make_nvp("sigma_z", sigma_z_);
    datatools::invalidate(x_);
    datatools::invalidate(y_);
    ar_& boost::serialization::make_nvp("x", x_);
    ar_& boost::serialization::make_nvp("y", y_);
    if (datatools::is_valid(x_) && datatools::is_valid(y_)) {
      set_trait_bit(true, xy);
    }
    datatools::invalidate(delayed_time_);
    datatools::invalidate(delayed_time_error_);
  }

  // From version 1 :
  ar_& boost::serialization::make_nvp("traits", traits_);
  ar_& boost::serialization::make_nvp("r", r_);
  ar_& boost::serialization::make_nvp("sigma_r", sigma_r_);
  ar_& boost::serialization::make_nvp("z", z_);
  ar_& boost::serialization::make_nvp("sigma_z", sigma_z_);
  if (has_xy()) {
    ar_& boost::serialization::make_nvp("x", x_);
    ar_& boost::serialization::make_nvp("y", y_);
  }
  if (is_delayed()) {
    ar_& boost::serialization::make_nvp("delayed_time", delayed_time_);
    ar_& boost::serialization::make_nvp("delayed_time_error", delayed_time_error_);
  }
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP
