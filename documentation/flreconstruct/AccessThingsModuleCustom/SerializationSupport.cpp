// Third party:
// - Boost:

// Load the serialization template implementation code
// for the class of interest:
#include "MyDataType-serial.ipp"

// - Bayeux/datatools:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include "bayeux/datatools/archives_instantiation.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// Generate the template code for various archive types
// (text, portable binary, xml) supported by the
// Boost/Serialization-based Bayeux/Falaise I/O system:
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(MyDataType)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/serialization/export.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Boost/Serialization class registration implementation:
BOOST_CLASS_EXPORT_IMPLEMENT(MyDataType)
