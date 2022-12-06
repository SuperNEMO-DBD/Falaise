#ifndef FALAISE_LTTC_PLUGIN_TANGENT_CIRCLE_3X1_HH
#define FALAISE_LTTC_PLUGIN_TANGENT_CIRCLE_3X1_HH

// Standard library:
#include <cmath>
#include <vector>
#include <iostream>
#include <limits>

// Third party:
#include <datatools/clhep_units.h>

namespace lttc {
  
  namespace gergonne {

    /// \brief Point of the plane with cartesian coordinates
    struct point
    {
      double x = std::numeric_limits<double>::quiet_NaN();
      double y = std::numeric_limits<double>::quiet_NaN();
      
      inline bool is_valid() const
      {
        return x == x and y == y;
      }
  
      inline void invalidate()
      {
        x = std::numeric_limits<double>::quiet_NaN();
        y = std::numeric_limits<double>::quiet_NaN();
      }
  
      inline double dist(const point & p) const
      {
        return std::hypot(x - p.x, y - p.y);
      }

      friend std::ostream & operator<<(std::ostream & out_, const point & p_)
      {
        out_ << '(' << p_.x << ',' << p_.y << ')';
        return out_;
      }
    };

    /// \brief Triplet of indexed point in the plane
    struct triplet
    {
      int   indexes[3]; ///< Hit indexes
      point p[3]; ///< Contact points associated to hits
    };

    /// \brief Circle in the plane
    struct circle
    {
      point  p; ///< Center
      double r = std::numeric_limits<double>::quiet_NaN(); ///< Radius
  
      inline bool is_valid() const
      {
        return p.is_valid() and r == r;
      }
  
      inline void invalidate()
      {
        p.invalidate();
        r = std::numeric_limits<double>::quiet_NaN();
      }
  
      inline bool tangent(const circle & c, double eps) const
      {
        double d = p.dist(c.p);
        double r1 = std::abs(r - c.r);
        double r2 = std::abs(r + c.r);
        if (std::abs(d-r1) <= eps) return true;
        if (std::abs(d-r2) <= eps) return true;
        return false;
      }
    };

    circle make_circle_from_three_points(const point & p1, const point & p2, const point & p3, double eps_, bool debug_ = false);

    /// Intermediate result of the 'solver' method
    struct result_type
    {
      int i = -1; ///< Hit rank
      gergonne::point p;
    };

    struct solver1
    {
      bool debug = false;
      const circle ** circles;
      int i;
      double a1,a2,b1,b2;
      double eps;
      double eps2;
      double eps3;
      
      solver1(const circle ** circles_,
              int i_,
              double a1_, double b1_,
              double a2_, double b2_,
              double eps_,
              bool debug_
              );

      void solve(double r1_, double r2_, double r3_,
                 std::vector<result_type> & results_) const;

    };

  } // namespace gergonne 

  struct tangent_circle_3x1
  {
  public:
  
    struct config_type
    {
      bool   debug   = false;
      double epsilon = 1e-5 * CLHEP::mm;
    };

    // Configuration:
    config_type config;

    // Working data:
    const gergonne::circle * circles[3];

  public:

    struct contact_config
    {
      inline contact_config(bool f0_, bool f1_, bool f2_)
      {
        flags[0] = f0_;
        flags[1] = f1_;
        flags[2] = f2_;
      }
      bool flags[3];
    };

    // List of four 3-contact configuration to be considered while solving the Appolonois problem 
    static const std::vector<contact_config> & contact_configs()
    {
      static const std::vector<contact_config> _cfgs{{true, true, true},
                                                     {true, true, false},
                                                     {true, false, true},
                                                     {true, false, false}};
                                                     // {false, true, true},
                                                     // {false, true, false},
                                                     // {false, false, true},
                                                     // {false, false, false}};
      return _cfgs;
    }

    /// \brief Result of the search of the circle tangent to 3 circles 
    struct circle_data
    {
      int              hit_index[3];
      gergonne::point  contacts[3];
      gergonne::circle tangent_circle;
      void print(std::ostream & out_) const;
    };

    struct input_data
    {
      int                hit_indexes[3]; ///< Hit indexes
      gergonne::circle * circles[3]; ///< Hit drift circle
    };

    struct output_data
    {
      void clear()
      {
        circles.clear();
      }
      std::vector<circle_data> circles;
    };

    /// Constructor
    tangent_circle_3x1(const config_type & cfg_);
    
    /// \brief Main processing method
    void process(const input_data & idata_, output_data & odata_);

    /// \brief Find some tangent circles for a specific contact configuration
    void solve(const contact_config & cfg_, std::vector<gergonne::result_type> & results_);

  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TANGENT_CIRCLE_3X1_HH
