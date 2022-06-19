#ifndef FALAISE_GEOMETRY_LINE2_HH
#define FALAISE_GEOMETRY_LINE2_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <falaise/geometry/i_curve2.hh>
#include <falaise/geometry/point.hh>

namespace falaise {
  
  namespace geometry {

    /// \brief Line in 2D space
    struct line2
      : public i_curve2
    {
   
      line2() = default;
    
      virtual ~line2() = default;
    
      line2(double a_, double b_, double c_);

      line2(const point2 & position_, const vector2 & direction_);

      bool is_valid() const;

      double get_a() const;

      double get_b() const;

      double get_c() const;

      double get_theta() const;

      double get_r() const;
    
      static line2 make_from_theta_r(double theta_, double r_);
          
      static line2 make_from_start_stop(const point2 & start_, const point2 & stop__);
          
      static line2 make_from_start_direction(double x0_, double y0_, double ux_, double uy_);

      bool is_horizontal(double tolerance_ = 1e-14) const;

      bool is_vertical(double tolerance_ = 1e-14) const;
    
      vector2 get_direction() const;
     
      vector2 get_orthogonal_direction() const;
    
      double distance(const point2 & pos_) const;

      point2 orthogonal_projection(const point2 & pos_) const;
   
      point2 get_reference_point() const override;
    
      void compute(double s_, point2 & vtx_) const override;
 
      void generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_ = 100) const override;

      line2 make_perp(const point2 & p_) const; 
    
      bool unique_intersection(const line2 & other_, point2 & p_) const; 
    
      bool unique_projection(const point2 & from_, const vector2 & direction_, point2 & p_) const; 

      bool is_same(const line2 & other_, double r_tolerance_, double theta_tolerance_) const;
    
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

  } // namespace geometry
  
} // namespace falaise

#endif // FALAISE_GEOMETRY_LINE2_HH
