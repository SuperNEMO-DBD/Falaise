// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/raw_calorimeter_hit.ipp */

#ifndef FALAISE_SNEMO_DATAMODELS_RAW_CALORIMETER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_RAW_CALORIMETER_HIT_IPP 1

// Ourselves
#include <falaise/snemo/datamodels/raw_calorimeter_hit.h>

// Third party
// - Boost
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template<class Archive>
    void raw_calorimeter_hit::serialize(Archive & ar, const unsigned int /* version */)
    {
      ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar & boost::serialization::make_nvp("trigger_id",            _trigger_id_);
      ar & boost::serialization::make_nvp("channel_address",       _channel_address_);
      ar & boost::serialization::make_nvp("time_info",             _time_info_);
      ar & boost::serialization::make_nvp("data_description",      _data_description_);
      if(has_waveform()) {
        ar & boost::serialization::make_nvp("waveform_first_sample", _waveform_first_sample_);
        ar & boost::serialization::make_nvp("waveform_last_sample",  _waveform_last_sample_);
        ar & boost::serialization::make_nvp("waveform",              _waveform_);
      }
      if(has_aux_baseline()) {
        ar & boost::serialization::make_nvp("aux_baseline",          _aux_baseline_);
      }
      if(has_aux_peak_charge()) {
        ar & boost::serialization::make_nvp("aux_peak_charge",       _aux_peak_charge_);
      }
      if(has_aux_peak_data()) {
        ar & boost::serialization::make_nvp("aux_peak_amplitude",    _aux_peak_amplitude_);
        ar & boost::serialization::make_nvp("aux_peak_time_max_amplitude",  _aux_peak_time_max_amplitude_);
        ar & boost::serialization::make_nvp("aux_underflow_tot",     _aux_underflow_tot_);
      }
      if(has_aux_cfd_interpolated_time()) {
        ar & boost::serialization::make_nvp("aux_time_cfd_interp",   _aux_time_cfd_interp_);
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_RAW_CALORIMETER_HIT_IPP
