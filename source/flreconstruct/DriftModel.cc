// DriftModel.cc - Geiger drift-time relation
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "DriftModel.h"
#include "RawGeigerHit.h"
#include <cmath>

DriftModel::DriftModel()
{
  // As defined in DocDB 786 page 2
  //
  _a  = 0.6759;
  _b  = 0.84;
  _c  = 287.09;
  _d  = 0.0188;
  _tdcResolution = 0.0;
  _delay = 0.0;
}

DriftModel::DriftModel(double a, double b, double c, double d)
: _a(a), _b(b), _c(c), _d(d)
{
  // in case we want to try some different parameters
  _tdcResolution = 0.0;
  _delay = 0.0;
}

double DriftModel::getDist(double time) const
{
  // calculate drift distance (cm) for time (ns)
  //
  double dist = _a*time/(std::pow(time, _b) + _c) + _d; // sheesh!
  return dist;
}

double DriftModel::getDist(const RawGeigerHit& h) const
{
  double time = getDriftTime(h);
  double dist = getDist(time);
  return dist;
}

double DriftModel::getDriftTime(const RawGeigerHit& h) const
{
  // Drift time is trigger_delay - anode_time
  //
  return _delay - _tdcResolution*h.anodeTDC();
}

void DriftModel::setTDCRes(double bin)
{
  _tdcResolution = bin;
}

void DriftModel::setDelay(double delay)
{
  _delay = 1000*delay;  // convert from microseconds to nanoseconds
}
