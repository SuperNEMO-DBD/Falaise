// em_field_equation_of_motion.cc

// Ourselves:
#include <snsim/em_field_equation_of_motion.h>

// Third party:
// - CLHEP:
#include <CLHEP/Vector/ThreeVector.h>
// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>
// - Geant4:
#include <G4PhysicalConstants.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>

// This project:
#include <snsim/em_field_g4_stuff.h>
#include <snsim/em_field_g4_utils.h>

namespace snsim {

void em_field_equation_of_motion::_set_defaults() {
  _with_spin_ = false;
  _only_magnetic_ = false;

  _mass_ = datatools::invalid_real();
  _charge_ = datatools::invalid_real();
  _anomaly_ = 0.0;
  _momentum_ = datatools::invalid_real();

  _charge_coef_ = datatools::invalid_real();
  _mass_sqr_ = datatools::invalid_real();
  _omegac_ = datatools::invalid_real();
  _energy_ = datatools::invalid_real();
  _gamma_ = datatools::invalid_real();
  _beta_ = datatools::invalid_real();
}

/*
void em_field_equation_of_motion::initialize(const datatools::properties & config_)
{
  return;
}
*/

/*
void em_field_equation_of_motion::reset()
{
  _set_defaults();
  return;
}
*/

em_field_equation_of_motion::em_field_equation_of_motion(G4Field* em_field_)
    : G4EquationOfMotion(em_field_) {
  // std::cerr << "DEVEL: " << "snsim::em_field_equation_of_motion::CTOR: "
  //           << "Entering..."
  //           << std::endl;
  _set_defaults();
}

em_field_equation_of_motion::~em_field_equation_of_motion() = default;

void em_field_equation_of_motion::set_anomaly(double a_) { _anomaly_ = a_; }

auto em_field_equation_of_motion::get_anomaly() const -> double { return _anomaly_; }

void em_field_equation_of_motion::set_with_spin(bool ws_) { _with_spin_ = ws_; }

void em_field_equation_of_motion::set_only_magnetic(bool om_) { _only_magnetic_ = om_; }

auto em_field_equation_of_motion::is_only_magnetic() const -> bool { return _only_magnetic_; }

auto em_field_equation_of_motion::is_with_spin() const -> bool { return _with_spin_; }

#if G4VERSION_NUMBER >= 1000
void em_field_equation_of_motion::SetChargeMomentumMass(G4ChargeState particle_charge_state_,
                                                        G4double particle_momentum_,
                                                        G4double particle_mass_) {
  DT_LOG_TRACE_ENTERING(_logprio());
  SetChargeMomentumMass(particle_charge_state_.GetCharge(), particle_momentum_, particle_mass_);
  DT_LOG_TRACE_EXITING(_logprio());
  return;
}
#endif

void em_field_equation_of_motion::SetChargeMomentumMass(G4double particle_charge_,  // in e+ units
                                                        G4double particle_momentum_,
                                                        G4double particle_mass_) {
  DT_LOG_TRACE_ENTERING(_logprio());

  DT_LOG_DEBUG(_logprio(), " -> Particle charge = " << particle_charge_ / CLHEP::eplus << "e ");
  DT_LOG_DEBUG(_logprio(), " -> Momentum = " << particle_momentum_ / CLHEP::MeV << " MeV");
  DT_LOG_DEBUG(_logprio(), " -> Mass = " << particle_mass_ / CLHEP::MeV << " MeV");

  _mass_ = particle_mass_;
  _charge_ = particle_charge_;
  _momentum_ = particle_momentum_;

  _charge_coef_ = CLHEP::eplus * _charge_ * CLHEP::c_light;
  _mass_sqr_ = _mass_ * _mass_;
  _omegac_ = (CLHEP::eplus / _mass_) * CLHEP::c_light;
  _energy_ = std::sqrt(_momentum_ * _momentum_ + _mass_sqr_);
  _beta_ = _momentum_ / _energy_;
  _gamma_ = _energy_ / _mass_;

  // G4Track * current_track = 0; // XXX
  // std::string pname = current_track->GetDefinition()->GetParticleName();
  // int total_occup = current_track->GetDynamicParticle()->GetTotalOccupancy();
  // static const double neutron_magnetic_anomaly = -2.9156797;
  // if (_charge_ == 0.) SetAnomaly(neutron_magnetic_anomaly);

  DT_LOG_TRACE_EXITING(_logprio());
}

void em_field_equation_of_motion::EvaluateRhsGivenB(const G4double y_[], const G4double field_[6],
                                                    G4double dydx_[]) const {
  DT_LOG_TRACE_ENTERING(_logprio());

  double p2 = y_[PX] * y_[PX] + y_[PY] * y_[PY] + y_[PZ] * y_[PZ];
  double energy = std::sqrt(p2 + _mass_sqr_);
  double coef2 = energy / CLHEP::c_light;
  if (is_only_magnetic()) {
    coef2 = 0.0;
  }
  double inverse_p = 1.0 / std::sqrt(p2);
  double inverse_velocity = energy * inverse_p / CLHEP::c_light;
  double coef1 = _charge_coef_ * inverse_p;

  dydx_[RX] = y_[PX] * inverse_p;
  dydx_[RY] = y_[PY] * inverse_p;
  dydx_[RZ] = y_[PZ] * inverse_p;
  dydx_[PX] = 0.0;
  dydx_[PY] = 0.0;
  dydx_[PZ] = 0.0;
  dydx_[UNDEF_6] = 0.0;
  dydx_[GLOBAL_TIME] = inverse_velocity;
  dydx_[UNDEF_8] = 0.0;
  dydx_[SPINX] = 0.0;
  dydx_[SPINY] = 0.0;
  dydx_[SPINZ] = 0.0;

  if (_charge_ != 0.0) {
    dydx_[PX] = coef1 * (y_[PY] * field_[EMFIELD_BZ] - y_[PZ] * field_[EMFIELD_BY]);
    dydx_[PY] = coef1 * (y_[PZ] * field_[EMFIELD_BX] - y_[PX] * field_[EMFIELD_BZ]);
    dydx_[PZ] = coef1 * (y_[PX] * field_[EMFIELD_BY] - y_[PY] * field_[EMFIELD_BX]);
    if (!_only_magnetic_) {
      double c1c2 = coef1 * coef2;
      dydx_[PX] += c1c2 * field_[EMFIELD_EX];
      dydx_[PY] += c1c2 * field_[EMFIELD_EY];
      dydx_[PZ] += c1c2 * field_[EMFIELD_EZ];
    }
  }

  G4ThreeVector Bfield;
  Bfield.set(field_[EMFIELD_BX], field_[EMFIELD_BY], field_[EMFIELD_BZ]);
  G4ThreeVector Efield;
  if (!is_only_magnetic()) {
    Efield.set(field_[EMFIELD_EX], field_[EMFIELD_EY], field_[EMFIELD_EZ]);
    Efield /= CLHEP::c_light;
  }

  if (is_with_spin()) {
    // The BMT equation, following J.D.Jackson, Classical Electrodynamics, Second Edition,
    // dS/dt = (e/mc) S \cross
    //              [ (g/2-1 +1/\gamma) B
    //               -(g/2-1)\gamma/(\gamma+1) (\beta \cdot B)\beta
    //               -(g/2-\gamma/(\gamma+1) \beta \cross E ]
    // where
    // S = \vec{s}, where S^2 = 1
    // B = \vec{B}
    // \beta = \vec{\beta} = \beta \vec{u} with u^2 = 1
    // E = \vec{E}
    G4ThreeVector u(y_[PX], y_[PY], y_[PZ]);
    u *= inverse_p;
    G4double udb = _anomaly_ * _beta_ * _gamma_ / (1. + _gamma_) * (Bfield * u);
    G4double ucb = (_anomaly_ + 1. / _gamma_) / _beta_;
    G4double uce = _anomaly_ + 1. / (_gamma_ + 1.);
    G4ThreeVector Spin(y_[SPINX], y_[SPINY], y_[SPINZ]);
    G4ThreeVector dSpin;
    dSpin = _charge_ * _omegac_ *
            (ucb * (Spin.cross(Bfield)) -
             udb * (Spin.cross(u))
             // from Jackson
             // -uce*Spin.cross(u.cross(EField)) );
             // but this form has one less operation
             - uce * (u * (Spin * Efield) - Efield * (Spin * u)));
    dydx_[SPINX] = dSpin.x();
    dydx_[SPINY] = dSpin.y();
    dydx_[SPINZ] = dSpin.z();
  }

  DT_LOG_TRACE_EXITING(_logprio());
}

}  // namespace snsim
