/* -*- mode: c++ -*- */

#include <CATAlgorithm/tracking_object.h>

//#include "TMath.h"
#include <boost/math/special_functions/gamma.hpp>


namespace CAT{
  namespace topology{

    using namespace std;  
    using namespace mybhep;
        
    void tracking_object::set_nsigma( double nsigma )
    {
      nsigma_ = nsigma;
    }
    
    double tracking_object::nsigma() const
    {
      return nsigma_;
    }
    
    double tracking_object::prob() const
    {
      
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

    double tracking_object::probof(double chi2, int ndof) const
    {
      //double p  =  TMath::Prob(chi2, ndof);
      double p=0.;
      if( ndof && chi2 )
	p = boost::math::gamma_q<double, double> (0.5 * ndof, 0.5 * chi2);
      return p;
    }

    double tracking_object::get_nsigma()const
    {
      return nsigma_;
    }

    /*
      inline void print_clock(){
      clock.dump();
      return;
      }
    */

  }    
}    
        





