/// \file snsim/magnetic_field.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-19
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 interface for magnetic field
 *
 * History:
 *
 */

#ifndef SNSIM_MAGNETIC_FIELD_H
#define SNSIM_MAGNETIC_FIELD_H 1

// Standard library:
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/geomtools :
#include <bayeux/geomtools/smart_id_locator.h>
// - Geant4:
#include <G4MagneticField.hh>

// This project:
#include <snsim/loggable_support.h>

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

namespace snsim {

class detector_construction;

/// \brief Magnetic field using the Geant4 interface
class magnetic_field : public G4MagneticField, public loggable_support {
 public:
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
  bool has_mag_field() const;

  /// Check if the field is defined
  bool has_field() const;

  /// Set the magnetic field
  void set_mag_field(const emfield::base_electromagnetic_field &);

  /// Set the field
  void set_field(const emfield::base_electromagnetic_field &);

  /// Return the field
  const emfield::base_electromagnetic_field &get_mag_field() const;

  /// Return the field
  const emfield::base_electromagnetic_field &get_field() const;

  /// Default constructor
  magnetic_field();

  /// Destructor
  virtual ~magnetic_field();

  /// Initialization
  void initialize(const datatools::properties &config_);

  /// Initialization
  void initialize();

  /// Reset
  void reset();

  /// Print
  void dump(std::ostream &out_ = std::clog) const;

  // G4 interface:
  void GetFieldValue(const double position_[3], double *b_field_) const;

 protected:
  /// Set default attributes values
  void _set_defaults();

 private:
  bool _initialized_;
  std::string _name_;
  const emfield::base_electromagnetic_field *_field_;
  bool _field_check_pos_time_;
  geomtools::vector_3d _standalone_constant_field_;

  friend class detector_construction;
};

}  // namespace snsim

#endif  // SNSIM_MAGNETIC_FIELD_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
