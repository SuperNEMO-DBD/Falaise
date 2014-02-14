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

#endif // MYDATATYPE_HH

