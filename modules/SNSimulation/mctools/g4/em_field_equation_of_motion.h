/// \file mctools/g4/em_field_equation_of_motion.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-08-27
 * Last modified: 2015-09-01
 *
 * License:
 *
 * Description:
 *
 *   G4 electromagnetic field equation of motion
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_EM_FIELD_EQUATION_OF_MOTION_H
#define MCTOOLS_G4_EM_FIELD_EQUATION_OF_MOTION_H

// Third party:
// - Geant4:
#include <G4EquationOfMotion.hh>
#include <G4Version.hh>

// This project:
#include <mctools/g4/loggable_support.h>

class G4Field;

namespace mctools {

namespace g4 {

/// \brief EM field equation of motion
///
/// This class merges the computation of the equation of motion for (charged) particles
/// in an electromagnetic field. The field can be purely magnetic, or a mix
/// magnetic or electric field.
/// The code is inspired by the following official Geant4 classes:
///   * G4EqMagElectricField
///   * G4EqEMFieldWithSpin
///   * G4Mag_UsualEqRhs
///   * G4Mag_SpinEqRhs
/// and merges their functionalities into one signgle class (to much inheritance in Geant4!)
/// Support of spin/mag.field interaction still needs to be developped
/// particularly because it is needed to set the particle's magnetic anomaly
/// and today it is forced to be 0.
/// The problem here is that only the SetChargeMomentumMass method is available from the Geant4
/// library and it is not possible to set the specific anomaly  that depends on the current
/// tracked particle.
class em_field_equation_of_motion : public G4EquationOfMotion, public loggable_support {
 public:
  /// \brief Index of the tracked particle's dynamic variables used
  ///        in the integration of motion
  enum y_index {
    RX = 0,           ///< Position X coordinate
    RY = 1,           ///< Position Y coordinate
    RZ = 2,           ///< Position Z coordinate
    PX = 3,           ///< Momentum X coordinate
    PY = 4,           ///< Momentum Y coordinate
    PZ = 5,           ///< Momentum Z coordinate
    UNDEF_6 = 6,      ///< Unused dynamic variable
    GLOBAL_TIME = 7,  ///< Global time (laboratory frame time of flight)
    UNDEF_8 = 8,      ///< Unused dynamic variable
    SPINX = 9,        ///< Spin X coordinate
    SPINY = 10,       ///< Spin Y coordinate
    SPINZ = 11        ///< Spin Z coordinate
  };

  /// Constructor
  em_field_equation_of_motion(G4Field* field_);

  /// Destructor
  virtual ~em_field_equation_of_motion();

  /// Given the value of the mixed fields (magnetic + electric), this method
  /// calculates the value of the derivative dydx for position, momentum and
  //  spin of the particle.
  /// Derivatives are defined as:
  ///   dr/ds, dp/ds, dt/ds, dSpin/ds
  virtual void EvaluateRhsGivenB(const G4double y_[], const G4double field_[6],
                                 G4double dydx_[]) const;

  /// Set the charge, momentum and mass of the current particle
  /// used to set the equation's coefficients
  /// Override a manadatory virtual method in Geant 4.9.6
  /// Signature has changed for some Geant 4.10.X version (see below)
  void SetChargeMomentumMass(G4double particle_charge_,  // in e+ units
                             G4double particle_momentum_, G4double particle_mass_);

#if G4VERSION_NUMBER >= 1000
  // New signature for this virtual method in Geant 4.10.X
  void SetChargeMomentumMass(G4ChargeState particle_charge_,  // with charge attribute in e+ units
                             G4double particle_momentum_, G4double particle_mass_);
#endif

  /// Set the magnetic anomaly of the particle
  void set_anomaly(double a_);

  /// Return the magnetic anomaly of the particle
  double get_anomaly() const;

  /// Set the spin usage flag
  void set_with_spin(bool);

  /// Check the spin usage flag
  bool is_with_spin() const;

  /// Set the only magnetic flag
  void set_only_magnetic(bool);

  /// Check the only magnetic flag
  bool is_only_magnetic() const;

 protected:
  void _set_defaults();

 private:
  // Configuration parameters:
  bool _with_spin_ = false;      ///< Particle with spin
  bool _only_magnetic_ = false;  ///< Field is only magnetic

  double _mass_;           ///< Effective mass of the particle
  double _charge_;         ///< Effective charge of the particle
  double _anomaly_ = 0.0;  ///< Magnetic anomaly of the particle
  double _momentum_;       ///< Momentum of the particle

  // Coefficients computed from particle effective mass and charge:
  double _charge_coef_;
  double _mass_sqr_;

  double _omegac_;
  double _energy_;
  double _gamma_;
  double _beta_;
};

}  // end of namespace g4

}  // end of namespace mctools

#endif  // MCTOOLS_G4_EM_FIELD_EQUATION_OF_MOTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
