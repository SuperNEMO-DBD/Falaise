

#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>

using namespace std;

int main(int argc_, char* argv_[]){

  topology::experimental_double a(0.9,0.1);
  topology::experimental_double b(2.,0.2);

  cout << " variable a: "; a.dump(); cout << " " << endl;
  cout << " variable b: "; b.dump(); cout << " " << endl;

  cout << " -a: "; (-a).dump(); cout << " " << endl;
  cout << " a + b: "; (a+b).dump(); cout << " " << endl;
  cout << " a - b: "; (a - b).dump(); cout << " " << endl;
  cout << " a * b: "; (a*b).dump(); cout << " " << endl;
  cout << " a / b: "; (a/b).dump(); cout << " " << endl;

  cout << " sin(a): "; (experimental_sin(a)).dump(); cout << " " << endl;
  cout << " cos(a): "; (experimental_cos(a)).dump(); cout << " " << endl;
  cout << " tan(a): "; (experimental_tan(a)).dump(); cout << " " << endl;
  cout << " arcsin(a): "; (experimental_asin(a)).dump(); cout << " " << endl;
  cout << " arccos(a): "; (experimental_acos(a)).dump(); cout << " " << endl;
  cout << " sin(arccos(a)): "; (experimental_sin(experimental_acos(a))).dump(); cout << " " << endl;
  cout << " atan(a/b): "; (experimental_atan2(a,b)).dump(); cout << " " << endl;
  cout << " sin(arcsin(a)): "; (experimental_sin(experimental_asin(a))).dump(); cout << " " << endl;
  cout << " cos(arccos(a)): "; (experimental_cos(experimental_acos(a))).dump(); cout << " " << endl;
  cout << " tan(arctan(a/b)): "; (experimental_tan(experimental_atan2(a,b))).dump(); cout << " " << endl;
  
  cout << " " << endl;

  topology::experimental_double c(5.,0.5);

  topology:: experimental_vector va(a, b, c);
  topology:: experimental_vector vb(3., 4., 0., 0.3, 0.4, 0.);

  cout << " vector va: "; va.dump(); cout << " " << endl;
  cout << " vector vb: "; vb.dump(); cout << " " << endl;

  cout << " va length: "; (va.length()).dump(); cout << " " << endl;
  cout << " va hor: "; (va.hor()).dump(); cout << " " << endl;
  cout << " va unit: "; (va.unit()).dump(); cout << " " << endl;
  cout << " va phi: "; (va.phi()).dump(); cout << " " << endl;
  cout << " va theta: "; (va.theta()).dump(); cout << " " << endl;
  cout << " va + vb: "; (va+vb).dump(); cout << " " << endl;
  cout << " va - vb: "; (va-vb).dump(); cout << " " << endl;
  cout << " va * vb: "; (va*vb).dump(); cout << " " << endl;
  cout << " va ^ vb: "; (va^vb).dump(); cout << " " << endl;
    
  return 1;    
}


