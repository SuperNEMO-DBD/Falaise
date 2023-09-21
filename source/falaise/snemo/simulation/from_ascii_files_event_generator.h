// -*- mode: c++ ; -*-
/** \file falaise/snemo/simulation/from_ascii_files_event_generator.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-09-20
 * Last modified: 2023-09-20
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
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_FROM_ASCII_FILES_EVENT_GENERATOR_H
#define FALAISE_SNEMO_SIMULATION_FROM_ASCII_FILES_EVENT_GENERATOR_H 1

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Bayeux/genbb_help:
#include <genbb_help/i_genbb.h>

namespace datatools {
  class properties;
  class multi_properties;
} // namespace datatools

namespace snemo {

  namespace simulation {

    /// Event generator from input ASCII files
    class from_ascii_files_event_generator
      : public ::genbb::i_genbb
    {
    public:

      /// Constructor
      from_ascii_files_event_generator();

      /// Destructor
      virtual ~from_ascii_files_event_generator();
 
      bool can_external_random() const override;
     
      bool is_initialized() const override;
 
      /// Main initialization interface method
      void initialize(const datatools::properties & dps,
		      datatools::service_manager & /* unused */,
		      genbb::detail::pg_dict_type & /* unused */) override;

      /// Reset the object
      void reset() override;

      /// Check if some next primary event is available
      bool has_next() override;

    protected:

      void _load_next(::genbb::primary_event & event_,
		      bool compute_classification_ = true) override;

    private:
      
      void _at_init_();

      void _at_reset_();

    private:
      
      bool _initialized_ = false; //!< Initialization flag
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_; //!< PIMPL

      GENBB_PG_REGISTRATION_INTERFACE(from_ascii_files_event_generator)
      
    };

  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_FROM_ASCII_FILES_EVENT_GENERATOR_H
