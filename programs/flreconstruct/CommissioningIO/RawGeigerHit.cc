// RawGeigerHit.cc
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "RawGeigerHit.h"
#include <stdexcept>
#include <iostream>

RawGeigerHit::RawGeigerHit(const RawGeigerHit &h)
{
  _module = h._module;
  _side = h._side;
  _layer = h._layer;
  _column = h._column;
  _anodeTDC = h._anodeTDC;
  _cathodeTopTDC = h._cathodeTopTDC;
  _cathodeBotTDC = h._cathodeBotTDC;
  _alphaTDC = h._alphaTDC;
  _validData = h._validData;
  _validAddress = h._validAddress;
}

RawGeigerHit& RawGeigerHit::operator=(const RawGeigerHit &h)
{
  if (this != &h)
  {
    _module = h._module;
    _side = h._side;
    _layer = h._layer;
    _column = h._column;
    _anodeTDC = h._anodeTDC;
    _cathodeTopTDC = h._cathodeTopTDC;
    _cathodeBotTDC = h._cathodeBotTDC;
    _alphaTDC = h._alphaTDC;
    _validData = h._validData;
    _validAddress = h._validAddress;
  }
  return *this;
}

void RawGeigerHit::setAddress(int m, int s, int l, int c)
{
  if (_validAddress) throw std::logic_error("Hit data D0 out of order");
  _module = m;
  _side = s;
  _layer = l;
  _column = c;
  _validAddress = true;
  _validData = false;
}

void RawGeigerHit::setData(int an, int ct, int cb, int al)
{
  if (!_validAddress) throw std::logic_error("Hit data D1 out of order");
  _anodeTDC = an;
  _cathodeTopTDC = ct;
  _cathodeBotTDC = cb;
  _alphaTDC = al;
  _validData = true;
  _validAddress = false;
}

std::ostream& operator<<(std::ostream &out, const RawGeigerHit &h)
{
  if (h.isValid())
  {
    out << "RawGeigerHit address:\n"
        << "module: " << h._module << '\n'
        << "side:   " << h._side << '\n'
        << "layer:  " << h._layer << '\n'
        << "column: " << h._column << '\n'
        << "RawGeigerHit TDCs:\n"
        << "anodeTDC:      " << h._anodeTDC << '\n'
        << "cathodeTopTDC: " << h._cathodeTopTDC << '\n'
        << "cathodeBotTDC: " << h._cathodeBotTDC << '\n'
        << "alphaTDC:      " << h._alphaTDC << std::endl;
  }
  else
  {
    out << "RawGeigerHit: WARNING, INVALID DATA!!" << std::endl;
  }
  return out;
}
