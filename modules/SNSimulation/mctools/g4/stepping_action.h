/// \file mctools/stepping_action.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user stepping action class
 *
 * History:
 *
 */

#ifndef SNSIM_STEPPING_ACTION_H
#define SNSIM_STEPPING_ACTION_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Geant4:
#include <G4StepStatus.hh>
#include <G4SteppingControl.hh>
#include <G4TrackStatus.hh>
#include <G4UserSteppingAction.hh>

// This project:
#include <mctools/g4/loggable_support.h>

namespace datatools {
class properties;
}

namespace snsim {

/// \brief Stepping action using the Geant4 interface
class stepping_action : public G4UserSteppingAction, public loggable_support {
 public:
  /// Constructor
  stepping_action();

  /// Destructor
  virtual ~stepping_action();

  /// Initialize
  void initialize(const datatools::properties &config_);

  /// Reset
  void reset();

  /// Set dumped flag
  void set_dumped(bool);

  /// Check dumped flag
  bool is_dumped() const;

  /// Main stepping action for the Geant4 interface
  void UserSteppingAction(const G4Step *);

  /// Convert a Geant4 track status to a printable string
  static std::string g4_track_status_to_label(G4TrackStatus);

  /// Convert a Geant4 stepping control to a printable string
  static std::string g4_stepping_control_to_label(G4SteppingControl);

  /// Convert a Geant4 step status to a printable string
  static std::string g4_step_status_to_label(G4StepStatus);

 protected:
  /// Embedded stepping action
  virtual void _stepping_action_base(const G4Step *);

 private:
  // Configuration:
  bool _dumped_;  ///< Activation flag for dump

  // Working data:
  uint32_t _number_of_steps_;  ///< Counter for steps
};

}  // namespace snsim

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::stepping_action)

#endif  // SNSIM_STEPPING_ACTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
