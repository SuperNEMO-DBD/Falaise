// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calorimeter_digitized_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_IPP 1

// This project:
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>

// Third party:
// - Boost
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template <class Archive>
    void calorimeter_digitized_hit::rtd_origin::serialize(Archive & ar_,
                                                   const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("hit_number", _hit_number_);
      ar_ & boost::serialization::make_nvp("trigger_id", _trigger_id_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void calorimeter_digitized_hit::serialize(Archive & ar_,
                                       const unsigned int version_)
    {
      // Inherit from the 'base_hit' mother class:
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));
      ar_ & boost::serialization::make_nvp("reference_time", _reference_time_);
      ar_ & boost::serialization::make_nvp("low_threshold_only", _low_threshold_only_);
      ar_ & boost::serialization::make_nvp("high_threshold", _high_threshold_);
      ar_ & boost::serialization::make_nvp("fcr", _fcr_);
      if (Archive::is_loading::value and version_ < 1) {
        // Archives with version=0 do not have this field, so we set it at a default valid value
        // in the instance.
        _lt_trigger_counter_ = 0;
      } else {
        ar_ & boost::serialization::make_nvp("lt_trigger_counter", _lt_trigger_counter_);
      }
      if (Archive::is_loading::value and version_ < 1) {
        // Archives with version=0 do not have this field, so we set it at a default valid value
        // in the instance.
        _lt_time_counter_ = 0;
      } else {
        ar_ & boost::serialization::make_nvp("lt_time_counter", _lt_time_counter_);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_WAVEFORM) {
        ar_ & boost::serialization::make_nvp("waveform", _waveform_);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_BASELINE) {
        ar_ & boost::serialization::make_nvp("fwmeas.baseline", _fwmeas_.baseline);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_PEAK_AMPLITUDE) {
        ar_ & boost::serialization::make_nvp("fwmeas.peak_amplitude", _fwmeas_.peak_amplitude);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_PEAK_CELL) {
        ar_ & boost::serialization::make_nvp("fwmeas.peak_cell", _fwmeas_.peak_cell);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_CHARGE) {
        ar_ & boost::serialization::make_nvp("fwmeas.charge", _fwmeas_.charge);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_RISING_CELL) {
        ar_ & boost::serialization::make_nvp("fwmeas.rising_cell", _fwmeas_.rising_cell);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_FWMEAS_FALLING_CELL) {
        ar_ & boost::serialization::make_nvp("fwmeas.falling_cell", _fwmeas_.falling_cell);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_ORIGIN) {
        ar_ & boost::serialization::make_nvp("origin", _origin_);
      }
      if (_store & calorimeter_digitized_hit::STORE_DIGIFLAGS_OM_ID) {
        ar_ & boost::serialization::make_nvp("om_id", _om_id_);
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_IPP
