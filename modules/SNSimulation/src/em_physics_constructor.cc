// em_physics_constructor.cc
/*
 * https://twiki.cern.ch/twiki/bin/view/Geant4/LoweMigratedLivermore
 * https://twiki.cern.ch/twiki/bin/view/Geant4/LoweAtomicDeexcitation
 */

// Ourselves:
#include <mctools/g4/em_physics_constructor.h>

// Standard library:
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/units.h>
// - Geant4:
#include <G4UnitsTable.hh>
#include <G4Version.hh>
#include <globals.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4ProcessManager.hh>
#include <G4eMultipleScattering.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Particles:
#include <G4Electron.hh>
#include <G4Gamma.hh>
#include <G4LossTableManager.hh>
#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4Positron.hh>
//#include <G4RegionStore.hh>
// EM processes :
#include <G4BetheBlochIonGasModel.hh>
#include <G4BetheBlochModel.hh>
#include <G4BraggIonGasModel.hh>
#include <G4BraggIonModel.hh>
#include <G4ComptonScattering.hh>
#include <G4EmProcessOptions.hh>
#include <G4GammaConversion.hh>
#include <G4GammaConversionToMuons.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuIonisation.hh>
#include <G4MuMultipleScattering.hh>
#include <G4MuPairProduction.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4RadioactiveDecay.hh>
#include <G4RayleighScattering.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eIonisation.hh>
#include <G4eplusAnnihilation.hh>
#include <G4hIonisation.hh>
#include <G4hMultipleScattering.hh>
#include <G4ionIonisation.hh>
// Livermore:
// Gammas :
#include <G4LivermoreComptonModel.hh>
#include <G4LivermoreGammaConversionModel.hh>
#include <G4LivermorePhotoElectricModel.hh>
#include <G4LivermoreRayleighModel.hh>
// Electrons :
#include <G4LivermoreBremsstrahlungModel.hh>
#include <G4LivermoreIonisationModel.hh>
// Penelope:
// Gammas :
#include <G4PenelopeComptonModel.hh>
#include <G4PenelopeGammaConversionModel.hh>
#include <G4PenelopePhotoElectricModel.hh>
#include <G4PenelopeRayleighModel.hh>
// Electrons/positrons :
#include <G4PenelopeBremsstrahlungModel.hh>
#include <G4PenelopeIonisationModel.hh>
// Positrons :
#include <G4EmConfigurator.hh>
#include <G4PenelopeAnnihilationModel.hh>
#include <G4RegionStore.hh>
#include <G4StepLimiter.hh>
#include <G4UserSpecialCuts.hh>
#include "G4UAtomicDeexcitation.hh"

// This project:
#include <mctools/g4/processes/em_extra_models.h>
#include <mctools/g4/processes/em_model_factory.h>

namespace snsim {

DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_physics_constructor,
                                                          em_physics_constructor,
                                                          "snsim::em_physics_constructor")

// PIMPL-ized working data:
struct em_physics_constructor::_work_type_ {
  std::map<std::string, ::snsim::processes::em_extra_model> em_extra_models;
  G4EmConfigurator em_configurator;
  int em_configurator_verbosity;
  ::snsim::processes::em_model_factory em_model_fact;
};

// *** em_physics_constructor::region_deexcitation_type *** //

em_physics_constructor::region_deexcitation_type::region_deexcitation_type() {
  fluorescence = false;
  auger = false;
  pixe = false;
}

em_physics_constructor::region_deexcitation_type::region_deexcitation_type(bool fluo_, bool auger_,
                                                                           bool pixe_) {
  fluorescence = fluo_;
  if (auger_) {
    fluorescence = true;
  }
  auger = auger_;
  pixe = pixe_;
}

auto em_physics_constructor::region_deexcitation_type::is_activated() const -> bool {
  return fluorescence || auger || pixe;
}

auto em_physics_constructor::region_deexcitation_type::is_fluorescence() const -> bool {
  return fluorescence;
}

auto em_physics_constructor::region_deexcitation_type::is_auger() const -> bool { return auger; }

auto em_physics_constructor::region_deexcitation_type::is_pixe() const -> bool { return pixe; }

// *** em_physics_constructor *** //

const std::string em_physics_constructor::EM_MODEL_STANDARD = "standard";
const std::string em_physics_constructor::EM_MODEL_LOW_ENERGY_LIVERMORE = "livermore";
const std::string em_physics_constructor::EM_MODEL_LOW_ENERGY_PENELOPE = "penelope";

const std::string em_physics_constructor::EM_PIXE_MODEL_EMPIRICAL = "Empirical";
const std::string em_physics_constructor::EM_PIXE_MODEL_ECPSSR_FORMFACTOR = "ECPSSR_FormFactor";
const std::string em_physics_constructor::EM_PIXE_MODEL_ECPSSR_ANALYTICAL = "ECPSSR_Analytical";

const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_LIVERMORE = "Livermore";
const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PENELOPE = "Penelope";
const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL = "Analytical";
const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL = "Empirical";

auto em_physics_constructor::get_em_model() const -> const std::string& { return _em_model_; }

auto em_physics_constructor::is_em_standard() const -> bool {
  return _em_model_ == EM_MODEL_STANDARD;
}

auto em_physics_constructor::is_em_low_energy_livermore() const -> bool {
  return _em_model_ == EM_MODEL_LOW_ENERGY_LIVERMORE;
}

auto em_physics_constructor::is_em_low_energy_penelope() const -> bool {
  return _em_model_ == EM_MODEL_LOW_ENERGY_PENELOPE;
}

em_physics_constructor::em_physics_constructor() {
  this->em_physics_constructor::_set_defaults();
  SetPhysicsName("electromagnetic");
  SetPhysicsType(1);
}

em_physics_constructor::~em_physics_constructor() {
  if (is_initialized()) {
    this->em_physics_constructor::reset();
  }
}

auto em_physics_constructor::_grab_work_() -> em_physics_constructor::_work_type_& {
  if (_work_.get() == nullptr) {
    _work_.reset(new _work_type_);
  }
  return *_work_;
}

void em_physics_constructor::initialize(const datatools::properties& config_,
                                        physics_constructor_dict_type& /*dict_*/) {
  DT_LOG_TRACE_ENTERING(_logprio());
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  // Parsing logging properties:
  loggable_support::_initialize_logging_support(config_);

  // Electro-magnetic processes:

  if (config_.has_key("em.model")) {
    _em_model_ = config_.fetch_string("em.model");
  }
  DT_THROW_IF(_em_model_ != EM_MODEL_STANDARD && _em_model_ != EM_MODEL_LOW_ENERGY_LIVERMORE &&
                  _em_model_ != EM_MODEL_LOW_ENERGY_PENELOPE,
              std::logic_error,
              "Invalid electromagnetic interaction model '" << _em_model_ << "' !");
  DT_LOG_NOTICE(_logprio(), "Default electromagnetic model set to: '" << _em_model_ << "'");

  // Gammas :
  if (config_.has_key("em.gamma.rayleigh_scattering")) {
    _em_gamma_rayleigh_scattering_ = config_.fetch_boolean("em.gamma.rayleigh_scattering");
  }
  DT_LOG_DEBUG(_logprio(),
               "Gamma Rayleigh scattering process set to: " << _em_gamma_rayleigh_scattering_);

  if (config_.has_key("em.gamma.photo_electric")) {
    _em_gamma_photo_electric_ = config_.fetch_boolean("em.gamma.photo_electric");
  }
  DT_LOG_DEBUG(_logprio(), "Gamma photoelectric process set to: " << _em_gamma_photo_electric_);

  if (config_.has_key("em.gamma.compton_scattering")) {
    _em_gamma_compton_scattering_ = config_.fetch_boolean("em.gamma.compton_scattering");
  }
  DT_LOG_DEBUG(_logprio(),
               "Gamma Compton scattering process set to: " << _em_gamma_compton_scattering_);

  if (config_.has_key("em.gamma.conversion")) {
    _em_gamma_conversion_ = config_.fetch_boolean("em.gamma.conversion");
  }
  DT_LOG_DEBUG(_logprio(), "Gamma conversion process set to: " << _em_gamma_conversion_);

  if (config_.has_key("em.gamma.conversion_to_muons")) {
    _em_gamma_conversion_to_muons_ = config_.fetch_boolean("em.gamma.conversion_to_muons");
  }
  DT_LOG_DEBUG(_logprio(),
               "Gamma conversion to muons process set to: " << _em_gamma_conversion_to_muons_);

  if (config_.has_key("em.gamma.step_limiter")) {
    _em_gamma_step_limiter_ = config_.fetch_boolean("em.gamma.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Gamma step limiter set to: " << _em_gamma_step_limiter_);

  if (config_.has_key("em.gamma.step_limiter")) {
    _em_gamma_step_limiter_ = config_.fetch_boolean("em.gamma.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Gamma step limiter set to: " << _em_gamma_step_limiter_);

  // Electrons/positrons :
  if (config_.has_key("em.electron.ionization")) {
    _em_electron_ionization_ = config_.fetch_boolean("em.electron.ionization");
  } else if (config_.has_key("em.electron.ionisation")) {
    _em_electron_ionization_ = config_.fetch_boolean("em.electron.ionisation");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron energy loss set to: " << _em_electron_ionization_);

  if (config_.has_key("em.electron.multiple_scattering")) {
    _em_electron_multiple_scattering_ = config_.fetch_boolean("em.electron.multiple_scattering");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron multiple scattering set to: "
                               << _em_electron_multiple_scattering_);

  if (config_.has_key("em.electron.multiple_scattering.use_distance_to_boundary")) {
    _em_electron_ms_use_distance_to_boundary_ =
        config_.fetch_boolean("em.electron.multiple_scattering.use_distance_to_boundary");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron multiple scattering use distance to boundary: "
                               << _em_electron_ms_use_distance_to_boundary_);

  if (config_.has_key("em.electron.multiple_scattering.range_factor")) {
    _em_electron_ms_range_factor_ =
        config_.fetch_real("em.electron.multiple_scattering.range_factor");
    DT_THROW_IF(_em_electron_ms_range_factor_ <= 0.0, std::domain_error,
                "Invalid multiple scattering range factor for electron/positron ("
                    << _em_electron_ms_range_factor_ << ") !");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron multiple scattering range factor: "
                               << _em_electron_ms_range_factor_);

  if (config_.has_key("em.electron.bremsstrahlung")) {
    _em_electron_bremsstrahlung_ = config_.fetch_boolean("em.electron.bremsstrahlung");
  }
  DT_LOG_DEBUG(_logprio(),
               "Electron/positron bremsstrahlung set to: " << _em_electron_bremsstrahlung_);

  // Positrons :
  if (config_.has_key("em.positron.annihilation")) {
    _em_positron_annihilation_ = config_.fetch_boolean("em.positron.annihilation");
  }
  DT_LOG_DEBUG(_logprio(), "Positron annihilation set to: " << _em_positron_annihilation_);

  if (config_.has_key("em.electron.step_limiter")) {
    _em_electron_step_limiter_ = config_.fetch_boolean("em.electron.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron step limiter set to: " << _em_electron_step_limiter_);

  if (config_.has_key("em.electron.step_limiter")) {
    _em_electron_step_limiter_ = config_.fetch_boolean("em.electron.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Electron/positron step limiter set to: " << _em_electron_step_limiter_);

  /* **** Atomic deexcitation **** */

  if (config_.has_key("em.deexcitation.fluorescence")) {
    _em_fluorescence_ = config_.fetch_boolean("em.deexcitation.fluorescence");
  }
  DT_LOG_DEBUG(_logprio(), "EM fluorescence set to : " << _em_fluorescence_);

  if (_em_fluorescence_) {
    if (config_.has_key("em.deexcitation.auger")) {
      _em_auger_ = config_.fetch_boolean("em.deexcitation.auger");
    }
    DT_LOG_DEBUG(_logprio(), "EM Auger set to : " << _em_auger_);
  }

  if (config_.has_key("em.deexcitation.pixe")) {
    _em_pixe_ = config_.fetch_boolean("em.deexcitation.pixe");
  }
  DT_LOG_DEBUG(_logprio(), "EM PIXE set to : " << _em_pixe_);

  if (config_.has_key("em.deexcitation.pixe.cross_section_model")) {
    _em_pixe_cross_section_model_ =
        config_.fetch_string("em.deexcitation.pixe.cross_section_model");
    DT_THROW_IF(_em_pixe_cross_section_model_ != EM_PIXE_MODEL_EMPIRICAL &&
                    _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_FORMFACTOR &&
                    _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_ANALYTICAL,
                std::logic_error,
                "Invalid EM PIXE cross-section model '" << _em_pixe_cross_section_model_ << "' !");
    DT_LOG_DEBUG(_logprio(),
                 "EM PIXE cross-section model set to : " << _em_pixe_cross_section_model_);
  }

  if (config_.has_key("em.deexcitation.pixe.electron_cross_section_model")) {
    _em_pixe_electron_cross_section_model_ =
        config_.fetch_string("em.deexcitation.pixe.electron_cross_section_model");
    DT_THROW_IF(
        _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_LIVERMORE &&
            _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL &&
            _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL &&
            _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PENELOPE,
        std::logic_error,
        "Invalid EM PIXE e+/e- cross-section model '" << _em_pixe_electron_cross_section_model_
                                                      << "' !");
    DT_LOG_DEBUG(_logprio(), "EM PIXE e+/e- cross-section model set to : "
                                 << _em_pixe_electron_cross_section_model_);
  }

  /* **** Atomic deexcitation per region **** */
  std::vector<std::string> deexcitation_regions;
  if (config_.has_key("em.deexcitation.regions")) {
    config_.fetch("em.deexcitation.regions", deexcitation_regions);
  }

  for (const auto& region_name : deexcitation_regions) {
    region_deexcitation_type rd(false, false, false);
    std::ostringstream region_fluo_key;
    region_fluo_key << "em.deexcitation.regions." << region_name << ".fluorescence";
    if (config_.has_key(region_fluo_key.str())) {
      rd.fluorescence = config_.fetch_boolean(region_fluo_key.str());
      if (rd.fluorescence) {
        _em_fluorescence_ = true;
      }
    }
    if (rd.fluorescence) {
      std::ostringstream region_auger_key;
      region_auger_key << "em.deexcitation.regions." << region_name << ".auger";
      if (config_.has_key(region_auger_key.str())) {
        rd.auger = config_.fetch_boolean(region_auger_key.str());
        if (rd.auger) {
          _em_auger_ = true;
        }
      }
    }
    std::ostringstream region_pixe_key;
    region_pixe_key << "em.deexcitation.regions." << region_name << ".pixe";
    if (config_.has_key(region_pixe_key.str())) {
      rd.pixe = config_.fetch_boolean(region_pixe_key.str());
      if (rd.pixe) {
        _em_pixe_ = true;
      }
    }
    _em_regions_deexcitation_[region_name] = rd;
  }

  // Ion :
  if (config_.has_key("em.ion.ionization")) {
    _em_ion_ionization_ = config_.fetch_boolean("em.ion.ionization");
  } else if (config_.has_key("em.ion.ionisation")) {
    _em_ion_ionization_ = config_.fetch_boolean("em.ion.ionisation");
  }
  DT_LOG_DEBUG(_logprio(), "Ion ionization set to: " << _em_ion_ionization_);

  if (config_.has_key("em.ion.multiple_scattering")) {
    _em_ion_multiple_scattering_ = config_.fetch_boolean("em.ion.multiple_scattering");
  }
  DT_LOG_DEBUG(_logprio(), "Ion multiple scattering set to: " << _em_ion_multiple_scattering_);

  if (config_.has_key("em.ion.step_limiter")) {
    _em_ion_step_limiter_ = config_.fetch_boolean("em.ion.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Ion step limiter set to: " << _em_ion_step_limiter_);

  if (config_.has_key("em.ion.user_special_cuts")) {
    _em_ion_user_special_cuts_ = config_.fetch_boolean("em.ion.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Ion user special cuts set to: " << _em_ion_user_special_cuts_);

  // Muons :
  if (config_.has_key("em.muon.ionization")) {
    _em_muon_ionization_ = config_.fetch_boolean("em.muon.ionization");
  } else if (config_.has_key("em.muon.ionisation")) {
    _em_muon_ionization_ = config_.fetch_boolean("em.muon.ionisation");
  }
  DT_LOG_DEBUG(_logprio(), "Muon ionization set to: " << _em_muon_ionization_);

  if (config_.has_key("em.muon.bremsstrahlung")) {
    _em_muon_bremsstrahlung_ = config_.fetch_boolean("em.muon.bremsstrahlung");
  }
  DT_LOG_DEBUG(_logprio(), "Muon bremsstrahlung set to: " << _em_muon_bremsstrahlung_);

  if (config_.has_key("em.muon.multiple_scattering")) {
    _em_muon_multiple_scattering_ = config_.fetch_boolean("em.muon.multiple_scattering");
  }
  DT_LOG_DEBUG(_logprio(), "Muon multiple scattering set to: " << _em_muon_multiple_scattering_);

  if (config_.has_key("em.muon.pair_production")) {
    _em_muon_pair_production_ = config_.fetch_boolean("em.muon.pair_production");
  }
  DT_LOG_DEBUG(_logprio(), "Muon pair production set to: " << _em_muon_pair_production_);

  if (config_.has_key("em.muon.step_limiter")) {
    _em_muon_step_limiter_ = config_.fetch_boolean("em.muon.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Muon step limiter set to: " << _em_muon_step_limiter_);

  if (config_.has_key("em.muon.user_special_cuts")) {
    _em_muon_user_special_cuts_ = config_.fetch_boolean("em.muon.user_special_cuts");
  }
  DT_LOG_DEBUG(_logprio(), "Muon user special cuts set to: " << _em_muon_user_special_cuts_);

  // Others:
  if (config_.has_key("em.other.ionization")) {
    _em_other_ionization_ = config_.fetch_boolean("em.other.ionization");
  } else if (config_.has_key("em.other.ionisation")) {
    _em_other_ionization_ = config_.fetch_boolean("em.other.ionisation");
  }
  DT_LOG_DEBUG(_logprio(), "Other ionization set to: " << _em_other_ionization_);

  if (config_.has_key("em.other.multiple_scattering")) {
    _em_other_multiple_scattering_ = config_.fetch_boolean("em.other.multiple_scattering");
  }
  DT_LOG_DEBUG(_logprio(), "Other multiple scattering set to: " << _em_other_multiple_scattering_);

  if (config_.has_key("em.other.step_limiter")) {
    _em_other_step_limiter_ = config_.fetch_boolean("em.other.step_limiter");
  }
  DT_LOG_DEBUG(_logprio(), "Other step limiter set to: " << _em_other_step_limiter_);

  if (config_.has_key("em.other.user_special_cuts")) {
    _em_other_user_special_cuts_ = config_.fetch_boolean("em.other.user_special_cuts");
  }
  DT_LOG_DEBUG(_logprio(), "Other user special cuts set to: " << _em_other_user_special_cuts_);

  // EM extra models support:
  if (config_.has_key("em.using_extra_models")) {
    _em_using_extra_models_ = config_.fetch_boolean("em.using_extra_models");
  }
  DT_LOG_DEBUG(_logprio(), "Using extra models set to: " << _em_using_extra_models_);
  if (_em_using_extra_models_) {
    DT_LOG_TRACE(_logprio(), "Invoke specific EM models configuration.");
    _setup_em_extra_models_configurator(config_);
  }

  // End.

  this->G4VPhysicsConstructor::SetPhysicsName(get_name());
  this->G4VPhysicsConstructor::SetPhysicsType(bElectromagnetic);

  _set_initialized(true);

  if (_logprio() >= datatools::logger::PRIO_INFORMATION) {
    DT_LOG_INFORMATION(_logprio(), "EM Physics Constructor: ");
    this->tree_dump(std::clog, "", "");
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::_setup_em_extra_models_configurator(
    const datatools::properties& config_) {
  DT_LOG_TRACE_ENTERING(_logprio());

  _grab_work_().em_configurator_verbosity = 1;
  if (config_.has_key("em.configurator.verbosity")) {
    std::string verbosity_label = config_.fetch_string("em.configurator.verbosity");
    if (verbosity_label == "mute") {
      _grab_work_().em_configurator_verbosity = 0;
    } else if (verbosity_label == "normal") {
      _grab_work_().em_configurator_verbosity = 1;
    } else if (verbosity_label == "verbose") {
      _grab_work_().em_configurator_verbosity = 2;
    } else {
      DT_THROW(std::logic_error, "Invalid EM configurator verbosity '" << verbosity_label << "'!");
    }
  }

  DT_LOG_NOTICE(_logprio(), "Setup the EM extra models...");
  datatools::properties extra_models_config;
  config_.export_and_rename_starting_with(extra_models_config, "em.extra_models.", "");

  if (extra_models_config.has_key("labels")) {
    std::vector<std::string> em_xmodel_labels;
    extra_models_config.fetch("labels", em_xmodel_labels);
    for (const auto& em_xmodel_label : em_xmodel_labels) {
      std::ostringstream em_xmodel_prefix;
      em_xmodel_prefix << em_xmodel_label << '.';
      datatools::properties extra_model_config;
      extra_models_config.export_and_rename_starting_with(extra_model_config,
                                                          em_xmodel_prefix.str(), "");
      {
        snsim::processes::em_extra_model dummy;
        _grab_work_().em_extra_models[em_xmodel_label] = dummy;
      }
      snsim::processes::em_extra_model& em_xmodel =
          _grab_work_().em_extra_models.find(em_xmodel_label)->second;
      em_xmodel.set_name(em_xmodel_label);
      DT_LOG_NOTICE(_logprio(),
                    "Fetch parameters for EM extra model '" << em_xmodel_label << "'...");
      em_xmodel.initialize(extra_model_config);
      DT_LOG_DEBUG(_logprio(), "EM extra model '" << em_xmodel_label << "' parameters are : ");
      if (_logprio() >= datatools::logger::PRIO_DEBUG) {
        em_xmodel.tree_dump(std::clog, "", "[debug]: ");
      }
    }
  }

  /*
  DT_LOG_NOTICE(_logprio(), "Setup the EM configurator...");
  _grab_work_().em_configurator.SetVerbose(em_configurator_verbosity);
  DT_LOG_DEBUG(_logprio(), "Initialize the EM model factory...");
  _grab_work_().em_model_fact.initialize();

  DT_LOG_DEBUG(_logprio(), "Apply EM extra models to the Geant4 EM configurator...");
  for (std::map<std::string, ::snsim::processes::em_extra_model>::const_iterator
         i = _grab_work_().em_extra_models.begin();
       i != _grab_work_().em_extra_models.end();
       i++){
    const std::string & em_xmodel_label = i->first;
    const ::snsim::processes::em_extra_model & em_xmodel = i->second;
    // Declare extra models to the embedded Geant4 EM configurator:
    DT_LOG_NOTICE(_logprio(), "Declare extra model '" << em_xmodel_label << "' in the Geant4 EM
  configurator..."); em_xmodel.apply_to_g4(_grab_work_().em_configurator,
                          _grab_work_().em_model_fact);
  }
  DT_LOG_DEBUG(_logprio(), "EM extra models have been applied to the Geant4 EM configurator.");
  */

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::_set_defaults() {
  // Process:
  _em_model_ = EM_MODEL_STANDARD;

  // gamma:
  _em_gamma_rayleigh_scattering_ = false;
  _em_gamma_photo_electric_ = true;
  _em_gamma_compton_scattering_ = true;
  _em_gamma_conversion_ = true;
  _em_gamma_conversion_to_muons_ = false;
  _em_gamma_step_limiter_ = false;
  _em_gamma_user_special_cuts_ = false;

  // electron/positron:
  _em_electron_ionization_ = true;
  _em_electron_bremsstrahlung_ = true;
  _em_electron_multiple_scattering_ = true;
  _em_electron_ms_use_distance_to_boundary_ = true;
  _em_electron_ms_range_factor_ = 0.005;
  _em_positron_annihilation_ = true;
  _em_electron_step_limiter_ = true;
  _em_electron_user_special_cuts_ = false;

  // atomic deexcitation
  _em_fluorescence_ = false;
  _em_auger_ = false;
  _em_pixe_ = false;
  _em_pixe_cross_section_model_ = EM_PIXE_MODEL_EMPIRICAL;                    // clear();
  _em_pixe_electron_cross_section_model_ = EM_PIXE_ELECTRON_MODEL_LIVERMORE;  // clear();
  _em_regions_deexcitation_.clear();

  // ion:
  _em_ion_multiple_scattering_ = true;
  _em_ion_ionization_ = true;
  _em_ion_step_limiter_ = false;
  _em_ion_user_special_cuts_ = false;

  // muon:
  _em_muon_multiple_scattering_ = true;
  _em_muon_ionization_ = true;
  _em_muon_bremsstrahlung_ = true;
  _em_muon_pair_production_ = true;
  _em_muon_step_limiter_ = false;
  _em_muon_user_special_cuts_ = false;

  // others:
  _em_other_multiple_scattering_ = true;
  _em_other_ionization_ = true;
  _em_other_step_limiter_ = false;
  _em_other_user_special_cuts_ = false;

  // EM extra models:
  _em_using_extra_models_ = false;
}

void em_physics_constructor::reset() {
  DT_LOG_TRACE(_logprio(), "Entering...");

  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  _set_initialized(false);

  _work_.reset();
  _em_regions_deexcitation_.clear();
  _set_defaults();

  this->base_physics_constructor::_reset();

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void em_physics_constructor::tree_dump(std::ostream& out_, const std::string& title_,
                                       const std::string& indent_, bool inherit_) const {
  this->base_physics_constructor::tree_dump(out_, title_, indent_, true);
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }

  // EM model:
  out_ << indent << datatools::i_tree_dumpable::tag << "Electro-magnetic model       : '"
       << _em_model_ << "'" << std::endl;

  // Gamma:
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma Rayleigh scattering    : " << (_em_gamma_rayleigh_scattering_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma photo-electric         : " << (_em_gamma_photo_electric_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma Compton scattering     : " << (_em_gamma_compton_scattering_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma conversion             : " << (_em_gamma_conversion_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma conversion to muons    : " << (_em_gamma_conversion_to_muons_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma step limiter : " << (_em_gamma_step_limiter_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Gamma user special cuts : " << (_em_gamma_user_special_cuts_ ? "Yes" : "No")
       << std::endl;

  // Electron/positron:
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Electron/positron ionization : " << (_em_electron_ionization_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Electron/positron bremsstrahlung : " << (_em_electron_bremsstrahlung_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag << "Electron/positron multiple scattering : "
       << (_em_electron_multiple_scattering_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Electron/positron multiple scattering uses distance to boundary : "
       << (_em_electron_ms_use_distance_to_boundary_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Electron/positron multiple scattering range factor : " << (_em_electron_ms_range_factor_)
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Positron annihilation : " << (_em_positron_annihilation_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Electron/positron step limiter : " << (_em_electron_step_limiter_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag << "Electron/positron user special cuts : "
       << (_em_electron_user_special_cuts_ ? "Yes" : "No") << std::endl;

  // Atomic deexcitation:
  out_ << indent << datatools::i_tree_dumpable::tag << "Atomic deexcitation : " << std::endl;
  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "Fluorescence : " << (_em_fluorescence_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "Auger : " << (_em_auger_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "PIXE : " << (_em_pixe_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "PIXE cross section model : "
       << "'" << _em_pixe_cross_section_model_ << "'" << std::endl;

  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "PIXE e+/e- cross section model : "
       << "'" << _em_pixe_electron_cross_section_model_ << "'" << std::endl;

  // Atomic deexcitation per region :
  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag
       << "Atomic deexcitation per region : " << std::endl;

  for (auto i = _em_regions_deexcitation_.begin(); i != _em_regions_deexcitation_.end(); i++) {
    out_ << indent << datatools::i_tree_dumpable::skip_tag
         << datatools::i_tree_dumpable::last_skip_tag;
    auto j = i;
    j++;
    if (j++ == _em_regions_deexcitation_.end()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Region '" << i->first << "' : "
         << "  fluorescence=" << (i->second.fluorescence ? "Yes" : "No")
         << "  Auger=" << (i->second.auger ? "Yes" : "No")
         << "  PIXE=" << (i->second.pixe ? "Yes" : "No") << std::endl;
  }

  // Ion:
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Ion ionization : " << (_em_ion_ionization_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Ion multiple scattering : " << (_em_ion_multiple_scattering_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Ion step limiter : " << (_em_ion_step_limiter_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Ion user special cuts : " << (_em_ion_user_special_cuts_ ? "Yes" : "No") << std::endl;

  // Muon:
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon ionization : " << (_em_muon_ionization_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon bremsstrahlung : " << (_em_muon_bremsstrahlung_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon pair production : " << (_em_muon_pair_production_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon multiple scattering : " << (_em_muon_multiple_scattering_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon step limiter : " << (_em_muon_step_limiter_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Muon user special cuts : " << (_em_muon_user_special_cuts_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Other ionization : " << (_em_other_ionization_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Other multiple scattering : " << (_em_other_multiple_scattering_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Other step limiter : " << (_em_other_step_limiter_ ? "Yes" : "No") << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Other user special cuts : " << (_em_other_user_special_cuts_ ? "Yes" : "No")
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Using EM extra models : " << (_em_using_extra_models_ ? "Yes" : "No") << std::endl;
}

void em_physics_constructor::ConstructParticle() {
  DT_LOG_TRACE_ENTERING(_logprio());
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  DT_LOG_DEBUG(_logprio(), "Defining gamma...");
  G4Gamma::GammaDefinition();

  DT_LOG_DEBUG(_logprio(), "Defining electron...");
  G4Electron::ElectronDefinition();

  DT_LOG_DEBUG(_logprio(), "Defining positron...");
  G4Positron::PositronDefinition();

  if (_em_gamma_conversion_to_muons_) {
    DT_LOG_DEBUG(_logprio(), "Defining muon plus...");
    G4MuonPlus::MuonPlusDefinition();
    DT_LOG_DEBUG(_logprio(), "Defining muon minus...");
    G4MuonMinus::MuonMinusDefinition();
  }

  /*
  G4Proton::Proton();
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
  */

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::ConstructProcess() {
  DT_LOG_TRACE_ENTERING(_logprio());

  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

  this->em_physics_constructor::_ConstructEMProcess();

  if (_em_fluorescence_ || _em_auger_ || _em_pixe_ || !_em_regions_deexcitation_.empty()) {
    DT_LOG_NOTICE(_logprio(), "Activating EM deexcitation...");
    this->em_physics_constructor::_ConstructEMDeexcitation();
  }

  if (_em_using_extra_models_) {
    this->em_physics_constructor::_ConstructExtraModels();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::_ConstructExtraModels() {
  DT_LOG_TRACE_ENTERING(_logprio());

  DT_LOG_NOTICE(_logprio(), "Setup the EM configurator...");
  _grab_work_().em_configurator.SetVerbose(_grab_work_().em_configurator_verbosity);
  DT_LOG_DEBUG(_logprio(), "Initialize the EM model factory...");
  _grab_work_().em_model_fact.initialize();

  DT_LOG_DEBUG(_logprio(), "Apply EM extra models to the Geant4 EM configurator...");
  for (std::map<std::string, ::snsim::processes::em_extra_model>::const_iterator i =
           _grab_work_().em_extra_models.begin();
       i != _grab_work_().em_extra_models.end(); i++) {
    const std::string& em_xmodel_label = i->first;
    const ::snsim::processes::em_extra_model& em_xmodel = i->second;
    // Declare extra models to the embedded Geant4 EM configurator:
    DT_LOG_NOTICE(_logprio(), "Declare extra model '" << em_xmodel_label
                                                      << "' in the Geant4 EM configurator...");
    em_xmodel.apply_to_g4(_grab_work_().em_configurator, _grab_work_().em_model_fact);
  }
  DT_LOG_DEBUG(_logprio(), "EM extra models have been applied to the Geant4 EM configurator.");

  DT_LOG_DEBUG(_logprio(), "Activating declared extra EM models through the EM configurator...");
  _grab_work_().em_configurator.AddModels();

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::_ConstructEMDeexcitation() {
  DT_LOG_TRACE_ENTERING(_logprio());
  // First setup the static atomic deexcitation:
  // G4RadioactiveDecay* radioactiveDecay = new G4RadioactiveDecay();
  // radioactiveDecay->SetHLThreshold(nanosecond);
  // radioactiveDecay->SetICM(true); // Internal Conversion
  // radioactiveDecay->SetARM(true); // Atomic Rearangement

  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);

  // Activate deexcitation processes through the EM options:
  G4EmProcessOptions emOptions;
  int verbose = 1;
  emOptions.SetVerbose(verbose);
  /*
    emOptions.SetMinEnergy(10*eV);     //default=100eV
    emOptions.SetMaxEnergy(10*TeV);    //default=100TeV
    emOptions.SetDEDXBinning(12*10);   //default=12*7
    emOptions.SetLambdaBinning(12*10); //default=12*7
    emOptions.SetMscStepLimitation(fUseDistanceToBoundary); //default=fUseSafety
    emOptions.SetPolarAngleLimit(CLHEP::pi);
  */

  emOptions.SetFluo(_em_fluorescence_);
  if (_em_fluorescence_) {
    emOptions.SetAuger(_em_auger_);
  }
  emOptions.SetPIXE(_em_pixe_);
  if (_em_pixe_) {
    if (!_em_pixe_cross_section_model_.empty()) {
      emOptions.SetPIXECrossSectionModel(_em_pixe_cross_section_model_);
    }
  }

  // Activate deexcitation processes per region :
  for (std::map<std::string, region_deexcitation_type>::const_iterator i =
           _em_regions_deexcitation_.begin();
       i != _em_regions_deexcitation_.end(); i++) {
    const std::string& region_name = i->first;
    const region_deexcitation_type& rd = i->second;
    G4Region* a_region = G4RegionStore::GetInstance()->GetRegion(region_name);
    DT_THROW_IF(
        a_region == nullptr, std::logic_error,
        "Cannot find region named '" << region_name << "' to apply de-excitation processes !");
    DT_LOG_NOTICE(_logprio(), "Activating EM deexcitation for region '"
                                  << region_name << "' :"
                                  << " Fluorescence=" << rd.fluorescence << " Auger=" << rd.auger
                                  << " PIXE=" << rd.pixe << "...");
    emOptions.SetDeexcitationActiveRegion(region_name, rd.fluorescence, rd.auger, rd.pixe);
  }

  // de->SetFluo(true);
  //  //de->SetFluo(_em_fluorescence_);
  //  //de->SetFluo(true);
  // de->SetAuger(true);
  // de->SetPIXE(true);

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_physics_constructor::_ConstructEMProcess() {
  DT_LOG_TRACE_ENTERING(_logprio());

#if G4VERSION_NUMBER < 1000
  theParticleIterator->reset();
  while ((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
#else
  G4ParticleTable::G4PTblDicIterator* theParticleIterator = theParticleTable->GetIterator();
  theParticleIterator->reset();
  while ((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
#endif
    G4ProcessManager* pmanager = particle->GetProcessManager();
    const G4String& particle_name = particle->GetParticleName();

    DT_LOG_NOTICE(_logprio(), "Set electromagnetic processes for '" << particle_name << "'");

    if (particle_name == "gamma") {
      /***********
       *  Gamma  *
       ***********/
      int process_rank = 0;

      G4RayleighScattering* the_rayleigh_scattering = nullptr;
      if (_em_gamma_rayleigh_scattering_) {
        the_rayleigh_scattering = new G4RayleighScattering();
      }
      G4PhotoElectricEffect* the_photoelectric_effect = nullptr;
      if (_em_gamma_photo_electric_) {
        the_photoelectric_effect = new G4PhotoElectricEffect();
      }
      G4ComptonScattering* the_compton_scattering = nullptr;
      if (_em_gamma_compton_scattering_) {
        the_compton_scattering = new G4ComptonScattering();
      }
      G4GammaConversion* the_gamma_conversion = nullptr;
      if (_em_gamma_conversion_) {
        the_gamma_conversion = new G4GammaConversion();
      }
      G4GammaConversionToMuons* the_gamma_conversion_to_muons = nullptr;
      if (_em_gamma_conversion_to_muons_) {
        the_gamma_conversion_to_muons = new G4GammaConversionToMuons();
      }

      if (is_em_low_energy_livermore()) {
        // Livermore:

        if (the_rayleigh_scattering != nullptr) {
          auto* the_livermore_rayleigh_model = new G4LivermoreRayleighModel();
          the_rayleigh_scattering->SetEmModel(the_livermore_rayleigh_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Livermore Rayleigh scattering model for '" << particle_name << "'");
        }

        if (the_photoelectric_effect != nullptr) {
          auto* the_livermore_photoelectric_model = new G4LivermorePhotoElectricModel();
          the_photoelectric_effect->SetEmModel(the_livermore_photoelectric_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Livermore photoelectric effect model for '" << particle_name << "'");
        }

        if (the_compton_scattering != nullptr) {
          auto* the_livermore_compton_model = new G4LivermoreComptonModel();
          the_compton_scattering->SetEmModel(the_livermore_compton_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Livermore Compton scattering model for '" << particle_name << "'");
        }

        if (the_gamma_conversion != nullptr) {
          auto* the_livermore_gamma_conversion_model = new G4LivermoreGammaConversionModel();
          the_gamma_conversion->SetEmModel(the_livermore_gamma_conversion_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Livermore conversion model for '" << particle_name << "'");
        }

      } else if (is_em_low_energy_penelope()) {
        // Penelope:

        if (the_rayleigh_scattering != nullptr) {
          auto* the_penelope_rayleigh_model = new G4PenelopeRayleighModel();
          the_rayleigh_scattering->SetEmModel(the_penelope_rayleigh_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Penelope Rayleigh scattering model for '" << particle_name << "'");
        }

        if (the_photoelectric_effect != nullptr) {
          auto* the_penelope_photoelectric_model = new G4PenelopePhotoElectricModel();
          the_photoelectric_effect->SetEmModel(the_penelope_photoelectric_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Penelope photoelectric effect model for '" << particle_name << "'");
        }

        if (the_compton_scattering != nullptr) {
          auto* the_penelope_compton_model = new G4PenelopeComptonModel();
          the_compton_scattering->SetEmModel(the_penelope_compton_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Penelope Compton scattering model for '" << particle_name << "'");
        }

        if (the_gamma_conversion != nullptr) {
          auto* the_penelope_gamma_conversion_model = new G4PenelopeGammaConversionModel();
          the_gamma_conversion->SetEmModel(the_penelope_gamma_conversion_model);
          DT_LOG_NOTICE(_logprio(),
                        "Setup Penelope conversion model for '" << particle_name << "'");
        }
      }

      if (the_rayleigh_scattering != nullptr) {
        DT_LOG_NOTICE(_logprio(), "Add Rayleigh scattering for '" << particle_name << "'");
        pmanager->AddDiscreteProcess(the_rayleigh_scattering);
      }
      if (the_photoelectric_effect != nullptr) {
        DT_LOG_NOTICE(_logprio(), "Add photoelectric effect for '" << particle_name << "'");
        pmanager->AddDiscreteProcess(the_photoelectric_effect);
      }
      if (the_compton_scattering != nullptr) {
        DT_LOG_NOTICE(_logprio(), "Add Compton scattering for '" << particle_name << "'");
        pmanager->AddDiscreteProcess(the_compton_scattering);
      }
      if (the_gamma_conversion != nullptr) {
        DT_LOG_NOTICE(_logprio(), "Add conversion for '" << particle_name << "'");
        pmanager->AddDiscreteProcess(the_gamma_conversion);
      }
      if (the_gamma_conversion_to_muons != nullptr) {
        DT_LOG_NOTICE(_logprio(), "Add conversion to muons for '" << particle_name << "'");
        pmanager->AddDiscreteProcess(the_gamma_conversion_to_muons);
      }

      if (_em_gamma_step_limiter_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add step limiter for '" << particle_name << "'");
        pmanager->AddProcess(new G4StepLimiter, -1, -1, process_rank);
      }

      if (_em_gamma_user_special_cuts_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add user special cuts limiter for '" << particle_name << "'");
        pmanager->AddProcess(new G4UserSpecialCuts, -1, -1, process_rank);
      }

    } else if (particle_name == "e-" || particle_name == "e+") {
      /***********************
       *  electron/positron  *
       ***********************/
      int process_rank = 0;

      if (_em_electron_multiple_scattering_) {
        // Multiple scattering:
        auto* the_electron_multiple_scattering = new G4eMultipleScattering();
        // Setting the FacRange to 0.005 instead of default value 0.2
        the_electron_multiple_scattering->SetRangeFactor(_em_electron_ms_range_factor_);
        //??? the_electron_multiple_scattering->SetStepLimitType(fUseDistanceToBoundary);
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add e+/e- multiple scattering for '" << particle_name << "'");
        pmanager->AddProcess(the_electron_multiple_scattering, -1, process_rank, process_rank);
      }

      if (_em_electron_ionization_) {
        // Ionization:
        auto* the_e_ionization = new G4eIonisation();
        if (is_em_low_energy_livermore()) {
          // Livermore:
          if (particle_name == "e-") {
            auto* the_livermore_ionization_model = new G4LivermoreIonisationModel();
            the_e_ionization->SetEmModel(the_livermore_ionization_model);
          } else if (particle_name == "e+") {
            auto* the_penelope_ionization_model = new G4PenelopeIonisationModel();
            the_e_ionization->SetEmModel(the_penelope_ionization_model);
          }
        } else if (is_em_low_energy_penelope()) {
          // Penelope:
          auto* the_penelope_ionization_model = new G4PenelopeIonisationModel();
          the_e_ionization->SetEmModel(the_penelope_ionization_model);
        }
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add e+/e- ionization for '" << particle_name << "'");
        pmanager->AddProcess(the_e_ionization, -1, process_rank, process_rank);
      }

      if (_em_electron_bremsstrahlung_) {
        // Bremsstrahlung:
        auto* the_e_bremsstrahlung = new G4eBremsstrahlung();
        if (is_em_low_energy_livermore()) {
          // Livermore:
          if (particle_name == "e-") {
            auto* the_livermore_bremsstrahlung_model = new G4LivermoreBremsstrahlungModel();
            the_e_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
          } else if (particle_name == "e+") {
            auto* the_penelope_bremsstrahlung_model = new G4PenelopeBremsstrahlungModel();
            the_e_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
          }
        } else if (is_em_low_energy_penelope()) {
          // Penelope:
          auto* the_penelope_bremsstrahlung_model = new G4PenelopeBremsstrahlungModel();
          the_e_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
        }
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add e+/e- bremsstrahlung for '" << particle_name << "'");
        pmanager->AddProcess(the_e_bremsstrahlung, -1, process_rank, process_rank);
      }

      if (particle_name == "e+" && _em_positron_annihilation_) {
        // e+ annihilation:
        auto* the_positron_annihilation = new G4eplusAnnihilation();
        if (is_em_low_energy_penelope()) {
          auto* the_penelope_annihilation_model = new G4PenelopeAnnihilationModel();
          the_positron_annihilation->SetEmModel(the_penelope_annihilation_model);
        }
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add positron annihilation for '" << particle_name << "'");
        pmanager->AddProcess(the_positron_annihilation, 0, -1, process_rank);
      }

      if (_em_electron_step_limiter_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add step limiter for '" << particle_name << "'");
        pmanager->AddProcess(new G4StepLimiter, -1, -1, process_rank);
      }

      if (_em_electron_user_special_cuts_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add user special cuts for '" << particle_name << "'");
        pmanager->AddProcess(new G4UserSpecialCuts, -1, -1, process_rank);
      }

    } else if (particle_name == "alpha" || particle_name == "anti_alpha" ||
               particle_name == "deuteron" || particle_name == "anti_deuteron" ||
               particle_name == "triton" || particle_name == "anti_triton" ||
               particle_name == "He3" || particle_name == "anti_He3" ||
               particle_name == "GenericIon") {
      int process_rank = 0;
      /************
       *   Ions   *
       ************/
      if (_em_ion_multiple_scattering_) {
        auto* the_ion_multiple_scattering = new G4hMultipleScattering();
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add hadron multiple scattering for '" << particle_name << "'");
        pmanager->AddProcess(the_ion_multiple_scattering, -1, process_rank, process_rank);
      }

      if (_em_ion_ionization_) {
        // use G4alphaIonisation for alpha...

        auto* the_ion_ionization = new G4ionIonisation();
        if (_logprio() >= datatools::logger::PRIO_TRACE) {
          the_ion_ionization->SetVerboseLevel(2);
        } else if (_logprio() >= datatools::logger::PRIO_WARNING) {
          the_ion_ionization->SetVerboseLevel(1);
        } else {
          the_ion_ionization->SetVerboseLevel(0);
        }

        // Special code for testing:
        // Ion ionization process use the G4BraggIonModel (E=0-2 MeV) and
        // G4BetheBlochModel (E>2MeV) models by default.
        // For ion with speficic atomic electron vacancy (1+, 2+, 3+...),
        // the energy loss process uses an effectuve charge to compute the energy loss
        // in any materials, particularly in solid state.
        // This has the effect to change the effective charge of the ion
        // with some arbitrary value (example: +6e -> +1e). This is a priori not a problem
        // in solid or liquid materials but the problem comes with low density materials
        // (vacuum, gas) where the charge is changed regardless of the effective density
        // and step length along the particle track. Now if an electric field is active
        // in the some gas volume, the electric force computation is done with the
        // changed (and wrong) effective charge and thus this results in an not correct
        // deflection of the ion track.
        // There is a solution to this issue: we can ask the particle to be associated
        // with a suitable ionization model for this specific category of material.
        // The G4BraggIonGasModel and G4BetheBlochIonGasModel do not use any effective
        // charge of the ion and thus the electric force should be correctly computed
        // in such medium. This implies to change the default ionization models for ions.
        //
        bool effective_charge_in_gas = true;
        // effective_charge_in_gas = false;
        if (effective_charge_in_gas) {
          // This is the default behaviour of the G4ionIonisation class:
          auto* bragg_ion_model = new G4BraggIonModel;
          the_ion_ionization->SetEmModel(bragg_ion_model, 1);
          auto* bethe_bloch_model = new G4BetheBlochModel;
          the_ion_ionization->SetEmModel(bethe_bloch_model, 2);
        } else {
          DT_LOG_DEBUG(_logprio(), "Using special ion ionization model in gas...");
          auto* bragg_ion_model = new G4BraggIonGasModel;
          the_ion_ionization->SetEmModel(bragg_ion_model, 1);
          auto* bethe_bloch_model = new G4BetheBlochIonGasModel;
          the_ion_ionization->SetEmModel(bethe_bloch_model, 2);
        }

        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add ion ionization for '" << particle_name << "'");
        pmanager->AddProcess(the_ion_ionization, -1, process_rank, process_rank);
      }

      if (_em_ion_step_limiter_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add step limiter for '" << particle_name << "'");
        pmanager->AddProcess(new G4StepLimiter, -1, -1, process_rank);
      }

      if (_em_ion_user_special_cuts_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add user special cuts for '" << particle_name << "'");
        pmanager->AddProcess(new G4UserSpecialCuts, -1, -1, process_rank);
      }

    } else if (particle_name == "mu+" || particle_name == "mu-") {
      int process_rank = 0;
      /*************
       *   Muons   *
       *************/

      if (_em_muon_multiple_scattering_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add muon multiple scattering for '" << particle_name << "'");
        pmanager->AddProcess(new G4MuMultipleScattering, -1, process_rank, process_rank);
      }

      if (_em_muon_ionization_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add muon ionization for '" << particle_name << "'");
        pmanager->AddProcess(new G4MuIonisation, -1, process_rank, process_rank);
      }

      if (_em_muon_bremsstrahlung_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add muon bremsstrahlung for '" << particle_name << "'");
        pmanager->AddProcess(new G4MuBremsstrahlung, -1, process_rank, process_rank);
      }

      if (_em_muon_pair_production_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add muon pair production '" << particle_name << "'");
        pmanager->AddProcess(new G4MuPairProduction, -1, process_rank, process_rank);
      }

      if (_em_muon_step_limiter_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add step limiter for '" << particle_name << "'");
        pmanager->AddProcess(new G4StepLimiter, -1, -1, process_rank);
      }

      if (_em_muon_user_special_cuts_) {
        ++process_rank;
        DT_LOG_NOTICE(_logprio(), "Add user special cuts for '" << particle_name << "'");
        pmanager->AddProcess(new G4UserSpecialCuts, -1, -1, process_rank);
      }

    } else {
      int process_rank = 0;
      /***********************
       *   other particles   *
       ***********************/
      // all others charged particles
      if ((!particle->IsShortLived()) && (particle->GetPDGCharge() != 0.0)) {
        if (_em_other_multiple_scattering_) {
          ++process_rank;
          DT_LOG_NOTICE(_logprio(), "Add hadron multiple scattering for '" << particle_name << "'");
          pmanager->AddProcess(new G4hMultipleScattering, -1, process_rank, process_rank);
        }

        if (_em_other_ionization_) {
          ++process_rank;
          DT_LOG_NOTICE(_logprio(), "Add hadron ionization for '" << particle_name << "'");
          pmanager->AddProcess(new G4hIonisation, -1, process_rank, process_rank);
        }

        if (_em_other_step_limiter_) {
          ++process_rank;
          DT_LOG_NOTICE(_logprio(), "Add step limiter for '" << particle_name << "'");
          pmanager->AddProcess(new G4StepLimiter, -1, -1, process_rank);
        }

        if (_em_other_user_special_cuts_) {
          ++process_rank;
          DT_LOG_NOTICE(_logprio(), "Add user special cuts for '" << particle_name << "'");
          pmanager->AddProcess(new G4UserSpecialCuts, -1, -1, process_rank);
        }
      }
    }
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

}  // namespace snsim

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snsim::em_physics_constructor, ocd_) {
  // The class name :
  ocd_.set_class_name("snsim::em_physics_constructor");

  // The class terse description :
  ocd_.set_class_description(
      "The Geant4 simulation manager embedded electro-magnetic physics list");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation(
      "The Geant4 simulation manager class embedded \n"
      "electromagnetic physics list.                \n");

  logger::declare_ocd_logging_configuration(ocd_, "warning");

  {
    // Description of the 'em.model' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.model")
        .set_terse_description("Name of the EM model")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("standard")
        .set_long_description(
            "Default: ``\"standard\"``                              \n"
            "                                                       \n"
            "Allowed values:                                        \n"
            "                                                       \n"
            " * ``standard``  : Standard EM processes               \n"
            " * ``livermore`` : Livermore low-energy model          \n"
            " * ``penelope``  : Penelope low-energy model           \n")
        .add_example(
            "Use the *Livermore* EM model: ::   \n"
            "                                   \n"
            " em.model : string = \"livermore\" \n"
            "                                   \n");
  }

  {
    // Description of the 'em.gamma.rayleigh_scattering' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.rayleigh_scattering")
        .set_terse_description("Flag to activate the gamma Rayleigh scattering")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate Rayleigh scattering : ::           \n"
            "                                            \n"
            " em.gamma.rayleigh_scattering : boolean = 1 \n"
            "                                            \n");
  }

  {
    // Description of the 'em.gamma.photo_electric' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.photo_electric")
        .set_terse_description("Flag to activate the gamma photo-electric effect")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate photoelectric effect : ::     \n"
            "                                       \n"
            " em.gamma.photo_electric : boolean = 1 \n"
            "                                       \n");
  }

  {
    // Description of the 'em.gamma.compton_scattering' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.compton_scattering")
        .set_terse_description("Flag to activate the gamma Compton scattering")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate Compton scattering : ::           \n"
            "                                           \n"
            " em.gamma.compton_scattering : boolean = 1 \n"
            "                                           \n");
  }

  {
    // Description of the 'em.gamma.conversion' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.conversion")
        .set_terse_description("Flag to activate the gamma conversion")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate gamma conversion : ::     \n"
            "                                   \n"
            " em.gamma.conversion : boolean = 1 \n"
            "                                   \n");
  }

  {
    // Description of the 'em.gamma.conversion_to_muons' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.conversion_to_muons")
        .set_terse_description("Flag to activate the gamma conversion to muons")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate gamma conversion to muons : ::     \n"
            "                                            \n"
            " em.gamma.conversion_to_muons : boolean = 1 \n"
            "                                            \n");
  }

  {
    // Description of the 'em.electron.ionization' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.ionization")
        .set_terse_description("Flag to activate the electron/positron ionization")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate electron/positron ionization : :: \n"
            "                                           \n"
            " em.electron.ionization : boolean = 1      \n"
            "                                           \n");
  }

  {
    // Description of the 'em.electron.bremsstrahlung' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.bremsstrahlung")
        .set_terse_description("Flag to activate the bremsstrahlung effect for electron/positron")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate electron/positron bremsstrahlung : :: \n"
            "                                               \n"
            " em.electron.bremsstrahlung : boolean = 1      \n"
            "                                               \n");
  }

  {
    // Description of the 'em.electron.multiple_scattering' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering")
        .set_terse_description("Flag to activate the electron/positron multiple scattering")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate electron/positron multiple scattering : :: \n"
            "                                                    \n"
            " em.electron.multiple_scattering : boolean = 1      \n"
            "                                                    \n");
  }

  {
    // Description of the 'em.electron.multiple_scattering.use_distance_to_boundary' configuration
    // property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering.use_distance_to_boundary")
        .set_terse_description(
            "Flag to use distance to boundary in the electron/positron multiple scattering")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Use the distance to boundary algorithm for electron/positron multiple scattering : :: "
            "\n"
            "                                                                         \n"
            " em.electron.multiple_scattering.use_distance_to_boundary : boolean = 1  \n"
            "                                                                         \n");
  }

  {
    // Description of the 'em.electron.multiple_scattering.range_factor' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering.range_factor")
        .set_terse_description("The range factor in the electron/positron multiple scattering")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.005)
        .add_example(
            "Set the electron/positron multiple scattering range factor : :: \n"
            "                                                                \n"
            " em.electron.multiple_scattering.range_factor : real = 0.005    \n"
            "                                                                \n");
  }

  {
    // Description of the 'em.electron.step_limiter' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.step_limiter")
        .set_terse_description("Flag to activate the electron/positron step limiter")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Set the electron/positron step limiter technique : ::  \n"
            "                                                       \n"
            " em.electron.step_limiter : boolean = 1                \n"
            "                                                       \n");
  }

  {
    // Description of the 'em.positron.annihilation' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.positron.annihilation")
        .set_terse_description("Flag to activate the positron annihilation")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example(
            "Activate positron annihilation : ::     \n"
            "                                        \n"
            " em.positron.annihilation : boolean = 1 \n"
            "                                        \n");
  }

  {
    // Description of the 'em.deexcitation.fluorescence' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.fluorescence")
        .set_terse_description("Flag to activate the atomic deexcitation and fluorescence")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate deexcitation by fluorescence : ::  \n"
            "                                            \n"
            " em.deexcitation.fluorescence : boolean = 1 \n"
            "                                            \n");
  }

  {
    // Description of the 'em.deexcitation.auger' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.auger")
        .set_terse_description("Flag to activate Auger process")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .set_long_description(
            "Auger effect is only activated if ``em.deexcitation.fluorescence``\n"
            "is also activated.                                                \n")
        .add_example(
            "Activate deexcitation by Auger effect : :: \n"
            "                                           \n"
            " em.deexcitation.auger : boolean = 1       \n"
            "                                           \n");
  }

  {
    // Description of the 'em.deexcitation.pixe' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe")
        .set_terse_description("Flag to activate PIXE process (Particle Induced X-ray Emission)")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate deexcitation by PIXE process : :: \n"
            "                                           \n"
            " em.deexcitation.pixe : boolean = 1        \n"
            "                                           \n");
  }

  {
    // Description of the 'em.deexcitation.pixe.cross_section_model' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe.cross_section_model")
        .set_terse_description("The name of the PIXE cross-section model")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("empirical")
        .set_long_description(
            "Allowed values are:                                    \n"
            "                                                       \n"
            " * ``\"empirical\"``                                   \n"
            " * ``\"ecpssr_formfactor\"``                           \n"
            " * ``\"ecpssr_analytical\"``                           \n")
        .add_example(
            "Use the *empirical* PIXE model : :: \n"
            "                                                       \n"
            " em.deexcitation.pixe.cross_section_model : string = \"empirical\"  \n"
            "                                                       \n");
  }

  {
    // Description of the 'em.deexcitation.pixe.cross_section_model' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe.electron_cross_section_model")
        .set_terse_description("The name of the PIXE e+/e- cross-section model")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("empirical")
        .set_long_description(
            "Allowed values are:                                    \n"
            "                                                       \n"
            " * ``\"livermore\"``                                   \n"
            " * ``\"proton_analytical\"``                           \n"
            " * ``\"proton_empirical\"``                            \n"
            " * ``\"penelope\"``                                    \n")
        .add_example(
            "Use the *empirical* PIXE model : :: \n"
            "                                                       \n"
            " em.deexcitation.pixe.electron_cross_section_model : string = \"livermore\"  \n"
            "                                                       \n");
  }

  {
    // Description of the 'em.deexcitation.regions' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions")
        .set_terse_description(
            "The name of the regions with special parameters for deexcitation processes")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .add_example(
            "Define two regions with activated deexcitation: :: \n"
            "                                                   \n"
            " em.deexcitation.regions : string[2] = \"A\" \"B\" \n"
            "                                                   \n");
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.fluorescence' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.fluorescence")
        .set_terse_description("Flag to activate fluorecence in some given region")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate fluorescence in two *deexcitation* regions: :: \n"
            "                                                        \n"
            " em.deexcitation.regions : string[2] = \"A\" \"B\"      \n"
            " em.deexcitation.regions.A.fluorescence : boolean = 1   \n"
            " em.deexcitation.regions.B.fluorescence : boolean = 1   \n"
            "                                                        \n");
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.auger' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.auger")
        .set_terse_description("Flag to activate Auger effect in some given region")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate Auger effect in two *deexcitation* regions: :: \n"
            "                                                        \n"
            " em.deexcitation.regions : string[2] = \"A\" \"B\"      \n"
            " em.deexcitation.regions.A.auger : boolean = 1          \n"
            " em.deexcitation.regions.B.auger : boolean = 1          \n"
            "                                                        \n");
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.pixe' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.pixe")
        .set_terse_description("Flag to activate PIXE effect in some given region")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate PIXE effect in two *deexcitation* regions: :: \n"
            "                                                       \n"
            " em.deexcitation.regions : string[2] = \"A\" \"B\"     \n"
            " em.deexcitation.regions.A.pixe : boolean = 1          \n"
            " em.deexcitation.regions.B.pixe : boolean = 1          \n"
            "                                                       \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Not available yet                    \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snsim::em_physics_constructor' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::em_physics_constructor, "snsim::em_physics_constructor")
