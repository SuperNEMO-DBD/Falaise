/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <zlib.h>

using namespace std;  
using namespace mybhep;

#define LEN 10000


/** Example of use of event and event svc */


int main(int, char* [])
{
  // instantiate the event service
  
  mybhep_svc esvc;
  
  const char *file = "idst.gz";
  const char *mode = "wb";
  gzFile gf = gzopen (file,mode);

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");

  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");

  time_t time1;
  time(&time1);
  char* ct1 = ctime(&time1);
  cout << " Start loop: Time= " << ct1 << endl;
   
  for(size_t ievent =1; ievent <= 200; ievent++)
    {
    
      generate_event gevt(ievent);
      gevt.create_particles(2);
      gevt.add_tracks(materials);
      gevt.add_hits(detectors);
      
      event* evt = gevt.retrieve();
      event_cvt cvt(*evt,ievent);
      
      string def = cvt.store();
      def+="\n";
      int nchar = gzputs (gf, def.c_str());
      
      if (ievent%100 == 0)
	{
	  cout << " storing event number = " << ievent << endl;
	  cout << nchar << " characters written to file " << endl;
	}
    }
  time_t time2;
  time(&time2);
  char* ct2 = ctime(&time2);
  cout << " End loop: Time= " << ct2 << endl;
  
  double telap = difftime(time2,time1);
  cout << " Elapsed time= " << telap << " seconds " << endl;
  
  int status =gzclose (gf);
  cout << " closing file with status " << status << endl;
  
  gf = gzopen (file,"rb");

  cout << "reading iDST = " << endl;
  time(&time1);
  ct1 = ctime(&time1);
  cout << " Start loop: Time= " << ct1 << endl;
   
  for(size_t ievent =1; ievent <= 200; ievent++)
    {
      char* buff = new char[LEN];
      buff = gzgets (gf, buff, LEN);

      if(buff == Z_NULL)
	{
	  cout << " error reading string from file "  << endl;
	  delete [] buff;
	  break;
	}

      string record = buff;
      event_cvt cvt(record);

      event* new_event = cvt.create();
      
      if (ievent%100 == 0)
	{
	  cout << "reading event number = " << ievent++ << endl;
	  cout << record.size() << " characters read from file " << endl;
	  cout << "making new event" << endl;
	  cout << *new_event << endl;      
	}



      mybhep_svc::instance().clear_event();
      delete new_event;
      delete [] buff;
    }
  time(&time2);
  ct2 = ctime(&time2);
  cout << " End loop: Time= " << ct2 << endl;
  
  telap = difftime(time2,time1);
  cout << " Elapsed time= " << telap << " seconds " << endl;

  return 0;
  
}
