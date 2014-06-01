/// \file falaise/TrackerPreClustering/gg_hit.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-30
 * Last modified: 2014-02-07
 *
 * Copyright 2012-2014 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *  Tracker Pre-Clustering algorithm.
 *
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_GG_HIT_H
#define FALAISE_TRACKERPRECLUSTERING_GG_HIT_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

namespace TrackerPreClustering {

  /// \brief Mock Geiger hit (inspired by NEMO-3 and SuperNEMO Geiger hit datamodels)
  struct gg_hit
  {

    /// Set default attribute values
    void set_defaults();

    /// Return hit Id
    int32_t get_id() const;

    /// Check if a geometry Id is available
    bool has_geom_id() const;

    /// Return the module Id
    int32_t get_module() const;

    /// Return the tracking chamber side Id
    int32_t get_side() const;

    /// Return the cell layer Id
    int32_t get_layer() const;

    /// Return the cell row Id
    int32_t get_row() const;

    /// Default constructor
    gg_hit();

    /// Reset attributes
    void reset();

    /// Check if X-Y position is available
    bool has_xy() const;

    /// Return the X position of the hit cell
    double get_x() const;

    /// Return the Y position of the hit cell
    double get_y() const;

    /// Check if longitudinal Z position and associated error are available
    bool has_zdz() const;

    /// Return the Z position of the hit
    double get_z() const;

    /// Return the error on Z position of the hit
    double get_dz() const;

    /// Check if drift radius and associated error are available
    bool has_rdr() const;

    /// Return the drift radius
    double get_r() const;

    /// Return the error on drift radius
    double get_dr() const;

    /// Check if the hit is sterile
    bool is_sterile() const;

    /// Check if the hit is delayed
    bool is_delayed() const;

    /// Check if the hit is prompt (not delayed)
    bool is_prompt() const;

    /// Check if the hit is noisy
    bool is_noisy() const;

    /// Check if the hit is peripheral (far from the anode central wire)
    bool is_peripheral() const;

    /// Check if top cathode signal is missing
    bool is_missing_top_cathode() const;

    /// Check if bottom cathode signal is missing
    bool is_missing_bottom_cathode() const;

    /// Check if the delayed time is available
    bool has_delayed_time() const;

    /// Return the delayed time
    double get_delayed_time() const;

    /// Return the error in delayed time
    double get_delayed_time_error() const;

    // Attributes:
    int32_t id;     /// Unique ID of the hit
    int32_t module; /// Geometry ID : module number
    int32_t side;   /// Geometry ID : tracker side number
    int32_t layer;  /// Geometry ID : tracking layer number
    int32_t row;    /// Geometry ID : cell row number
    bool sterile;   /// Sterile flag
    bool delayed;   /// Delayed flag
    bool peripheral; /// Peripheral flag
    bool noisy;      /// Noisy flag
    bool missing_top_cathode; /// Missing top cathode flag
    bool missing_bottom_cathode; /// Missing bottom cathode flag
    double x, y;    /// Position of the anode wire in module coordinates system
    double z, dz;   /// Longitudinal position of the hit along the anode wire
    double r, dr;   /// Transverse drift distance
    double delayed_time; /// Delayed time for a delayed hit
    double delayed_time_error; /// Delayed time error for a delayed hit

  };

} // end of namespace TrackerPreClustering

#endif // FALAISE_TRACKERPRECLUSTERING_GG_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
