/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICYLINDER
#define __CATAlgorithm__ICYLINDER
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <topology/experimental_point.h>
#include <topology/experimental_vector.h>

namespace CAT {
namespace topology {

class cylinder : public tracking_object {
  // a cylinder is identified by one point (center)
  // one axis
  // and a radius

 private:
  std::string appname_;

  // experimental point
  experimental_point center_;

  // axis
  experimental_vector axis_;

  // radius
  experimental_double radius_;

 public:
  //! Default constructor
  cylinder(prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    appname_ = "cylinder: ";
    center_ = experimental_point();
    axis_ = experimental_vector(small_neg, small_neg, small_neg, small_neg, small_neg, small_neg);
    radius = experimental_double(small_neg, small_neg);
    set_print_level(level);
    set_probmin(probmin);
  }

  //! Default destructor
  virtual ~cylinder(){};

  //! constructor
  cylinder(experimental_point &center, experimental_vector &axis, experimental_double &radius,
           prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    set_print_level(level);
    set_probmin(probmin);
    appname_ = "cylinder: ";
    center_ = center;
    axis_ = axis;
    radius_ = radius;
  }

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool a_inherit = false) {
    {
      std::string indent;
      if (!a_indent.empty()) indent = a_indent;
      if (!a_title.empty()) {
        a_out << indent << a_title << std::endl;
      }

      a_out << indent << appname_ << " -------------- " << std::endl;
      a_out << indent << " center " << std::endl;
      this->center().dump(a_out, "", indent + "    ");
      a_out << indent << " axis " << std::endl;
      this->axis().dump(a_out, "", indent + "    ");
      a_out << indent << " radius: ";
      radius().dump();
      a_out << " " << std::endl;
      a_out << indent << " -------------- " << std::endl;

      return;
    }
  }

  //! set
  void set(experimental_point &center, experimental_vector &axis, experimental_double &radius) {
    center_ = center;
    axis_ = axis;
    radius_ = radius;
  }

  //! set center
  void set_center(experimental_point &center) { center_ = center; }

  //! set axis
  void set_axis(experimental_vector &axis) { axis_ = axis; }

  //! set radius
  void set_radius(experimental_double &radius) { radius_ = radius; }

  //! get center
  const experimental_point &center() const { return center_; }

  //! get axis
  const experimental_vector &axis() const { return axis_; }

  //! get radius
  const experimental_double &radius() const { return radius_; }
};
}  // namespace topology
}  // namespace CAT

#endif
