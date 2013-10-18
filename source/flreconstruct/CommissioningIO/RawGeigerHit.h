#ifndef RAWGEIGERHIT_HPP
#define RAWGEIGERHIT_HPP
// RawGeigerHit.h
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
// Simple container for raw hit data.
//
// This is little more than a struct with methods to allow two-stage
// creation of a complete hit necessitated by the annoying raw data format.
//
#include <iosfwd>

class RawGeigerHit
{
  public:

    // The default constructor creates a hit in an invalid state.
    //
    RawGeigerHit()
    : _validAddress(false), _validData(false)
    {}

    RawGeigerHit(const RawGeigerHit &h);
    RawGeigerHit& operator=(const RawGeigerHit &h);

    // Methods to fill in hit data in two stages.
    //
    void setAddress(int m, int s, int l, int c);
    void setData(int an, int ct, int cb, int al);

    // access methods
    //
    bool isValid() const { return _validData; }
    int module() const { return _module; }
    int side() const { return _side; }
    int layer() const { return _layer; }
    int column() const { return _column; }
    int anodeTDC() const { return _anodeTDC; }
    int cathodeTopTDC() const { return _cathodeTopTDC; }
    int cathodeBotTDC() const { return _cathodeBotTDC; }
    int alphaTDC() const { return _alphaTDC; }

    friend std::ostream& operator<<(std::ostream &s, const RawGeigerHit &h);

  private:
    int _module, _side, _layer, _column;  // Geiger cell address.
    int _anodeTDC;
    int _cathodeTopTDC;
    int _cathodeBotTDC;
    int _alphaTDC;
    bool _validAddress;   // These two values are used to ensure
    bool _validData;      // proper ordering of the data cards.
};

#endif    /* RAWGEIGERHIT_HPP */
