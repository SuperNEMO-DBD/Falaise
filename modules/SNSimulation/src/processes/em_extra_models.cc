// em_extra_models.cc

// Ourselves:
#include <snsim/processes/em_extra_models.h>

// Third party:
// Geant4:
#include <G4EmConfigurator.hh>
#include <G4ParticleTable.hh>
#include <G4Region.hh>
#include <G4RegionStore.hh>
#include <G4VEmFluctuationModel.hh>
#include <G4VEmModel.hh>

// Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// This project:
#include <snsim/processes/em_model_factory.h>

namespace snsim {

namespace processes {

em_extra_model::em_extra_model() {
  _initialized_ = false;
  _set_defaults();
}

em_extra_model::~em_extra_model() {
  if (is_initialized()) {
    reset();
  }
}

auto em_extra_model::is_initialized() const -> bool { return _initialized_; }

void em_extra_model::initialize() {
  datatools::properties dummy;
  initialize(dummy);
}

void em_extra_model::initialize(const datatools::properties& config_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");

  this->datatools::enriched_base::initialize(config_);
  DT_LOG_DEBUG(get_logging_priority(), "EM extra model name : '" << get_name() << "'");

  if (_particle_name_.empty()) {
    if (config_.has_key("particle_name")) {
      std::string particle_name = config_.fetch_string("particle_name");
      set_particle_name(particle_name);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(),
               "EM extra model particle name : '" << _particle_name_ << "'");

  if (_process_name_.empty()) {
    if (config_.has_key("process_name")) {
      std::string process_name = config_.fetch_string("process_name");
      set_process_name(process_name);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), "EM extra model process name : '" << _process_name_ << "'");

  if (_model_type_id_.empty()) {
    if (config_.has_key("model_type_id")) {
      std::string model_type_id = config_.fetch_string("model_type_id");
      set_model_type_id(model_type_id);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), "EM extra model type id : '" << _model_type_id_ << "'");

  if (_model_name_.empty()) {
    if (config_.has_key("model_name")) {
      std::string model_name = config_.fetch_string("model_name");
      set_model_name(model_name);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), "EM extra model name : '" << _model_name_ << "'");

  if (_region_name_.empty()) {
    if (config_.has_key("region_name")) {
      std::string region_name = config_.fetch_string("region_name");
      set_region_name(region_name);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), "EM extra model region name : '" << _region_name_ << "'");

  double default_energy_unit = CLHEP::MeV;
  DT_LOG_DEBUG(get_logging_priority(),
               "Default energy unit is : " << default_energy_unit / CLHEP::MeV << " MeV");

  if (!datatools::is_valid(_energy_min_)) {
    if (config_.has_key("energy_min")) {
      double emin = config_.fetch_real("energy_min");
      if (!config_.has_explicit_unit("energy_min")) {
        emin *= default_energy_unit;
      }
      set_energy_min(emin);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(),
               "EM extra model energy min : " << _energy_min_ / CLHEP::MeV << " [MeV]");

  if (!datatools::is_valid(_energy_max_)) {
    if (config_.has_key("energy_max")) {
      double emax = config_.fetch_real("energy_max");
      if (!config_.has_explicit_unit("energy_max")) {
        emax *= default_energy_unit;
      }
      set_energy_max(emax);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(),
               "EM extra model energy max : " << _energy_max_ / CLHEP::MeV << " [MeV]");

  DT_THROW_IF(
      _model_type_id_.empty(), std::logic_error,
      "Missing electromagnetic model type identifier for extra model '" << get_name() << "'!");
  /*
  if (! datatools::is_valid(_energy_min_)) {
    set_energy_min(0.0);
  }
  */

  DT_LOG_DEBUG(get_logging_priority(), "Extra model parameters: ");
  if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
    this->tree_dump(std::clog, "", "[debug]: ");
  }

  _initialized_ = true;
}

void em_extra_model::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized!");
  _initialized_ = false;
  _particle_name_.clear();
  _process_name_.clear();
  _model_type_id_.clear();
  _model_name_.clear();
  _region_name_.clear();
  _set_defaults();
}

void em_extra_model::_set_defaults() {
  _energy_min_ = datatools::invalid_real();
  _energy_max_ = datatools::invalid_real();
}

// static
auto em_extra_model::validate_g4_model_type_id(std::string& model_type_id_) -> bool {
  std::string mtid = model_type_id_;
  bool valid = false;
  if (mtid.empty()) {
    mtid = "Dummy";
    valid = true;
  } else {
    if (em_model_factory::get_instance().has_model_type(model_type_id_)) {
      valid = true;
    } else {
      // DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
      //                "Geant4 EM model factory has no model type with identifier '"
      //                << model_type_id_ << "'!");
    }
  }
  if (valid) {
    model_type_id_ = mtid;
  }
  return valid;
}

// static
auto em_extra_model::validate_g4_region_name(std::string& region_name_) -> bool {
  std::string r = region_name_;
  bool valid = false;
  if (r.empty() || r == "world" || r == "World") {
    r = "DefaultRegionForTheWorld";
    valid = true;
  } else {
    G4RegionStore* region_store = G4RegionStore::GetInstance();
    if (region_store->empty()) {
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Geant4 region store is empty!");
    } else {
      G4Region* region = region_store->GetRegion(r, true);
      if (region != nullptr) {
        valid = true;
      }
    }
  }
  if (valid) {
    region_name_ = r;
  }
  return valid;
}

// static
auto em_extra_model::validate_g4_particle_name(std::string& particle_name_) -> bool {
  std::string n = particle_name_;
  bool valid = false;
  if (particle_name_.empty() || particle_name_ == "*" || particle_name_ == "all" ||
      particle_name_ == "ALL") {
    n = "all";
    valid = true;
  } else if (particle_name_ == "charged") {
    valid = true;
  } else {
    valid = true;
    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    if (particle_table->size() == 0) {
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                     "Geant4 particle table is not initialized yet!");
    } else {
      // for (int i = 0; i < (int) particle_table->size(); i++) {
      //   DT_LOG_DEBUG(get_logging_priority(), "Tabulated particle [#" << i << "] = '"
      //                << particle_table->GetParticleName(i) << "'");
      // }
      if (!particle_table->contains(n)) {
        valid = false;
      }
    }
  }
  if (valid) {
    particle_name_ = n;
  }
  return valid;
}

void em_extra_model::set_particle_name(const std::string& name_, bool check_) {
  std::string n = name_;
  if (check_) {
    DT_THROW_IF(!validate_g4_particle_name(n), std::logic_error,
                "Geant4 particle name '" << n << "' is not valid!");
  }
  _particle_name_ = n;
}

auto em_extra_model::get_particle_name() const -> const std::string& { return _particle_name_; }

void em_extra_model::set_process_name(const std::string& name_) { _process_name_ = name_; }

auto em_extra_model::get_process_name() const -> const std::string& { return _process_name_; }

void em_extra_model::set_model_type_id(const std::string& model_type_id_) {
  _model_type_id_ = model_type_id_;
}

auto em_extra_model::get_model_type_id() const -> const std::string& { return _model_type_id_; }

void em_extra_model::set_model_name(const std::string& name_) { _model_name_ = name_; }

auto em_extra_model::get_model_name() const -> const std::string& { return _model_name_; }

void em_extra_model::set_region_name(const std::string& region_name_, bool check_) {
  std::string r = region_name_;
  if (check_) {
    DT_THROW_IF(!validate_g4_region_name(r), std::logic_error,
                "Geant4 region name '" << r << "' is not valid!");
  }
  _region_name_ = r;
}

auto em_extra_model::get_region_name() const -> const std::string& { return _region_name_; }

void em_extra_model::set_energy_min(double e_) {
  DT_THROW_IF(e_ < 0.0, std::domain_error, "Invalid minimum energy!");
  _energy_min_ = e_;
}

auto em_extra_model::get_energy_min() const -> double { return _energy_min_; }

void em_extra_model::set_energy_max(double e_) {
  DT_THROW_IF(e_ < 0.0, std::domain_error, "Invalid maximum energy!");
  _energy_max_ = e_;
}

auto em_extra_model::get_energy_max() const -> double { return _energy_max_; }

void em_extra_model::tree_dump(std::ostream& out_, const std::string& title_,
                               const std::string& indent_, bool inherit_) const {
  this->datatools::enriched_base::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Particle          : '" << _particle_name_
       << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Process           : '" << _process_name_
       << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Model type identifier : '"
       << _model_type_id_ << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Model             : '" << _model_name_
       << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Region            : '" << _region_name_
       << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Minimal energy    : " << _energy_min_ / CLHEP::MeV << " [MeV]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Maximal energy    : " << _energy_max_ / CLHEP::MeV << " [MeV]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Initialized       : " << (is_initialized() ? "Yes" : "No") << std::endl;
}

void em_extra_model::apply_to_g4(G4EmConfigurator& em_config_,
                                 em_model_factory& em_mod_fact_) const {
  // Particle:
  std::string part_name;
  if (!_particle_name_.empty()) {
    part_name = _particle_name_;
  }
  DT_THROW_IF(!validate_g4_particle_name(part_name), std::logic_error,
              "Geant4 particle name '" << part_name << "' is not valid!");
  DT_LOG_NOTICE(get_logging_priority(), "Particle name : '" << part_name << "'");

  // Process:
  std::string process_name;
  if (!_process_name_.empty()) {
    process_name = _process_name_;
  }
  DT_LOG_NOTICE(get_logging_priority(), "Process name : '" << process_name << "'");

  // Model type:
  std::string model_type_id;
  if (!_model_type_id_.empty()) {
    model_type_id = _model_type_id_;
  }
  DT_THROW_IF(!validate_g4_model_type_id(model_type_id), std::logic_error,
              "Geant4 model type identifier '" << model_type_id << "' is not valid!");
  DT_LOG_NOTICE(get_logging_priority(), "Model type identifier : '" << model_type_id << "'");

  // Model:
  std::string model_name;
  if (!_model_name_.empty()) {
    model_name = _model_name_;
  }
  DT_LOG_NOTICE(get_logging_priority(), "Model name : '" << model_name << "'");

  // Region:
  std::string region_name;
  if (!_region_name_.empty()) {
    region_name = _region_name_;
  }
  DT_THROW_IF(!validate_g4_region_name(region_name), std::logic_error,
              "Geant4 region name '" << region_name << "' is not valid!");
  DT_LOG_NOTICE(get_logging_priority(), "Region name : '" << region_name << "'");

  // Energy range:
  double emin = 0.0;
  double emax = DBL_MAX;
  if (datatools::is_valid(_energy_min_)) {
    emin = _energy_min_;
  }
  if (datatools::is_valid(_energy_max_)) {
    emax = _energy_max_;
    DT_THROW_IF(emax <= emin, std::range_error, "Invalid energy range!");
  }
  DT_LOG_NOTICE(get_logging_priority(), "Energy min : " << emin / CLHEP::MeV << " [MeV]");
  DT_LOG_NOTICE(get_logging_priority(), "Energy max : " << emax / CLHEP::MeV << " [MeV]");

  G4VEmModel* model = nullptr;
  G4VEmFluctuationModel* fluct_model = nullptr;

  DT_LOG_DEBUG(get_logging_priority(), "Allocating EM extra model...");
  model = em_mod_fact_.create_model(model_type_id, model_name);

  em_config_.SetExtraEmModel(part_name, process_name, model, region_name, emin, emax, fluct_model);
}

}  // end of namespace processes

}  // namespace snsim
