/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__Circle
#define __CATAlgorithm__Circle

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Circle                                                               //
//                                                                      //
// Description of the circle parameters                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "TObject.h"
#include "TColor.h"
#include <CATAlgorithm/circle.h>

namespace CAT {
  namespace topology{

    class Circle : public circle{

    private:

    public:
      Circle(){};
      virtual ~Circle(){};

      double phi(double x, double y);
      void draw(Color_t color);
      void build_random(double x0, double y0, double Lx, double Ly);

    };
  }
}

#endif
