// Ourselves:
#include <fecom/calo_hit.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>

namespace fecom {

  template<class Archive>
  void calo_hit::serialize(Archive & ar,
			   const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
    ar & boost::serialization::make_nvp("event_id", event_id);
    ar & boost::serialization::make_nvp("raw_tdc", raw_tdc);
    ar & boost::serialization::make_nvp("tdc_ns", tdc_ns);
    ar & boost::serialization::make_nvp("low_threshold", low_threshold);
    ar & boost::serialization::make_nvp("high_threshold", high_threshold);
    ar & boost::serialization::make_nvp("low_threshold_trig_count", low_threshold_trig_count);
    ar & boost::serialization::make_nvp("low_threshold_time_count", low_threshold_time_count);
    ar & boost::serialization::make_nvp("waveform_data_size", waveform_data_size);
    ar & boost::serialization::make_nvp("fcr", fcr);
    ar & boost::serialization::make_nvp("raw_waveform_data", raw_waveform_data);
    ar & boost::serialization::make_nvp("raw_baseline", raw_baseline);
    ar & boost::serialization::make_nvp("baseline_volt", baseline_volt);
    ar & boost::serialization::make_nvp("raw_peak", raw_peak);
    ar & boost::serialization::make_nvp("peak_volt", peak_volt);
    ar & boost::serialization::make_nvp("raw_charge", raw_charge);
    ar & boost::serialization::make_nvp("charge_picocoulomb", charge_picocoulomb);
    ar & boost::serialization::make_nvp("raw_charge_overflow", raw_charge_overflow);
    ar & boost::serialization::make_nvp("rising_cell", rising_cell);
    ar & boost::serialization::make_nvp("rising_offset", rising_offset);
    ar & boost::serialization::make_nvp("rising_time_ns", rising_time_ns);
    ar & boost::serialization::make_nvp("falling_cell", falling_cell);
    ar & boost::serialization::make_nvp("falling_offset", falling_offset);
    ar & boost::serialization::make_nvp("falling_time_ns", falling_time_ns);

    return;
  }

} // end of fecom namespace
