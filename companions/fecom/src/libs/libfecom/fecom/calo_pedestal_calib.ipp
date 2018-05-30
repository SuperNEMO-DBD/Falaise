// -*- mode: c++ ; -*-
/// \file fecom/calo_pedestal_calib.ipp

#ifndef FECOM_CALO_PEDESTAL_CALIB_IPP
#define FECOM_CALO_PEDESTAL_CALIB_IPP 1

// Ourselves:
#include <fecom/calo_pedestal_calib.hpp>

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
  void calo_pedestal_calib::serialize(Archive & ar,
				      const unsigned int /* version */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("board_id", board_id);
    ar & boost::serialization::make_nvp("channel", channel);
    ar & boost::serialization::make_nvp("offset_size", offset_size);
    ar & boost::serialization::make_nvp("offset", offset);
    return;
  }

} // namespace fecom

#endif // FECOM_CALO_PEDESTAL_CALIB_IPP
