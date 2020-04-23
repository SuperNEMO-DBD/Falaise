// utils.cc

// Ourselves:
#include <mctools/g4/processes/utils.h>

// Third party:
// - Geant4:
#include <G4EmProcessSubType.hh>

namespace mctools {

  namespace g4 {

    namespace processes {

      auto process_type_to_label(G4ProcessType type_) -> std::string
      {
        switch (type_) {
        case fNotDefined         : return std::string("undefined");
        case fTransportation     : return std::string("transportation");
        case fElectromagnetic    : return std::string("electromagnetic");
        case fOptical            : return std::string("optical");
        case fHadronic           : return std::string("hadronic");
        case fPhotolepton_hadron : return std::string("photolepton_hadron");
        case fDecay              : return std::string("decay");
        case fGeneral            : return std::string("general");
        case fParameterisation   : return std::string("parameterisation");
        case fUserDefined        : return std::string("user_defined");
        case fParallel           : return std::string("parallel");
        default                  : return std::string("undefined");
        }
      }

      auto label_to_process_type(const std::string & label_) -> G4ProcessType
      {
        if (label_ == process_type_to_label(fNotDefined)) { return fNotDefined;
}
        if (label_ == process_type_to_label(fTransportation)) { return fTransportation;
}
        if (label_ == process_type_to_label(fElectromagnetic)) { return fElectromagnetic;
}
        if (label_ == process_type_to_label(fOptical)) { return fOptical;
}
        if (label_ == process_type_to_label(fHadronic)) { return fHadronic;
}
        if (label_ == process_type_to_label(fPhotolepton_hadron)) { return fPhotolepton_hadron;
}
        if (label_ == process_type_to_label(fDecay)) { return fDecay;
}
        if (label_ == process_type_to_label(fGeneral)) { return fGeneral;
}
        if (label_ == process_type_to_label(fParameterisation)) { return fParameterisation;
}
        if (label_ == process_type_to_label(fUserDefined)) { return fUserDefined;
}
        if (label_ == process_type_to_label(fParallel)) { return fParallel;
}
        return fNotDefined;
      }

      auto em_process_subtype_to_label(int type_) -> std::string
      {
        switch (type_) {
        case fCoulombScattering     : return std::string("coulomb_scattering");
        case fIonisation            : return std::string("ionisation");
        case fBremsstrahlung        : return std::string("bremsstrahlung");
        case fPairProdByCharged     : return std::string("pair_production_by_charged");
        case fAnnihilation          : return std::string("annihilation");
        case fAnnihilationToMuMu    : return std::string("annihilation_to_mumu");
        case fAnnihilationToHadrons : return std::string("annihilation_to_hadrons");
        case fNuclearStopping       : return std::string("nuclear_stopping");
        case fMultipleScattering    : return std::string("multiple_scattering");
        case fRayleigh              : return std::string("rayleigh");
        case fPhotoElectricEffect   : return std::string("photoelectric_effect");
        case fComptonScattering     : return std::string("compton_scattering");
        case fGammaConversion       : return std::string("gamma_conversion");
        case fGammaConversionToMuMu : return std::string("gamma_conversion_to_mumu");
        case fCerenkov              : return std::string("cerenkov");
        case fScintillation         : return std::string("scintillation");
        case fSynchrotronRadiation  : return std::string("synchrotron_radiation");
        case fTransitionRadiation   : return std::string("transition_radiation");
        default: return std::string("");
        }
      }

      auto label_to_em_process_subtype(const std::string & label_) -> int
      {
        if (label_ == em_process_subtype_to_label(fCoulombScattering)) { return fCoulombScattering;
}
        if (label_ == em_process_subtype_to_label(fIonisation)) { return fIonisation;
}
        if (label_ == em_process_subtype_to_label(fBremsstrahlung)) { return fBremsstrahlung;
}
        if (label_ == em_process_subtype_to_label(fPairProdByCharged)) { return fPairProdByCharged;
}
        if (label_ == em_process_subtype_to_label(fAnnihilation)) { return fAnnihilation;
}
        if (label_ == em_process_subtype_to_label(fAnnihilationToMuMu)) { return fAnnihilationToMuMu;
}
        if (label_ == em_process_subtype_to_label(fAnnihilationToHadrons)) { return fAnnihilationToHadrons;
}
        if (label_ == em_process_subtype_to_label(fNuclearStopping)) { return fNuclearStopping;
}
        if (label_ == em_process_subtype_to_label(fMultipleScattering)) { return fMultipleScattering;
}
        if (label_ == em_process_subtype_to_label(fRayleigh)) { return fRayleigh;
}
        if (label_ == em_process_subtype_to_label(fPhotoElectricEffect)) { return fPhotoElectricEffect;
}
        if (label_ == em_process_subtype_to_label(fComptonScattering)) { return fComptonScattering;
}
        if (label_ == em_process_subtype_to_label(fGammaConversion)) { return fGammaConversion;
}
        if (label_ == em_process_subtype_to_label(fGammaConversionToMuMu)) { return fGammaConversionToMuMu;
}
        if (label_ == em_process_subtype_to_label(fCerenkov)) { return fCerenkov;
}
        if (label_ == em_process_subtype_to_label(fScintillation)) { return fScintillation;
}
        if (label_ == em_process_subtype_to_label(fSynchrotronRadiation)) { return fSynchrotronRadiation;
}
        if (label_ == em_process_subtype_to_label(fTransitionRadiation)) { return fTransitionRadiation;
}
        return -1;
      }

    } // end of namespace processes

  } // end of namespace g4

} // end of namespace mctools
