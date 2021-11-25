
#ifndef FALAISE_LTTC_PLUGIN_POINT_HH
#define FALAISE_LTTC_PLUGIN_POINT_HH

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Bayeux :
#include <datatools/utils.h>
#include <geomtools/utils.h>

namespace lttc {

  typedef geomtools::vector_2d point;
  typedef geomtools::vector_2d vector2;
  typedef std::vector<point>   polyline;

  void clear(point & p_);

  point barycenter(const point & p1_, double w1_, const point & p2_, double w2_);
  
  point middle(const point & p1_, const point & p2_);
 
  bool are_aligned(const point & p1_,
                   const point & p2_,
                   const point & p3_,
                   double tolerance_= 1e-14);

  void draw_point(std::ostream & out_, const point & p_, int tag_ = 0, bool endl_ = true);

  std::ostream & operator<<(std::ostream & out_, const polyline & pl_);

  void draw_polyline(std::ostream & out_, const polyline & pl_, int tag_ = 0);

  /// \brief Fitted point
  struct fitted_point
  {
    double x = datatools::invalid_real();     ///< X coordinate
    double y = datatools::invalid_real();     ///< Y coordinate
    double x_err = datatools::invalid_real(); ///< Error on X coordinate
    double y_err = datatools::invalid_real(); ///< Error on Y coordinate
    double angle = 0.0; ///< Angle of the main axis w/r to the x-axis

    bool is_valid() const;
    void reset();
    void draw(std::ostream & out_, double nsigma_ = 1.0, bool no_cross_ = false, int tag_ = 0) const;
    bool inside(const point & p_, double nsigma_ = 1.0) const;
    
  };
  
} // namespace lttc

#endif // FALAISE_LTTC_PLUGIN_POINT_HH
