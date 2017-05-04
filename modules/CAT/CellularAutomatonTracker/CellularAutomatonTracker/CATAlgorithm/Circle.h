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
#include <CATAlgorithm/circle_base.h>

namespace CAT {
  namespace topology{

    class Circle : public circle{

    private:

    public:
      Circle(){};
      virtual ~Circle(){};

      double phi(double x, double y);
    };
  }
}

#endif
