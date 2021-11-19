#ifndef FALAISE_LTTC_PLUGIN_ROTATED_CURVE_HH
#define FALAISE_LTTC_PLUGIN_ROTATED_CURVE_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>

namespace lttc {
  
  /// \brief Wrapper for a rotated curve in 2D space
  class rotated_curve
    : public i_curve
  {
  public:
    
    rotated_curve() = default;
    
    virtual ~rotated_curve() = default;
   
    rotated_curve(const i_curve & crv_, const point & center_, double angle_);
    
    void set_rotated_curve(const i_curve & crv_, const point & center_, double angle_);

    void compute(double t_, point & vtx_) const override;
 
    void generate_samples(double t1_, double t2_, polyline & samples_) const override;

  private:
    
    const i_curve * _crv_ = nullptr;
    point  _center_;
    double _angle_ = 0.0;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_ROTATED_CURVE_HH
