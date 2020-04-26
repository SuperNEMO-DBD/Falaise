/// \file mctools/g4/physics_list_utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-04
 * Last modified: 2013-07-04
 */

#ifndef SNSIM_PHYSICS_LIST_UTILS_H
#define SNSIM_PHYSICS_LIST_UTILS_H 1

// Standard library:
#include <map>
#include <string>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/handle.h>
#include <datatools/properties.h>
// - Geant4 :
#include <G4BuilderType.hh>
#include <G4VModularPhysicsList.hh>
#include <G4VPhysicsConstructor.hh>

namespace snsim {

// Forward class declaration:
class base_physics_constructor;

std::string get_builder_type_label(G4BuilderType);

G4BuilderType get_builder_type(const std::string& label_);

/// \brief Entry class for the dictionnary of physics constructors
struct physics_constructor_entry {
 public:
  /// \brief Status of the managed physics constructor
  enum status_type { STATUS_CREATED = 0x1, STATUS_INITIALIZED = 0x2 };

  /// Default constructor
  physics_constructor_entry();

  /// Return a const reference to the embedded physics constructor
  const base_physics_constructor& get_physics_constructor() const;

  /// Return a mutable reference to the embedded physics constructor
  base_physics_constructor& grab_physics_constructor();

 public:
  std::string name;              //!< Name of the physics constructor
  std::string id;                //!< Class ID of the physics constructor
  datatools::properties config;  //!< Configuration container
  uint32_t status;               //!< Status of the physics constructor
  datatools::handle<base_physics_constructor>
      handle;  //!< Handle to the instantiated physics constructor
};

/// \brief Type alias for a dictionary of physics constructors
typedef std::map<std::string, physics_constructor_entry> physics_constructor_dict_type;

/// \brief Physics constructor proxy class with weak referencing
struct physics_constructor_proxy : public G4VPhysicsConstructor {
  physics_constructor_proxy(base_physics_constructor& pc_);
  virtual ~physics_constructor_proxy();
  virtual void ConstructParticle();
  virtual void ConstructProcess();
  base_physics_constructor* pc;  //!< Reference to a mctools physics constructor object
};

/// \brief Physics list proxy class with weak referencing
struct physics_list_proxy : public G4VModularPhysicsList {
  physics_list_proxy(G4VModularPhysicsList& pl_);
  virtual ~physics_list_proxy();
  virtual void ConstructParticle();
  virtual void ConstructProcess();
  virtual void SetCuts();
  G4VModularPhysicsList* pl;  //!< Reference to a Geant4 physics list object
};

/// \brief User limits data class
struct user_limits_info {
 public:
  user_limits_info();
  void reset();
  void make_step_limitation(double step_max_length_);
  void make_track_limitation(double track_max_length_, double track_max_time_,
                             double track_min_kinetic_energy_, double track_min_range_);

 public:
  double step_max_length;
  double track_max_length;
  double track_max_time;
  double track_min_kinetic_energy;
  double track_min_range;
};

}  // namespace snsim

#endif  // SNSIM_PHYSICS_LIST_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
