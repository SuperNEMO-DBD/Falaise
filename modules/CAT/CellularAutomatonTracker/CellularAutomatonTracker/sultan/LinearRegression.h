/* -*- mode: c++ -*- */
#ifndef __sultan__ILINEARREGRESSION
#define __sultan__ILINEARREGRESSION

#include <iostream>
#include <cmath>
#include <sultan/experimental_double.h>
#include <sultan/tracking_object.h>

namespace SULTAN {
namespace topology {

class LinearRegression : public tracking_object {
  // operate weighted linear regression with formula:
  //  y = y0 + tangent x
  // to find coefficients y0 and tangent

 private:
  std::string appname_;

  double y0_;

  double tangent_;

  std::vector<double> yi_;
  std::vector<double> xi_;

 public:
  //! Default destructor
  virtual ~LinearRegression(){};

  //! Default constructor
  LinearRegression(mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    appname_ = "LinearRegression: ";
    y0_ = 0.;
    tangent_ = 0.;
    xi_.clear();
    yi_.clear();
    set_print_level(level);
    set_probmin(probmin);
  }

  //! constructor
  LinearRegression(std::vector<double> &xi, std::vector<double> &yi,
                   mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    set_print_level(level);
    set_probmin(probmin);
    appname_ = "LinearRegression: ";
    xi_ = xi;
    yi_ = yi;
  }

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool /*a_inherit*/ = false) {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;
    if (!a_title.empty()) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << appname_ << " -------------- " << std::endl;
    a_out << indent << " points: " << std::endl;
    for (std::vector<double>::iterator it = xi_.begin(); it != xi_.end(); ++it) {
      a_out << indent << " .. x " << *it << " y " << yi_[it - xi_.begin()] << " predicted "
            << position(*it) << std::endl;
    }
    a_out << indent << " y0: " << y0() << " " << std::endl;
    a_out << indent << " tangent: " << tangent() << " " << std::endl;

    a_out << indent << " -------------- " << std::endl;

    return;
  }

  //! set
  void set(const std::vector<double> &xi, const std::vector<double> &yi);

  //! set xi
  void set_xi(const std::vector<double> &xi);

  //! set yi
  void set_yi(const std::vector<double> &yi);

  //! set y0
  void set_y0(const double &y0);

  //! set tangent
  void set_tangent(const double &tangent);

  //! get xi
  const std::vector<double> &xi() const;

  //! get yi
  const std::vector<double> &yi() const;

  //! get y0
  const double &y0() const;

  //! get tangent
  const double &tangent() const;

  bool fit(void);

  double position(const double &x);

  void invert();

  double parameter(const double &x);
};

}  // namespace topology
}  // namespace SULTAN
#endif
