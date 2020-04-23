/// physics_list.cc
///
/// http://geant4.cern.ch/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch06.html
/// http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml

// Ourselves:
#include <mctools/g4/physics_list.h>

// Standard library:
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/algorithm/string/predicate.hpp>
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/units.h>
// - Geant4 :
#include <G4UnitsTable.hh>
#include <G4Version.hh>
#include <globals.hh>
//#include <G4ProcessManager.hh>
// Particles:
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4PhysListFactory.hh>
#include <G4ProductionCuts.hh>
#include <G4RegionStore.hh>
#if G4VERSION_NUMBER >= 1000
#include <G4StepLimiterPhysics.hh>
#else  // Geant4 9.X
#include <G4StepLimiterBuilder.hh>
#endif

namespace mctools {

namespace g4 {

// *** physics_list::production_cuts_info *** //

physics_list::production_cuts_info::production_cuts_info() {
  datatools::invalidate(gamma);
  datatools::invalidate(electron);
  datatools::invalidate(positron);
  datatools::invalidate(proton);
}

physics_list::production_cuts_info::production_cuts_info(double all_value_) {
  gamma = all_value_;
  electron = all_value_;
  positron = all_value_;
  proton = all_value_;
}

physics_list::production_cuts_info::production_cuts_info(double gamma_value_,
                                                         double electron_value_,
                                                         double positron_value_,
                                                         double proton_value_) {
  datatools::invalidate(gamma);
  datatools::invalidate(electron);
  datatools::invalidate(positron);
  datatools::invalidate(proton);
  gamma = gamma_value_;
  electron = electron_value_;
  positron = positron_value_;
  proton = proton_value_;
}

void physics_list::production_cuts_info::initialize(double default_cut_gamma_,
                                                    double default_cut_electron_,
                                                    double default_cut_positron_,
                                                    double default_cut_proton_) {
  if (!datatools::is_valid(gamma)) {
    gamma = default_cut_gamma_;
  }
  if (!datatools::is_valid(electron)) {
    electron = default_cut_electron_;
  }
  if (!datatools::is_valid(positron)) {
    positron = default_cut_positron_;
  }
  if (!datatools::is_valid(proton)) {
    proton = default_cut_proton_;
  }

  DT_THROW_IF(datatools::is_valid(gamma) && gamma <= 0.0, std::domain_error,
              "Invalid production cut for gamma (" << gamma << ") !");
  DT_THROW_IF(datatools::is_valid(electron) && electron <= 0.0, std::domain_error,
              "Invalid production cut for electron (" << electron << ") !");
  DT_THROW_IF(datatools::is_valid(positron) && positron <= 0.0, std::domain_error,
              "Invalid production cut for positron (" << positron << ") !");
  DT_THROW_IF(datatools::is_valid(proton) && proton <= 0.0, std::domain_error,
              "Invalid production cut for proton (" << proton << ") !");
}

// *** physics_list *** //

auto physics_list::has_geant4_physics_list() const -> bool {
  return _geant4_physics_list_.get() != nullptr;
}

auto physics_list::get_geant4_physics_list() const -> const G4VModularPhysicsList& {
  DT_THROW_IF(!_geant4_physics_list_, std::logic_error, "No Geant4 physics list is defined !");
  return *_geant4_physics_list_;
}

auto physics_list::grab_geant4_physics_list() -> G4VModularPhysicsList& {
  DT_THROW_IF(!_geant4_physics_list_, std::logic_error, "No Geant4 physics list is defined !");
  return *_geant4_physics_list_;
}

physics_list::physics_list() {
  _initialized_ = false;
  _geant4_physics_list_.reset(nullptr);
  this->_set_defaults();
}

physics_list::~physics_list() {
  if (_geant4_physics_list_) {
    _geant4_physics_list_.reset();
  }
}

void physics_list::_set_defaults() {
  // Cuts:
  _using_production_cuts_ = false;
  datatools::invalidate(_production_cuts_low_energy_);
  datatools::invalidate(_production_cuts_high_energy_);
  _production_cuts_default_value_ = 1 * CLHEP::mm;
  _production_cuts_values_.proton = _production_cuts_default_value_;
  _production_cuts_values_.electron = _production_cuts_default_value_;
  _production_cuts_values_.positron = _production_cuts_default_value_;
  _production_cuts_values_.gamma = _production_cuts_default_value_;
}

auto physics_list::get_physics_constructors() const -> const physics_constructor_dict_type& {
  return _physics_constructors_;
}

auto physics_list::grab_physics_constructors() -> physics_constructor_dict_type& {
  return _physics_constructors_;
}

auto physics_list::has_physics_constructor(const std::string& pc_name_) const -> bool {
  auto pc_found = _physics_constructors_.find(pc_name_);
  return pc_found != _physics_constructors_.end() && pc_found->second.handle;
}

auto physics_list::get_physics_constructor(const std::string& pc_name_)
    -> const base_physics_constructor& {
  physics_constructor_dict_type::const_iterator pc_found = _physics_constructors_.find(pc_name_);
  DT_THROW_IF(pc_found == _physics_constructors_.end(), std::logic_error,
              "Cannot find physics constructor with name '" << pc_name_ << "' !");
  DT_THROW_IF(pc_found->second.handle, std::logic_error,
              "Physics constructor named '" << pc_name_ << "' is not instantiated !");
  return pc_found->second.handle.get();
}

void physics_list::initialize(const datatools::properties& config_) {
  DT_LOG_TRACE_ENTERING(_logprio());
  DT_THROW_IF(_initialized_, std::logic_error, "Already initialized !");

  // *********************** General properties *************************** //

  loggable_support::_initialize_logging_support(config_);

  _factory_register_.set_label("mctools::g4::physics_list/factory");
  _factory_register_.set_logging_priority(_logprio());
  _factory_register_.import(
      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(mctools::g4::base_physics_constructor));

  DT_LOG_DEBUG(_logprio(), "Physics list configuration : ");
  if (_logprio() >= datatools::logger::PRIO_DEBUG) {
    config_.tree_dump(std::cerr);
  }
  // Set G4VUserPhysicsList verbosity
  if (_logprio() < datatools::logger::PRIO_NOTICE) {
    SetVerboseLevel(VERBOSITY_SILENT);
  } else if (_logprio() < datatools::logger::PRIO_DEBUG) {
    SetVerboseLevel(VERBOSITY_WARNING);
  } else {
    SetVerboseLevel(VERBOSITY_MORE);
  }

  // *********************** Geant4 physics list *************************** //

  // Try to use the Geant4 official G4PhysListFactory:
  G4PhysListFactory geant4_physics_list_factory;
  if (config_.has_key("geant4.physics_list")) {
    _geant4_physics_list_name_ = config_.fetch_string("geant4.physics_list");
    DT_THROW_IF(!geant4_physics_list_factory.IsReferencePhysList(_geant4_physics_list_name_),
                std::logic_error,
                "Geant4 physics list factory does not have any '" << _geant4_physics_list_name_
                                                                  << "' physics list !");
    _geant4_physics_list_.reset(
        geant4_physics_list_factory.GetReferencePhysList(_geant4_physics_list_name_));
  }

  // *********************** Physics constructors *************************** //

  // If we do not use a Geant4 official G4PhysListFactory, we build
  // our own physics list from a set of physics constructors instantiated
  // from a dedicated factory :
  if (!has_geant4_physics_list()) {
    std::vector<std::string> physics_constructors_names;
    if (config_.has_key("physics_constructors.names")) {
      config_.fetch("physics_constructors.names", physics_constructors_names);
    }

    DT_THROW_IF(physics_constructors_names.empty(), std::logic_error,
                "No physics constructor is provided !");

    for (const auto& pc_name : physics_constructors_names) {
      DT_THROW_IF(pc_name.empty(), std::logic_error, "Empty physics constructor name !");
      std::ostringstream pc_id_key;
      pc_id_key << "physics_constructors." << pc_name << ".id";
      DT_THROW_IF(!config_.has_key(pc_id_key.str()), std::logic_error,
                  "Missing ID property '" << pc_id_key.str() << "' for physics constructor '"
                                          << pc_name << "' !");
      const std::string& pc_id = config_.fetch_string(pc_id_key.str());

      std::ostringstream pc_config_key;
      pc_config_key << "physics_constructors." << pc_name << ".config";
      DT_THROW_IF(!config_.has_key(pc_config_key.str()), std::logic_error,
                  "Missing configuration property '"
                      << pc_config_key.str() << "' for physics constructor '" << pc_name << "' !");
      const std::string& pc_config = config_.fetch_string(pc_config_key.str());
      {
        physics_constructor_entry pc_dummy;
        pc_dummy.name = pc_name;
        pc_dummy.id = pc_id;
        pc_dummy.status = 0;
        physics_constructor_dict_type::const_iterator pc_found =
            _physics_constructors_.find(pc_name);
        DT_THROW_IF(pc_found != _physics_constructors_.end(), std::logic_error,
                    "A physics constructor with name '" << pc_name << "' already exists !");
        _physics_constructors_[pc_name] = pc_dummy;
      }
      physics_constructor_entry& pce = _physics_constructors_[pc_name];
      std::string pc_config_filename = pc_config;
      datatools::fetch_path_with_env(pc_config_filename);
      pce.config.read_configuration(pc_config_filename);
      pce.status = 0;

      // if (boost::starts_with(pce.id, "geant4::")) {
      //
      //   // There is no support yet for Geant builder classes.
      //   // This would imply some dedicated factory (not provided
      //   // by Geant4 itself) and also some mechanism to parametrize
      //   // physics builder objects (properties + CAMP ?)
      //
      // } else
      {
        // Search for the physics constructor ID in the factory dictionary:
        DT_THROW_IF(!_factory_register_.has(pce.id), std::logic_error,
                    "Cannot find physics constructor factory with ID '"
                        << pce.id << "' for physics constructor named '" << pce.name << "' !");
        const base_physics_constructor::factory_register_type::factory_type& the_factory =
            _factory_register_.get(pce.id);
        base_physics_constructor* pc = the_factory();
        DT_THROW_IF(pc == nullptr, std::logic_error,
                    "Creation of '" << pce.name << "' physics constructor of type '" << pce.id
                                    << "' failed !");
        pc->set_name(pce.name);
        pc->set_class_id(pce.id);
        pc->_set_mother_physics_list(*this);
        pce.handle.reset(pc);
        pce.status |= physics_constructor_entry::STATUS_CREATED;
        pc->initialize(pce.config, _physics_constructors_);
        pce.status |= physics_constructor_entry::STATUS_INITIALIZED;
      }
    }  // for (int i = 0; i < physics_constructors_names.size(); i++) {
  }    // if (! has_geant4_physics_list())

  // *********************** Production cuts *************************** //

  if (config_.has_key("using_production_cuts")) {
    _using_production_cuts_ = config_.fetch_boolean("using_production_cuts");
  }
  DT_LOG_DEBUG(_logprio(), "Use production cuts set to: " << _using_production_cuts_);

  datatools::properties pc_config;
  config_.export_and_rename_starting_with(pc_config, "production_cuts.", "");

  // Warning for users:
  if (!pc_config.empty() && !_using_production_cuts_) {
    DT_LOG_WARNING(_logprio(),
                   "Your configuration has 'production_cuts' related parameters but "
                   "'using_production_cuts' is not activated!");
  }

  if (_using_production_cuts_) {
    // Production cuts:
    std::vector<std::string> regions;
    double lunit = CLHEP::mm;
    double eunit = CLHEP::MeV;

    // Production cuts length unit :
    if (config_.has_key("production_cuts.length_unit")) {
      std::string lunit_str = config_.fetch_string("production_cuts.length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    // Production cuts energy unit :
    if (config_.has_key("production_cuts.energy_unit")) {
      std::string eunit_str = config_.fetch_string("production_cuts.energy_unit");
      eunit = datatools::units::get_energy_unit_from(eunit_str);
    }

    // Production cuts low edge energy :
    if (config_.has_key("production_cuts.low_energy")) {
      _production_cuts_low_energy_ = config_.fetch_real("production_cuts.low_energy");
      if (!config_.has_explicit_unit("production_cuts.low_energy")) {
        _production_cuts_low_energy_ *= eunit;
      }
    }
    DT_LOG_NOTICE(_logprio(), "Production cuts low edge energy: "
                                  << _production_cuts_low_energy_ / CLHEP::eV << " eV");

    // Production cuts high edge energy :
    if (config_.has_key("production_cuts.high_energy")) {
      _production_cuts_high_energy_ = config_.fetch_real("production_cuts.high_energy");
      if (!config_.has_explicit_unit("production_cuts.high_energy")) {
        _production_cuts_high_energy_ *= eunit;
      }
    }
    DT_LOG_NOTICE(_logprio(), "Production cuts high edge energy: "
                                  << _production_cuts_high_energy_ / CLHEP::MeV << " MeV");

    // Default production cut value
    if (config_.has_key("production_cuts.default_value")) {
      _production_cuts_default_value_ = config_.fetch_real("production_cuts.default_value");
      DT_THROW_IF(_production_cuts_default_value_ <= 0.0, std::domain_error,
                  "Invalid default cut value (" << _production_cuts_default_value_ << ") !");
      if (!config_.has_explicit_unit("production_cuts.default_value")) {
        _production_cuts_default_value_ *= lunit;
      }
    }

    // Default production cut values per particle :
    _production_cuts_values_.gamma = _production_cuts_default_value_;
    _production_cuts_values_.electron = _production_cuts_default_value_;
    _production_cuts_values_.positron = _production_cuts_default_value_;
    _production_cuts_values_.proton = _production_cuts_default_value_;

    // Default gamma production cut :
    if (config_.has_key("production_cuts.gamma")) {
      _production_cuts_values_.gamma = config_.fetch_real("production_cuts.gamma");
      if (!config_.has_explicit_unit("production_cuts.gamma")) {
        _production_cuts_values_.gamma *= lunit;
      }
    }
    DT_LOG_NOTICE(_logprio(),
                  "Gamma cut set to: " << _production_cuts_values_.gamma / CLHEP::mm << " mm");

    // Default electron production cut :
    if (config_.has_key("production_cuts.electron")) {
      _production_cuts_values_.electron = config_.fetch_real("production_cuts.electron");
      if (!config_.has_explicit_unit("production_cuts.electron")) {
        _production_cuts_values_.electron *= lunit;
      }
    }
    DT_LOG_NOTICE(_logprio(), "Electron cut set to: "
                                  << _production_cuts_values_.electron / CLHEP::mm << " mm");

    // Default positron production cut :
    if (config_.has_key("production_cuts.positron")) {
      _production_cuts_values_.positron = config_.fetch_real("production_cuts.positron");
      if (!config_.has_explicit_unit("production_cuts.positron")) {
        _production_cuts_values_.positron *= lunit;
      }
    }
    DT_LOG_NOTICE(_logprio(), "Positron cut set to: "
                                  << _production_cuts_values_.positron / CLHEP::mm << " mm");

    // Default proton production cut :
    if (config_.has_key("production_cuts.proton")) {
      _production_cuts_values_.proton = config_.fetch_real("production_cuts.proton");
      if (!config_.has_explicit_unit("production_cuts.proton")) {
        _production_cuts_values_.proton *= lunit;
      }
    }
    DT_LOG_NOTICE(_logprio(),
                  "Proton cut set to: " << _production_cuts_values_.proton / CLHEP::mm << " mm");

    // Production cuts per regions :
    if (config_.has_key("production_cuts.regions")) {
      config_.fetch("production_cuts.regions", regions);

      for (const auto& region_label : regions) {
        production_cuts_info pc_info;
        pc_info.gamma = _production_cuts_values_.gamma;
        pc_info.electron = _production_cuts_values_.electron;
        pc_info.positron = _production_cuts_values_.positron;
        pc_info.proton = _production_cuts_values_.proton;
        int checked = 0;
        // try all particles first:
        std::ostringstream key_ss;
        key_ss << "production_cuts.regions." << region_label << ".all";
        const std::string key = key_ss.str();
        if (config_.has_key(key)) {
          double all_production_cut = config_.fetch_real(key);
          if (!config_.has_explicit_unit(key)) {
            all_production_cut *= lunit;
          }
          pc_info.gamma = all_production_cut;
          pc_info.electron = all_production_cut;
          pc_info.positron = all_production_cut;
          pc_info.proton = all_production_cut;
          checked = 1;
        } else {
          // try gamma:
          {
            std::ostringstream gamma_key_ss;
            gamma_key_ss << "production_cuts.regions." << region_label << ".gamma";
            const std::string gamma_key = gamma_key_ss.str();
            if (config_.has_key(gamma_key)) {
              double gamma_production_cut = config_.fetch_real(gamma_key);
              if (!config_.has_explicit_unit(gamma_key)) {
                gamma_production_cut *= lunit;
              }
              pc_info.gamma = gamma_production_cut;
              checked = 1;
            }
          }
          // try electron:
          {
            std::ostringstream electron_key_ss;
            electron_key_ss << "production_cuts.regions." << region_label << ".electron";
            const std::string electron_key = electron_key_ss.str();
            if (config_.has_key(electron_key)) {
              double electron_production_cut = config_.fetch_real(electron_key);
              if (!config_.has_explicit_unit(electron_key)) {
                electron_production_cut *= lunit;
              }
              pc_info.electron = electron_production_cut;
              checked = 1;
            }
          }
          // try positron:
          {
            std::ostringstream positron_key_ss;
            positron_key_ss << "production_cuts.regions." << region_label << ".positron";
            const std::string positron_key = positron_key_ss.str();
            if (config_.has_key(positron_key)) {
              double positron_production_cut = config_.fetch_real(positron_key);
              if (!config_.has_explicit_unit(positron_key)) {
                positron_production_cut *= lunit;
              }
              pc_info.positron = positron_production_cut;
              checked = 1;
            }
          }
          // try proton:
          {
            std::ostringstream proton_key_ss;
            proton_key_ss << "production_cuts.regions." << region_label << ".proton";
            const std::string proton_key = proton_key_ss.str();
            if (config_.has_key(proton_key)) {
              double proton_production_cut = config_.fetch_real(proton_key);
              if (!config_.has_explicit_unit(proton_key)) {
                proton_production_cut *= lunit;
              }
              pc_info.proton = proton_production_cut;
              checked = 1;
            }
          }
        }
        DT_THROW_IF(!checked, std::logic_error,
                    "Missing production cuts value(s)  for region '" << region_label << "' !");
        pc_info.initialize(_production_cuts_values_.gamma, _production_cuts_values_.electron,
                           _production_cuts_values_.positron, _production_cuts_values_.proton);
        _production_cuts_per_region_[region_label] = pc_info;
      }
    }

    if (is_debug()) {
      DT_LOG_DEBUG(_logprio(), "Production cuts for regions : ");
      for (std::map<std::string, production_cuts_info>::const_iterator i =
               _production_cuts_per_region_.begin();
           i != _production_cuts_per_region_.end(); i++) {
        auto j = i;
        j++;
        if (j == _production_cuts_per_region_.end()) {
          std::clog << "`-- ";
        } else {
          std::clog << "|-- ";
        }
        std::clog << "Region '" << i->first << "'  :  Production cuts are : "
                  << " gamma = " << i->second.gamma / CLHEP::mm << " mm"
                  << " electron = " << i->second.electron / CLHEP::mm << " mm"
                  << " positron = " << i->second.positron / CLHEP::mm << " mm"
                  << " proton = " << i->second.proton / CLHEP::mm << " mm";
        std::clog << std::endl;
      }
    }
  }

  // SetVerboseLevel(_g4_verbosity_);
  DT_LOG_NOTICE(_logprio(),
                "Original default cut Length : " << G4BestUnit(defaultCutValue, "Length"));

  _register_physics_constructors();

  _initialized_ = true;

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::_register_physics_constructors() {
  DT_LOG_TRACE_ENTERING(_logprio());
  DT_THROW_IF(_initialized_, std::logic_error, "Already initialized !");

  for (auto i = _physics_constructors_.begin(); i != _physics_constructors_.end(); i++) {
    physics_constructor_entry& pc_entry = i->second;
    DT_THROW_IF(!(pc_entry.status & physics_constructor_entry::STATUS_INITIALIZED),
                std::logic_error, "Physics constructor '" << i->first << "' is not initialized !");
    base_physics_constructor& pc = pc_entry.grab_physics_constructor();
    DT_LOG_NOTICE(_logprio(), "Registering physics constructor '" << i->first << "'...");
    RegisterPhysics(new physics_constructor_proxy(pc));
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::reset() {
  DT_LOG_TRACE_ENTERING(_logprio());
  DT_THROW_IF(!_initialized_, std::logic_error, "Not initialized !");

  _initialized_ = false;

  if (has_geant4_physics_list()) {
    _geant4_physics_list_.reset(nullptr);
    _geant4_physics_list_name_.clear();
  } else {
    _physics_constructors_.clear();
    _production_cuts_per_region_.clear();
  }
  _set_defaults();

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::ConstructParticle() {
  DT_LOG_TRACE_ENTERING(_logprio());

  if (has_geant4_physics_list()) {
    grab_geant4_physics_list().ConstructParticle();
  } else {
    this->G4VModularPhysicsList::ConstructParticle();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::ConstructProcess() {
  DT_LOG_TRACE_ENTERING(_logprio());

  if (has_geant4_physics_list()) {
    grab_geant4_physics_list().ConstructProcess();
#if G4VERSION_NUMBER >= 1000
    grab_geant4_physics_list().RegisterPhysics(new G4StepLimiterPhysics());
#else
    grab_geant4_physics_list().RegisterPhysics(new G4StepLimiterBuilder());
#endif
  } else {
    this->G4VModularPhysicsList::ConstructProcess();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::SetCuts() {
  DT_LOG_TRACE_ENTERING(_logprio());

  if (has_geant4_physics_list()) {
    grab_geant4_physics_list().SetCuts();
  } else {
    _SetCuts();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::_SetCuts() {
  DT_LOG_TRACE_ENTERING(_logprio());

  SetDefaultCutValue(_production_cuts_default_value_);
  DT_LOG_NOTICE(_logprio(), "Default cut Length : " << G4BestUnit(defaultCutValue, "Length"));

  // G4VUserPhysicsList::SetCutsWithDefault method sets the default cut value for all particle types
  // :
  SetCutsWithDefault();

  double pct_low_edge_energy = G4ProductionCutsTable::GetProductionCutsTable()->GetLowEdgeEnergy();
  double pct_high_edge_energy =
      G4ProductionCutsTable::GetProductionCutsTable()->GetHighEdgeEnergy();
  DT_LOG_NOTICE(_logprio(), "Default production cuts low edge energy = "
                                << pct_low_edge_energy / CLHEP::eV << " eV");
  DT_LOG_NOTICE(_logprio(), "Default production cuts high edge energy = "
                                << pct_high_edge_energy / CLHEP::GeV << " GeV");

  if (datatools::is_valid(_production_cuts_low_energy_)) {
    pct_low_edge_energy = _production_cuts_low_energy_;
  }
  if (datatools::is_valid(_production_cuts_high_energy_)) {
    pct_high_edge_energy = _production_cuts_high_energy_;
  }
  DT_LOG_NOTICE(_logprio(), "Effective production cuts low edge energy = "
                                << pct_low_edge_energy / CLHEP::eV << " eV");
  DT_LOG_NOTICE(_logprio(), "Effective production cuts high edge energy = "
                                << pct_high_edge_energy / CLHEP::GeV << " GeV");
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(pct_low_edge_energy,
                                                                  pct_high_edge_energy);

  if (_using_production_cuts_) {
    DT_LOG_TRACE(_logprio(), "Using production cuts...");
    // Defining production cuts for different particles:
    // set production cut values for gamma at first and for e- second and next for e+ then proton
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(_production_cuts_values_.gamma, "gamma");
    SetCutValue(_production_cuts_values_.electron, "e-");
    SetCutValue(_production_cuts_values_.positron, "e+");
    SetCutValue(_production_cuts_values_.proton, "proton");

    // Defining production cuts for the different regions :
    for (std::map<std::string, production_cuts_info>::const_iterator i =
             _production_cuts_per_region_.begin();
         i != _production_cuts_per_region_.end(); i++) {
      const std::string& region_name = i->first;
      const production_cuts_info& pc_info = i->second;
      G4Region* a_region = G4RegionStore::GetInstance()->GetRegion(region_name);
      if (a_region != nullptr) {
        auto* a_region_cuts = new G4ProductionCuts;
        a_region_cuts->SetProductionCut(pc_info.gamma, G4ProductionCuts::GetIndex("gamma"));
        a_region_cuts->SetProductionCut(pc_info.electron, G4ProductionCuts::GetIndex("e-"));
        a_region_cuts->SetProductionCut(pc_info.positron, G4ProductionCuts::GetIndex("e+"));
        a_region_cuts->SetProductionCut(pc_info.proton, G4ProductionCuts::GetIndex("proton"));
        a_region->SetProductionCuts(a_region_cuts);
      } else {
        DT_THROW(std::logic_error,
                 "No region named '" << region_name << "' to be given production cuts !");
        // DT_LOG_WARNING(_logprio(), "No region named '"
        //                << region_name << "' to be given production cuts !");
      }
    }

    // Verbosing:
    DumpCutValuesTable();
  }
  DT_LOG_TRACE_EXITING(_logprio());
}

void physics_list::tree_dump(std::ostream& out_, const std::string& title_,
                             const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Initialized                  : " << (_initialized_ ? "Yes" : "No") << std::endl;

  // Geant4 physics list:
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Geant4 physics list  : " << (has_geant4_physics_list() ? "Yes" : "No") << std::endl;
  out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
       << "Name : '" << _geant4_physics_list_name_ << "'" << std::endl;

  // Physics constructors:
  out_ << indent << datatools::i_tree_dumpable::tag << "Physics constructors : ";
  if (_physics_constructors_.empty()) {
    out_ << "None";
  }
  out_ << std::endl;
  for (auto i = _physics_constructors_.begin(); i != _physics_constructors_.end(); i++) {
    const physics_constructor_entry& pc_entry = i->second;
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    auto j = i;
    j++;
    std::ostringstream indent2;
    indent2 << indent << datatools::i_tree_dumpable::skip_tag;
    if (j == _physics_constructors_.end()) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "'" << pc_entry.name << "' of type '" << pc_entry.id << "' (status=" << pc_entry.status
         << ")";
    out_ << std::endl;
    if ((pc_entry.status & physics_constructor_entry::STATUS_CREATED) != 0U) {
      pc_entry.get_physics_constructor().tree_dump(out_, "", indent2.str());
    }
  }

  // Production cuts :
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Using production cuts        : " << (_using_production_cuts_ ? "Yes" : "No")
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Default production cut value : " << _production_cuts_default_value_ / CLHEP::mm << " mm"
       << std::endl;
  // Default particles cuts:
  out_ << indent << datatools::i_tree_dumpable::last_tag
       << "Default gamma production cut                  : "
       << _production_cuts_values_.gamma / CLHEP::mm << " mm" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Default electron production cut               : "
       << _production_cuts_values_.electron / CLHEP::mm << " mm" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Default positron production cut               : "
       << _production_cuts_values_.positron / CLHEP::mm << " mm" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag
       << "Default proton production cut               : "
       << _production_cuts_values_.proton / CLHEP::mm << " mm" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Regions cuts                 : "
       << (!_production_cuts_per_region_.empty() ? "Yes" : "No") << std::endl;
  for (auto i = _production_cuts_per_region_.begin(); i != _production_cuts_per_region_.end();
       i++) {
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    auto j = i;
    j++;
    if (j == _production_cuts_per_region_.end()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Region '" << i->first << "' :  Production cuts : "
         << " gamma = " << i->second.gamma / CLHEP::mm << " mm"
         << " electron = " << i->second.electron / CLHEP::mm << " mm"
         << " positron = " << i->second.positron / CLHEP::mm << " mm" << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Logging priority             : " << datatools::logger::get_priority_label(_logprio())
       << std::endl;
}

}  // end of namespace g4

}  // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::physics_list, ocd_) {
  // The class name :
  ocd_.set_class_name("mctools::g4::physics_list");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation manager embedded physics list");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation(
      "The Geant4 simulation manager class embedded               \n"
      "physics list.                                              \n"
      "The ``mctools::g4::physics_list`` enables to:              \n"
      "                                                           \n"
      " * use an official physics list published                  \n"
      "   by the Geant4 API.                                      \n"
      " * alternatively, use some user-made physics               \n"
      "   constructors available from the mctools/G4 API itself   \n"
      "   or some client API.                                     \n"
      " * use some specific production cuts,                      \n"
      "   possibly per region of the geometrical.                 \n"
      "                                                           \n");

  // ***** General ***** //

  logger::declare_ocd_logging_configuration(ocd_, "warning");

  // ***** Geant4 physics list ***** //

  {
    // Description of the 'geant4.physics_list' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();

    // Dynamic build the long description :
    std::ostringstream ld;
    ld << "Geant4 provides a physics list factory that enables the instantiation \n"
       << "and initialization of some predefined physics list from its registration\n"
       << "name.                                                                 \n"
       << "                                                                      \n"
       << "Allowed values from Geant4 [" << G4VERSION_NUMBER
       << "] (``G4PhysListFactory`` class) : \n"
       << "                                                                      \n";
    G4PhysListFactory f;
    for (const auto& i : f.AvailablePhysLists()) {
      ld << " * ``\"" << i << "\"`` \n";
    }
    ld << "                                                                      \n";
    ld << "See also: http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml "
          "\n";
    ld << "                                                                      \n";
    ld << "This property is not compatible with the ``physics_constructors.names``\n"
       << "property.                                                              \n";

    cpd.set_name_pattern("geant4.physics_list")
        .set_terse_description("The name of the Geant4 physics list to activate")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_complex_triggering_conditions(true)
        .set_long_description(ld.str())
        .add_example(
            "                                                              \n"
            "  geant4.physics_list : string= \"FTFP_BERT_EMV\"             \n"
            "                                                              \n");
  }

  // ***** Physics constructor plugins ***** //

  {
    // Description of the 'physics_constructors.names' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("physics_constructors.names")
        .set_terse_description("The names of physics constructor plugins to be loaded")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .set_complex_triggering_conditions(true)
        .set_long_description(
            "This property is not compatible with the ``geant4.physics_list``   \n"
            "property.                                                          \n")
        .add_example(
            "                                                                            \n"
            " physics_constructors.names : string[2] = \"particles\" \"electromagnetic\" \n"
            "                                                                            \n");
  }

  {
    // Description of the 'physics_constructors.XXX.id' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("physics_constructors.${physics_constructors.names}.id")
        .set_terse_description("The class ID of a given physics constructor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example(
            "Registration of particles and electromagnetic processes: ::                 \n"
            "                                                                            \n"
            " physics_constructors.names : string[2] = \"particles\" \"electromagnetic\" \n"
            " physics_constructors.particles.id : string = \\                            \n"
            "    \"mctools::g4::particles_physics_constructor\"                          \n"
            " physics_constructors.electromagnetic.id : string = \\                      \n"
            "    \"mctools::g4::em_physics_constructor\"                                 \n"
            "                                                                            \n");
  }

  {
    // Description of the 'physics_constructors.XXX.config' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("physics_constructors.${physics_constructors.names}.config")
        .set_terse_description("The configuration file name of a given physics constructor")
        .set_traits(datatools::TYPE_STRING)
        .set_path(true)
        .set_mandatory(true)
        .add_example(
            "Specify the configuration files for some physics lists: ::                  \n"
            "                                                                            \n"
            " physics_constructors.names : string[2] = \"particles\" \"electromagnetic\" \n"
            " physics_constructors.particles.config : string as path = \\                \n"
            "    \"config/simulation/particles.conf\"                                    \n"
            " physics_constructors.electromagnetic.config : string as path = \\          \n"
            "    \"config/simulation/electromagnetic.conf\"                              \n"
            "                                                                            \n");
  }

  // ***** Production cuts ***** //

  {
    // Description of the 'using_production_cuts' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("using_production_cuts")
        .set_terse_description("Flag to activate production cuts")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate the production cuts mechanism: ::         \n"
            "                                                   \n"
            " using_production_cuts : boolean = 1               \n"
            "                                                   \n");
  }

  {
    // Description of the 'production_cuts.length_unit' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.length_unit")
        .set_terse_description("The production cuts length unit symbol")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .set_default_value_string("mm")
        .add_example(
            "Set the production cut default length unit: ::         \n"
            "                                                       \n"
            " production_cuts.length_unit : string = \"mm\"         \n"
            "                                                       \n");
  }

  {
    // Description of the 'production_cuts.energy_unit' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.energy_unit")
        .set_terse_description("The production cuts energy unit symbol")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .set_default_value_string("MeV")
        .add_example(
            "Set the production cut default energy unit: ::          \n"
            "                                                        \n"
            " production_cuts.energy_unit : string = \"MeV\"         \n"
            "                                                        \n");
  }

  {
    // Description of the 'production_cuts.default_value' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.default_value")
        .set_terse_description("The production cuts default value")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut default value::                  \n"
            "                                                        \n"
            " production_cuts.default_value : real as length = 10 um \n"
            "                                                        \n");
  }

  {
    // Description of the 'production_cuts.low_energy' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.low_energy")
        .set_terse_description("The production cuts energy low edge")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut low energy bound::              \n"
            "                                                       \n"
            " production_cuts.low_energy : real as energy =  250 eV \n"
            "                                                       \n");
  }

  {
    // Description of the 'production_cuts.high_energy' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.high_energy")
        .set_terse_description("The production cuts energy high edge")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut high energy bound::             \n"
            "                                                       \n"
            " production_cuts.high_energy : real as energy =  1 GeV \n"
            "                                                       \n");
  }

  {
    // Description of the 'production_cuts.electron' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.electron")
        .set_terse_description("The cut value for electrons")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut for electrons::                   \n"
            "                                                         \n"
            " production_cuts.electron : real as length = 1 mm        \n"
            "                                                         \n");
  }

  {
    // Description of the 'production_cuts.positron' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.positron")
        .set_terse_description("The cut value for positrons")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut for positrons::                   \n"
            "                                                         \n"
            " production_cuts.positron : real as length = 1 mm        \n"
            "                                                         \n");
  }

  {
    // Description of the 'production_cuts.gamma' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.gamma")
        .set_terse_description("The cut value for gammas")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Set the production cut for gammas::                   \n"
            "                                                      \n"
            " production_cuts.gamma : real as length = 1 mm        \n"
            "                                                      \n");
  }

  {
    // Description of the 'production_cuts.regions' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.regions")
        .set_terse_description("The name of the regions with special production cuts")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Define regions where to apply production cuts: ::      \n"
            "                                                       \n"
            " production_cuts.regions : string[2] = \"A\" \"B\"     \n"
            "                                                       \n");
  }

  {
    // Description of the 'production_cuts.regions.XXX.all' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.regions.${production_cuts.regions}.all")
        .set_terse_description("The cut value applied to all particles within a given region")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Define production cuts for all particles in specific regions: :: \n"
            "                                                         \n"
            " production_cuts.regions : string[2] = \"A\" \"B\"       \n"
            " production_cuts.regions.A.all : real as length = 5 mm   \n"
            " production_cuts.regions.B.all : real as length = 1 mm   \n"
            "                                                         \n");
  }

  {
    // Description of the 'production_cuts.regions.XXX.gamma' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.regions.${production_cuts.regions}.gamma")
        .set_terse_description("The cut value applied to gamma within a given region")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Define production cuts for gammas in specific regions: :: \n"
            "                                                         \n"
            " production_cuts.regions : string[2] = \"A\" \"B\"       \n"
            " production_cuts.regions.A.gamma : real as length = 5 mm \n"
            " production_cuts.regions.B.gamma : real as length = 1 mm \n"
            "                                                         \n");
  }

  {
    // Description of the 'production_cuts.regions.XXX.electron' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.regions.${production_cuts.regions}.electron")
        .set_terse_description("The cut value applied to electron within a given region")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Define production cuts for electrons in specific regions: :: \n"
            "                                                            \n"
            " production_cuts.regions : string[2] = \"A\" \"B\"          \n"
            " production_cuts.regions.A.electron : real as length = 5 mm \n"
            " production_cuts.regions.B.electron : real as length = 1 mm \n"
            "                                                            \n");
  }

  {
    // Description of the 'production_cuts.regions.XXX.positron' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("production_cuts.regions.${production_cuts.regions}.positron")
        .set_terse_description("The cut value applied to positron within a given region")
        .set_traits(datatools::TYPE_REAL)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_mandatory(false)
        .set_triggered_by_flag("using_production_cuts", true)
        .add_example(
            "Define production cuts for positrons in specific regions: :: \n"
            "                                                            \n"
            " production_cuts.regions : string[2] = \"A\" \"B\"          \n"
            " production_cuts.regions.A.positron : real as length = 5 mm \n"
            " production_cuts.regions.B.positron : real as length = 1 mm \n"
            "                                                            \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Example using official Geant4 physics list:        \n"
      "                                                   \n"
      "  logging.priority    : string = \"fatal\"         \n"
      "  geant4.physics_list : string = \"FTFP_BERT_EMV\" \n"
      "  using_production_cuts : boolean = true           \n"
      "  production_cuts.default_value : real = 1 mm      \n"
      "                                                   \n"
      "                                                   \n"
      "Example using user physics constructors:           \n"
      "                                                   \n"
      "  logging.priority    : string = \"fatal\"         \n"
      "  physics_constructors.names : string[2] = \"particles\" \"em\" \n"
      "  physics_constructors.particles.id     : string = "
      "\"mctools::g4::particles_physics_constructor\" \n"
      "  physics_constructors.particles.config : string = \"particles.def\" \n"
      "  physics_constructors.em.id            : string = \"mctools::g4::em_physics_constructor\" "
      "\n"
      "  physics_constructors.em.config        : string = \"em.def\" \n"

      "  using_production_cuts : boolean = true           \n"
      "  production_cuts.default_value : real = 1 mm      \n"
      "                                                   \n"
      "                                                   \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'mctools::g4::physics_list' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::physics_list, "mctools::g4::physics_list")
