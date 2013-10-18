// testCommissioningIO.cc - component test for commissioning tracker unpack tool
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
#include "Unpack.h"
#include "DriftModel.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  Unpack unp(argv[1]);  // file name specified on command line
  GeigerEvent ev;

  // Check we have valid TRIGCONF data at the beginning.
  //
  if (unp.getTrigConfig() != Unpack::OK)
  {
    cout << "WARNING: valid TRIGCONF not found!!\n"
      << "Carrying on recklessly.\n" << endl;
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

  // Loop over events.
  //
  Unpack::status ret = Unpack::OK;
  long evno = -1;
  while (true)
  {
    ret = unp.getNextEvent(ev);
    if (ret == Unpack::END) break;
    evno = ev.evtNumber();
    if (ret != Unpack::OK)
    {
      cout << "WARNING: Bad status from event " << evno << endl;
      const UnpackingErrors err = unp.getErrors();
      cout << "There are " << err.size() << " errors recorded" << endl;
      const UnpackingErrors::ErrorList el = err.getErrors();
      UnpackingErrors::ErrorList::const_iterator it = el.begin();
      for ( ; it != el.end(); ++it)
      {
        cout << "Error code: " << it->first << '\n'
            << "Error text: " << it->second;
      }
    }
    GeigerEvent::HitList hits = ev.getHits();
    cout << "Completed event number " << evno << " with "
      << hits.size() << " hits." << endl;
    GeigerEvent::HitList::iterator it = hits.begin();
    for ( ; it != hits.end(); ++it)
    {
      cout << *it;
      cout << "anode distance = " << drift.getDist(*it) << endl;
    }
  }

  return 0;
}
