/// \file mctools/tracking_action.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user tracking action class
 *
 * History:
 *
 */

#ifndef SNSIM_TRACKING_ACTION_H
#define SNSIM_TRACKING_ACTION_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Geant4:
#include <G4UserTrackingAction.hh>

// This project:
#include <snsim/loggable_support.h>

namespace datatools {
class properties;
}

namespace snsim {

/// \brief Tracking action using the Geant4 interface
class tracking_action : public G4UserTrackingAction, public loggable_support {
 public:
  /// Constructor
  tracking_action();

  /// Destructor
  virtual ~tracking_action();

  /// Initialize from properties
  void initialize(const datatools::properties& config_);

  /// Pre-tracking action for the Geant4 interface
  virtual void PreUserTrackingAction(const G4Track*);

  /// Post-tracking action for the Geant4 interface
  virtual void PostUserTrackingAction(const G4Track*);
};

}  // namespace snsim

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::tracking_action)

#endif  // SNSIM_TRACKING_ACTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
