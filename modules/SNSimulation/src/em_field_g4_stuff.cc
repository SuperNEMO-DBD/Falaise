// em_field_g4_stuff.cc

// Ourselves:
#include <snsim/em_field_g4_stuff.h>

// Third party:
// - Geant4:
#include <G4FieldManager.hh>
#include <globals.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4PropagatorInField.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <G4ChordFinder.hh>
#include <G4MagIntegratorDriver.hh>
#include <G4MagIntegratorStepper.hh>
#include <G4TransportationManager.hh>

// Steppers:
#include <G4CashKarpRKF45.hh>
#include <G4ClassicalRK4.hh>
#include <G4ExactHelixStepper.hh>
#include <G4ExplicitEuler.hh>
#include <G4HelixExplicitEuler.hh>
#include <G4HelixHeum.hh>
#include <G4HelixImplicitEuler.hh>
#include <G4HelixMixedStepper.hh>
#include <G4HelixSimpleRunge.hh>
#include <G4ImplicitEuler.hh>
#include <G4SimpleHeum.hh>
#include <G4SimpleRunge.hh>
// Equations:
#include <G4EqEMFieldWithSpin.hh>
#include <G4EqMagElectricField.hh>
#include <G4Mag_SpinEqRhs.hh>
#include <G4Mag_UsualEqRhs.hh>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/units.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <snsim/electromagnetic_field.h>
#include <snsim/em_field_equation_of_motion.h>
#include <snsim/magnetic_field.h>

namespace snsim {

// static
auto em_field_g4_stuff::label_from_stepper(stepper_type s_) -> std::string {
  switch (s_) {
    case STEPPER_CASHKARP_RKF45:
      return "CashKarpRKF45";
    case STEPPER_CLASSICAL_RK4:
      return "ClassicalRK4";
    case STEPPER_EXPLICIT_EULER:
      return "ExplicitEuler";
    case STEPPER_IMPLICIT_EULER:
      return "ImplicitEuler";
    case STEPPER_SIMPLE_HEUM:
      return "SimpleHeum";
    case STEPPER_SIMPLE_RUNGE:
      return "SimpleRunge";
    case STEPPER_EXACT_HELIX:
      return "ExactHelix";
    case STEPPER_HELIX_EXPLICIT_EULER:
      return "HelixExplicitEuler";
    case STEPPER_HELIX_HEUM:
      return "HelixHeumEuler";
    case STEPPER_HELIX_IMPLICIT_EULER:
      return "HelixImplicitEuler";
    case STEPPER_HELIX_MIXED_EULER:
      return "HelixMixedEuler";
    case STEPPER_HELIX_SIMPLE_RUNGE:
      return "HelixSimpleRunge";
    default:
      return "";
  }
  return "";
}

// static
auto em_field_g4_stuff::stepper_from_label(const std::string& label_)
    -> em_field_g4_stuff::stepper_type {
  if (label_ == label_from_stepper(STEPPER_CASHKARP_RKF45)) {
    return STEPPER_CASHKARP_RKF45;
  }
  if (label_ == label_from_stepper(STEPPER_CLASSICAL_RK4)) {
    return STEPPER_CLASSICAL_RK4;
  }
  if (label_ == label_from_stepper(STEPPER_EXPLICIT_EULER)) {
    return STEPPER_EXPLICIT_EULER;
  }
  if (label_ == label_from_stepper(STEPPER_IMPLICIT_EULER)) {
    return STEPPER_IMPLICIT_EULER;
  } else if (label_ == label_from_stepper(STEPPER_SIMPLE_HEUM)) {
    return STEPPER_SIMPLE_HEUM;
  } else if (label_ == label_from_stepper(STEPPER_SIMPLE_RUNGE)) {
    return STEPPER_SIMPLE_RUNGE;
  } else if (label_ == label_from_stepper(STEPPER_EXACT_HELIX)) {
    return STEPPER_EXACT_HELIX;
  } else if (label_ == label_from_stepper(STEPPER_HELIX_EXPLICIT_EULER)) {
    return STEPPER_HELIX_EXPLICIT_EULER;
  } else if (label_ == label_from_stepper(STEPPER_HELIX_HEUM)) {
    return STEPPER_HELIX_HEUM;
  } else if (label_ == label_from_stepper(STEPPER_HELIX_IMPLICIT_EULER)) {
    return STEPPER_HELIX_IMPLICIT_EULER;
  } else if (label_ == label_from_stepper(STEPPER_HELIX_MIXED_EULER)) {
    return STEPPER_HELIX_MIXED_EULER;
  } else if (label_ == label_from_stepper(STEPPER_HELIX_SIMPLE_RUNGE)) {
    return STEPPER_HELIX_SIMPLE_RUNGE;
  }
  return STEPPER_INVALID;
}

void em_field_g4_stuff::_set_defaults() {
  _stepper_type_ = STEPPER_INVALID;
  datatools::invalidate(_min_step_);
  datatools::invalidate(_delta_chord_);
  datatools::invalidate(_delta_one_step_);
  datatools::invalidate(_delta_intersection_);
  datatools::invalidate(_eps_min_);
  datatools::invalidate(_eps_max_);
  _spin_ = false;
  _propagate_to_daughters_ = true;
}

void em_field_g4_stuff::set_g4_magnetic_field(magnetic_field* bfield_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Geant4 EM field stuff is already initialized !");
  DT_THROW_IF(_eb_field_ != nullptr, std::logic_error,
              "General electromagnetic field is already set!");
  _b_field_ = bfield_;
}

void em_field_g4_stuff::set_g4_electromagnetic_field(electromagnetic_field* ebfield_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Geant4 EM field stuff is already initialized !");
  DT_THROW_IF(_b_field_ != nullptr, std::logic_error, "Pure magnetic field is already set!");
  _eb_field_ = ebfield_;
}

auto em_field_g4_stuff::has_g4_field() const -> bool {
  return _b_field_ != nullptr || _eb_field_ != nullptr;
}

auto em_field_g4_stuff::is_pure_magnetic_g4_field() const -> bool { return _b_field_ != nullptr; }

auto em_field_g4_stuff::is_general_electromagnetic_g4_field() const -> bool {
  return _eb_field_ != nullptr;
}

auto em_field_g4_stuff::get_g4_field_name() const -> std::string {
  DT_THROW_IF(!has_g4_field(), std::logic_error, "No G4 EM field is set!");
  if (_b_field_ != nullptr) {
    return _b_field_->get_name();
  }
  if (_eb_field_ != nullptr) {
    return _eb_field_->get_name();
  }
  return "";
}

em_field_g4_stuff::em_field_g4_stuff() {
  _initialized_ = false;
  _set_defaults();
  _b_field_ = nullptr;
  _eb_field_ = nullptr;
  _equation_ = nullptr;
  _field_manager_ = nullptr;
  _field_propagator_ = nullptr;
  _field_stepper_ = nullptr;
  _chord_finder_ = nullptr;
  _integration_driver_ = nullptr;
}

em_field_g4_stuff::~em_field_g4_stuff() {
  if (is_initialized()) {
    reset();
  }
}

auto em_field_g4_stuff::is_initialized() const -> bool { return _initialized_; }

void em_field_g4_stuff::initialize() {
  datatools::properties empty;
  initialize(empty);
}

void em_field_g4_stuff::initialize(const datatools::properties& config_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Geant4 EM field stuff is already initialized !");

  loggable_support::_initialize_logging_support(config_);

  DT_THROW_IF(_b_field_ == nullptr && _eb_field_ == nullptr, std::logic_error,
              "No G4 EM field was defined!");

  double default_length_unit = 1.0 * CLHEP::mm;
  bool force_explicit_parameters = false;

  if (config_.has_key("force_explicit_parameters")) {
    force_explicit_parameters = config_.fetch_boolean("force_explicit_parameters");
  }

  if (config_.has_key("length_unit")) {
    std::string lu = config_.fetch_string("length_unit");
    default_length_unit = datatools::units::get_length_unit_from(lu);
  }

  // Stepper:
  if (config_.has_key("stepper")) {
    const std::string& stepper_label = config_.fetch_string("stepper");
    stepper_type st = stepper_from_label(stepper_label);
    DT_THROW_IF(st == STEPPER_INVALID, std::logic_error,
                "Invalid stepper name '" << stepper_label << "' for field labelled '"
                                         << get_g4_field_name() << "'!");
  }
  DT_THROW_IF(force_explicit_parameters && _stepper_type_ == STEPPER_INVALID, std::logic_error,
              "Invalid stepper!");
  if (_stepper_type_ == STEPPER_INVALID) {
    DT_LOG_WARNING(
        datatools::logger::PRIO_ALWAYS,
        "Using default EM field stepper for field labelled '" << get_g4_field_name() << "'!");
    _stepper_type_ = STEPPER_CLASSICAL_RK4;
  }

  if (config_.has_key("min_step")) {
    _min_step_ = config_.fetch_real("min_step");
    if (!config_.has_explicit_unit("min_step")) {
      _min_step_ *= default_length_unit;
    }
  }
  DT_THROW_IF(force_explicit_parameters && (!datatools::is_valid(_min_step_) || _min_step_ <= 0.0),
              std::logic_error,
              "Invalid min step value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("delta_chord")) {
    _delta_chord_ = config_.fetch_real("delta_chord");
    if (!config_.has_explicit_unit("delta_chord")) {
      _delta_chord_ *= default_length_unit;
    }
  }
  DT_THROW_IF(
      force_explicit_parameters && (!datatools::is_valid(_delta_chord_) || _delta_chord_ <= 0.0),
      std::logic_error,
      "Invalid delta chord value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("delta_one_step")) {
    _delta_one_step_ = config_.fetch_real("delta_one_step");
    if (!config_.has_explicit_unit("delta_one_step")) {
      _delta_one_step_ *= default_length_unit;
    }
  }
  DT_THROW_IF(force_explicit_parameters &&
                  (!datatools::is_valid(_delta_one_step_) || _delta_one_step_ <= 0.0),
              std::logic_error,
              "Invalid delta one step value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("delta_intersection")) {
    _delta_intersection_ = config_.fetch_real("delta_intersection");
    if (!config_.has_explicit_unit("delta_intersection")) {
      _delta_intersection_ *= default_length_unit;
    }
  }
  DT_THROW_IF(
      force_explicit_parameters &&
          (!datatools::is_valid(_delta_intersection_) || _delta_intersection_ <= 0.0),
      std::logic_error,
      "Invalid delta intersection value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("eps_min")) {
    _eps_min_ = config_.fetch_dimensionless_real("eps_min");
  }
  DT_THROW_IF(force_explicit_parameters && (!datatools::is_valid(_eps_min_) || _eps_min_ <= 0.0),
              std::logic_error,
              "Invalid eps min value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("eps_max")) {
    _eps_max_ = config_.fetch_dimensionless_real("eps_max");
  }
  DT_THROW_IF(force_explicit_parameters && (!datatools::is_valid(_eps_max_) || _eps_max_ <= 0.0),
              std::logic_error,
              "Invalid eps max value for field labelled '" << get_g4_field_name() << "'!");

  if (config_.has_key("spin")) {
    set_spin(config_.fetch_boolean("spin"));
  }
  if (is_spin()) {
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                   "Field labelled '" << get_g4_field_name() << "' takes into accounts spin!");
  }

  if (config_.has_key("propagate_to_daughters")) {
    set_propagate_to_daughters(config_.fetch_boolean("propagate_to_daughters"));
  }
  if (is_propagate_to_daughters()) {
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Field labelled '"
                                                       << get_g4_field_name()
                                                       << "' is propagated to daughter volumes!");
  }

  _at_init();

  _initialized_ = true;
}

void em_field_g4_stuff::_at_init() {
  DT_LOG_TRACE(_logprio(), "Entering...");

  bool with_spin = is_spin();
  if (_eb_field_ != nullptr) {
    auto* eqom = new em_field_equation_of_motion(_eb_field_);
    eqom->set_logging_priority(_logprio());
    eqom->set_with_spin(with_spin);
    eqom->set_only_magnetic(false);
    _equation_ = eqom;
    /*
    // General EM field:
    if (with_spin) {
      DT_LOG_DEBUG(_logprio(), "Using 'G4EqEMFieldWithSpin' equation of motion...");
      _equation_ = new G4EqEMFieldWithSpin(_eb_field_);
    } else {
      DT_LOG_DEBUG(_logprio(), "Using 'G4EqMagElectricField' equation of motion...");
      _equation_ = new G4EqMagElectricField(_eb_field_);
    }
    */
  } else if (_b_field_ != nullptr) {
    auto* eqom = new em_field_equation_of_motion(_b_field_);
    eqom->set_logging_priority(_logprio());
    eqom->set_with_spin(with_spin);
    eqom->set_only_magnetic(true);
    _equation_ = eqom;
    /*
    // Pure magnetic field:
    if (with_spin) {
      DT_LOG_DEBUG(_logprio(), "Using 'G4Mag_SpinEqRhs' equation of motion...");
      _equation_ = new G4Mag_SpinEqRhs(_b_field_);
    } else {
      DT_LOG_DEBUG(_logprio(), "Using 'G4Mag_UsualEqRhs' equation of motion...");
      _equation_ = new G4Mag_UsualEqRhs(_b_field_);
    }
    */
  }
  DT_LOG_DEBUG(_logprio(), "Equation of motion is set at [@" << _equation_ << "]");

  bool allocate_stepper = true;
  if (allocate_stepper) {
    int ndata = 12;
    switch (_stepper_type_) {
      case STEPPER_CASHKARP_RKF45:
        _field_stepper_ = new G4CashKarpRKF45(_equation_, ndata);
        break;
      case STEPPER_CLASSICAL_RK4:
        _field_stepper_ = new G4ClassicalRK4(_equation_, ndata);
        break;
      case STEPPER_EXPLICIT_EULER:
        _field_stepper_ = new G4ExplicitEuler(_equation_, ndata);
        break;
      case STEPPER_IMPLICIT_EULER:
        _field_stepper_ = new G4ImplicitEuler(_equation_, ndata);
        break;
      case STEPPER_SIMPLE_HEUM:
        _field_stepper_ = new G4SimpleHeum(_equation_, ndata);
        break;
      case STEPPER_SIMPLE_RUNGE:
        _field_stepper_ = new G4SimpleRunge(_equation_, ndata);
        break;
      case STEPPER_EXACT_HELIX:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4ExactHelixStepper(dynamic_cast<G4Mag_EqRhs*>(_equation_));
        break;
      case STEPPER_HELIX_EXPLICIT_EULER:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4HelixExplicitEuler(dynamic_cast<G4Mag_EqRhs*>(_equation_));
        break;
      case STEPPER_HELIX_HEUM:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4HelixHeum(dynamic_cast<G4Mag_EqRhs*>(_equation_));
        break;
      case STEPPER_HELIX_IMPLICIT_EULER:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4HelixImplicitEuler(dynamic_cast<G4Mag_EqRhs*>(_equation_));
        break;
      case STEPPER_HELIX_MIXED_EULER:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4HelixMixedStepper(dynamic_cast<G4Mag_EqRhs*>(_equation_), ndata);
        break;
      case STEPPER_HELIX_SIMPLE_RUNGE:
        DT_THROW_IF(
            _eb_field_, std::logic_error,
            "Stepper [" << _stepper_type_ << "] cannot be used with a non-pure magnetic field!");
        _field_stepper_ = new G4HelixSimpleRunge(dynamic_cast<G4Mag_EqRhs*>(_equation_));
        break;
      default:
        DT_THROW(std::logic_error, "Invalid stepper!");
        break;
    }
  }

  // Set accuracy parameters:
  double delta_chord = 0.25 * CLHEP::mm;
  double min_step = 0.001 * CLHEP::mm;
  double delta_one_step = 0.01 * CLHEP::mm;
  double delta_intersection = 0.05 * CLHEP::mm;
  double eps_min = 5e-5;
  double eps_max = 0.001;

  if (datatools::is_valid(_min_step_)) {
    min_step = _min_step_;
  }
  DT_LOG_DEBUG(_logprio(),
               "Using integration driver minimum step = " << min_step / CLHEP::mm << " [mm]");

  int driver_stat_verbosity = 1;
  _integration_driver_ = new G4MagInt_Driver(
      min_step, _field_stepper_, _field_stepper_->GetNumberOfVariables(), driver_stat_verbosity);

  if (datatools::is_valid(_delta_chord_)) {
    delta_chord = _delta_chord_;
  }

  // Chord finder:
  DT_LOG_DEBUG(_logprio(), "Create chord finder for this integration driver..");
  _chord_finder_ = new G4ChordFinder(_integration_driver_);

  /*
  if (_b_field_) {
    DT_LOG_DEBUG(_logprio(), "Create chord finder for a pure B field..");
    _chord_finder_ = new G4ChordFinder(_b_field_,
                                       min_step,
                                       _field_stepper_);
  }
  if (_eb_field_) {
    DT_LOG_DEBUG(_logprio(), "Create chord finder for a general E-B field");
    _chord_finder_ = new G4ChordFinder((G4MagneticField *) _eb_field_,
                                       min_step,
                                       _field_stepper_);
  }
  */
  DT_THROW_IF(_chord_finder_ == nullptr, std::logic_error, "Missing chord finder!");
  _chord_finder_->SetDeltaChord(delta_chord);

  // Create a field manager ():
  _field_manager_ = new G4FieldManager((G4Field*)nullptr, (G4ChordFinder*)nullptr, false);
  DT_LOG_DEBUG(_logprio(), "Create a field manager");

  G4TransportationManager* transport_mgr = G4TransportationManager::GetTransportationManager();
  _field_propagator_ = transport_mgr->GetPropagatorInField();

  if (_eb_field_ != nullptr) {
    DT_LOG_DEBUG(_logprio(), "Set a general E-B field");
    _field_manager_->SetDetectorField(_eb_field_);
    _field_manager_->SetFieldChangesEnergy(true);
  } else if (_b_field_ != nullptr) {
    DT_LOG_DEBUG(_logprio(), "Set a pure B field");
    _field_manager_->SetDetectorField(_b_field_);
    _field_manager_->SetFieldChangesEnergy(false);
  }
  _field_manager_->SetChordFinder(_chord_finder_);

  if (datatools::is_valid(_delta_one_step_)) {
    delta_one_step = _delta_one_step_;
  }
  if (datatools::is_valid(_delta_intersection_)) {
    delta_intersection = _delta_intersection_;
  }

  // TODO: Check relative values of delta_one_step & delta_intersection...

  _field_manager_->SetAccuraciesWithDeltaOneStep(delta_one_step);
  _field_manager_->SetDeltaIntersection(delta_intersection);
  if (datatools::is_valid(_eps_max_)) {
    eps_max = _eps_max_;
  }
  if (datatools::is_valid(_eps_min_)) {
    eps_min = _eps_min_;
  }
  _field_manager_->SetMinimumEpsilonStep(eps_min);
  _field_manager_->SetMaximumEpsilonStep(eps_max);

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

void em_field_g4_stuff::reset() {
  DT_LOG_TRACE(_logprio(), "Entering...");
  DT_THROW_IF(!is_initialized(), std::logic_error, "Geant4 EM field internal is not initialized !");
  _initialized_ = false;

  // 1)
  if (_field_manager_ != nullptr) {
    DT_LOG_TRACE(_logprio(), "Deleting the field manager...");
    delete _field_manager_;
    DT_LOG_TRACE(_logprio(), "Deleting the field manager... done.");
    _field_manager_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the field manager... done.");
  }
  // 2)
  if (_chord_finder_ != nullptr) {
    DT_LOG_TRACE(_logprio(), "Deleting the chord finder...");
    delete _chord_finder_;
    DT_LOG_TRACE(_logprio(), "Deleting the chord finder... done.");
    _chord_finder_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the chord finder... done.");
  }
  // 3)
  if (_integration_driver_ != nullptr) {
    _integration_driver_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the integration driver... done.");
  }
  // 4)
  if (_field_stepper_ != nullptr) {
    DT_LOG_TRACE(_logprio(), "Deleting the field stepper...");
    delete _field_stepper_;
    DT_LOG_TRACE(_logprio(), "Deleting the field stepper... done.");
    _field_stepper_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the field stepper... done.");
  }
  // 5)
  if (_equation_ != nullptr) {
    // DT_LOG_TRACE(_logprio(), "Deleting the equation of motion...");
    // delete _equation_;
    // DT_LOG_TRACE(_logprio(), "Deleting the equation of motion... done.");
    _equation_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the equation of motion... done.");
  }
  // 6)
  if (_field_propagator_ != nullptr) {
    _field_propagator_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the field propagator... done.");
  }
  // 7)
  if (_b_field_ != nullptr) {
    _b_field_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the pure B field... done.");
  }
  if (_eb_field_ != nullptr) {
    _eb_field_ = nullptr;
    DT_LOG_TRACE(_logprio(), "Null the general EM field... done.");
  }

  _set_defaults();
  DT_LOG_TRACE(_logprio(), "Exiting.");
}

auto em_field_g4_stuff::get_stepper_type() const -> em_field_g4_stuff::stepper_type {
  return _stepper_type_;
}

void em_field_g4_stuff::set_stepper_type(stepper_type t_) { _stepper_type_ = t_; }

auto em_field_g4_stuff::get_min_step() const -> double { return _min_step_; }

void em_field_g4_stuff::set_min_step(double s_) { _min_step_ = s_; }

auto em_field_g4_stuff::is_spin() const -> bool { return _spin_; }

void em_field_g4_stuff::set_spin(bool spin_) { _spin_ = spin_; }

auto em_field_g4_stuff::is_propagate_to_daughters() const -> bool {
  return _propagate_to_daughters_;
}

void em_field_g4_stuff::set_propagate_to_daughters(bool ptd_) { _propagate_to_daughters_ = ptd_; }

auto em_field_g4_stuff::get_delta_chord() const -> double { return _delta_chord_; }

void em_field_g4_stuff::set_delta_chord(double c_) { _delta_chord_ = c_; }

auto em_field_g4_stuff::get_delta_one_step() const -> double { return _delta_one_step_; }

void em_field_g4_stuff::set_delta_one_step(double os_) { _delta_one_step_ = os_; }

auto em_field_g4_stuff::get_delta_intersection() const -> double { return _delta_intersection_; }

void em_field_g4_stuff::set_delta_intersection(double di_) { _delta_intersection_ = di_; }

auto em_field_g4_stuff::get_eps_min() const -> double { return _eps_min_; }

void em_field_g4_stuff::set_eps_min(double e_) { _eps_min_ = e_; }

auto em_field_g4_stuff::get_eps_max() const -> double { return _eps_max_; }

void em_field_g4_stuff::set_eps_max(double e_) { _eps_max_ = e_; }

auto em_field_g4_stuff::grab_field_manager() -> G4FieldManager* {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Geant4 EM field internal is not initialized !");
  return _field_manager_;
}

void em_field_g4_stuff::tree_dump(std::ostream& out_, const std::string& title_,
                                  const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Logging priority      : '"
       << datatools::logger::get_priority_label(_logprio()) << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Stepper type          : '"
       << label_from_stepper(_stepper_type_) << "'" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Minimal step          : " << _min_step_ / CLHEP::mm << " [mm]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Delta chord           : " << _delta_chord_ / CLHEP::mm << " [mm]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Delta one step        : " << _delta_one_step_ / CLHEP::mm << " [mm]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Delta intersection    : " << _delta_intersection_ / CLHEP::mm << " [mm]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Epsilon min           : " << _eps_min_
       << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Epsilon max           : " << _eps_max_
       << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Spin                  : " << _spin_
       << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Propagate to daughters : " << _propagate_to_daughters_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Equation of motion    : [@" << _equation_
       << "]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Field manager         : [@"
       << _field_manager_ << "]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Field propagator      : [@"
       << _field_propagator_ << "]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Field stepper         : [@"
       << _field_stepper_ << "]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Chord finder          : [@"
       << _chord_finder_ << "]" << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Initialized           : " << (is_initialized() ? "Yes" : "No") << "" << std::endl;
}

}  // namespace snsim
