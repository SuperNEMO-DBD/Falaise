// -*- mode: c++; -*-
/// the_serializable.hpp
#ifndef THE_SERIALIZABLE_HPP
#define THE_SERIALIZABLE_HPP

// Third Party
// - Boost:
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// Include the specific serialization code of serializable classes:
#include <fecom/base_hit.ipp>
#include <fecom/calo_hit.ipp>
#include <fecom/tracker_channel_hit.ipp>
#include <fecom/commissioning_event.ipp>

// Force instantiation of template code associated to the classes above
// for the Boost archive classes defined in Bayeux/datatools:
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(fecom::base_hit)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(fecom::calo_hit)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(fecom::tracker_channel_hit)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(fecom::commissioning_event)

// Special implentation macro to register the class through the
// Boost/Serialization system :
BOOST_CLASS_EXPORT_IMPLEMENT(fecom::commissioning_event)

#endif // THE_SERIALIZABLE_HPP
