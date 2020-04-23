/// \file mctools/g4/particles_physics_constructor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-04
 * Last modified: 2014-12-12
 *
 * License:
 *
 * Description:
 *
 *   G4 particles physics list
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_PARTICLES_PHYSICS_CONSTRUCTOR_H
#define MCTOOLS_G4_PARTICLES_PHYSICS_CONSTRUCTOR_H 1

// This project:
#include <mctools/g4/base_physics_constructor.h>

namespace mctools {

  namespace g4 {

    class particles_physics_constructor : public base_physics_constructor
    {
    public:

      particles_physics_constructor ();

      virtual ~particles_physics_constructor ();

      virtual void initialize (const datatools::properties & config_,
                               physics_constructor_dict_type & dict_);

      virtual void reset ();

      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;


      /// Particles construction Geant4 interface :
      virtual void ConstructParticle ();

    protected:

      // these methods construct particles
      void _ConstructBosons();
      void _ConstructLeptons();
      void _ConstructMesons();
      void _ConstructBaryons();
      void _ConstructIons();
      void _ConstructUserLimits();

      void _set_defaults();

    private:

      // Particles:
      bool        _use_geantinos_;
      bool        _use_optical_photons_;
      bool        _use_muon_leptons_;
      bool        _use_tau_leptons_;
      bool        _use_light_mesons_;
      bool        _use_charm_mesons_;
      bool        _use_bottom_mesons_;
      bool        _use_nucleons_;
      bool        _use_strange_baryons_;
      bool        _use_charm_baryons_;
      bool        _use_bottom_baryons_;
      bool        _use_light_nuclei_;
      bool        _he3_as_generic_ion_;
      bool        _use_light_anti_nuclei_;
      bool        _use_generic_ion_;

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (base_physics_constructor,
                                                            particles_physics_constructor)

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::particles_physics_constructor)

#endif // MCTOOLS_G4_PARTICLES_PHYSICS_CONSTRUCTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
