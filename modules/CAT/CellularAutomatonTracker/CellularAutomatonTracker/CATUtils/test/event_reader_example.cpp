/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <mybhep/reader_txt.h>
#include <fstream>

using namespace std;
using namespace mybhep;

/** Example of use of event and event svc */

int main(int, char* []) {
  // instantiate the event service

  mybhep_svc esvc;

  string fileName = "idst_example.txt";
  reader_txt tr;
  tr.open(fileName);

  // read event 11 to max
  size_t maxEvent = tr.max_events();
  cout << "Max number of events " << maxEvent << endl;
  cout << "reading event 11 to " << maxEvent << endl;
  for (size_t ievent = 11; ievent < maxEvent; ievent++) {
    cout << "reading event " << ievent << endl;
    event& ev = tr.read_event(ievent);
    // Dump event
    cout << "Dumping event " << ievent << endl;
    cout << ev << endl;
  }

  {
    // read event 1
    cout << "reading event 1" << endl;
    event& ev = tr.read_event(1);
    // Dump event
    cout << "Dumping event 1" << endl;
    cout << ev << endl;
  }
  {
    // read event 5
    cout << "reading event 5" << endl;
    event& ev = tr.read_event(5);
    // Dump event
    cout << "Dumping event 5" << endl;
    cout << ev << endl;
  }
  {
    // read event 3
    cout << "reading event 3" << endl;
    event& ev = tr.read_event(3);
    // Dump event
    cout << "Dumping event 3" << endl;
    cout << ev << endl;
  }

  cout << "Sequential reading" << endl;
  tr.open(fileName);

  maxEvent = tr.max_events();

  cout << "Max number of events " << maxEvent << endl;
  for (size_t ievent = 1; ievent <= maxEvent; ievent++) {
    cout << "reading event " << ievent << endl;
    event& ev = tr.read_next();  // tr.sequential_read() also OK
    // Dump event
    cout << "Dumping event " << ievent << endl;
    cout << ev << endl;
  }

  return 0;
}
