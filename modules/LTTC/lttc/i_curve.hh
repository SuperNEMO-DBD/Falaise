#ifndef FALAISE_LTTC_PLUGIN_I_CURVE_HH
#define FALAISE_LTTC_PLUGIN_I_CURVE_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/point.hh>

namespace lttc {
  
  struct i_curve
  {
   
    virtual ~i_curve() = default;

    virtual void compute(double t_, point & vtx_) const = 0;
 
    virtual void generate_samples(double t1_, double t2_, polyline & samples_) const = 0;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_I_CURVE_HH
