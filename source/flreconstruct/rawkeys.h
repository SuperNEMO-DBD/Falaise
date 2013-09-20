// rawkeys.h
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#ifndef RAWKEYS_HPP
#define RAWKEYS_HPP
#include <string>

enum cardType 
  {kD0=0, kD1=1, kENDRUN=2, kEvent=3, kRUNSTART=4, kTRIGCONF=5, kBAD=-1};

int whichKey(const std::string& line);

#endif    /* RAWKEYS_HPP */
