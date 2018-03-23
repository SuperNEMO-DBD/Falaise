/* -*- mode: c++ -*- */
/*
 */
#include <mybhep/mybhep_svc.h>
#include <mybhep/event.h>
#include <mybhep/material.h>
#include <mybhep/particle.h>
#include <mybhep/track.h>
#include <mybhep/hit.h>
#include <mybhep/clhep.h>
#include <mybhep/system_of_units.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

#include <fstream>

using namespace std;
using namespace mybhep;

/** Example of reading an idst, creating another, reading it back*/

int main(int, char* []) {
  mybhep_svc evsvc;
  string idst_i = "idst.txt";
  string idst_o = "idst2.txt";

  ifstream is(idst_i.c_str());
  ofstream os(idst_o.c_str());

  size_t ievt = 0;
  cout << "reading iDST = " << idst_i << endl;
  cout << "writting iDST = " << idst_o << endl;
  ievt = 0;
  while (!is.eof() && ievt < 100) {
    cout << "reading event number = " << ievt++ << endl;
    string record;
    bool test = getline(is, record);
    if (!test) break;

    cout << "record = " << record << endl;

    event_cvt cvt(record);

    cout << "making new event" << endl;
    event* evt = cvt.create();
    cout << *evt << endl;

    // names of properties
    cout << "names of event properties" << print_vector(evt->properties()) << endl;
    cout << "values of event properties" << print_vector(evt->properties_values()) << endl;
    // Getting the first particle of event

    const vector<particle*> vp = evt->true_particles();
    if (!vp.size()) continue;

    particle* pp = vp[0];

    cout << "first particle in event = " << *pp << endl;
    cout << "names of particles properties" << print_vector(pp->properties()) << endl;
    cout << "values of particle properties" << print_vector(pp->properties_values()) << endl;

    // Creating a new event
    event evt2(ievt);
    // add a property
    evt2.add_property("event_type", "selected_event");
    if (evt2.find_property("event_type"))
      cout << " event type = " << evt2.fetch_property("event_type") << endl;

    // add just one particle
    evt2.add_true_particle(pp);

    cout << "making selected event" << endl;
    cout << evt2 << endl;

    // new converter
    event_cvt cvt2(evt2, ievt);
    string def2 = cvt2.store();
    os << def2 << endl;
    mybhep_svc::instance().clear_event();
  }

  is.close();
  os.close();

  ifstream is2(idst_o.c_str());

  cout << "reading iDST = " << idst_o << endl;
  ievt = 0;
  while (!is2.eof()) {
    cout << "reading event number = " << ievt++ << endl;
    string record;
    bool test = getline(is2, record);
    if (!test) break;

    event_cvt cvt(record);
    cout << "making new event" << endl;
    event* evt = cvt.create();
    cout << *evt << endl;
  }

  return 0;
}
