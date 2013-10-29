
#include <cmath>
#include <CATAlgorithm/Legendre_function.h>

namespace CAT {
  namespace topology{

    using namespace std;

    //______________________________________________________________________________
    Legendre_function::Legendre_function(){
    }

    //______________________________________________________________________________
    Legendre_function::~Legendre_function()
    {
    }

    double Legendre_function::Evaluate(double *x, double *parm) {
      // returns the radius of a circle that could have activated a gg cell
      //
      // parm[0]: x coordinate of cell anode wire
      // parm[1]: y coordinate of cell anode wire
      // x[0]: x coordinate of circle center
      // y[0]: y coordinate of circle center

      double dx = parm[0] - x[0];
      double dy = parm[1] - x[1];
      double p = -dx/dy;
      double R = fabs(p*dx - dy)/sqrt(1+pow(p,2));
      return R;

    }
  }
}
