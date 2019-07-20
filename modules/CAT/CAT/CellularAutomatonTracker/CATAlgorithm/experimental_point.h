/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IEPOINT
#define __CATAlgorithm__IEPOINT
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/experimental_double.h>

namespace CAT {

namespace topology {

//  class experimental_point : public point{
class experimental_point {
  // an experimental point is composed of an ordinary point (x, y, z)
  // with corresponding error (ex, ey, ez)

 private:
  std::string appname_;

  // x coordinate
  experimental_double x_;

  // y coordinate
  experimental_double y_;

  // z coordinate
  experimental_double z_;

  // radius in horizontal view (= x^2 + z^2)
  experimental_double radius_;

 public:
  //! Default constructor
  experimental_point();

  //! Default destructor
  virtual ~experimental_point();

  //! constructor
  experimental_point(const experimental_double &x, const experimental_double &y,
                     const experimental_double &z);

  //! constructor from coordinates with error
  experimental_point(double x, double y, double z, double ex, double ey, double ez);

  //! constructor
  experimental_point(const mybhep::point &p, double ex, double ey, double ez);

  //! constructor from bhep hit
  experimental_point(const mybhep::hit &hit);

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool a_inherit = false) const;

  //! set point and errors
  void set(const mybhep::point &p, double ex, double ey, double ez);

  //! set x
  void set_x(const experimental_double &x);

  //! set ex
  void set_ex(double ex);

  //! set y
  void set_y(const experimental_double &y);

  //! set ey
  void set_ey(double ey);

  //! set z
  void set_z(const experimental_double &z);

  //! set ez
  void set_ez(double ez);

  //! get experimental x
  const experimental_double &x() const;

  //! get experimental y
  const experimental_double &y() const;

  //! get experimental z
  const experimental_double &z() const;

  //! get experimental radius
  const experimental_double &radius() const;

  //! distance
  experimental_double distance(const experimental_point &p2) const;

  //! distance
  experimental_double hor_distance(const experimental_point &p2) const;

 private:
  void set_radius();
};

// average
experimental_point average(const std::vector<experimental_point> &vs);

}  // namespace topology
}  // namespace CAT

#endif
