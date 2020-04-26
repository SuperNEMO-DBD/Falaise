/// \file snsim/em_field_g4_stuff.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-04-20
 * Last modified: 2015-04-20
 *
 * License:
 *
 * Description:
 *
 *   G4 interface for electromagnetic field
 *
 * History:
 *
 */

#ifndef SNSIM_EM_FIELD_G4_STUFF_H
#define SNSIM_EM_FIELD_G4_STUFF_H

// This project:
#include <snsim/loggable_support.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_tree_dump.h>

class G4EquationOfMotion;
class G4FieldManager;
class G4PropagatorInField;
class G4MagIntegratorStepper;
class G4ChordFinder;
class G4MagInt_Driver;

// Forward class declarations:
namespace datatools {
class properties;
}

namespace snsim {

// Forward class declarations:
class magnetic_field;
class electromagnetic_field;

/// \brief Working data for G4 EM field handling
class em_field_g4_stuff : public datatools::i_tree_dumpable, public loggable_support {
 public:
  /// \Brief Type of ODE stepper
  enum stepper_type {
    STEPPER_INVALID = 0,
    STEPPER_CASHKARP_RKF45,  // order 4/5
    STEPPER_CLASSICAL_RK4,   // order 4
    STEPPER_EXPLICIT_EULER,  // order 1
    STEPPER_IMPLICIT_EULER,  // order 2
    STEPPER_SIMPLE_HEUM,     // order 3
    STEPPER_SIMPLE_RUNGE,    // order 2
    STEPPER_EXACT_HELIX,
    STEPPER_HELIX_EXPLICIT_EULER,
    STEPPER_HELIX_HEUM,
    STEPPER_HELIX_IMPLICIT_EULER,
    STEPPER_HELIX_MIXED_EULER,
    STEPPER_HELIX_SIMPLE_RUNGE
  };

  static std::string label_from_stepper(stepper_type);

  static stepper_type stepper_from_label(const std::string& label_);

  /// Default constructor
  em_field_g4_stuff();

  /// Destructor
  virtual ~em_field_g4_stuff();

  /// Initialization
  void initialize();

  /// Initialization
  void initialize(const datatools::properties& config_);

  /// Reset
  void reset();

  /// Check initialization flag
  bool is_initialized() const;

  /// Set the field
  void set_g4_magnetic_field(magnetic_field* bfield_);

  /// Set the field
  void set_g4_electromagnetic_field(electromagnetic_field* ebfield_);

  /// Check if the G4 EM field is set
  bool has_g4_field() const;

  /// Check if the G4 EM field is pure magnetic
  bool is_pure_magnetic_g4_field() const;

  /// Check if the G4 EM field is a general electromagnetic
  bool is_general_electromagnetic_g4_field() const;

  /// Return the nale of the embedded G4 EM field
  std::string get_g4_field_name() const;

  /// Return the stepper type
  stepper_type get_stepper_type() const;

  /// Set the stepper type
  void set_stepper_type(stepper_type);

  /// Return the minimum step
  double get_min_step() const;

  /// Set the minimum step
  void set_min_step(double);

  /// Return the spin flag
  bool is_spin() const;

  /// Set the spin flag
  void set_spin(bool);

  /// Return the propagate-to-daughters flag
  bool is_propagate_to_daughters() const;

  /// Set the propagate-to-daughters flag
  void set_propagate_to_daughters(bool);

  /// Check delta chord
  bool has_delta_chord() const;

  /// Return the delta chord (maximum sagitta delta chord)
  double get_delta_chord() const;

  /// Set the delta chord (maximum sagitta delta chord)
  void set_delta_chord(double);

  double get_delta_one_step() const;

  void set_delta_one_step(double);

  double get_delta_intersection() const;

  void set_delta_intersection(double);

  double get_eps_min() const;

  void set_eps_min(double);

  double get_eps_max() const;

  void set_eps_max(double);

  /// Return the field manager
  G4FieldManager* grab_field_manager();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 protected:
  /// Set default attributes' values
  void _set_defaults();

  /// Initialization action
  void _at_init();

 private:
  bool _initialized_;  //!< Initialization flag

  magnetic_field* _b_field_;          //!< Pure magnetic field (referenced, not deleted)
  electromagnetic_field* _eb_field_;  //!< Electromagnetic field (referenced, not deleted)

  // Some useful documentation: http://geant4.in2p3.fr/2007/prog/JohnApostolakis/EMField.pdf
  stepper_type _stepper_type_;  //!< Type of the ODE stepper
  double _min_step_;            //!< Minimum step length
  double _delta_chord_;         //!< Upper bound for the chord distance
  double _delta_one_step_;      //!< Position error which is acceptable in an integration step (same
                                //!< order of magnitude as _delta_intersection_)
  double _delta_intersection_;  //!< Upper limit inaccuracy of a single boundary crossing (same
                                //!< order of magnitude as _delta_one_step_)
  double _eps_min_;  //!< Lower limit on the relative error of the position/momentum inaccuracy
  double _eps_max_;  //!< Upper limit on the relative error of the position/momentum inaccuracy
  bool _spin_;       //!< Spin flag
  bool _propagate_to_daughters_;  //!< Flag to propagate the field to all daughter volumes

  G4EquationOfMotion* _equation_;           //!< Equation of motion (deleted by the chord finder!)
  G4PropagatorInField* _field_propagator_;  //!< Field propagator   (referenced, not deleted)
  G4MagIntegratorStepper* _field_stepper_;  //!< ODE stepper        (deleted by this object)
  G4MagInt_Driver* _integration_driver_;    //!< Integration driver (deleted by the chord finder!)
  G4ChordFinder* _chord_finder_;            //!< Chord finder       (deleted by this object)
  G4FieldManager* _field_manager_;          //!< Field manager      (deleted by this object)
};

}  // namespace snsim

#endif  // SNSIM_EM_FIELD_G4_STUFF_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
