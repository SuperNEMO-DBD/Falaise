// evdisp.cc - primitive event display for commissioning data
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110614
//
#include "Unpack.h"
#include "DriftModel.h"
#include "EventViewer.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  Unpack unp(argv[1]);  // file name specified on command line

  // Check we have valid TRIGCONF data at the beginning.
  //
  if (unp.getTrigConfig() != Unpack::OK)
  {
    cout << "WARNING: valid TRIGCONF not found!! Exiting." << endl;
    return 1;
  }
  double delay = unp.trigDelay();
  double alpha = unp.trigAlpha();
  cout << "Trigger delay: " << delay << endl
       << "Trigger alpha: " << alpha << endl;

  // setup drift model parameterisation
  //
  DriftModel drift;
  drift.setTDCRes(20.0);
  drift.setDelay(delay);

  // Create event viewer
  //
  EventViewer viewer(drift, unp);

  return 0;
}
