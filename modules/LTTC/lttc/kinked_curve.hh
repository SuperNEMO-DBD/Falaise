#ifndef FALAISE_LTTC_PLUGIN_KINKED_CURVE_HH
#define FALAISE_LTTC_PLUGIN_KINKED_CURVE_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>
#include <lttc/rotated_curve.hh>

namespace lttc {
  
  /// \brief Wrapper for a kinked curve in 2D space
  class kinked_curve
    : public i_curve
  {
  public:
    
    kinked_curve() = default;
    
    virtual ~kinked_curve() = default;
   
    kinked_curve(const i_curve & crv_, double tpivot_, double angle_);

    double get_tpivot() const;

    double get_angle() const;
 
    const point & get_pivot() const;

    void compute(double t_, point & vtx_) const override;
 
    void generate_samples(double t1_, double t2_, polyline & samples_) const override;
    
  private:
    
    const i_curve * _crv_ = nullptr;
    double _tpivot_;
    double _angle_ = 0.0;
    point _pivot_;
    rotated_curve _kinked_arm_;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_KINKED_CURVE_HH
