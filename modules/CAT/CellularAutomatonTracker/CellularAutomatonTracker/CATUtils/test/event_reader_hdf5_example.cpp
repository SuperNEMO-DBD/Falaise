/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <mybhep/reader_hdf5.h>
#include <fstream>

using namespace std;  
using namespace mybhep;


/** Example of use of event and event svc */


int main(int, char* [])
{
  // instantiate the event service
  
  mybhep_svc esvc;

  string fileName = "idst.h5";
  reader_hdf5 tr;
  tr.open(fileName);

  time_t time1;
  time(&time1);
  char* ct1 = ctime(&time1);
  cout << " Start loop: Time= " << ct1 << endl;
  int maxEvent = 200;
  for (size_t ievent=1; ievent<= maxEvent; ievent++)
    {
      event& ev = tr.read_event(ievent);  // tr.sequential_read() also OK
      // Dump event
      if (ievent%100 ==0)
	{
	  cout << "reading event " << ievent << endl;
	  cout << "Dumping event " << ievent << endl;
	  cout << ev << endl;
	}
    }
  time_t time2;
  time(&time2);
  char* ct2 = ctime(&time2);
  cout << " End loop: Time= " << ct2 << endl;

  double telap = difftime(time2,time1);
  cout << " Elapsed time= " << telap << " seconds " << endl;

  return 0;
  
}
