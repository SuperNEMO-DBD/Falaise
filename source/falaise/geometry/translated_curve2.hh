#ifndef FALAISE_GEOMETRY_TRANSLATED_CURVE2_HH
#define FALAISE_GEOMETRY_TRANSLATED_CURVE2_HH

// Standard library:
#include <iostream>

// This project:
#include <falaise/geometry/i_curve2.hh>
#include <falaise/geometry/point.hh>

namespace falaise {
  
  namespace geometry {

    /// \brief Wrapper for a translated curve in 2D space
    class translated_curve2
      : public i_curve2
    {
    public:
    
      translated_curve2() = default;
    
      virtual ~translated_curve2() = default;
    
      translated_curve2(const i_curve2 & crv_, const point2 & translation_);

      translated_curve2(const i_curve2 & crv_, double dx_, double dy_);

      point2 get_reference_point() const override;

      void compute(double t_, point2 & vtx_) const override;
 
      void generate_samples(double t1_, double t2_, polyline2 & samples_, size_t nsamples_ = 100) const override;

      const point2 & translation() const;

    private:
    
      const i_curve2 * _crv_ = nullptr;
      point2 _translation_;
      
    };
  
  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_TRANSLATED_CURVE2_HH
