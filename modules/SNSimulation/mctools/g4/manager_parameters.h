/// \file mctools/g4/manager_parameters.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-16
 * Last modified: 2016-06-27
 *
 * License:
 *
 * Copyright (C) 2010-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_MANAGER_PARAMETERS_H
#define MCTOOLS_G4_MANAGER_PARAMETERS_H 1

// Standard library:
#include <string>
#include <iostream>
#include <vector>
#include <set>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <mctools/utils.h>

namespace mctools {

  namespace g4 {

    class manager;

    //! \brief The set of parameters for the Geant4 simulation manager
    struct manager_parameters
    {
      bool        interactive = false;     //!< Flag to activate the interactive mode
      std::string g4_macro;                //!< Name of a Geant4 macro to be executed
      bool        g4_visu = false;         //!< Flag to activate Geant4 visulization
      std::string logging;                 //!< Logging priority threshold label
      std::vector<std::string> dlls;       //!< List of DLLs to be loaded
      std::string dll_loader_config;       //!< Configuration file of the DLL loader
      std::string manager_config_filename; //!< Main manager configuration file
      uint32_t    number_of_events;        //!< Number of simulated event
      uint32_t    number_of_events_modulo; //!< Number of events modulo
      int         mgr_seed;                //!< Seed for the Geant4 engine's PRNG
      std::string input_prng_states_file;  //!< Input file for PRNG's states
      std::string output_prng_states_file; //!< Output file for PRNG's states
      int         prng_states_save_modulo; //!< Period saving the PRNG's internal states
      std::string input_prng_seeds_file;   //!< Input file for PRNG's seeds
      std::string init_seed_method;        //!< Method to define the seed initialization
      std::string output_prng_seeds_file;  //!< Output file for PRNG's seeds
      std::string output_data_format;      //!< Output data file format
      std::string output_data_bank_label;  //!< Name of the data bank for the bank output format
      std::string output_data_file;        //!< Name of the output data file
      std::string vg_name;                 //!< Name of the vertex generator
      int         vg_seed;                 //!< Seed for the vertex generator's PRNG
      std::string eg_name;                 //!< Name of the primary event generator
      int         eg_seed;                 //!< Seed for the primary event generator's PRNG
      int         shpf_seed;               //!< Seed for the hit post-processing factory
      bool        using_time_stat = false;         //!< Print time statistic at the end of simulation run
      std::string output_profiles_activation_rule; //!< The rule to activate Monte Carlo simulation output profiles
      bool        forbid_private_hits = false;     //!< Do not save 'private' MC hits at the end of the simulated event
      bool        dont_save_no_sensitive_hit_events = false; //!< Do not save 'no sensitive' MC hits
      bool        use_run_header_footer = false;   //!< Add a header/footer in the output data file

    public:

      //! Default constructor
      manager_parameters();

      //! Set default values
      void set_defaults();

      //! Reset
      void reset();

      //! Smart dump
      void tree_dump(std::ostream & out_,
                     const std::string & title_  = "",
                     const std::string & indent_ = "",
                     bool inherit_               = false) const;

      //! Setup a simulation manager from a set of parameters
      static void setup(const manager_parameters & a_params,
                        manager & a_manager);

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_MANAGER_PARAMETERS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
