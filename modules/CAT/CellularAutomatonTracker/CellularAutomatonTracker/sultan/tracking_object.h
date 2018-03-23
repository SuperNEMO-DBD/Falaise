/* -*- mode: c++ -*- */

#ifndef __sultan__tracking_object_h
#define __sultan__tracking_object_h 1

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/printable.h>

namespace SULTAN {
namespace topology {

// a generic tracking object
class tracking_object : public printable {
 protected:
  //    Clock clock;

 public:
  /// print level
  mybhep::prlevel level_;

  // the minimum probabilty
  double probmin_;

  void set_probmin(double probmin);

  double probmin() const;

  double probof(double chi2, int ndof) const;

  double get_probmin() const;
};

}  // namespace topology
}  // namespace SULTAN

#endif  // __sultan__tracking_object_h
