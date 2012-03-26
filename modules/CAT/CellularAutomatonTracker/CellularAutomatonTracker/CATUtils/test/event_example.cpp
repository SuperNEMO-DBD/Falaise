/* -*- mode: c++ -*- */
/*
 */


#include <mybhep/generate_event.h>
using namespace std;  
using namespace mybhep;

/** Example of use of event and event svc */

int main(int, char* [])
{
  // instantiate the event service

  mybhep_svc sevt;

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");

  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");

  for(size_t ievent =1; ievent <= 20; ievent++){

    cout << "event number = " << ievent << endl;
    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    //    cout << gevt << endl;

    // get event
    event* evt = gevt.retrieve();
    event_cvt cvt(*evt,ievent);

    string def = cvt.store();
    cout << "storing event " << ievent << " in string" << endl;
    cout << def << endl;

    cout << "restoring event " << endl;
    event_cvt cvt2(def);

    cout << "making new event" << endl;
    event* new_event = cvt2.create();
    cout << *new_event << endl;

    mybhep_svc::instance().clear_event();
    delete new_event;
  }

  return 0;
  
}


