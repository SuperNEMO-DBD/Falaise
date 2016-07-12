// Serialization implementation for MyDataType
//
//----------------------------------------------------------------------
// In this first section, we implement the read/write "serialize method.
// If, and only if, you expect MyDataType to be inherited from (and this
// is *not* recommended for data types), this section should go into a
// separate "MyDataType-xxx.ipp" header file. This allows it to
// be #included by derived classes' own "DerivedData-xxx.ipp" files
// which is required to propaget the serialization correctly.

// Interface for the class we're serializing
#include "MyDataType.h"

// Implement the serialize method
// - Boost:
//#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux
#include <bayeux/datatools/i_serializable.ipp>

template<class Archive>
void MyDataType::serialize(Archive& ar, const unsigned int /*version_*/)
{
  // Serialize the base class first
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  // Now the concrete data members *in order*
  // Note the use of Boost's "make_nvp" function, "nvp" being
  // "name-value pair". This creates an effective map in the archive.
  // It's a template method, so easy to use for most types.
  ar & boost::serialization::make_nvp("mdtCounter", mdtCounter_);
}

//----------------------------------------------------------------------
// This second section adds the boilerplate for registering the class
// with the Boost Serialization core. It ensures that all needed code
// for serialization is instantiated (TOD: add clearer explanation of
// what's happening here, bottom line, allows use when loaded as
// a plugin or linked as library without exposing details to clients).

// Include the headers for the file formats we want code exported for
// This must come before the BOOST_CLASS_EXPORT_IMPLEMENT expansion,
// then code export is automatic.
#include "bayeux/datatools/archives_instantiation.h"
// Boost.Serialization class export definition
BOOST_CLASS_EXPORT_IMPLEMENT(MyDataType)


