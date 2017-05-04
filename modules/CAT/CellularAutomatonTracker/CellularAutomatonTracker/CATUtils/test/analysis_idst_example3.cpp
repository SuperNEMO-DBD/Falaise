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


int main(int, char* [])
{
  mybhep_svc evsvc;
  string idst_i = "idst.txt";
  
  ifstream is(idst_i.c_str());
  
  size_t ievt = 0;
  cout << "reading iDST = " << idst_i << endl;
  ievt = 0;
  while (!is.eof() && ievt < 100) 
    {
      cout << "reading event number = " << ievt++ << endl;
      string record;
      bool test = getline(is, record);  
      if(!test) break;

      cout << "record = " << record << endl;

      event_cvt cvt(record);

      cout << "making new event" << endl;
      event* evt = cvt.create();
      cout << *evt << endl;
      mybhep_svc::instance().clear_event();
    }

  is.close();

  return 0;
  
}

