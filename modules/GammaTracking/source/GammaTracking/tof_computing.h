#ifndef GT_TOF_COMPUTING_H
#define GT_TOF_COMPUTING_H 1

#include "event.h"

namespace gt {

/// \brief Basic class to host Time-Of-Flight related functions
class tof_computing {
 public:
  /// Return beta factor
  static double beta(double energy_, double mass_);

  /// Return theoritical time given the distance, energy ans mass of the particle
  static double get_theoritical_time(double energy_, double mass_, double track_length_);

  /// Return distance between two calorimeter hits
  static double get_track_length(const event::calorimeter_hit& hit1_,
                                 const event::calorimeter_hit& hit2_);

  /// Return delta time between two calorimeters hits
  static double get_delta_time(const event::calorimeter_hit& hit1_,
                               const event::calorimeter_hit& hit2_);

  /// Compute X² value between two calorimeter hits
  static double get_chi2(const event::calorimeter_hit& hit1_, const event::calorimeter_hit& hit2_);

  /// Return the internal probability given the X² value
  static double get_internal_probability(double chi2_, size_t ndf_ = 1);
};

}  // namespace gt
#endif  // GT_TOF_COMPUTING_H
