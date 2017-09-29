/// \file falaise/snemo/cuts/particle_track_cut.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2015-03-05
 * Last modified : 2015-03-05
 *
 * Copyright (C) 2015 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   Particle track cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_PARTICLE_TRACK_CUT_H
#define FALAISE_SNEMO_CUT_PARTICLE_TRACK_CUT_H 1

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

/// \brief A cut performed on individual 'particle track' record
class particle_track_cut : public cuts::i_cut {
 public:
  /// Mode of the cut
  enum mode_type {
    MODE_UNDEFINED = 0,
    MODE_FLAG = datatools::bit_mask::bit00,
    MODE_HAS_CHARGE = datatools::bit_mask::bit01,
    MODE_HAS_ASSOCIATED_CALORIMETER_HITS = datatools::bit_mask::bit02,
    MODE_RANGE_ASSOCIATED_CALORIMETER_HITS = datatools::bit_mask::bit03,
    MODE_HAS_VERTEX = datatools::bit_mask::bit04,
    MODE_HAS_DELAYED_CLUSTER = datatools::bit_mask::bit05
  };

  /// Return the cut mode
  uint32_t get_mode() const;

  /// Check mode FLAG
  bool is_mode_flag() const;

  /// Check mode HAS_ASSOCIATED_CALORIMETER_HITS
  bool is_mode_has_associated_calorimeter_hits() const;

  /// Check mode RANGE_ASSOCIATED_CALORIMETER_HITS
  bool is_mode_range_associated_calorimeter_hits() const;

  /// Check mode HAS_VERTEX
  bool is_mode_has_vertex() const;

  /// Check mode HAS_CHARGE
  bool is_mode_has_charge() const;

  /// Check mode HAS_DELAYED_CLUSTER
  bool is_mode_has_delayed_cluster() const;

  /// Constructor
  particle_track_cut(datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~particle_track_cut();

  /// Initilization
  virtual void initialize(const datatools::properties& configuration_,
                          datatools::service_manager& service_manager_,
                          cuts::cut_handle_dict_type& cut_dict_);

  /// Reset
  virtual void reset();

 protected:
  /// Default values
  void _set_defaults();

  /// Selection
  virtual int _accept();

 private:
  uint32_t _mode_;            //!< Mode of the cut
  std::string _flag_name_;    //!< Name of the boolean property in the particle track
  std::string _charge_type_;  //!< Name of the charge type
  std::string _vertex_type_;  //!< Name of the vertex type

  std::string _calorimeter_hits_range_category_;  //!< Calorimeter category
  int _calorimeter_hits_range_min_;               //!< Minimal number of associated calorimeter
  int _calorimeter_hits_range_max_;               //!< Maximal number of associated calorimeter

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(particle_track_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::particle_track_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::cut::particle_track_cut)

#endif  // FALAISE_SNEMO_CUT_PARTICLE_TRACK_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
