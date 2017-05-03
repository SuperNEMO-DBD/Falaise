// -*- mode: c++ ; -*-
/// \file fecom/tracker_hit.ipp

#ifndef FECOM_TRACKER_HIT_IPP
#define FECOM_TRACKER_HIT_IPP 1

// Ourselves:
#include <fecom/tracker_hit.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
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
  void tracker_hit::serialize(Archive & ar,
			      const unsigned int /* version */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("cell_geometric_id", cell_geometric_id);
    ar & boost::serialization::make_nvp("anodic_timestamp_t0", anodic_timestamp_t0);
    ar & boost::serialization::make_nvp("anodic_timestamp_t1", anodic_timestamp_t1);
    ar & boost::serialization::make_nvp("anodic_timestamp_t2", anodic_timestamp_t2);
    ar & boost::serialization::make_nvp("anodic_timestamp_t3", anodic_timestamp_t3);
    ar & boost::serialization::make_nvp("anodic_timestamp_t4", anodic_timestamp_t4);
    ar & boost::serialization::make_nvp("anodic_t0_ns", anodic_t0_ns);
    ar & boost::serialization::make_nvp("anodic_t1_ns", anodic_t1_ns);
    ar & boost::serialization::make_nvp("anodic_t2_ns", anodic_t2_ns);
    ar & boost::serialization::make_nvp("anodic_t3_ns", anodic_t3_ns);
    ar & boost::serialization::make_nvp("anodic_t4_ns", anodic_t4_ns);
    ar & boost::serialization::make_nvp("bot_cathodic_timestamp", bot_cathodic_timestamp);
    ar & boost::serialization::make_nvp("top_cathodic_timestamp", top_cathodic_timestamp);
    ar & boost::serialization::make_nvp("bot_cathodic_time_ns", bot_cathodic_time_ns);
    ar & boost::serialization::make_nvp("top_cathodic_time_ns", top_cathodic_time_ns);

    return;
  }

} // namespace fecom

#endif // FECOM_TRACKER_HIT_IPP
