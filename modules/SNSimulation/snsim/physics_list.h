/// \file snsim/physics_list.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2015-04-30
 *
 * License:
 *
 * Description:
 *
 *   G4 physics list
 *
 * History:
 *
 */

#ifndef SNSIM_PHYSICS_LIST_H
#define SNSIM_PHYSICS_LIST_H 1

// Standard library:
#include <map>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools :
#include <datatools/i_tree_dump.h>
// - Geant4:
#include "G4VModularPhysicsList.hh"

// This project:
#include <snsim/base_physics_constructor.h>
#include <snsim/loggable_support.h>
#include <snsim/physics_list_utils.h>

namespace datatools {
class properties;
}

namespace snsim {

/// \brief Physics list
class physics_list : public G4VModularPhysicsList,
                     public datatools::i_tree_dumpable,
                     public loggable_support {
 public:
  /// \brief Geant4 verbosity levels
  enum g4_verbosity_type { VERBOSITY_SILENT = 0, VERBOSITY_WARNING = 1, VERBOSITY_MORE = 2 };

  /// \brief Production cuts associated to basic particles
  struct production_cuts_info {
   public:
    /// Default constructor
    production_cuts_info();

    /// Constructor
    ///
    /// @arg all_value_ production cut applied to all particles
    production_cuts_info(double all_value_);

    /// Constructor
    ///
    /// @arg gamma_value_ production cut applied to gammas
    /// @arg electron_value_ production cut applied to electrons
    /// @arg positron_value_ production cut applied to positrons
    /// @arg proton_value_ production cut applied to protons
    production_cuts_info(double gamma_value_, double electron_value_, double positron_value_,
                         double proton_value_);

    /// Initialization
    ///
    /// @arg gamma_value_ production cut applied to gammas
    /// @arg electron_value_ production cut applied to electrons
    /// @arg positron_value_ production cut applied to positrons
    /// @arg proton_value_ production cut applied to protons
    void initialize(double default_cut_gamma_, double default_cut_electron_,
                    double default_cut_positron_, double default_cut_proton_);

   public:
    double gamma;     //!< Production cut applied to gammas
    double electron;  //!< Production cut applied to electron
    double positron;  //!< Production cut applied to positrons
    double proton;    //!< Production cut applied to protons
  };

  /// Check if the physics list uses a Geant4 physics list
  bool has_geant4_physics_list() const;

  /// Return a const reference to the embedded Geant4 physics list
  const G4VModularPhysicsList& get_geant4_physics_list() const;

  /// Return a mutable reference to the embedded Geant4 physics list
  G4VModularPhysicsList& grab_geant4_physics_list();

  /// Check if the physics list has physics constructor with a given name
  bool has_physics_constructor(const std::string& pc_name_) const;

  /// Return a const reference to a physics constructor with a given name
  const base_physics_constructor& get_physics_constructor(const std::string& pc_name_);

  /// Return a const reference to the dictionary of physics constructors
  const physics_constructor_dict_type& get_physics_constructors() const;

  /// Return a mutable reference to the dictionary of physics constructors
  physics_constructor_dict_type& grab_physics_constructors();

  /// Default constructor
  physics_list();

  /// Destructor
  virtual ~physics_list();

  /// Intialization from a set of configuration parameters
  void initialize(const datatools::properties& config_);

  /// Reset
  void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

  // G4 mandatory interface: construct particle and physics

  /// Construct Geant4 particles
  virtual void ConstructParticle();

  /// Construct Geant4 processes
  virtual void ConstructProcess();

  /// Set the cuts associated to particles
  virtual void SetCuts();

 protected:
  void _set_defaults();

  void _register_physics_constructors();

  void _SetCuts();

 private:
  // Management
  bool _initialized_;  //!< Initialization flag

  // Configuration parameters:
  std::string _geant4_physics_list_name_;  //!< Name of the Geant4 physics list
  // Production cuts for secondary particles (only gamma, electron, positron, proton):
  bool _using_production_cuts_;         //!< Flag to activate the use of production cuts
  double _production_cuts_low_energy_;  //!< Low edge energy for production cuts (in unit of energy)
  double
      _production_cuts_high_energy_;  //!< High edge energy for production cuts (in unit of energy)
  double
      _production_cuts_default_value_;  //!< Default value for production cuts (in unit of length)
  production_cuts_info _production_cuts_values_;  //!< Default production cuts for gammas, electron,
                                                  //!< positron and protons
  std::map<std::string, production_cuts_info>
      _production_cuts_per_region_;  //!< Map region and specific production cuts

  // Internal resources and data:
  boost::scoped_ptr<G4VModularPhysicsList>
      _geant4_physics_list_;  //!< Handle to an official Geant4 physics list
  base_physics_constructor::factory_register_type
      _factory_register_;  //!< The embedded factory register for physics constructors
  physics_constructor_dict_type
      _physics_constructors_;  //!< The embedded dictionnary of physics constructors
};

}  // namespace snsim

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::physics_list)

#endif  // SNSIM_PHYSICS_LIST_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
