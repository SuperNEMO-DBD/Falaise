/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__Legendre_function
#define __CATAlgorithm__Legendre_function

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Legendre_function                                                    //
//                                                                      //
// The Legendre transform of a circle function                          //
// returns the radius of a circle that could have activated a gg cell   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

namespace CAT {
  namespace topology{

    class Legendre_function {

    private:
      double x0_;
      double y0_;
      double r_;

    public:
      Legendre_function();
      virtual ~Legendre_function();

      double Evaluate(double *x, double *parm);

    };
  }
}


#endif
