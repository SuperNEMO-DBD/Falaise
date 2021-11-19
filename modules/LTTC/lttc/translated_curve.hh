#ifndef FALAISE_LTTC_PLUGIN_TRANSLATED_CURVE_HH
#define FALAISE_LTTC_PLUGIN_TRANSLATED_CURVE_HH

// Standard library:
#include <iostream>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>

namespace lttc {

  /// \brief Wrapper for a translated curve in 2D space
  class translated_curve
    : public i_curve
  {
  public:
    
    translated_curve() = default;
    
    virtual ~translated_curve() = default;
    
    translated_curve(const i_curve & crv_, const point & translation_);

    translated_curve(const i_curve & crv_, double dx_, double dy_);

    void compute(double t_, point & vtx_) const override;
 
    void generate_samples(double t1_, double t2_, polyline & samples_) const override;

    const point & translation() const;

  private:
    
    const i_curve * _crv_ = nullptr;
    point _translation_;
      
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRANSLATED_CURVE_HH
