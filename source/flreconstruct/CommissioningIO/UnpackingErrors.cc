// UnpackingErrors.cc
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "UnpackingErrors.h"

void UnpackingErrors::add(int code, const std::string& s)
{
  // code is an Unpack::status value
  // s is the associated message
  //
  _errors.push_back(anError(code, s));
}
