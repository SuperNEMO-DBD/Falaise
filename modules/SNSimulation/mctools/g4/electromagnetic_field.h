/// \file mctools/g4/electromagnetic_field.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-12-08
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

#ifndef MCTOOLS_G4_ELECTROMAGNETIC_FIELD_H
#define MCTOOLS_G4_ELECTROMAGNETIC_FIELD_H 1

// Standard library:
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Geant4:
#include <G4ElectroMagneticField.hh>
#include <G4MagneticField.hh>
// - Bayeux/geomtools :
#include <geomtools/smart_id_locator.h>

// This project:
#include <mctools/g4/loggable_support.h>

// Forward class declarations:
namespace datatools {
class properties;
}

namespace geomtools {
class manager;
}

namespace emfield {
class base_electromagnetic_field;
class electromagnetic_field_manager;
}  // namespace emfield

namespace mctools {

namespace g4 {

class detector_construction;

/// \brief Electromagnetic field using the Geant4 interface
class electromagnetic_field :  // public G4MagneticField,
                               public G4ElectroMagneticField,
                               public loggable_support {
 public:
  /// \brief Field mode
  enum standalone_field_mode {
    STANDALONE_UNDEFINED = 0,
    STANDALONE_MAGNETIC_FIELD = 1,
    STANDALONE_ELECTRIC_FIELD = 2
  };

  // typedef std::map<uint32_t, double> field_map_type;

  /// Check initialization status
  bool is_initialized() const;

  /// Check active status
  bool is_active() const;

  /// Check if the name is defined
  bool has_name() const;

  /// Set the name
  void set_name(const std::string &name_);

  /// Return the name
  const std::string &get_name() const;

  /// Set the flag for checking position and time
  void set_field_check_pos_time(bool);

  /// Check the flag for checking position and time
  bool is_field_check_pos_time() const;

  /// Check if the field is defined
  bool has_field() const;

  /// Set the field
  void set_field(const emfield::base_electromagnetic_field &);

  /// Return the field
  const emfield::base_electromagnetic_field &get_field() const;

  /// Default constructor
  electromagnetic_field();

  /// Destructor
  virtual ~electromagnetic_field();

  /// Initialization
  void initialize(const datatools::properties &config_);

  /// Initialization
  void initialize();

  /// Reset
  void reset();

  /// Print
  void dump(std::ostream &out_ = std::clog) const;

  // G4 interface:

  /// Return the field value at given position/time
  void GetFieldValue(const double position_[4], double *em_field_) const;

  /// Check if the field changes the energy
  virtual G4bool DoesFieldChangeEnergy() const;

 protected:
  /// Set default attributes values
  void _set_defaults();

 private:
  bool _initialized_;                                    //!< Initialization flag
  std::string _name_;                                    //!< Name
  const emfield::base_electromagnetic_field *_field_;    //!< Handle to the electromagnetic field
  bool _field_check_pos_time_;                           //!< Flag for checking position/time
  geomtools::vector_3d _standalone_constant_mag_field_;  //!< Standalone uniform magnetic field
  geomtools::vector_3d _standalone_constant_electric_field_;  //!< Standalone uniform electric field

  friend class detector_construction;
};

}  // end of namespace g4

}  // end of namespace mctools

#endif  // MCTOOLS_G4_ELECTROMAGNETIC_FIELD_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
