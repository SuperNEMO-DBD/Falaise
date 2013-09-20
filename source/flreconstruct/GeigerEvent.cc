// GeigerEvent.cc
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "GeigerEvent.h"

GeigerEvent::GeigerEvent()
{
  _evtNum = -666; // invalid.
}

void GeigerEvent::newEvent(long evno)
{
  _evtNum = evno;
  _hits.clear();
}

void GeigerEvent::addHit(const RawGeigerHit &hit)
{
  _hits.push_back(hit);
}
