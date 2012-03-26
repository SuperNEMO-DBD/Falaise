/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <mybhep/writer_txt.h>
#include <fstream>

using namespace std;  
using namespace mybhep;


/** Example of use of event and event svc */


int main(int, char* [])
{
  // instantiate the event service
  
  mybhep_svc esvc;

  string fileName = "idst.txt";
  size_t nEvents = 1000;
  writer_txt wr;
  wr.open(fileName);

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");

  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");
  
  for(size_t ievent =1; ievent <= nEvents; ievent++){
    
    cout << " storing event number = " << ievent << endl;
    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    event* evt = gevt.retrieve();
    wr.write(*evt,ievent);

  }

  return 0;
  
}
