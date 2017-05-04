
#include <iostream>
#include <TMath.h>
#include <boost/math/special_functions/gamma.hpp>

using namespace std;

int main ()
{
  double chi2 = 2.4;
  int ndof = 2;
  double pr = TMath::Prob(chi2, ndof);
  double pb = boost::math::gamma_q<double, double> (0.5 * ndof, 0.5 * chi2);
  
  cout << "ROOT Prob = " << pr << endl;
  cout << "Boost Prob = " << pb << endl;
  return 0;
}
