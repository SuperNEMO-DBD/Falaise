/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__line_h
#define __CATAlgorithm__line_h 1

#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/experimental_vector.h>

namespace CAT {

namespace topology {

class line : public tracking_object {
  // a line is composed of two experimental points

 private:
  std::string appname_;

  // first experimental point
  experimental_point epa_;

  // second experimental point
  experimental_point epb_;

  // axis from first to second point (not normalized)
  experimental_vector forward_axis_;

  // line status
  bool used_;

 public:
  //! Default constructor
  line(mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200);

  //! Default destructor
  virtual ~line();

  //! constructor
  line(const experimental_point& epa, const experimental_point& epb,
       mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200);

  /*** dump ***/
  virtual void dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                    const std::string& a_indent = "", bool a_inherit = false) const;

  //! set experimental_points
  void set(const experimental_point& epa, const experimental_point& epb);

  //! set first experimental_point
  void set_epa(const experimental_point& epa);

  //! set second experimental_point
  void set_epb(const experimental_point& epb);

  //! set used
  void set_used(bool used);

  //! get experimental_point a
  const experimental_point& epa() const;

  //! get experimental_point b
  const experimental_point& epb() const;

  //! get forward axis
  const experimental_vector& forward_axis() const;

  //! get used
  bool used() const;

  experimental_double phi();

  experimental_double theta();

  experimental_double kink_phi(const line& l);

  experimental_double kink_theta(const line& l);

  double chi2(const line& l, bool use_theta_kink, double* chi2_just_phi);

  void set_a_forward_axis(const experimental_vector& v);

  line invert();

 private:
  void set_forward_axis();
};
}  // namespace topology
}  // namespace CAT

#endif  // __CATAlgorithm__line_h
