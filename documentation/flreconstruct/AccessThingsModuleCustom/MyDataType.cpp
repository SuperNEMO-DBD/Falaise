#include "MyDataType.h"

//! Implement the serialization tag mechanism
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(MyDataType,"MyDataType");

//! Constructor
MyDataType::MyDataType() : datatools::i_serializable(), mdtCounter_(0) {
}

//! Destructor
MyDataType::~MyDataType() {
}

void MyDataType::increment() {
  ++mdtCounter_;
}

int MyDataType::current_value() const {
  return mdtCounter_;
}

