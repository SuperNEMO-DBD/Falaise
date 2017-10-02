/* -*- mode: c++ -*- */

#include <mybhep/error.h>
#include <mybhep/sreader.h>
#include <mybhep/utilities.h>

using namespace std;
using namespace mybhep;

int main(int, char* []) {
  string reader_file = "reader_data.par";
  string group1 = "A";
  string group2 = "B";
  string group3 = "C";

  // create a generic store
  gstore store;

  // and two readers

  sreader reader1(store);
  reader1.file(reader_file);
  reader1.info_level(VVERBOSE);

  sreader reader2(store);
  reader2.file(reader_file);
  sreader reader3(store);
  reader3.file(reader_file);
  reader1.group(group1);
  reader2.group(group2);
  reader3.group(group3);

  reader1.read();
  reader2.read();
  reader3.read();

  cout << "print store" << endl;
  cout << store << endl;
  return 0;
}
