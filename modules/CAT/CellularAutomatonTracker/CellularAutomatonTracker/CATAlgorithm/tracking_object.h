/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__TROBJ__
#define __CATAlgorithm__TROBJ__

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
    class tracking_object : public printable{
    
    protected:
    
      //    Clock clock;

    public:
    
      /// print level 
      prlevel level_;
    
      // the number of sigmas
      double nsigma_ ;
    
    
      inline void set_nsigma( double nsigma ){
        nsigma_ = nsigma;
      }
    
      inline const double nsigma() const{
        return nsigma_;
      }

      inline const double prob() const{

        return 0.;

        size_t n = (size_t)(nsigma_ + 0.5);

        if( n == 1 )
          return 0.682689;
        if( n == 2 )
          return 0.954450;
        if( n == 3 )
          return 0.997302;
        if( n == 4 )
          return 0.99993666;
        if( n == 5 )
          return 0.99999943;
        if( n == 6 )
          return 0.999999998;
        if( n == 7 )
          return 0.999999999997;

        return 1.;

      }

      inline double probof(double chi2, int ndof)const{
        //double p  =  TMath::Prob(chi2, ndof);
        double p = boost::math::gamma_q<double, double> (0.5 * ndof, 0.5 * chi2);
        return p;
      }

      inline double get_nsigma()const{
        return nsigma_;
      }

      /*
        inline void print_clock(){
        clock.dump();
        return;
        }
      */

    };

  }    
}    
#endif 
        





