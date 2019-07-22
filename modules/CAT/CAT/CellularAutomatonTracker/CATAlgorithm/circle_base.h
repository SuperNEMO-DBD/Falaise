/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__circle_h
#define __CATAlgorithm__circle_h 1

#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/plane.h>

namespace CAT {
namespace topology {

class circle : public tracking_object {
  // a circle is identified by origin and radius

 private:
  std::string appname_;

  // experimental point
  experimental_point center_;

  // radius
  experimental_double radius_;

  // points in the circle are given by:
  // x(phi) = center_.x() + radius*cos(phi)
  // z(phi) = center_.z() + radius*sin(phi)

 public:
  //! Default constructor
  circle(mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200);

  //! Default destructor
  virtual ~circle();

  //! constructor
  circle(const experimental_point &center, const experimental_double &radius,
         mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200);

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool a_inherit = false) const;

  //! set
  void set(const experimental_point &center, const experimental_double &radius);

  //! set center
  void set_center(const experimental_point &center);

  //! set radius
  void set_radius(const experimental_double &radius);

  //! get center
  const experimental_point &center() const;

  //! get radius
  const experimental_double &radius() const;

  //! get curvature
  experimental_double curvature() const;

  // get the phi of a point
  experimental_double phi_of_point(const experimental_point &ep) const;
  experimental_double phi_of_point(const experimental_point &ep, double phi_ref) const;

  // get the position at parameter phi
  experimental_point position(const experimental_double &phi) const;

  // get the position at the theta of point p
  experimental_point position(const experimental_point &ep) const;
  experimental_point position(const experimental_point &ep, double phi_ref) const;

  // get the chi2 with point p
  double chi2(experimental_point &ep);
  double chi2(experimental_point &ep, double phi_ref);

  // get the chi2 with set of points
  double chi2(std::vector<experimental_point> &ps);

  void best_fit_pitch(std::vector<experimental_point> &ps, experimental_double *_pitch,
                      experimental_double *_center);

  bool intersect_plane(plane pl, experimental_point *ep, experimental_double _phi);

  bool intersect_circle(circle c, experimental_point *ep, experimental_double _phi);

  // get the points of max and min radius (from the origin) along the arc of circle between epa and
  // epb
  void point_of_max_min_radius(experimental_point epa, experimental_point epb,
                               experimental_point *epmax, experimental_point *epmin);
};

// average
circle average(const std::vector<circle> &vs);

// get circle through three points
circle three_points_circle(const experimental_point &epa, const experimental_point &epb,
                           const experimental_point &epc);

// get circle that best fits coordinates
circle best_fit_circle(std::vector<experimental_double> &xs, std::vector<experimental_double> &zs);

}  // namespace topology
}  // namespace CAT

#endif  // __CATAlgorithm__circle_h
