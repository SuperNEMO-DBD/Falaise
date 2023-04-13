
#ifndef FALAISE_GEOMETRY_POINT_HH
#define FALAISE_GEOMETRY_POINT_HH

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - Bayeux :
#include <bayeux/datatools/utils.h>
#include <bayeux/geomtools/utils.h>

namespace falaise {
  
  namespace geometry {

    typedef geomtools::vector_2d point2;
    typedef geomtools::vector_2d vector2;

    typedef geomtools::vector_3d point3;
    typedef geomtools::vector_3d vector3;

    point3 to_3d(const point2 & p2_, double z_ = 0.0);

    point2 to_2d(const point3 & p3_);

    void clear(point2 & p_);

    void clear(point3 & p_);

    point2 barycenter(const point2 & p1_, double w1_, const point2 & p2_, double w2_);

    point3 barycenter(const point3 & p1_, double w1_, const point3 & p2_, double w2_);
  
    point2 middle(const point2 & p1_, const point2 & p2_);
  
    point3 middle(const point3 & p1_, const point3 & p2_);
 
    bool are_aligned(const point2 & p1_,
                     const point2 & p2_,
                     const point2 & p3_,
                     double tolerance_= 1e-14);

    void draw_point(std::ostream & out_, const point2 & p_, int tag_ = 0, bool endl_ = true);

    void draw_point(std::ostream & out_, const point3 & p_, int tag_ = 0, bool endl_ = true);
 
    struct polyline2 : public std::vector<point2>
    {
      std::vector<double> s; ///< Curvilinear coordinates associated to each point
      void clear();
    };

    void draw_polyline(std::ostream & out_, const polyline2 & pl_, int tag_ = 0);
 
    struct polyline3 : public std::vector<point3>
    {
      std::vector<double> s; ///< Curvilinear coordinates associated to each point
      void from_2d(const polyline2 & pl2_, double z_);
      void clear();
    };

    void draw_polyline(std::ostream & out_, const polyline3 & pl_, int tag_ = 0);

  } // namespace geometry
  
} // namespace falaise

#endif // FALAISE_GEOMETRY_POINT_HH
