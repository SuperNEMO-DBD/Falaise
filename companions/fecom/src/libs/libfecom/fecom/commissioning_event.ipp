// -*- mode: c++ ; -*-
/// \file fecom/commissioning_event.ipp

#ifndef FECOM_COMMISSIONING_EVENT_IPP
#define FECOM_COMMISSIONING_EVENT_IPP 1

// Ourselves:
#include <fecom/commissioning_event.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>
#include <datatools/event_id.ipp>

// This project:
#include <fecom/calo_hit.ipp>
#include <fecom/tracker_hit.ipp>
#include <fecom/tracker_channel_hit.ipp>

/** The main Boost/Serialization template method for class calo_hit
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */

namespace fecom {

  template<class Archive>
  void commissioning_event::serialize(Archive & ar,
				      const unsigned int /* version */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("event_id", _event_id_);
    ar & boost::serialization::make_nvp("time_start_ns", _time_start_ns_);
    ar & boost::serialization::make_nvp("traits", _traits_);
    ar & boost::serialization::make_nvp("calo_hit_collection", _calo_hit_collection_);
    ar & boost::serialization::make_nvp("tracker_channel_hit_collection", _tracker_channel_hit_collection_);
    ar & boost::serialization::make_nvp("tracker_hit_collection", _tracker_hit_collection_);
    return;
  }

} // namespace fecom

#endif // FECOM_COMMISSIONING_EVENT_IPP
