// -*- mode: c++ ; -*-
/// \file fecom/tracker_channel_hit.ipp

#ifndef FECOM_TRACKER_CHANNEL_HIT_IPP
#define FECOM_TRACKER_CHANNEL_HIT_IPP 1

// Ourselves:
#include <fecom/tracker_channel_hit.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>

/** The main Boost/Serialization template method for class calo_hit
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */

namespace fecom {

  template<class Archive>
  void tracker_channel_hit::serialize(Archive & ar,
				      const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
    ar & boost::serialization::make_nvp("timestamp_type", timestamp_type);
    ar & boost::serialization::make_nvp("timestamp_value", timestamp_value);
    ar & boost::serialization::make_nvp("timestamp_time_ns", timestamp_time_ns);
    ar & boost::serialization::make_nvp("associated", associated);
    return;
  }


} // namespace fecom

#endif // FECOM_TRACKER_CHANNEL_HIT_IPP
