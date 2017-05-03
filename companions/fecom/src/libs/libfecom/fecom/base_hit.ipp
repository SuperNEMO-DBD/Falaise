// -*- mode: c++ ; -*-
/// \file fecom/base_hit.ipp

#ifndef FECOM_BASE_HIT_IPP
#define FECOM_BASE_HIT_IPP 1

// This project:
#include <fecom/base_hit.hpp>
// To see if it's necessary :
// #include <geomtools/geom_id.ipp>

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
  void base_hit::serialize(Archive & ar,
			   const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("hit_id",  hit_id);
    ar & boost::serialization::make_nvp("hitmode", hitmode);
    ar & boost::serialization::make_nvp("electronic_id", electronic_id);
    ar & boost::serialization::make_nvp("trigger_id", trigger_id);
    return;
  }

} // end of fecom namespace

#endif // FECOM_BASE_HIT_IPP
