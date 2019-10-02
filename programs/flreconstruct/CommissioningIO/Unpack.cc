// Unpack.cc
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "Unpack.h"
#include "rawkeys.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

Unpack::Unpack()
{
  _rawFile = 0;
  _iOpened = false;
  init();
}

Unpack::Unpack(std::ifstream& infile)
{
  _rawFile = &infile;
  _iOpened = false;
  init();
}

Unpack::Unpack(const std::string& fname)
{
  _rawFile = new std::ifstream(fname.c_str());
  _iOpened = true;
  init();
}

Unpack::~Unpack()
{
  if (_iOpened) _rawFile->close();
}

void Unpack::init()
{
  // finish initialisation for constructors.
  //
  _haveLine = false;
  _inEvent = false;
  _completedEvent = false;
  _line = "";
  _trigDriftDelay = -1.0;
  _alphaRegister = -1.0;
}

Unpack::status Unpack::getNextEvent(GeigerEvent& ev)
{
  // This is the main user interface. Unpack the next event from the input file
  // and return a reference to the caller.
  //
  _completedEvent = false;
  _errors.clear();
  status ret, retf = OK;
  while (!_completedEvent)
  {
    try
    {
      ret = parseNextRecord();
      if (ret == END)
      {
        _event.newEvent(-9999);
        ev = _event;
        return ret;
      }
      if (ret != OK) retf = ret;
    }
    catch (std::exception &e)
    {
      // handle logic_error, skip rest of current event.
      std::stringstream ss;
      ss << "Exception thrown during getNextEvent!! "
          << e.what() << "\nSkipping rest of the event..." << std::endl;
      int key = kD0;
      while (key == kD0 || key == kD1) key = whichKey(nextLine());
      _myHit = RawGeigerHit();  // ensure we leave things in a clean state.
      retf = BAD_SYNC;
      _errors.add(retf, ss.str());
    }
  }
  ev = _event;
  return retf;
}

std::string Unpack::nextLine()
{
  if (_haveLine) return _line;  // reuse buffered line.
  if (_rawFile) std::getline(*_rawFile, _line);
  return _line;
}

Unpack::status Unpack::parseNextRecord()
{
  // Get the next record and invoke the relevant parsing method.
  // If we are currently processing an event and detect a new Event tag push the
  // record back and call the end of event method.
  //
  status ret = OK;
  if (!_rawFile->eof())
  {
    std::string s = nextLine();
    int theKey = whichKey(s);
    if (theKey == kTRIGCONF) ret = setTrig(s);
    else if (theKey == kRUNSTART) ret = parseRunStart(s);
    else if (theKey == kEvent) ret = parseEvent(s);
    else if (theKey == kD0) ret = parseD0(s);
    else if (theKey == kD1) ret = parseD1(s);
    else if (theKey == kENDRUN) ret = parseEndRun(s);

    return ret;
  }
  else return END; // end of file reached.
}

Unpack::status Unpack::getTrigConfig()
{
  // This method may be called before the first event. The intention is to
  // process input until a valid TRIGCONF card is reached.
  // If we see the first event before we have reached a TRIGCONF return a
  // bad status.
  //
  // However, if you don't care at all then you don't need to call this method.
  // Then it won't matter if there is no TRIGCONF.
  //
  if (_trigDriftDelay > 0.0) return OK; // already done.
  while (_trigDriftDelay < 0.0)
  {
    status ret = parseNextRecord();
    if (ret != OK) return ret;
  }
  return OK;
}

Unpack::status Unpack::setTrig(std::string s)
{
  // Unpack the TRIGCONF data.  The hitmask fields are ignored here.
  // This assumes a fixed 20ns binning on all TDCs.
  //
  std::istringstream cstr(s, std::ios_base::in);
  int delay, alpha;
  std::string key;
  cstr >> key;
  try
  {
    delay = getHexValue(cstr, s);
    alpha = getHexValue(cstr, s);
    getHexValue(cstr, s);  // just to check for error in previous item!
  }
  catch (std::exception &e)
  {
    std::stringstream ss;
    ss << "Bad TRIGCONF data: " << e.what() << std::endl;
    _errors.add(BAD_TRIG, ss.str());
    return BAD_TRIG;
  }
  double tdcTick = 20; // time binning in ns.
  _trigDriftDelay = delay*tdcTick/1000.;  // micro-seconds.
  _alphaRegister = alpha*tdcTick/1000.;
  return OK;
}

Unpack::status Unpack::parseRunStart(std::string /*s*/)
{
  // std::cout << "Processing RUNSTART" << std::endl;
  return OK;
}

Unpack::status Unpack::parseEvent(std::string s)
{
  if (_inEvent)
  {
    _haveLine = true;
    _inEvent = false;
    _completedEvent = true;
  }
  else
  {
    _inEvent = true;
    _haveLine = false;
    _completedEvent = false;
    std::string key;
    long eventNumber;
    std::istringstream cstr(s, std::ios_base::in);
    cstr >> key >> eventNumber;
    _event.newEvent(eventNumber); // reset to a new event.

    // Get the rest of the line as a single string
    std::string dateStamp;
    std::getline(cstr, dateStamp);
    std::cout << "\nProcessing Event " << eventNumber
              << " written at " << dateStamp << std::endl;
  }

  // sanity check
  //
  if (_trigDriftDelay < 0.0)
  {
    std::stringstream ss;
    ss << "\nWARNING: Event record seen before valid TRIGCONF!!"
      << std::endl;
    _errors.add(BAD_SYNC, ss.str());
    return BAD_SYNC;
  }
  return OK;
}

Unpack::status Unpack::parseD0(std::string s)
{
  if (!_inEvent) throw std::logic_error("Unexpected TDC D0 card");
  std::istringstream cstr(s, std::ios_base::in);
  std::string key;
  cstr >> key;
  int address;  // hardware address, Geiger cell address.
  try
  {
    getHexValue(cstr, s);
    address = getHexValue(cstr, s);
    if (!cstr.eof()) throw std::invalid_argument(s.c_str());
  }
  catch (...)
  {
    std::stringstream ss;
    ss << "Skip Bad hit: " << s << std::endl;
    // Dump the next line which should be a D1 card
    //
    ss << "Ignoring: " << nextLine() << std::endl;
    _errors.add(BAD_HIT, ss.str());
    return BAD_HIT;
  }
  int column, layer, side, module;  // these are packed into address, DocB 1570
  column = address & 0x7f; address >>= 7;
  layer  = address & 0xf;  address >>= 4;
  side   = address & 0x1;  address >>= 1;
  module = address & 0xf;
  _myHit.setAddress(module, side, layer, column);
  return OK;
}

Unpack::status Unpack::parseD1(std::string s)
{
  if (!_inEvent) throw std::logic_error("Unexpected TDC D1 card");
  std::istringstream cstr(s, std::ios_base::in);
  std::string key;
  int anode, cathodeT, cathodeB, alpha; // ASIC status + 4 TDC values.
  cstr >> key;
  try
  {
    getHexValue(cstr, s);
    anode = getHexValue(cstr, s);
    cathodeT = getHexValue(cstr, s);
    cathodeB = getHexValue(cstr, s);
    alpha = getHexValue(cstr, s);
    if (!cstr.eof()) throw std::invalid_argument(s.c_str());
  }
  catch (...)
  {
    std::stringstream ss;
    ss << "Skip Bad hit: " << s << std::endl;
    _errors.add(BAD_HIT, ss.str());
    _myHit.setData(0, 0, 0, 0); // this is just to avoid a half-baked hit.
    return BAD_HIT;
  }
  _myHit.setData(anode, cathodeT, cathodeB, alpha);
  _event.addHit(_myHit);
  return OK;
}

Unpack::status Unpack::parseEndRun(std::string /*s*/)
{
  if (_inEvent)
  {
    _completedEvent = true;
  }
  // std::cout << "Processing ENDRUN" << std::endl;
  return OK;
}

int Unpack::getHexValue(std::istringstream& cstr, std::string& s)
{
  int value(0);
  cstr >> std::hex >> value;
  if (cstr.fail()) throw std::invalid_argument(s.c_str());
  return value;
}
