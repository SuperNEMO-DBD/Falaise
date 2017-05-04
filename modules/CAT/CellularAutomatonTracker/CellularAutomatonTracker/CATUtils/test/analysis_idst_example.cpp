/* -*- mode: c++ -*- */
/*
 */
#include <mybhep/generate_event.h>
#include <fstream>

using namespace std;  
using namespace mybhep;


/** Example of reading an idst*/


int main(int, char* [])
{
  mybhep_svc evsvc;
  string idst_i = "idst.txt";

  ifstream is(idst_i.c_str());  
  
  size_t ievent = 0;
  cout << "reading iDST = " << endl;
  while (!is.eof() && ievent < 100) 
    {
      cout << "reading event number = " << ievent++ << endl;
      string record;
      bool test = getline(is, record);  
      if(!test) break;

      event_cvt cvt(record);

      cout << "making new event" << endl;
      event* new_event = cvt.create();
      cout << *new_event << endl;

      mybhep_svc::instance().clear_event();
      delete new_event;
    }

  return 0;
  
}

