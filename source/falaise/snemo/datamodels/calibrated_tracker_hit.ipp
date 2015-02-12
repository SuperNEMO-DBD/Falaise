// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_tracker_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template<class Archive>
    void calibrated_tracker_hit::serialize(Archive & ar_,
                                           const unsigned int version_)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      if (version_ == 0 && Archive::is_loading::value) {
        _traits_ = 0x0;
        ar_ & boost::serialization::make_nvp("r",       _r_);
        ar_ & boost::serialization::make_nvp("sigma_r", _sigma_r_);
        ar_ & boost::serialization::make_nvp("z",       _z_);
        ar_ & boost::serialization::make_nvp("sigma_z", _sigma_z_);
        datatools::invalidate(_x_);
        datatools::invalidate(_y_);
        ar_ & boost::serialization::make_nvp("x", _x_);
        ar_ & boost::serialization::make_nvp("y", _y_);
        if(datatools::is_valid(_x_) && datatools::is_valid(_y_)) {
          _set_trait_bit(true, xy);
        }
        datatools::invalidate(_delayed_time_);
        datatools::invalidate(_delayed_time_error_);
        return;
      }

      // From version 1 :
      ar_ & boost::serialization::make_nvp("traits", _traits_);
      ar_ & boost::serialization::make_nvp("r",       _r_);
      ar_ & boost::serialization::make_nvp("sigma_r", _sigma_r_);
      ar_ & boost::serialization::make_nvp("z",       _z_);
      ar_ & boost::serialization::make_nvp("sigma_z", _sigma_z_);
      if (has_xy()) {
        ar_ & boost::serialization::make_nvp("x", _x_);
        ar_ & boost::serialization::make_nvp("y", _y_);
      }
      if (is_delayed()) {
        ar_ & boost::serialization::make_nvp("delayed_time",       _delayed_time_);
        ar_ & boost::serialization::make_nvp("delayed_time_error", _delayed_time_error_);
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_IPP
