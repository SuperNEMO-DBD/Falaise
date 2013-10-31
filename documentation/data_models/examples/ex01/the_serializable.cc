// Instantiation of serialization code for the Boost I/O library

// Third party
// - Boost
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
// - Bayeux/datatools
#include <datatools/archives_instantiation.h>

// This project
#include "foo.ipp"
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(falaise::example::foo)
// Special implementation macro to register the class through the Boost/Serialization system :
BOOST_CLASS_EXPORT_IMPLEMENT(falaise::example::foo)
