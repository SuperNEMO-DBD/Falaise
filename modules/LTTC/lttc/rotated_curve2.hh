#ifndef FALAISE_LTTC_PLUGIN_ROTATED_CURVE2_HH
#define FALAISE_LTTC_PLUGIN_ROTATED_CURVE2_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve2.hh>
#include <lttc/point.hh>

namespace lttc {
  
  /// \brief Wrapper for a rotated curve in 2D space
  class rotated_curve2
    : public i_curve2
  {
  public:
    
    rotated_curve2() = default;
    
    virtual ~rotated_curve2() = default;
   
    rotated_curve2(const i_curve2 & crv_, const point2 & center_, double angle_);
    
    void set_rotated_curve(const i_curve2 & crv_, const point2 & center_, double angle_);
 
    void compute(double s_, point2 & vtx_) const override;
 
    void generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_ = 100) const override;

  private:
    
    const i_curve2 * _crv_ = nullptr;
    point2 _center_;
    double _angle_ = 0.0;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_ROTATED_CURVE2_HH
