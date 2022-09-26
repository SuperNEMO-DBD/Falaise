#ifndef FALAISE_GEOMETRY_ROTATED_CURVE2_HH
#define FALAISE_GEOMETRY_ROTATED_CURVE2_HH

// Standard library:
#include <iostream>

// This project:
#include <falaise/geometry/i_curve2.hh>
#include <falaise/geometry/point.hh>

namespace falaise {
  
  namespace geometry {
  
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
    
  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_ROTATED_CURVE2_HH
