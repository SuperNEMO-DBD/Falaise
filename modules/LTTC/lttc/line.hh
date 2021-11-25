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

    line(const point & position_, const vector2 & direction_);

    bool is_valid() const;

    double get_a() const;

    double get_b() const;

    double get_c() const;

    double get_theta() const;

    double get_r() const;
    
    static line make_from_theta_r(double theta_, double r_);
          
    static line make_from_start_stop(const point & start_, const point & stop__);
          
    static line make_from_start_direction(double x0_, double y0_, double ux_, double uy_);

    bool is_horizontal(double tolerance_ = 1e-14) const;

    bool is_vertical(double tolerance_ = 1e-14) const;
   
    point get_reference_point() const;
    
    vector2 get_direction() const;
     
    vector2 get_orthogonal_direction() const;
    
    double distance(const point & pos_) const;

    vector2 orthogonal_projection(const point & pos_) const;
    
    void compute(double t_, point & vtx_) const override;
 
    void generate_samples(double t1_, double t2_, polyline & samples_) const override;

    line make_perp(const point & p_) const; 
    
    bool unique_intersection(const line & other_, point & p_) const; 

    bool is_same(const line & other_, double r_tolerance_, double theta_tolerance_) const;
    
    void clear();
   
    void _compute_theta_r_from_abc();

    void print(std::ostream & out_, const std::string & indent_ = "") const;

  private:
    
    // Hessian form:   a.x + b.y + c = 0
    double _a_ = std::numeric_limits<double>::quiet_NaN(); ///< a = cos(theta)
    double _b_ = std::numeric_limits<double>::quiet_NaN(); ///< b = sin(theta) >= 0
    double _c_ = std::numeric_limits<double>::quiet_NaN(); ///< c = -r
    vector2 _direction_;
    // Normal equation:
    double _r_ = std::numeric_limits<double>::quiet_NaN(); ///< (signed) Orthogonal distance of the origin 
    double _theta_ = std::numeric_limits<double>::quiet_NaN(); ///< in [0, Pi)
    
  };

  /// \brief Fitted line
  struct fitted_line
  {
    bool is_valid() const;
    void print(std::ostream & out_, const std::string & indent_ = "") const;
    // Atributes:
    double theta = std::numeric_limits<double>::quiet_NaN();     ///< Angle [0, pi]
    double r = std::numeric_limits<double>::quiet_NaN();         ///< orthogonal radius to the origin
    double theta_err = std::numeric_limits<double>::quiet_NaN(); ///< Error on angle
    double r_err = std::numeric_limits<double>::quiet_NaN();     ///< Error on radius
  };

  void draw_fitted_line(std::ostream & out_,
                        const fitted_line & fline_,
                        int tag_ = 0);

  void distance_to_fitted_line(const fitted_line & fline_,
                               const point & pos_,
                               double & distance_,
                               double & distance_err_);
 
  void orthogonal_projection_to_fitted_line(const fitted_line & fline_,
                                            const point & pos_,
                                            fitted_point & fpoint_);
   
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LINE_HH
