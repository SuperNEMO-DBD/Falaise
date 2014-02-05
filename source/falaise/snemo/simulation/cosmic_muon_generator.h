// -*- mode: c++ ; -*-
/** \file falaise/snemo/simulation/cosmic_muon_generator.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-01
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Copyright 2010-2014 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *   cosmic muon event generator based on event generators
 *   provided by the 'genbb_help' package
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_COSMIC_MUON_GENERATOR_H
#define FALAISE_SNEMO_SIMULATION_COSMIC_MUON_GENERATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/i_genbb.h>

namespace mygsl {
  class i_unary_function;
  class von_neumann_method;
}
namespace datatools {
  class properties;
  class multi_properties;
}

namespace snemo {

  namespace simulation {

    /// Generator for cosmic muons
    class cosmic_muon_generator : public ::genbb::i_genbb
    {
    public:

      enum mode_type {
        MODE_INVALID     = -1,
        MODE_SEA_LEVEL   = 0,
        MODE_UNDERGROUND = 1,
      };

      enum sea_level_mode_type {
        SEA_LEVEL_INVALID = -1,
        SEA_LEVEL_TOY     = 0,
        SEA_LEVEL_PDG     = 1,
      };

      struct sea_level_toy_setup {
        double energy_mean;   //!< ~ 4 GeV
        double energy_sigma;  //!< ~ 1 GeV
        double muon_ratio;    //!< Ratio (Nmu+/Nmu-) ~ 1.1
        double maximum_theta; //!< Maximum azimuthal angle (70 degree)
        mygsl::i_unary_function   * theta_density_function;
        mygsl::von_neumann_method * angular_VNM;
        sea_level_toy_setup ();
        void set_defaults ();
        void reset ();
      };

      struct sea_level_pdg_setup {
        sea_level_pdg_setup ();
        void set_defaults ();
        void reset ();
      };

      struct underground_setup {
        std::string underground_lab;
        double underground_depth;
        underground_setup ();
        void set_defaults ();
        void reset ();
      };

    public:

      bool is_debug () const;
      void set_debug (bool);
      bool is_initialized () const;
      int get_mode () const;
      void set_mode (int);
      const mygsl::rng & get_random () const;
      mygsl::rng & grab_random ();
      bool can_external_random () const;

      /// Constructor
      cosmic_muon_generator ();

      /// Destructor
      virtual ~cosmic_muon_generator ();

      /// Main initialization interface method
      virtual void initialize (const datatools::properties & setup_,
                               datatools::service_manager & service_manager_,
                               ::genbb::detail::pg_dict_type & dictionary_);

      /// Reset the object
      virtual void reset ();

      /// Check if some next primary event is available
      virtual bool has_next ();

      static double energy_spectrum_at_sea_level_HE (double muon_cos_theta,
                                                     double muon_energy);

    protected:

      virtual void _load_next (::genbb::primary_event & event_,
                               bool compute_classification_ = true);
    private:

      void _at_init_ ();

      void _at_reset_ ();

    private:

      bool   _debug_;       //!< Debug flag
      bool   _initialized_; //!< Initialization flag
      int    _mode_;        //!< Mode

      // muon generator at sea level :
      int    _sea_level_mode_;
      sea_level_toy_setup _sea_level_toy_setup_; //!< Using inputs from JPG 37, 075021 (2010) (http://pdg.lbl.gov)
      sea_level_pdg_setup _sea_level_pdg_setup_; //!< [not implemented yet]

      // muon generator in an underground location :
      underground_setup   _underground_setup_;   //!< [not implemented yet]

      unsigned long _seed_;   //!< Local PRNG's seed
      mygsl::rng    _random_; //!< Local PRNG

      GENBB_PG_REGISTRATION_INTERFACE(cosmic_muon_generator);

    };

  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_COSMIC_MUON_GENERATOR_H

// end of falaise/snemo/simulation/cosmic_muon_generator.h
