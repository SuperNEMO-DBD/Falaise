/// \file mctools/g4/sensitive_hit.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2010-05-26
 * Last modified : 2013-06-03
 *
 * License:
 *
 * Description:
 *
 *   GEANT4 sensitive hit implementation
 *
 * History:
 *
 */

#ifndef SNSIM_SENSITIVE_HIT_H
#define SNSIM_SENSITIVE_HIT_H 1

// Third party:
// - Geant4
#include <G4VHit.hh>

// This project:
#include <mctools/base_step_hit.h>

namespace snsim {

/// \brief A sensitive hit using the Geant4 interface with an embedded base step hit
class sensitive_hit : public G4VHit {
 public:
  /// Return const reference to embedded base step hit
  const mctools::base_step_hit &get_hit_data() const;

  /// Return mutable reference to embedded base step hit
  mctools::base_step_hit &grab_hit_data();

  /// Default constructor
  sensitive_hit() = default;

  /// Destructor
  virtual ~sensitive_hit() = default;

  /// Copy Constructor
  sensitive_hit(const sensitive_hit &) = default;

  /// Copy assignment
  sensitive_hit &operator=(const sensitive_hit &) = default;

  /// Move Constructor
  sensitive_hit(sensitive_hit &&) = default;

  /// Move assignment
  sensitive_hit &operator=(sensitive_hit &&) = default;

  /// Reset to default values
  void reset();

 private:
  mctools::base_step_hit _hit_data_;  //!< Basic MC step hit data
};

}  // namespace snsim

#endif  // SNSIM_SENSITIVE_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
