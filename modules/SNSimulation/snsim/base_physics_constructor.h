/// \file snsim/base_physics_constructor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-03
 * Last modified: 2015-04-30
 *
 * License: GPL3
 *
 * Description:
 *
 *   Base class for physics constructors
 *
 */

#ifndef SNSIM_BASE_PHYSICS_CONSTRUCTOR_H
#define SNSIM_BASE_PHYSICS_CONSTRUCTOR_H 1

// Standard library:
#include <map>
#include <string>

// Third party:
// - Bayeux/datatools :
#include <datatools/factory_macros.h>
#include <datatools/i_tree_dump.h>
// - Geant4
#include <G4VPhysicsConstructor.hh>
#include <globals.hh>

// This project:
#include <snsim/loggable_support.h>
#include <snsim/physics_list_utils.h>

namespace snsim {

// Forward class declaration:
class physics_list;

std::string g4_builder_type_to_label(int);

int label_to_g4_builder_type(const std::string &label_);

/// \brief Base class for G4-based physics constructor with factory registration support
class base_physics_constructor : public G4VPhysicsConstructor,
                                 public datatools::i_tree_dumpable,
                                 public loggable_support {
 public:
  /// Check initialization status
  bool is_initialized() const;

  /// Set the name of the physics constructor
  void set_name(const std::string &name_);

  /// Return the name of the physics constructor
  const std::string &get_name() const;

  /// Set the class unique identifier of the physics constructor
  void set_class_id(const std::string &class_id_);

  /// Return the class unique identifier of the physics constructor
  const std::string &get_class_id() const;

  /// Default constructor
  base_physics_constructor();

  /// Desctructor
  virtual ~base_physics_constructor();

  /// Initialization from a set of configuration properties and a dictionary of physics constructors
  virtual void initialize(const datatools::properties &config_,
                          physics_constructor_dict_type &dict_) = 0;

  /// Initialization from a set of configuration properties
  void initialize_standalone(const datatools::properties &config_);

  /// Reset
  virtual void reset() = 0;

  // G4 mandatory interface: construct particle and physics

  /// Construct the Geant4 particle list
  virtual void ConstructParticle();  // Default empty implementation

  /// Construct the Geant4 processes list
  virtual void ConstructProcess();  // Default empty implementation

  /// Check if the constructor has a mother physics list
  bool has_mother_physics_list() const;

  /// Return a const reference to the mother physics list (if any)
  const physics_list &get_mother_physics_list() const;

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

 protected:
  void _set_initialized(bool);

  void _set_mother_physics_list(physics_list &);

  void _reset();

 private:
  std::string _name_;                             //!< Name of the physics constructor
  std::string _class_id_;                         //!< Class ID of the physics constructor
  bool _initialized_;                             //!< Initialization flag
  physics_list *_mother_physics_list_ = nullptr;  //!< Reference to the mother physics list

  // Factory registration system :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_physics_constructor)

  friend class physics_list;
};

}  // namespace snsim

#endif  // SNSIM_BASE_PHYSICS_CONSTRUCTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
