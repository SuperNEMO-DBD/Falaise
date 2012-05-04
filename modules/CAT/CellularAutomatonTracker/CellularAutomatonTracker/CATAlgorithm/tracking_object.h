/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__tracking_object_h
#define __CATAlgorithm__tracking_object_h 1

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <mybhep/utilities.h>
#include <CATAlgorithm/printable.h>
//#include "TMath.h"
#include <boost/math/special_functions/gamma.hpp>

//#include <util/Clock.h>


namespace CAT{
  namespace topology{

    using namespace std;  
    using namespace mybhep;

    // a generic tracking object
    class tracking_object : public printable
    {
    
    protected:
    
      //    Clock clock;

    public:
    
      /// print level 
      prlevel level_;
    
      // the number of sigmas
      double nsigma_;
    
    
      void set_nsigma( double nsigma );
    
      double nsigma() const;

      double prob() const;

      double probof(double chi2, int ndof)const;

      double get_nsigma()const;

      /*
        inline void print_clock(){
        clock.dump();
        return;
        }
      */

    };

  }    
}    

#endif // __CATAlgorithm__tracking_object_h
        





