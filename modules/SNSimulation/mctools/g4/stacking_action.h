/// \file mctools/stacking_action.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *   G4 user stacking action class
 *
 * History:
 *
 */

#ifndef SNSIM_STACKING_ACTION_H
#define SNSIM_STACKING_ACTION_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Geant4:
#include <G4UserStackingAction.hh>

// This project:
#include <mctools/g4/loggable_support.h>

class G4Track;
class G4LogicalVolume;
class G4Navigator;
class G4Material;

namespace datatools {
class properties;
}

namespace snsim {

/// \brief Stacking action using the Geant4 interface
class stacking_action : public G4UserStackingAction, public loggable_support {
 public:
  /// Constructor
  stacking_action();

  /// Destructor
  virtual ~stacking_action();

  /// Initialize
  void initialize(const datatools::properties &config_);

  // Geant4 interface :
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *a_track);

  // Geant4 interface :
  virtual void NewStage();

  // Geant4 interface :
  virtual void PrepareNewEvent();

 private:
  G4Navigator *_g4_navigator_;
  bool _kill_secondary_particles_;
  std::vector<std::string> _killer_volume_names_;
  std::vector<G4LogicalVolume *> _killer_volumes_;
  std::vector<std::string> _killer_material_names_;
  std::vector<G4Material *> _killer_materials_;
};

}  // namespace snsim

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::stacking_action)

#endif  // SNSIM_STACKING_ACTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
