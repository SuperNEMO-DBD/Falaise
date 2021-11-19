#ifndef FALAISE_LTTC_PLUGIN_LINE_HH
#define FALAISE_LTTC_PLUGIN_LINE_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>

namespace lttc {

  /// \brief Line in 2D space
  struct line : public i_curve
  {
   
    line() = default;
    
    virtual ~line() = default;
    
    line(double a_, double b_, double c_);

    line(const point & start_, const vector2 & direction_);
    
    static line make_from_theta_r(double theta_, double r_);
          
    static line make_from_start_stop(const point & start_, const point & stop__);
          
    static line make_from_start_direction(double x0_, double y0_, double ux_, double uy_);

    bool is_horizontal(double tolerance_ = 1e-14) const;

    bool is_vertical(double tolerance_ = 1e-14) const;
    
    vector2 get_direction() const;
     
    vector2 get_orthogonal_direction() const;
    
    double distance(const point & pos_) const;
    
    void compute(double t_, point & vtx_) const override;
 
    void generate_samples(double t1_, double t2_, polyline & samples_) const override;

    line make_perp(const point & p_) const; 
    
    bool unique_intersection(const line & other_, point & p_) const; 
    
    void clear();
    
    void _compute_abc();

    void _compute_refdir();

    void print(std::ostream & out_, const std::string & indent_ = "") const;
    
    point   start;
    double  t0 = 0.0;
    vector2 direction;
    // Hessian form:   a.x + b.y + c = 0
    double a = std::numeric_limits<double>::quiet_NaN(); ///< a = cos(phi)
    double b = std::numeric_limits<double>::quiet_NaN(); ///< b = sin(phi)
    double c = std::numeric_limits<double>::quiet_NaN(); ///< c = -p
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LINE_HH
