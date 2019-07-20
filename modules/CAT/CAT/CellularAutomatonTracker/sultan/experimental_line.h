/* -*- mode: c++ -*- */
#ifndef __sultan__EXPERIMENTALLINE
#define __sultan__EXPERIMENTALLINE
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/experimental_point.h>
#include <sultan/cell.h>

namespace SULTAN {
namespace topology {

class experimental_line : public tracking_object {
  // a experimental_line is identified by two points

 private:
  std::string appname_;

  experimental_point epa_;
  experimental_point epb_;

  // points in the line are given by:
  //
  // dir = (epb - epa).unit()
  // x(s) = xa + dirx * s
  // y(s) = ya + diry * s
  // z(s) = za + dirz * s

  experimental_vector dir_;
  experimental_double length_;

 public:
  //! Default constructor
  experimental_line(mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    appname_ = "experimental_line: ";
    epa_ = experimental_point();
    epb_ = experimental_point();
    set_print_level(level);
    set_probmin(probmin);
  }

  //! Default destructor
  virtual ~experimental_line(){};

  //! constructor
  experimental_line(experimental_point epa, experimental_point epb,
                    mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    set_print_level(level);
    set_probmin(probmin);
    appname_ = "experimental_line: ";
    epa_ = epa;
    epb_ = epb;
    calculate_dir();
  }

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool /*a_inherit*/ = false) const {
    {
      std::string indent;
      if (!a_indent.empty()) indent = a_indent;
      if (!a_title.empty()) {
        a_out << indent << a_title << std::endl;
      }

      a_out << indent << appname_ << " pa [";
      epa_.dump();
      a_out << "] pb [";
      epb_.dump();
      a_out << " ]" << std::endl;

      return;
    }
  }

  void calculate_dir();

  void set_epa(experimental_point a) {
    epa_ = a;
    calculate_dir();
  }
  void set_epb(experimental_point b) {
    epb_ = b;
    calculate_dir();
  }
  void set(topology::experimental_point epa, topology::experimental_point epb) {
    epa_ = epa;
    epb_ = epb;
    calculate_dir();
  }

  topology::experimental_point epa() const { return epa_; }
  topology::experimental_point epb() const { return epb_; }

  topology::experimental_point position(topology::experimental_double s) const;

  double distance_from_cell_measurement(topology::cell c, topology::experimental_double *DR,
                                        topology::experimental_double *DH) const;

  double distance_from_cell_center(topology::cell c, topology::experimental_double *DR,
                                   topology::experimental_double *DH) const;

  topology::experimental_vector dir() const { return dir_; }

  topology::experimental_double length() const { return length_; }
};

}  // namespace topology

}  // namespace SULTAN

#endif
