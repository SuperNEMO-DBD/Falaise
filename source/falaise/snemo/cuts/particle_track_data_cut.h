/// \file falaise/snemo/cuts/particle_track_data_cut.h
/* Author(s)     : Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date : 2012-06-15
 * Last modified : 2014-06-04
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 *
 * Description:
 *
 *   Particle track data cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_PARTICLE_TRACK_DATA_CUT_H
#define FALAISE_SNEMO_CUT_PARTICLE_TRACK_DATA_CUT_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace snemo {

  namespace cut {

    /// \brief A cut performed on the event record's 'particle track data' bank
    class particle_track_data_cut : public cuts::i_cut
    {
    public:

      /// Mode of the cut
      enum mode_type {
        MODE_UNDEFINED                     = 0,
        MODE_CHARGE                        = datatools::bit_mask::bit00,
        MODE_HAS_PARTICLES                 = datatools::bit_mask::bit01,
        MODE_RANGE_PARTICLES               = datatools::bit_mask::bit02,
        MODE_HAS_ASSOCIATED_CALORIMETERS   = datatools::bit_mask::bit03,
        MODE_RANGE_ASSOCIATED_CALORIMETERS = datatools::bit_mask::bit04,
        MODE_HAS_VERTEX_ON_FOIL            = datatools::bit_mask::bit05
      };

      /// Set the 'Particle Track Data' bank label/name
      void set_PTD_label(const std::string & PTD_label_);

      /// Return the 'Particle Track Data' bank label/name
      const std::string & get_PTD_label() const;

      /// Return the cut mode
      uint32_t get_mode() const;

      /// Check mode HAS_PARTICLES
      bool is_mode_has_particles() const;

      /// Check mode RANGE_PARTICLES
      bool is_mode_range_particles() const;

      /// Check mode HAS_ASSOCIATED_CALORIMETERS
      bool is_mode_has_associated_calorimeters() const;

      /// Check mode RANGE_ASSOCIATED_CALORIMETERS
      bool is_mode_range_associated_calorimeters() const;

      /// Check mode HAS_VERTEX_ON_FOIL
      bool is_mode_has_vertex_on_foil() const;

      /// Check mode CHARGE
      bool is_mode_charge() const;

      /// Constructor
      particle_track_data_cut(datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~particle_track_data_cut();

      /// Initilization
      virtual void initialize(const datatools::properties & configuration_,
                              datatools::service_manager & service_manager_,
                              cuts::cut_handle_dict_type & cut_dict_);

      /// Reset
      virtual void reset();

    protected:

      /// Default values
      void _set_defaults();

      /// Selection
      virtual int _accept();

    private:

      std::string _PTD_label_;              //!< Name of the "Particle track data" bank
      uint32_t    _mode_;                   //!< Mode of the cut
      std::string _charge_type_;            //!< Name of the boolean property in the particle track data
      int         _particles_range_min_;    //!< Minimal number of particles for range_particles mode
      int         _particles_range_max_;    //!< Maximal number of particles for range_particles mode
      int         _calorimeters_range_min_; //!< Minimal number of ass. calo for range_associated_calorimeters mode
      int         _calorimeters_range_max_; //!< Maximal number of ass. calo for range_associated_calorimeters mode

      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(particle_track_data_cut);
    };

  }  // end of namespace cut

}  // end of namespace snemo

#endif // FALAISE_SNEMO_CUT_PARTICLE_TRACK_DATA_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
