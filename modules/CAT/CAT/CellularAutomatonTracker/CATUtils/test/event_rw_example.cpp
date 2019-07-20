/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <fstream>

using namespace std;
using namespace mybhep;

/** Example of use of event and event svc */

int main(int, char* []) {
  // instantiate the event service

  mybhep_svc esvc;

  ofstream os("idst.txt");

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");

  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");

  for (size_t ievent = 1; ievent <= 1000; ievent++) {
    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    event* evt = gevt.retrieve();
    event_cvt cvt(*evt, ievent);

    string def = cvt.store();
    if (ievent % 100 == 0) {
      cout << " storing event number = " << ievent << endl;
    }
    os << def << endl;
  }

  os.close();
  ifstream is("idst.txt");
  size_t ievent = 0;
  cout << "reading iDST = " << endl;
  while (!is.eof()) {
    string record;
    bool test = getline(is, record);
    if (!test) break;

    event_cvt cvt(record);
    event* new_event = cvt.create();
    if (ievent % 100 == 0) {
      cout << "reading event number = " << ievent++ << endl;
      cout << "making new event" << endl;
      cout << *new_event << endl;
    }
    mybhep_svc::instance().clear_event();
    delete new_event;
  }

  return 0;
}
