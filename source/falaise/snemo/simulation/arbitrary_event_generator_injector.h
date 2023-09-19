// -*- mode: c++ ; -*-
/** \file falaise/snemo/simulation/arbitrary_event_generator_injector.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-09-19
 * Last modified: 2023-09-19
 *
 * License:
 *
 * Copyright 2023 F. Mauger
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
 *   A generator which can wrap an arbotrary event generator loaded from a dedicated configuration.
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_ARBITRARY_EVENT_GENERATOR_INJECTOR_H
#define FALAISE_SNEMO_SIMULATION_ARBITRARY_EVENT_GENERATOR_INJECTOR_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/i_genbb.h>
#include <genbb_help/manager.h>

namespace datatools {
  class properties;
  class multi_properties;
} // namespace datatools

namespace snemo {

  namespace simulation {

    /// Generator for cosmic muons
    class arbitrary_event_generator_injector
      : public ::genbb::i_genbb
    {

    public:
  
      bool is_initialized() const override;

      const mygsl::rng & get_random() const;
      mygsl::rng & grab_random();
      bool can_external_random() const override;

      /// Constructor
      arbitrary_event_generator_injector();

      /// Destructor
      virtual ~arbitrary_event_generator_injector();

      /// Main initialization interface method
      ///
      /// # Verbose level:
      /// logging.priority : string = "debug"
      ///
      /// # Flag to assign generation IDs to generated events:
      /// assign_generation_ids : boolean = false
      ///
      /// # Path of a file which contains a list of setup for the event/particle generators to be used:
      /// setup_list_file : string as path = "/path/to/some/list_of_config_files_for_some_event_generators"
      ///
      /// # Path of a single file which setups the event/particle generators to be used:
      /// setup_file : string as path = "/path/to/some/config_file_for_some_event_generators"
      /// 
      /// # Name of the event/particle generator to be used (default='aegir'):
      /// generator : string = "aegir"
      ///
      /// 
      /// Format of the setup list file (plain text with one file per line):
      ///
      /// /path/to/some/config_file_for_some_event_generators_1
      /// /path/to/some/config_file_for_some_event_generators_2
      /// ...
      /// /path/to/some/config_file_for_some_event_generators_n
      ///
      ///
      /// Format of a single setup file (datatools::multi_properties):
      /// 
      /// #@key_label  "name"
      /// #@meta_label "type"
      ///
      /// [name="pg_1" type="pg_type_1"]
      /// param_1 : type_1 = value_1
      /// param_2 : type_2 = value_2
      /// ...
      /// param_i : type_i = value_i
      ///
      /// [name="pg_2" type="pg_type_2"]
      /// param_1 : type_1 = value_1
      /// param_2 : type_2 = value_2
      /// ...
      /// param_j : type_j = value_j
      ///
      /// ...
      ///
      /// [name="pg_n" type="pg_type_n"]
      /// param_1 : type_1 = value_1
      /// param_2 : type_2 = value_2
      /// ...
      /// param_k : type_k = value_k
      ///
      ///
      void initialize(const datatools::properties & dps,
		      datatools::service_manager & srv_mgr_,
		      genbb::detail::pg_dict_type & pg_dict_) override;

      /// Reset the object
      void reset() override;

      /// Check if some next primary event is available
      bool has_next() override;

    protected:
  
      virtual void _load_next(::genbb::primary_event & event_, bool compute_classification_ = true);

    private:
  
      void _at_init_();

      void _at_reset_();

    private:
      
      bool _initialized_ = false;  //!< Initialization flag
      unsigned long _seed_ = 0;    //!< Local PRNG's seed
      mygsl::rng _random_;         //!< Local PRNG

      std::vector<std::string> _setup_file_paths_; //!< List of setup files for event/particle generators
      genbb::manager _mgr_; //!< Private manager for event/particle generators 
      std::string _pg_name_; //!< name of the event particle generator to be used from the list of embedded event/particle generators 
      const genbb::detail::pg_dict_type * _pg_dict_ = nullptr; //!< Dictionary of particle generators
      ::genbb::i_genbb * _pg_ = nullptr; //<! Selected event/particle generator

      GENBB_PG_REGISTRATION_INTERFACE(arbitrary_event_generator_injector)
      
    };

  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_ARBITRARY_EVENT_GENERATOR_INJECTOR_H
