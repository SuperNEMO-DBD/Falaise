// stepping_verbose.cc

// Ourselves:
#include <mctools/g4/stepping_verbose.h>

// Third party:
// - Geant4:
// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include "G4SteppingManager.hh"
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "G4NavigationHistory.hh"
#include "G4ReferenceCountedHandle.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"

namespace snsim {

stepping_verbose::stepping_verbose() = default;

stepping_verbose::~stepping_verbose() = default;

void stepping_verbose::StepInfo() {
  CopyState();

  int saved_precision = std::clog.precision(10);

  if (verboseLevel >= 1) {
    if (verboseLevel >= 4) {
      VerboseTrack();
    }
    if (verboseLevel >= 3) {
      std::clog << std::endl;
      std::clog << "snsim::stepping_verbose::StepInfo " << std::endl
                << std::setw(6) << "Step num" << std::setw(6) << "X"
                << "    " << std::setw(6) << "Y"
                << "    " << std::setw(6) << "Z"
                << "    " << std::setw(9) << "KineE"
                << " " << std::setw(9) << "dEStep"
                << " " << std::setw(10) << "StepLeng" << std::setw(10) << "TrakLeng"
                << "  " << std::setw(20) << "Volume"
                << "  " << std::setw(10) << "Material" << std::setw(10) << "Process" << std::endl;
    }

    std::clog << std::setw(5) << fTrack->GetCurrentStepNumber() << " " << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().x(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().y(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().z(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetKineticEnergy(), "Energy") << std::setw(6)
              << G4BestUnit(fStep->GetTotalEnergyDeposit(), "Energy") << std::setw(6)
              << G4BestUnit(fStep->GetStepLength(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetTrackLength(), "Length") << "  ";

    // if (fStepStatus != fWorldBoundary){
    if (fTrack->GetNextVolume() != nullptr) {
      std::clog << std::setw(20) << fTrack->GetVolume()->GetName();
    } else {
      std::clog << std::setw(20) << "<out of world>";
    }

    std::clog << std::setw(10) << fTrack->GetMaterial()->GetName();

    if (fStep->GetPostStepPoint()->GetProcessDefinedStep() != nullptr) {
      std::clog << "  " << std::setw(10)
                << fStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    } else {
      std::clog << "   UserLimit";
    }

    std::clog << std::endl;

    if (verboseLevel == 2) {
      G4int tN2ndariesTot = fN2ndariesAtRestDoIt + fN2ndariesAlongStepDoIt + fN2ndariesPostStepDoIt;
      if (tN2ndariesTot > 0) {
        std::clog << std::endl
                  << "    :----- List of 2ndaries - "
                  << "#SpawnInStep=" << std::setw(3) << tN2ndariesTot << "  "
                  << " (Rest=" << std::setw(2) << fN2ndariesAtRestDoIt << ",Along=" << std::setw(2)
                  << fN2ndariesAlongStepDoIt << ",Post=" << std::setw(2) << fN2ndariesPostStepDoIt
                  << "), "
                  << "#SpawnTotal=" << std::setw(3) << (*fSecondary).size() << " ---------------"
                  << std::endl;

        for (size_t lp1 = (*fSecondary).size() - tN2ndariesTot; lp1 < (*fSecondary).size(); lp1++) {
          std::clog << "    : " << std::setw(6)
                    << G4BestUnit((*fSecondary)[lp1]->GetPosition().x(), "Length") << std::setw(6)
                    << G4BestUnit((*fSecondary)[lp1]->GetPosition().y(), "Length") << std::setw(6)
                    << G4BestUnit((*fSecondary)[lp1]->GetPosition().z(), "Length") << std::setw(6)
                    << G4BestUnit((*fSecondary)[lp1]->GetKineticEnergy(), "Energy") << std::setw(10)
                    << (*fSecondary)[lp1]->GetDefinition()->GetParticleName() << std::endl;
        }

        std::clog << "    :-----------------------------"
                  << "----------------------------------"
                  << "-- EndOf2ndaries Info ---------------" << std::endl;
      }
    }
  }
  std::clog.precision(saved_precision);
}

void stepping_verbose::TrackingStarted() {
  CopyState();

  int saved_precision = std::clog.precision(10);
  if (verboseLevel > 0) {
    std::clog << std::setw(5) << "In Stepping Verbose: Tracking Started:" << std::endl
              << " Step " << std::setw(6) << "X"
              << "    " << std::setw(6) << "Y"
              << "    " << std::setw(6) << "Z"
              << "    " << std::setw(9) << "KineE"
              << " " << std::setw(9) << "dEStep"
              << " " << std::setw(10) << "StepLeng" << std::setw(10) << "TrakLeng"
              << "  " << std::setw(20) << "Volume"
              << "  " << std::setw(10) << "Material" << std::setw(10) << "Process" << std::endl;

    std::clog << std::setw(5) << fTrack->GetCurrentStepNumber() << " " << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().x(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().y(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetPosition().z(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetKineticEnergy(), "Energy") << std::setw(6)
              << G4BestUnit(fStep->GetTotalEnergyDeposit(), "Energy") << std::setw(6)
              << G4BestUnit(fStep->GetStepLength(), "Length") << std::setw(6)
              << G4BestUnit(fTrack->GetTrackLength(), "Length") << "  ";

    if (fTrack->GetNextVolume() != nullptr) {
      std::clog << std::setw(10) << fTrack->GetVolume()->GetName();
    } else {
      std::clog << std::setw(20) << "<out of world>";
    }
    // std::clog << std::setw (10) << "  XXXXXX";
    // std::clog << "    initStep" << std::endl;
    std::clog << std::endl;
  }
  std::clog.precision(saved_precision);
}

}  // namespace snsim
