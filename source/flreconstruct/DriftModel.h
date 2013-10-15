#ifndef DRIFTMODEL_HPP
#define DRIFTMODEL_HPP
// DriftModel.h - Parameterisation of the Geiger Cell drift time relation
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
class RawGeigerHit;

class DriftModel
{
  public:
    DriftModel();
    DriftModel(double a, double b, double c, double d);
    double getDist(double time) const; // calculate distance from drift time.
    double getDist(const RawGeigerHit& h) const;  // calculate from TDC data.
    double getDriftTime(const RawGeigerHit& h) const;  // convert TDC to time.
    void setTDCRes(double bin);   // used to convert raw tdc counts to time.
    void setDelay(double delay);  // specified by trigger.
  private:
    double _a, _b, _c, _d;  // parameters. DocDB 786 page 2
    double _tdcResolution;  // we assume only ONE value for all TDCs ??
    double _delay;          // trigger delay time.

};

#endif    /* DRIFTMODEL_HPP */
