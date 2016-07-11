//! \file    MyDataType.h
//! \brief   Example custom data type for use with datatools::things
//! \details Store an integer for later use

#ifndef MYDATATYPE_HH
#define MYDATATYPE_HH
// Standard Library

// Third Party
// - Bayeux
#include "bayeux/datatools/i_serializable.h"

// This Project

class MyDataType : public datatools::i_serializable {
 public:
  //! Construct type
  MyDataType();

  //! Destructor
  virtual ~MyDataType();

  //! Increment counter
  void increment();

  //! Return value of counter
  int current_value() const;

  //! Declare serialization interfaces for tagging and streaming
  DATATOOLS_SERIALIZATION_DECLARATION()

 private:
  int mdtCounter_;  //!< Stored counter
};

// Boost/Serialization class registration declaration:
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
BOOST_CLASS_EXPORT_KEY2(MyDataType, "MyDataType")

#endif // MYDATATYPE_HH
