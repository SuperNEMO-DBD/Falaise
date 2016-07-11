// Third party:
// - Boost:

// Load the serialization template implementation code
// for the class of interest:
#include "MyDataType-serial.ipp"

// - Bayeux/datatools:
// Note: maybe the pragmas could be removed...
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
// 2016-07-11 FM: it seems that if I remove this macro above, the system
// still works as long as the 'bayeux/datatools/archives_instantiation.h'
// is included. AFAIR, it was not the case with former versions of the
// Boost/library. I let the macro here because I'm not sure yet of possible
// side-effects if it is removed.

// Boost/Serialization class registration implementation:
BOOST_CLASS_EXPORT_IMPLEMENT(MyDataType)
