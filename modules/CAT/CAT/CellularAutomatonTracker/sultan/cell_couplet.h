/* -*- mode: c++ -*- */
#ifndef __sultan__ICELLCOUP
#define __sultan__ICELLCOUP
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>

namespace SULTAN {
namespace topology {

class cell_couplet : public tracking_object {
  // a cell_couplet is composed of two cells
  // and the tangents between them

 protected:
  std::string appname_;

  // first cell
  cell ca_;

  // second cell
  cell cb_;

  // unit axis from first to second cell
  experimental_vector forward_axis_;
  bool forward_axis_calculated_;

  // unit transverse axis
  experimental_vector transverse_axis_;
  bool transverse_axis_calculated_;

  // distance from first to second cell
  experimental_double distance_;

  // horizontal distance from first to second cell
  experimental_double distance_hor_;

 public:
  //! Default constructor
  cell_couplet();

  //! Default destructor
  virtual ~cell_couplet();

  //! constructor
  cell_couplet(const cell &ca, const cell &cb, mybhep::prlevel level = mybhep::NORMAL,
               double probmin = 1.e-200);

  //! constructor
  cell_couplet(const cell &ca, const cell &cb, const std::string &just,
               mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200);

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool a_inherit = false) const;

  //! set cells
  void set(const cell &ca, const cell &cb);

  //! set fwd axis
  void set_a_forward_axis(const experimental_vector &v);

  //! set trv axis
  void set_a_transverse_axis(const experimental_vector &v);

  //! set distance
  void set_a_distance(const experimental_double &d);

  //! set hor distance
  void set_a_hor_distance(const experimental_double &d);

  //! get first cell
  const cell &ca() const;

  //! get second cell
  const cell &cb() const;

  //! get forward axis
  const experimental_vector &forward_axis() const;

  //! get transverse axis
  const experimental_vector &transverse_axis() const;

  //! get distance
  const experimental_double &distance() const;

  //! get horizontal distance
  const experimental_double &distance_hor() const;

 public:
  void set_forward_axis(void);
  void set_transverse_axis(void);
  cell_couplet invert();

  friend bool operator==(const cell_couplet &left, const cell_couplet &right);
};

}  // namespace topology

}  // namespace SULTAN

#endif
