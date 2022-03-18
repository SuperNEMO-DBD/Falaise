#ifndef FALAISE_LTTC_PLUGIN_KINKED_CURVE2_HH
#define FALAISE_LTTC_PLUGIN_KINKED_CURVE2_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve2.hh>
#include <lttc/point.hh>
#include <lttc/rotated_curve2.hh>

namespace lttc {
  
  /// \brief Wrapper for a kinked curve in 2D space
  class kinked_curve2
    : public i_curve2
  {
  public:
    
    kinked_curve2() = default;
    
    virtual ~kinked_curve2() = default;
   
    kinked_curve2(const i_curve2 & crv_, double tpivot_, double angle_);

    double get_s_pivot() const;

    double get_angle() const;
 
    const point2 & get_pivot() const;

    void compute(double s_, point2 & vtx_) const override;
 
    void generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_ = 100) const override;
     
  private:
    
    const i_curve2 * _crv_ = nullptr; ///< Original curve to be kinked
    double _s_pivot_; ///< Curvilinear coordinate at pivot vertex
    point2 _pivot_; ///< Pivot vertex
    double _angle_ = 0.0; ///< Kink angle
    rotated_curve2 _kinked_arm_; ///< Rotated curve built from the original
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_KINKED_CURVE2_HH
