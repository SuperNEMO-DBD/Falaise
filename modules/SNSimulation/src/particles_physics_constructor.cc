// particles_physics_constructor.cc

// Ourselves:
#include <mctools/g4/particles_physics_constructor.h>

// Standard library:
#include <stdexcept>
#include <iostream>

#include <datatools/units.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>

// G4 stuff :
#include <globals.hh>
#include <G4Version.hh>
#include <G4UnitsTable.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4ProcessManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Particles:
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>

// Bosons:
//#include <G4BosonConstructor.hh>
#include <G4ChargedGeantino.hh>
#include <G4Geantino.hh>
#include <G4Gamma.hh>
#include <G4OpticalPhoton.hh>

// Leptons
//#include <G4LeptonConstructor.hh>
#include <G4MuonPlus.hh>
#include <G4MuonMinus.hh>
#include <G4TauMinus.hh>
#include <G4TauPlus.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4NeutrinoTau.hh>
#include <G4AntiNeutrinoTau.hh>
#include <G4NeutrinoMu.hh>
#include <G4AntiNeutrinoMu.hh>
#include <G4NeutrinoE.hh>
#include <G4AntiNeutrinoE.hh>

// Mesons:
//#include <G4MesonConstructor.hh>
#include <G4PionPlus.hh>
#include <G4PionMinus.hh>
#include <G4PionZero.hh>
#include <G4Eta.hh>
#include <G4EtaPrime.hh>

#include <G4KaonPlus.hh>
#include <G4KaonMinus.hh>
#include <G4KaonZero.hh>
#include <G4AntiKaonZero.hh>
#include <G4KaonZeroLong.hh>
#include <G4KaonZeroShort.hh>

#include <G4DMesonPlus.hh>
#include <G4DMesonMinus.hh>
#include <G4DMesonZero.hh>
#include <G4AntiDMesonZero.hh>
#include <G4DsMesonPlus.hh>
#include <G4DsMesonMinus.hh>
#if G4VERSION_NUMBER == 960
#include <G4Etac.hh>
#endif // G4VERSION_NUMBER == 960
#include <G4JPsi.hh>

#include <G4BMesonPlus.hh>
#include <G4BMesonMinus.hh>
#include <G4BMesonZero.hh>
#include <G4AntiBMesonZero.hh>
#include <G4BsMesonZero.hh>
#include <G4AntiBsMesonZero.hh>
#if G4VERSION_NUMBER == 960
#include <G4BcMesonPlus.hh>
#include <G4BcMesonMinus.hh>
#include <G4Upsiron.hh>
#endif // G4VERSION_NUMBER == 960

// Baryons:
//#include <G4BaryonConstructor.hh>
#include <G4Proton.hh>
#include <G4AntiProton.hh>
#include <G4Neutron.hh>
#include <G4AntiNeutron.hh>

#include <G4Lambda.hh>
#include <G4SigmaPlus.hh>
#include <G4SigmaZero.hh>
#include <G4SigmaMinus.hh>
#include <G4XiMinus.hh>
#include <G4XiZero.hh>
#include <G4OmegaMinus.hh>

#include <G4AntiLambda.hh>
#include <G4AntiSigmaPlus.hh>
#include <G4AntiSigmaZero.hh>
#include <G4AntiSigmaMinus.hh>
#include <G4AntiXiMinus.hh>
#include <G4AntiXiZero.hh>
#include <G4AntiOmegaMinus.hh>

#include <G4LambdacPlus.hh>
#include <G4SigmacPlusPlus.hh>
#include <G4SigmacPlus.hh>
#include <G4SigmacZero.hh>
#include <G4XicPlus.hh>
#include <G4XicZero.hh>
#include <G4OmegacZero.hh>

#include <G4AntiLambdacPlus.hh>
#include <G4AntiSigmacPlusPlus.hh>
#include <G4AntiSigmacPlus.hh>
#include <G4AntiSigmacZero.hh>
#include <G4AntiXicPlus.hh>
#include <G4AntiXicZero.hh>
#include <G4AntiOmegacZero.hh>

#if G4VERSION_NUMBER == 960
#include <G4Lambdab.hh>
#include <G4SigmabPlus.hh>
#include <G4SigmabZero.hh>
#include <G4SigmabMinus.hh>
#include <G4XibZero.hh>
#include <G4XibMinus.hh>
#include <G4OmegabMinus.hh>
#endif // G4VERSION_NUMBER == 960

#if G4VERSION_NUMBER == 960
#include <G4AntiLambdab.hh>
#include <G4AntiSigmabPlus.hh>
#include <G4AntiSigmabZero.hh>
#include <G4AntiSigmabMinus.hh>
#include <G4AntiXibZero.hh>
#include <G4AntiXibMinus.hh>
#include <G4AntiOmegabMinus.hh>
#endif // G4VERSION_NUMBER == 960


// Ions:
// #include <G4IonConstructor.hh>
// Nuclei
#include <G4Alpha.hh>
#include <G4Deuteron.hh>
#include <G4Triton.hh>
#include <G4He3.hh>
#include <G4GenericIon.hh>
// AntiNuclei
#include <G4AntiAlpha.hh>
#include <G4AntiDeuteron.hh>
#include <G4AntiTriton.hh>
#include <G4AntiHe3.hh>

namespace mctools {

  namespace g4 {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (base_physics_constructor,
                                                               particles_physics_constructor,
                                                               "mctools::g4::particles_physics_constructor")


    particles_physics_constructor::particles_physics_constructor () : base_physics_constructor ()
    {
      this->particles_physics_constructor::_set_defaults ();
      SetPhysicsName("particles");
      SetPhysicsType(0);
      return;
    }

    particles_physics_constructor::~particles_physics_constructor ()
    {
      if (is_initialized()) {
        this->particles_physics_constructor::reset();
      }
      return;
    }

    void particles_physics_constructor::_set_defaults ()
    {
      // Bosons:
      _use_geantinos_                = false;
      _use_optical_photons_          = false;

      // Leptons:
      _use_muon_leptons_             = false;
      _use_tau_leptons_              = false;

      // Mesons:
      _use_light_mesons_             = false;
      _use_charm_mesons_             = false;
      _use_bottom_mesons_            = false;

      // Baryons:
      _use_nucleons_                 = true;
      _use_strange_baryons_          = false;
      _use_charm_baryons_            = false;
      _use_bottom_baryons_           = false;

      // Ions:
      _use_light_nuclei_             = true;
      _he3_as_generic_ion_           = false;
      _use_light_anti_nuclei_        = false;
      _use_generic_ion_              = true;

      return;
    }

    void particles_physics_constructor::initialize (const datatools::properties & config_,
                                                    physics_constructor_dict_type & /*dict_*/)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

      /*** parsing properties from 'config_'  ***/
      loggable_support::_initialize_logging_support(config_);


      // *********************** Particles *************************** //

      // Bosons:
      if (config_.has_flag ("use_geantinos")) {
        _use_geantinos_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use geantinos set to: " << _use_geantinos_);

      if (config_.has_flag ("use_optical_photons")) {
        _use_optical_photons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use optical photons set to: " << _use_optical_photons_);

      // Leptons:
      if (config_.has_flag ("use_muon_leptons")) {
        _use_muon_leptons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use muon leptons set to: " << _use_muon_leptons_);

      if (config_.has_flag ("use_tau_leptons")) {
        _use_tau_leptons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use tau leptons set to: " << _use_tau_leptons_);


      // Mesons:
      if (config_.has_flag ("use_light_mesons")) {
        _use_light_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use light mesons set to: " << _use_light_mesons_);

      if (config_.has_flag ("use_charm_mesons")) {
        _use_charm_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use charm mesons set to: " << _use_charm_mesons_);

      if (config_.has_flag ("use_bottom_mesons")) {
        _use_bottom_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use bottom mesons set to: " << _use_bottom_mesons_);

      // Baryons :
      if (config_.has_key ("use_nucleons")) {
        _use_nucleons_ = config_.fetch_boolean ("use_nucleons");
      }
      DT_LOG_DEBUG(_logprio(),"Use nucleons set to: " << _use_nucleons_);

      if (config_.has_flag ("use_strange_baryons")) {
        _use_strange_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use strange baryons set to: " << _use_strange_baryons_);

      if (config_.has_flag ("use_charm_baryons")) {
        _use_charm_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use charm baryons set to: " << _use_charm_baryons_);

      if (config_.has_flag ("use_bottom_baryons")) {
        _use_bottom_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use bottom baryons set to: " << _use_bottom_baryons_);

      // Nuclei:
      if (config_.has_key ("use_light_nuclei")) {
        _use_light_nuclei_ = config_.fetch_boolean ("use_light_nuclei");
      }
      DT_LOG_DEBUG(_logprio(),"Use light nuclei set to: " << _use_light_nuclei_);

      if (_use_light_nuclei_) {
        if (config_.has_flag ("he3_as_generic_ion")) {
          _he3_as_generic_ion_ = true;
        }
      }
      DT_LOG_DEBUG(_logprio(),"He3 is implemented as a generic ion: " << _he3_as_generic_ion_);

      if (config_.has_flag ("use_light_anti_nuclei")) {
        _use_light_anti_nuclei_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use light anti-nuclei set to: " << _use_light_anti_nuclei_);

      if (config_.has_key ("use_generic_ion")) {
        _use_generic_ion_ = config_.fetch_boolean ("use_generic_ion");
      }

      // End of parsing.

      if (_use_light_nuclei_ && _he3_as_generic_ion_) {
        _use_generic_ion_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use generic ion set to: " << _use_generic_ion_);

      _set_initialized(true);
      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void particles_physics_constructor::reset ()
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      _set_initialized(false);

      _set_defaults ();
      this->base_physics_constructor::_reset();

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void particles_physics_constructor::tree_dump (std::ostream & out_,
                                                   const std::string & title_,
                                                   const std::string & indent_,
                                                   bool inherit_) const
    {
      this->base_physics_constructor::tree_dump(out_, title_, indent_, true);
      std::string indent;
      if (! indent_.empty ()) indent = indent_;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use geantinos                : " << (_use_geantinos_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use optical photons          : " << (_use_optical_photons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use muon leptons             : " << (_use_muon_leptons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use tau leptons              : " << (_use_tau_leptons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use nucleons                 : " << (_use_nucleons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use strange baryons          : " << (_use_strange_baryons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use charm baryons            : " << (_use_charm_baryons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use bottom baryons           : " << (_use_bottom_baryons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use light mesons             : " << (_use_light_mesons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use charm mesons             : " << (_use_charm_mesons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use bottom mesons            : " << (_use_bottom_mesons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use light nuclei             : " << (_use_light_nuclei_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Use light anti-nuclei        : " << (_use_light_anti_nuclei_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Use generic ion              : " << (_use_generic_ion_ ? "Yes" : "No") << std::endl;

      return;
    }


    void particles_physics_constructor::ConstructParticle ()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      // In this method, static member functions should be called
      // for all particles which you want to use.
      // This ensures that objects of these particle types will be
      // created in the program.

      // bosons
      _ConstructBosons ();

      // leptons
      _ConstructLeptons ();

      // mesons
      _ConstructMesons ();

      // baryons
      _ConstructBaryons ();

      // ions
      _ConstructIons ();

      return;
    }

    // Extracted from:
    // G4IonConstructor ion_constructor;
    // ion_constructor.ConstructParticle();
    void particles_physics_constructor::_ConstructIons ()
    {
      if (_use_light_nuclei_) {
        DT_LOG_NOTICE(_logprio(), "Activating light nuclei...");
        G4Alpha::AlphaDefinition();
        G4Deuteron::DeuteronDefinition();
        G4Triton::TritonDefinition();
        if (! _he3_as_generic_ion_) {
          G4He3::He3Definition();
        }
      }
      if (_use_light_anti_nuclei_) {
        DT_LOG_NOTICE(_logprio(), "Activating light anti-nuclei...");
        G4AntiAlpha::AntiAlphaDefinition();
        G4AntiDeuteron::AntiDeuteronDefinition();
        G4AntiTriton::AntiTritonDefinition();
        if (! _he3_as_generic_ion_) {
          G4AntiHe3::AntiHe3Definition();
        }
      }
      if (_use_generic_ion_) {
        DT_LOG_NOTICE(_logprio(), "Activating generic ion...");
        G4GenericIon::GenericIonDefinition();
      }


      return;
    }


    void particles_physics_constructor::_ConstructBosons ()
    {
      // gammas
      DT_LOG_NOTICE(_logprio(), "Activating gamma...");
      G4Gamma::GammaDefinition ();

      if (_use_geantinos_) {
        DT_LOG_NOTICE(_logprio(), "Activating geantino...");
        G4Geantino::GeantinoDefinition ();
        G4ChargedGeantino::ChargedGeantinoDefinition ();
      }
      if (_use_optical_photons_) {
        DT_LOG_NOTICE(_logprio(), "Activating optical photons...");
        G4OpticalPhoton::OpticalPhotonDefinition ();
      }

      return;
    }

    void particles_physics_constructor::_ConstructLeptons ()
    {
      // Light leptons e+/e- :
      // From: G4LeptonConstructor::ConstructELeptons();
      DT_LOG_NOTICE(_logprio(), "Activating e+/e-...");
      G4Electron::ElectronDefinition();
      G4Positron::PositronDefinition();
      DT_LOG_NOTICE(_logprio(), "Activating (anti-)nu_e...");
      G4NeutrinoE::NeutrinoEDefinition();
      G4AntiNeutrinoE::AntiNeutrinoEDefinition();

      // Muon (anti-)leptons :
      if (_use_muon_leptons_) {
        DT_LOG_NOTICE(_logprio(), "Activating mu+/mu-...");
        // From: G4LeptonConstructor::ConstructMuLeptons();
        G4MuonPlus::MuonPlusDefinition();
        G4MuonMinus::MuonMinusDefinition();
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)nu_mu...");
        G4NeutrinoMu::NeutrinoMuDefinition();
        G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
      }

      // Tau (anti-)leptons :
      if (_use_tau_leptons_) {
        DT_LOG_NOTICE(_logprio(), "Activating tau+/tau-...");
        // From: G4LeptonConstructor::ConstructTauLeptons();
        G4TauMinus::TauMinusDefinition();
        G4TauPlus::TauPlusDefinition();
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)nu_tau...");
        G4NeutrinoTau::NeutrinoTauDefinition();
        G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
      }

      return;
    }


    void particles_physics_constructor::_ConstructMesons ()
    {
      if (_use_light_mesons_) {
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)light mesons...");
        // From: G4MesonConstructor::ConstructLightMesons();
        G4PionPlus::PionPlusDefinition();
        G4PionMinus::PionMinusDefinition();
        G4PionZero::PionZeroDefinition();
        G4Eta::EtaDefinition();
        G4EtaPrime::EtaPrimeDefinition();
        G4KaonPlus::KaonPlusDefinition();
        G4KaonMinus::KaonMinusDefinition();
        G4KaonZero::KaonZeroDefinition();
        G4AntiKaonZero::AntiKaonZeroDefinition();
        G4KaonZeroLong::KaonZeroLongDefinition();
        G4KaonZeroShort::KaonZeroShortDefinition();
      }
      if (_use_charm_mesons_) {
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)light charm mesons...");
        // From: G4MesonConstructor::ConstructCharmMesons();
        G4DMesonPlus::DMesonPlusDefinition();
        G4DMesonMinus::DMesonMinusDefinition();
        G4DMesonZero::DMesonZeroDefinition();
        G4AntiDMesonZero::AntiDMesonZeroDefinition();
        G4DsMesonPlus::DsMesonPlusDefinition();
        G4DsMesonMinus::DsMesonMinusDefinition();
#if G4VERSION_NUMBER == 960
        G4Etac::EtacDefinition();
#endif // G4VERSION_NUMBER == 960
        G4JPsi::JPsiDefinition();
      }
      if (_use_bottom_mesons_) {
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)light bottom mesons...");
        // From: G4MesonConstructor::ConstructBottomMesons();
        G4BMesonPlus::BMesonPlusDefinition();
        G4BMesonMinus::BMesonMinusDefinition();
        G4BMesonZero::BMesonZeroDefinition();
        G4AntiBMesonZero::AntiBMesonZeroDefinition();
        G4BsMesonZero::BsMesonZeroDefinition();
        G4AntiBsMesonZero::AntiBsMesonZeroDefinition();
#if G4VERSION_NUMBER == 960
        G4BcMesonPlus::BcMesonPlusDefinition();
        G4BcMesonMinus::BcMesonMinusDefinition();
        G4Upsiron::UpsironDefinition();
#endif // G4VERSION_NUMBER == 960

      }
      return;
    }


    void particles_physics_constructor::_ConstructBaryons ()
    {
      if (_use_nucleons_) {
        DT_LOG_NOTICE(_logprio(), "Activating (anti-)nucleons...");
        // From: G4BaryonConstructor::ConstructNucleons();
        G4Proton::ProtonDefinition();
        G4Neutron::NeutronDefinition();
        G4AntiProton::AntiProtonDefinition();
        G4AntiNeutron::AntiNeutronDefinition();
      }
      if (_use_strange_baryons_) {
        DT_LOG_NOTICE(_logprio(), "Activating strange (anti-)baryons...");
        // From: G4BaryonConstructor::ConstructStrangeBaryons();
        G4Lambda::LambdaDefinition();
        G4AntiLambda::AntiLambdaDefinition();
        G4SigmaZero::SigmaZeroDefinition();
        G4AntiSigmaZero::AntiSigmaZeroDefinition();
        G4SigmaPlus::SigmaPlusDefinition();
        G4AntiSigmaPlus::AntiSigmaPlusDefinition();
        G4SigmaMinus::SigmaMinusDefinition();
        G4AntiSigmaMinus::AntiSigmaMinusDefinition();
        G4XiZero::XiZeroDefinition();
        G4AntiXiZero::AntiXiZeroDefinition();
        G4XiMinus::XiMinusDefinition();
        G4AntiXiMinus::AntiXiMinusDefinition();
        G4OmegaMinus::OmegaMinusDefinition();
        G4AntiOmegaMinus::AntiOmegaMinusDefinition();
      }
      if (_use_charm_baryons_) {
        DT_LOG_NOTICE(_logprio(), "Activating charm (anti-)baryons...");
        // From: G4BaryonConstructor::ConstructCharmBaryons();
        G4LambdacPlus::LambdacPlusDefinition();
        G4SigmacPlusPlus::SigmacPlusPlusDefinition();
        G4SigmacPlus::SigmacPlusDefinition();
        G4SigmacZero::SigmacZeroDefinition();
        G4XicPlus::XicPlusDefinition();
        G4XicZero::XicZeroDefinition();
        G4OmegacZero::OmegacZeroDefinition();

        G4AntiLambdacPlus::AntiLambdacPlusDefinition();
        G4AntiSigmacPlusPlus::AntiSigmacPlusPlusDefinition();
        G4AntiSigmacPlus::AntiSigmacPlusDefinition();
        G4AntiSigmacZero::AntiSigmacZeroDefinition();
        G4AntiXicPlus::AntiXicPlusDefinition();
        G4AntiXicZero::AntiXicZeroDefinition();
        G4AntiOmegacZero::AntiOmegacZeroDefinition();
      }
      if (_use_bottom_baryons_) {
        DT_LOG_NOTICE(_logprio(), "Activating bottom baryons...");
        // From: G4BaryonConstructor::ConstructBottomBaryons();
#if G4VERSION_NUMBER == 960
        G4Lambdab::LambdabDefinition();
        G4SigmabPlus::SigmabPlusDefinition();
        G4SigmabZero::SigmabZeroDefinition();
        G4SigmabMinus::SigmabMinusDefinition();
        G4XibZero::XibZeroDefinition();
        G4XibMinus::XibMinusDefinition();
        G4OmegabMinus::OmegabMinusDefinition();
#else // G4VERSION_NUMBER == 960
        DT_LOG_WARNING(_logprio(), "Geant4 version " << G4VERSION_NUMBER << " does not support bottom baryons.");
#endif // G4VERSION_NUMBER == 960

#if G4VERSION_NUMBER == 960
        DT_LOG_NOTICE(_logprio(), "Activating bottom anti-baryons...");
        G4AntiLambdab::AntiLambdabDefinition();
        G4AntiSigmabPlus::AntiSigmabPlusDefinition();
        G4AntiSigmabZero::AntiSigmabZeroDefinition();
        G4AntiSigmabMinus::AntiSigmabMinusDefinition();
        G4AntiXibZero::AntiXibZeroDefinition();
        G4AntiXibMinus::AntiXibMinusDefinition();
        G4AntiOmegabMinus::AntiOmegabMinusDefinition();
#else // G4VERSION_NUMBER == 960
        DT_LOG_WARNING(_logprio(), "Geant4 version " << G4VERSION_NUMBER << " does not support bottom anti-baryons.");
#endif // G4VERSION_NUMBER == 960
      }
      return;
    }

    void particles_physics_constructor::_ConstructUserLimits ()
    {

      return;
    }

  } // end of namespace g4

} // end of namespace mctools


/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::particles_physics_constructor,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::particles_physics_constructor");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation manager particles definition physics constructor");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is a very simple physics constructor that       \n"
                                "initialize a list of Geant4 particles on user demand.\n"
                                );

  logger::declare_ocd_logging_configuration(ocd_, "warning");

  {
    // Description of the 'use_geantinos' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_geantinos")
      .set_terse_description("Flag to setup the Geantinos mock particles")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *geantinos* mock particles: :: \n"
                   "                                            \n"
                   " use_geantinos : boolean = 1                \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'use_optical_photons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_optical_photons")
      .set_terse_description("Flag to setup the optical photons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *optical* photons: :: \n"
                   "                                   \n"
                   " use_optical_photons : boolean = 1 \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'use_muon_leptons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_muon_leptons")
      .set_terse_description("Flag to setup the muon leptons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *muon* leptons: :: \n"
                   "                                \n"
                   " use_muon_leptons : boolean = 1 \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'use_tau_leptons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_tau_leptons")
      .set_terse_description("Flag to setup the tau leptons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *tau* leptons: :: \n"
                   "                               \n"
                   " use_tau_leptons : boolean = 1 \n"
                   "                               \n"
                   )
      ;
  }

  {
    // Description of the 'use_light_mesons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_light_mesons")
      .set_terse_description("Flag to setup the light mesons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *light* mesons: :: \n"
                   "                                \n"
                   " use_light_mesons : boolean = 1 \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'use_charm_mesons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_charm_mesons")
      .set_terse_description("Flag to setup the charm mesons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *charm* mesons: :: \n"
                   "                                \n"
                   " use_charm_mesons : boolean = 1 \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'use_bottom_mesons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_bottom_mesons")
      .set_terse_description("Flag to setup the bottom mesons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *bottom* mesons: :: \n"
                   "                                 \n"
                   " use_bottom_mesons : boolean = 1 \n"
                   "                                 \n"
                   )
      ;
  }

  {
    // Description of the 'use_nucleons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_nucleons")
      .set_terse_description("Flag to setup the nucleons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate the *nucleons*: :: \n"
                   "                            \n"
                   " use_nucleons : boolean = 1 \n"
                   "                            \n"
                   )
      ;
  }

  {
    // Description of the 'use_strange_baryons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_strange_baryons")
      .set_terse_description("Flag to setup strange baryons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *strange* baryons: :: \n"
                   "                                   \n"
                   " use_strange_baryons : boolean = 1 \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'use_charm_baryons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_charm_baryons")
      .set_terse_description("Flag to setup charm baryons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *charm* baryons: :: \n"
                   "                                 \n"
                   " use_charm_baryons : boolean = 1 \n"
                   "                                 \n"
                   )
      ;
  }

  {
    // Description of the 'use_bottom_baryons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_bottom_baryons")
      .set_terse_description("Flag to setup bottom baryons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *bottom* baryons: :: \n"
                   "                                  \n"
                   " use_bottom_baryons : boolean = 1 \n"
                   "                                  \n"
                   )
      ;
  }

  {
    // Description of the 'use_light_nuclei' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_light_nuclei")
      .set_terse_description("Flag to setup light nuclei")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate the *light* nuclei: :: \n"
                   "                                \n"
                   " use_light_nuclei : boolean = 1 \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'use_light_anti_nuclei' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_light_anti_nuclei")
      .set_terse_description("Flag to setup light anti-nuclei")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate the *light* anti nuclei: :: \n"
                   "                                     \n"
                   " use_light_anti_nuclei : boolean = 1 \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'use_generic_ion' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("use_generic_ion")
      .set_terse_description("Flag to setup generic ion")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate the *generic ion*: :: \n"
                   "                               \n"
                   " use_generic_ion : boolean = 1 \n"
                   "                               \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("The configuration of a ``mctools::g4::particles_physics_constructor``  \n"
                               "object can be provided through a ``datatools::properties`` object.     \n"
                               "                                                                       \n"
                               "A typical sample file is::                                             \n"
                               "                                                                       \n"
                               " logging.priority      : string = \"warning\"                          \n"
                               " use_geantinos         : boolean = 0                                   \n"
                               " use_optical_photons   : boolean = 0                                   \n"
                               " use_muon_leptons      : boolean = 0                                   \n"
                               " use_tau_leptons       : boolean = 0                                   \n"
                               " use_light_mesons      : boolean = 0                                   \n"
                               " use_charm_mesons      : boolean = 0                                   \n"
                               " use_bottom_mesons     : boolean = 0                                   \n"
                               " use_nucleons          : boolean = 1                                   \n"
                               " use_strange_baryons   : boolean = 0                                   \n"
                               " use_charm_baryons     : boolean = 0                                   \n"
                               " use_bottom_baryons    : boolean = 0                                   \n"
                               " use_light_nuclei      : boolean = 1                                   \n"
                               " use_light_anti_nuclei : boolean = 0                                   \n"
                               " use_generic_ion       : boolean = 1                                   \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(false);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::particles_physics_constructor' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::particles_physics_constructor,"mctools::g4::particles_physics_constructor")
