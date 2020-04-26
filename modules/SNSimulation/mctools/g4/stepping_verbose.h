/// \file mctools/stepping_verbose.h

#ifndef SNSIM_STEPPING_VERBOSE_H
#define SNSIM_STEPPING_VERBOSE_H 1

// Third party:
// - Geant4:
// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4SteppingVerbose.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace snsim {

/// \brief Stepping verbose support using the Geant4 interface
class stepping_verbose : public G4SteppingVerbose {
 public:
  /// Constructor
  stepping_verbose();

  /// Destructor
  virtual ~stepping_verbose();

  /// Geant4 interface
  void StepInfo();

  /// Geant4 interface
  void TrackingStarted();
};

}  // namespace snsim

#endif  // SNSIM_STEPPING_VERBOSE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
