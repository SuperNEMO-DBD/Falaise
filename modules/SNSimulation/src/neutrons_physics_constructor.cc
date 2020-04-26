// neutrons_physics_constructor.cc

// Ourselves:
#include <snsim/neutrons_physics_constructor.h>

// Standard library:
#include <iostream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/units.h>
// - Geant4:
#include <G4UnitsTable.hh>
#include <G4Version.hh>
#include <globals.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4PreCompoundModel.hh>
#include <G4ProcessManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Particles:
#include <G4BaryonConstructor.hh>
#include <G4IonConstructor.hh>
#include <G4MesonConstructor.hh>
#include <G4ParticleTypes.hh>
#include <G4ShortLivedConstructor.hh>

#include <G4ExcitationHandler.hh>
#include <G4ExcitedStringDecay.hh>
#include <G4GeneratorPrecompoundInterface.hh>
#include <G4PreCompoundModel.hh>
#include <G4QGSMFragmentation.hh>
#include <G4QGSModel.hh>
#include <G4TheoFSGenerator.hh>

// Hadronic processes:
#include <G4CascadeInterface.hh>
#include <G4HadronElasticProcess.hh>

#if G4VERSION_NUMBER < 1000
// Not available anymore in Geant4 version 10.X
#include <G4HEProtonInelastic.hh>
#include <G4LCapture.hh>
#include <G4LEProtonInelastic.hh>
#include <G4LElastic.hh>
#include <G4NeutronHPorLElasticModel.hh>
//#include <G4LENeutronInelastic.hh>
#include <G4ElectroNuclearReaction.hh>
#include <G4GammaNuclearReaction.hh>
#include <G4LENeutronInelastic.hh>
#endif

// From 4.10.00 :
// - http://hypernews.slac.stanford.edu/HyperNews/geant4/get/phys-list/929.html
// - http://hypernews.slac.stanford.edu/HyperNews/geant4/get/phys-list/828/1.html
#include <G4BinaryCascade.hh>
#include <G4ElectronNuclearProcess.hh>
#include <G4ExcitedStringDecay.hh>
#include <G4HadronCaptureProcess.hh>
#include <G4HadronElastic.hh>
#include <G4HadronElasticProcess.hh>
#include <G4HadronFissionProcess.hh>
#include <G4LFission.hh>
#include <G4LundStringFragmentation.hh>
#include <G4NeutronHPCapture.hh>
#include <G4NeutronHPCaptureData.hh>
#include <G4NeutronHPElasticData.hh>
#include <G4NeutronHPFission.hh>
#include <G4NeutronHPFissionData.hh>
#include <G4NeutronHPInelastic.hh>
#include <G4NeutronHPInelasticData.hh>
#include <G4NeutronHPThermalScattering.hh>
#include <G4NeutronHPThermalScatteringData.hh>
#include <G4NeutronInelasticProcess.hh>
#include <G4NeutronRadCapture.hh>
#include <G4NucleonNuclearCrossSection.hh>
#include <G4PhotoNuclearProcess.hh>
#include <G4ProtonInelasticProcess.hh>

/*
  From Geant4 version 9.6:

  QGSP_BERT contents:

  protons:
  Bertini cascade (0 – 9.9 GeV), GHEISHA (9.5 – 25 GeV), QGSP (12 GeV – 100 TeV)
  HadronElastic (all energies)
  Standard EM package

  neutrons:
  Bertini cascade (0 – 9.9 GeV), GHEISHA (9.5 – 25 GeV), QGSP (12 GeV to 100 TeV)
  HadronElastic (all energies)
  GHEISHA capture (all energies)
  GHEISHA fission (all energies)

  Gammas
  CHIPS gamma-nuclear (0 – 3.5 GeV)
  QGSP (3 GeV – 100 TeV)
  Standard EM package

  Electrons, positrons
  CHIP electro-nuclear (0 – 30 TeV)
  Standard EM package

  QGSP_BERT_HP contents:

  Identical to QGSP_BERT, except high precision neutron (NeutronHP) models used for neutron elastic,
  inelastic, capture and fission below energies of 20 MeV
*/

namespace snsim {

DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_physics_constructor,
                                                          neutrons_physics_constructor,
                                                          "snsim::neutrons_physics_constructor")

// *** neutrons_physics_constructor *** //
neutrons_physics_constructor::neutrons_physics_constructor() {
  this->neutrons_physics_constructor::_set_defaults();
  SetPhysicsName("neutrons");
  SetPhysicsType(2);
}

neutrons_physics_constructor::~neutrons_physics_constructor() {
  if (is_initialized()) {
    this->neutrons_physics_constructor::reset();
  }
}

void neutrons_physics_constructor::initialize(const datatools::properties& config_,
                                              physics_constructor_dict_type& /*dict_*/) {
  DT_LOG_TRACE(_logprio(), "Entering...");
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  // parsing properties from 'config_' :
  loggable_support::_initialize_logging_support(config_);

  // Neutron processes :
  if (config_.has_key("neutrons.high_energy")) {
    _use_HE_ = config_.fetch_boolean("neutrons.high_energy");
  }
  DT_LOG_DEBUG(_logprio(), "Use high-energy process: " << _use_HE_);

  if (config_.has_key("neutrons.inelastic")) {
    _use_neutrons_inelastic_process_ = config_.fetch_boolean("neutrons.inelastic");
  }
  DT_LOG_DEBUG(_logprio(), "Use neutrons inelastic process: " << _use_neutrons_inelastic_process_);

  if (config_.has_key("neutrons.elastic")) {
    _use_neutrons_elastic_process_ = config_.fetch_boolean("neutrons.elastic");
  }
  DT_LOG_DEBUG(_logprio(), "Use neutrons elastic process: " << _use_neutrons_elastic_process_);

  if (config_.has_key("neutrons.capture")) {
    _use_neutrons_capture_ = config_.fetch_boolean("neutrons.capture");
  }
  DT_LOG_DEBUG(_logprio(), "Use neutrons capture: " << _use_neutrons_capture_);

  if (config_.has_key("neutrons.fission")) {
    _use_neutrons_fission_ = config_.fetch_boolean("neutrons.fission");
  }
  DT_LOG_DEBUG(_logprio(), "Use neutrons fission: " << _use_neutrons_fission_);

  // Proton processes :

  if (config_.has_key("protons.inelastic")) {
    _use_protons_inelastic_process_ = config_.fetch_boolean("protons.inelastic");
  }
  DT_LOG_DEBUG(_logprio(), "Use protons inelastic process: " << _use_protons_inelastic_process_);

  if (config_.has_key("protons.elastic")) {
    _use_protons_elastic_process_ = config_.fetch_boolean("protons.elastic");
  }
  DT_LOG_DEBUG(_logprio(), "Use protons elastic process: " << _use_protons_elastic_process_);

  // Electron processes :
  if (config_.has_key("electrons.hadronic")) {
    _use_electrons_hadronic_process_ = config_.fetch_boolean("electrons.hadronic");
  }
  DT_LOG_DEBUG(_logprio(), "Use electrons-hadrons process: " << _use_electrons_hadronic_process_);

  // Gamma processes :
  if (config_.has_key("gammas.hadronic")) {
    _use_gammas_hadronic_process_ = config_.fetch_boolean("gammas.hadronic");
  }
  DT_LOG_DEBUG(_logprio(), "Use gamma-hadrons process: " << _use_gammas_hadronic_process_);

  // End

  _set_initialized(true);

  if (_logprio() >= datatools::logger::PRIO_INFORMATION) {
    DT_LOG_INFORMATION(_logprio(), "Neutrons Physics Constructor: ");
    this->tree_dump(std::clog, "", "");
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void neutrons_physics_constructor::_set_defaults() {
  _use_HE_ = false;
  _use_neutrons_inelastic_process_ = true;
  _use_neutrons_elastic_process_ = true;
  _use_neutrons_capture_ = true;
  _use_neutrons_fission_ = true;
  _use_protons_inelastic_process_ = true;
  _use_protons_elastic_process_ = true;
  _use_electrons_hadronic_process_ = true;
  _use_gammas_hadronic_process_ = true;
}

void neutrons_physics_constructor::reset() {
  DT_LOG_TRACE(_logprio(), "Entering...");
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  _set_initialized(false);
  _set_defaults();

  this->base_physics_constructor::_reset();

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void neutrons_physics_constructor::tree_dump(std::ostream& out_, const std::string& title_,
                                             const std::string& indent_, bool /*inherit_*/) const {
  this->base_physics_constructor::tree_dump(out_, title_, indent_, true);
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use high-energy process        : " << (_use_HE_ ? "Yes" : "No") << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use neutrons inelastic process : " << (_use_neutrons_inelastic_process_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use neutrons elastic process   : " << (_use_neutrons_elastic_process_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use neutrons capture           : " << (_use_neutrons_capture_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use neutrons fission           : " << (_use_neutrons_fission_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use protons inelastic process  : " << (_use_protons_inelastic_process_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use protons elastic process    : " << (_use_protons_elastic_process_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Use electrons hadronic process : " << (_use_electrons_hadronic_process_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::last_tag
       << "Use gammas hadronic process    : " << (_use_gammas_hadronic_process_ ? "Yes" : "No")
       << std::endl;
}

void neutrons_physics_constructor::ConstructParticle() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
  G4Gamma::GammaDefinition();
  G4Electron::ElectronDefinition();
  G4Proton::ProtonDefinition();
  G4Neutron::NeutronDefinition();
}

void neutrons_physics_constructor::ConstructProcess() {
  DT_LOG_TRACE(_logprio(), "Entering...");
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  // standard part - elastic - all hadrons but neutrons
  auto* hadrons_elastic_model = new G4HadronElastic();
  auto* hadrons_elastic_process = new G4HadronElasticProcess();
  hadrons_elastic_process->RegisterMe(hadrons_elastic_model);

  // High-energy model for p, n
  auto* QGSP_model = new G4TheoFSGenerator("QGSP");
  auto* the_cascade = new G4GeneratorPrecompoundInterface();
  auto* the_handler = new G4ExcitationHandler();
  auto* the_pre_equilib = new G4PreCompoundModel(the_handler);
  the_cascade->SetDeExcitation(the_pre_equilib);
  QGSP_model->SetTransport(the_cascade);

  auto* QGSM_fragmentation = new G4QGSMFragmentation();
  auto* the_string_decay = new G4ExcitedStringDecay(QGSM_fragmentation);
  auto* the_string_model = new G4QGSModel<G4QGSParticipants>;
  the_string_model->SetFragmentationModel(the_string_decay);
  QGSP_model->SetHighEnergyGenerator(the_string_model);
  QGSP_model->SetMinEnergy(11.9 * CLHEP::GeV);
  QGSP_model->SetMaxEnergy(100. * CLHEP::TeV);
#if G4VERSION_NUMBER < 1000
#endif

  // High-energy model for photons
  auto* the_HE_model = new G4TheoFSGenerator();
  the_string_model = new G4QGSModel<G4QGSParticipants>;
  the_HE_model->SetTransport(the_cascade);
  the_HE_model->SetHighEnergyGenerator(the_string_model);
  the_HE_model->SetMinEnergy(3. * CLHEP::GeV);
  the_HE_model->SetMaxEnergy(100 * CLHEP::TeV);

  // particles iteration
#if G4VERSION_NUMBER >= 1030
  // http://hypernews.slac.stanford.edu/HyperNews/geant4/get/phys-list/838/2/1.html
  auto theParticleIterator = GetParticleIterator();
#endif
  theParticleIterator->reset();
  while ((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    const G4String& pname = particle->GetParticleName();

    // neutrons
    if (pname == "neutron") {
      // inelastic process
      if (_use_neutrons_inelastic_process_) {
        auto* the_neutron_inelastic_process = new G4NeutronInelasticProcess();

        G4CascadeInterface* the_bertini_model = nullptr;
        if (_use_HE_) {
          the_bertini_model = new G4CascadeInterface();
          the_bertini_model->SetMinEnergy(19.9 * CLHEP::MeV);
          the_bertini_model->SetMaxEnergy(9.9 * CLHEP::GeV);
        }
        auto* the_low_energy_neutron_inelastic_model = new G4NeutronHPInelastic();
        the_low_energy_neutron_inelastic_model->SetMaxEnergy(20. * CLHEP::MeV);

#if G4VERSION_NUMBER < 1000
        G4LENeutronInelastic* the_intermediate_energy_neutron_inelastic_model = nullptr;
        if (_use_HE_) {
          the_intermediate_energy_neutron_inelastic_model = new G4LENeutronInelastic();
          the_intermediate_energy_neutron_inelastic_model->SetMinEnergy(9.5 * CLHEP::GeV);
          the_intermediate_energy_neutron_inelastic_model->SetMaxEnergy(25. * CLHEP::GeV);
        }
#endif

        auto* the_HP_inelastic_data = new G4NeutronHPInelasticData();
        the_neutron_inelastic_process->AddDataSet(the_HP_inelastic_data);

        if (_use_HE_) {
          QGSP_model->SetMinEnergy(11.9 * CLHEP::GeV);
        }

        if (_use_HE_) {
          the_neutron_inelastic_process->RegisterMe(QGSP_model);
        }
        if (_use_HE_) {
          the_neutron_inelastic_process->RegisterMe(the_bertini_model);
        }
        the_neutron_inelastic_process->RegisterMe(the_low_energy_neutron_inelastic_model);
#if G4VERSION_NUMBER < 1000
        if (_use_HE_) {
          the_neutron_inelastic_process->RegisterMe(
              the_intermediate_energy_neutron_inelastic_model);
        }
#endif
        pmanager->AddDiscreteProcess(the_neutron_inelastic_process);
      }
      // elastic process
      if (_use_neutrons_elastic_process_) {
        auto* the_neutron_elastic_process = new G4HadronElasticProcess();
#if G4VERSION_NUMBER < 1000
        auto* the_neutron_elastic_model = new G4NeutronHPorLElasticModel();
        the_neutron_elastic_model->SetMinEnergy(4. * CLHEP::eV);
        the_neutron_elastic_model->SetMaxEnergy(20 * CLHEP::MeV);
#endif
        auto* the_neutron_thermal_elastic_model = new G4NeutronHPThermalScattering();
        the_neutron_thermal_elastic_model->SetMaxEnergy(4. * CLHEP::eV);

#if G4VERSION_NUMBER < 1000
        G4LElastic* the_high_energy_neutron_elastic_model = nullptr;
        if (_use_HE_) {
          the_high_energy_neutron_elastic_model = new G4LElastic();
          the_high_energy_neutron_elastic_model->SetMinEnergy(19.5 * CLHEP::MeV);
          the_high_energy_neutron_elastic_model->SetMaxEnergy(10. * CLHEP::TeV);
        }
#endif

        auto* the_HP_elastic_data = new G4NeutronHPElasticData();
        auto* the_HP_thermal_scattering_data = new G4NeutronHPThermalScatteringData();
#if G4VERSION_NUMBER < 1000
        the_neutron_elastic_process->AddDataSet(the_neutron_elastic_model->GiveHPXSectionDataSet());
#endif
        the_neutron_elastic_process->AddDataSet(the_HP_elastic_data);
        the_neutron_elastic_process->AddDataSet(the_HP_thermal_scattering_data);
        the_neutron_elastic_process->RegisterMe(the_neutron_thermal_elastic_model);
#if G4VERSION_NUMBER < 1000
        the_neutron_elastic_process->RegisterMe(the_neutron_elastic_model);
        if (_use_HE_) {
          the_neutron_elastic_process->RegisterMe(the_high_energy_neutron_elastic_model);
        }
#endif
        pmanager->AddDiscreteProcess(the_neutron_elastic_process);
      }
      // capture
      if (_use_neutrons_capture_) {
        auto* the_neutron_capture_process = new G4HadronCaptureProcess();
        auto* the_HP_capture_data = new G4NeutronHPCaptureData();
        auto* the_neutron_capture_model = new G4NeutronHPCapture();
        the_neutron_capture_process->AddDataSet(the_HP_capture_data);
        the_neutron_capture_process->RegisterMe(the_neutron_capture_model);
#if G4VERSION_NUMBER < 1000
        auto* the_capture_model = new G4LCapture();
        the_capture_model->SetMinEnergy(19.9 * CLHEP::MeV);
        the_capture_model->SetMaxEnergy(20. * CLHEP::TeV);
        the_neutron_capture_process->RegisterMe(the_capture_model);
#endif
        pmanager->AddDiscreteProcess(the_neutron_capture_process);
      }
      // fission
      if (_use_neutrons_fission_) {
        auto* the_neutron_fission_process = new G4HadronFissionProcess();
        auto* the_neutron_fission_model = new G4NeutronHPFission();
        auto* the_fission_data = new G4NeutronHPFissionData();
        auto* the_fission_model = new G4LFission();
        the_fission_model->SetMinEnergy(19.9 * CLHEP::MeV);
        the_fission_model->SetMaxEnergy(20. * CLHEP::TeV);
        the_neutron_fission_process->AddDataSet(the_fission_data);

        the_neutron_fission_process->RegisterMe(the_neutron_fission_model);
        the_neutron_fission_process->RegisterMe(the_fission_model);
        pmanager->AddDiscreteProcess(the_neutron_fission_process);
      }
    }

    // protons
    if (pname == "proton") {
      // inelastic process
      if (_use_protons_inelastic_process_) {
        auto* the_proton_inelastic_process = new G4ProtonInelasticProcess();
        auto* the_bertini_model = new G4CascadeInterface();
        the_bertini_model->SetMinEnergy(0. * CLHEP::MeV);
        the_bertini_model->SetMaxEnergy(10. * CLHEP::GeV);
#if G4VERSION_NUMBER < 1000
        G4LEProtonInelastic* the_high_energy_proton_inelastic_model = nullptr;
        if (_use_HE_) {
          the_high_energy_proton_inelastic_model = new G4LEProtonInelastic();
          the_high_energy_proton_inelastic_model->SetMinEnergy(9.5 * CLHEP::GeV);
          the_high_energy_proton_inelastic_model->SetMaxEnergy(25. * CLHEP::GeV);
          QGSP_model->SetMinEnergy(11.9 * CLHEP::GeV);
        }
#endif

        if (_use_HE_) {
          the_proton_inelastic_process->RegisterMe(QGSP_model);
        }
        the_proton_inelastic_process->RegisterMe(the_bertini_model);
#if G4VERSION_NUMBER < 1000
        if (_use_HE_) {
          the_proton_inelastic_process->RegisterMe(the_high_energy_proton_inelastic_model);
        }
#endif
        pmanager->AddDiscreteProcess(the_proton_inelastic_process);
      }
      // elastic process
      if (_use_protons_elastic_process_) {
        pmanager->AddDiscreteProcess(hadrons_elastic_process);
      }
    }

    // electrons
    if (pname == "e-") {
      if (_use_electrons_hadronic_process_) {
        auto* the_electron_inelastic_process = new G4ElectronNuclearProcess();
#if G4VERSION_NUMBER < 1000
        auto* the_electron_nuclear_reaction = new G4ElectroNuclearReaction();
        the_electron_inelastic_process->RegisterMe(the_electron_nuclear_reaction);
#endif
        pmanager->AddProcess(the_electron_inelastic_process, -1, -1, 5);
      }
    }

    // gamma
    if (pname == "gamma") {
      if (_use_gammas_hadronic_process_) {
        auto* the_gamma_hadron_process = new G4PhotoNuclearProcess();
#if G4VERSION_NUMBER < 1000
        auto* the_gamma_nuclear_reaction = new G4GammaNuclearReaction();
        the_gamma_nuclear_reaction->SetMaxEnergy(3.5 * CLHEP::GeV);
        the_gamma_hadron_process->RegisterMe(the_gamma_nuclear_reaction);
#endif
        the_gamma_hadron_process->RegisterMe(the_HE_model);
        pmanager->AddDiscreteProcess(the_gamma_hadron_process);
      }
    }
  }
}

// static
void neutrons_physics_constructor::init_ocd(datatools::object_configuration_description& ocd_) {
  // Inherits configuration properties from its base class:
  // ::snsim::base_physics_constructor::init_ocd(ocd_);

  {
    // Description of the 'neutrons.use_high_energy_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("neutrons.high_energy")
        .set_terse_description("Flag to activate high-energy process for neutrons and protons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate high-energy process for neutrons and protons: \n"
            "                                                       \n"
            " neutrons.high_energy : boolean = 1                    \n"
            "                                                       \n");
  }

  {
    // Description of the 'neutrons.use_neutrons_inelastic_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("neutrons.inelastic")
        .set_terse_description("Flag to activate inelastic process for neutrons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate inelastic process for neutrons: \n"
            "                                         \n"
            " neutrons.inelastic : boolean = 1        \n"
            "                                         \n");
  }

  {
    // Description of the 'neutrons.use_neutrons_elastic_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("neutrons.elastic")
        .set_terse_description("Flag to activate elastic process for neutrons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate elastic process for neutrons: \n"
            "                                       \n"
            " neutrons.elastic : boolean = 1        \n"
            "                                       \n");
  }

  {
    // Description of the 'neutrons.use_neutrons_capture' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("neutrons.capture")
        .set_terse_description("Flag to activate capture for neutrons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate capture for neutrons:   \n"
            "                                 \n"
            " neutrons.capture : boolean = 1  \n"
            "                                 \n");
  }

  {
    // Description of the 'neutrons.use_neutrons_fission' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("neutrons.fission")
        .set_terse_description("Flag to activate fission for neutrons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate fission for neutrons:    \n"
            "                                  \n"
            " neutrons.fission : boolean = 1   \n"
            "                                  \n");
  }

  {
    // Description of the 'neutrons.use_protons_inelastic_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("protons.inelastic")
        .set_terse_description("Flag to activate inelastic process for protons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate inelastic process for protons:  \n"
            "                                         \n"
            " protons.inelastic : boolean = 1         \n"
            "                                         \n");
  }

  {
    // Description of the 'neutrons.use_protons_elastic_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("protons.elastic")
        .set_terse_description("Flag to activate elastic process for protons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate elastic process for protons: \n"
            "                                      \n"
            " protons.elastic : boolean = 1        \n"
            "                                      \n");
  }

  {
    // Description of the 'neutrons.use_electrons_hadrons_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("electrons.hadronic")
        .set_terse_description("Flag to activate electrons-hadrons process")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate elastic process for protons:  \n"
            "                                       \n"
            " electrons.hadronic : boolean = 1      \n"
            "                                       \n");
  }

  {
    // Description of the 'neutrons.use_gamma_hadrons_process' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("gammas.hadronic")
        .set_terse_description("Flag to activate gamma-hadrons process")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate elastic process for protons:  \n"
            "                                       \n"
            " gammas.hadronic : boolean = 1         \n"
            "                                       \n");
  }
}

}  // namespace snsim

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::snsim::neutrons_physics_constructor, ocd_) {
  ocd_.set_class_name("snsim::neutrons_physics_constructor");
  ocd_.set_class_description(
      "The Geant4 simulation manager embedded nuclear neutrons physics list");
  ocd_.set_class_library("mctools_g4");
  ocd_.set_class_documentation(
      "The Geant4 simulation manager class embedded \n"
      "nuclear neutrons physics list.               \n");

  ::snsim::neutrons_physics_constructor::init_ocd(ocd_);

  ocd_.set_configuration_hints("Not available yet                    \n");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}

DOCD_CLASS_IMPLEMENT_LOAD_END()

// Registration macro for class 'snsim::neutrons_physics_constructor' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::neutrons_physics_constructor,
                               "snsim::neutrons_physics_constructor")
