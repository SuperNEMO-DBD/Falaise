/// \file mctools/g4/neutrons_physics_constructor.h
/* Author(s) :    Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2014-08-26
 * Last modified: 2019-05-01
 *
 * License: GPL3
 *
 * Description:
 *
 *   G4 neutrons physics constructor
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_NEUTRONS_PHYSICS_CONSTRUCTOR_H
#define MCTOOLS_G4_NEUTRONS_PHYSICS_CONSTRUCTOR_H 1

// This project:
#include <mctools/g4/base_physics_constructor.h>

namespace mctools {

namespace g4 {

/// \brief Physics constructor dedicated to neutrons
class neutrons_physics_constructor : public base_physics_constructor {
 public:
  /// Default constructor
  neutrons_physics_constructor();

  /// Destructor
  virtual ~neutrons_physics_constructor();

  /// Initialization
  virtual void initialize(const datatools::properties &config_,
                          physics_constructor_dict_type &dict_);

  /// Reset
  virtual void reset();

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

  /// Particles construction (Geant4 interface)
  virtual void ConstructParticle();

  /// Processes construction (Geant4 interface)
  virtual void ConstructProcess();

  /// OCD support
  static void init_ocd(datatools::object_configuration_description &);

 protected:
  /// Set default values for attributes
  void _set_defaults();

 private:
  bool _use_HE_;                          //!< Flag for high-energy regime
  bool _use_gammas_hadronic_process_;     //!< Flag to use hadronic process for gammas
  bool _use_electrons_hadronic_process_;  //!< Flag to use hadronic process for electrons
  bool _use_neutrons_inelastic_process_;  //!< Flag to use inelastic process for neutrons
  bool _use_neutrons_elastic_process_;    //!< Flag to use elastic process for neutrons
  bool _use_neutrons_capture_;            //!< Flag to use neutron capture
  bool _use_neutrons_fission_;            //!< Flag to use neutron fission
  bool _use_protons_inelastic_process_;   //!< Flag to use inelastic process for protons
  bool _use_protons_elastic_process_;     //!< Flag to use elastic process for protons

  /// Registration macro :
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_physics_constructor,
                                                       neutrons_physics_constructor)
};

}  // end of namespace g4

}  // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::neutrons_physics_constructor)

#endif  // MCTOOLS_G4_NEUTRONS_PHYSICS_CONSTRUCTOR_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
