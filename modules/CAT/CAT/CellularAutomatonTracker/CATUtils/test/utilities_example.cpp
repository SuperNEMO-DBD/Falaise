/* -*- mode: c++ -*- */

#include <mybhep/error.h>
#include <mybhep/sreader.h>
#include <mybhep/utilities.h>

using namespace std;
using namespace mybhep;

int main(int, char* []) {
  vector<double> vd;
  vector<double> vb;
  vd.push_back(1.);
  vd.push_back(10.);
  vd.push_back(100.);
  vd.push_back(1000.);

  string st = vector_to_string(vd);
  cout << "st = " << st << endl;

  vector_from_string(st, vb);
  cout << "sb = " << print_vector(vd) << endl;
  return 0;
}
