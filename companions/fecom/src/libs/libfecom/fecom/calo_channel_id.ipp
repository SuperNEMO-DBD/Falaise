// -*- mode: c++ ; -*-
/// \file fecom/calo_channel_id.ipp

#ifndef FECOM_CALO_CHANNEL_ID_IPP
#define FECOM_CALO_CHANNEL_ID_IPP 1

// Ourselves:
#include <fecom/calo_channel_id.hpp>

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
  void calo_channel_id::serialize(Archive & ar,
				  const unsigned int /* version */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("board_id", board_id);
    ar & boost::serialization::make_nvp("channel_id", channel_id);
    return;
  }

} // namespace fecom

#endif // FECOM_CALO_CHANNEL_ID_IPP
