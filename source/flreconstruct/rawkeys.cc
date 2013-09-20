// rawkeys.cc
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "rawkeys.h"

int whichKey(const std::string& line)
{
  static const int maxkeys = 6; // increase this if we add more keywords!
  static const std::string keyword[] =
    {"D0", "D1", "ENDRUN", "Event", "RUNSTART", "TRIGCONF"}; 
  for (int i=0; i<maxkeys; i++)
  {
    if (line.substr(0, keyword[i].length()) == keyword[i]) return i;
  }
  return kBAD;  // no match found.
}
