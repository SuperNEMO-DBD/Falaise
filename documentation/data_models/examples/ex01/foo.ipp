// -*- mode: c++; -*-

#ifndef FALAISE_FOO_IPP
#define FALAISE_FOO_IPP

// Ourselves
#include "foo.h"

// Third party
// - Boost (support for XML 'key-value' based archives)
#include <boost/serialization/nvp.hpp>

// - Datatools (support for serializable objects)
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

namespace falaise {
namespace example {

/** The main Boost/Serialization template method for class falaise::foo
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used here)
 */
template<class Archive>
void foo::serialize(Archive & ar, const unsigned int version)
{
  // Macro to support inheritance from the "datatools::i_serializable" class
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  // Serialization of attributes:
  ar & boost::serialization::make_nvp("id", _id_);
  ar & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
  return;
}

} // end of namespace example
} // end of namespace falaise

#endif // FALAISE_FOO_IPP
