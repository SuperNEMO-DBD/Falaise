#ifndef UNPACK_HPP
#define UNPACK_HPP
// Unpack.h
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
// Class for Geiger Card raw data unpacking.
// The raw data is in text format for commissioning data.
//
#include <iosfwd>
#include <string>
#include "GeigerEvent.h"
#include "RawGeigerHit.h"
#include "UnpackingErrors.h"

class Unpack
{
  public:
    // define some status codes
    //
    enum status { OK, END, BAD_HIT, BAD_SYNC, BAD_TRIG };

    // constructors and destructor.
    //
    Unpack();
    Unpack(std::ifstream& infile);    // initialise from open stream.
    Unpack(const std::string& fname); // initialise from specified file name.
    ~Unpack();

    // user interface
    //
    status getTrigConfig();   // ensure valid TRIGCONF data exists up front.
    status getNextEvent(GeigerEvent&);  // unpack an event.
    double trigDelay() const { return _trigDriftDelay; }
    double trigAlpha() const { return _alphaRegister; }
    const UnpackingErrors& getErrors() const { return _errors; }

  private:
    std::ifstream* _rawFile;
    bool _iOpened;            // did I open this file?
    bool _haveLine;           // is there a cached input line?
    bool _inEvent;            // are we processing an event?
    bool _completedEvent;     // test for event ready.
    std::string _line;        // input line buffer for "read-again"
    double _trigDriftDelay;   // TRIGCONF
    double _alphaRegister;    // TRIGCONF
    GeigerEvent _event;       // container for an event.
    RawGeigerHit _myHit;      // temporary hit for building event.
    UnpackingErrors _errors;  // collection of errors for an event.

    // implementation methods.
    void init();                // helper for constructors.
    std::string nextLine();     // helper method for input buffering.
    status parseNextRecord();     // handle a single input record.
    status setTrig(std::string s); // decode TRIGCONF record.
    status parseRunStart(std::string s);
    status parseEvent(std::string s);
    status parseD0(std::string s);
    status parseD1(std::string s);
    status parseEndRun(std::string s);
    int  getHexValue(std::istringstream& cstr, std::string& s);
};

#endif    /* UNPACK_HPP */
