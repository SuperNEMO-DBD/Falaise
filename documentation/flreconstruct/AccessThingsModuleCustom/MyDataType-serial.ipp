//! \file    MyDataType-serial.ipp
//! \brief   Boost/Serialization method implementation

#ifndef MYDATATYPE_SERIAL_IPP
#define MYDATATYPE_SERIAL_IPP

// Ourselves:
#include "MyDataType.h"

// Third Party
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux
#include <datatools/i_serializable.ipp>

template<class Archive>
void MyDataType::serialize (Archive & ar_, const unsigned int version_)
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp("mdtCounter", mdtCounter_);
  return;
}


#endif // MYDATATYPE_SERIAL_IPP
