/** \file lttc/tracker_hit.hh
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2021-11-16
 * Last modified: 2021-11-16
 *
 * Copyright (C) 2021 François Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Tracker hit model for LTTC
 *
 */

#ifndef FALAISE_LTTC_PLUGIN_TRACKER_HIT_HH
#define FALAISE_LTTC_PLUGIN_TRACKER_HIT_HH

// Standard library:                                                            
#include <iostream>
#include <vector>
#include <limits>

// This project:
#include <lttc/sngeometry.hh>

namespace lttc {

  /// \brief Working LTTC data model for tracker hit
  class tracker_hit
  {
  public:

    tracker_hit() = default;

    tracker_hit(int id_,
                int side_id_,
                int layer_id_,
                int row_id_,
                double drift_radius_,
                double drift_radius_err_,
                double z_,
                double z_err_,
                bool delayed_ = false);

    ~tracker_hit() = default;

    /// Reset the hit data
    void reset();

    /// Check if all hit data are set
    bool check() const;

    /// print
    void print(std::ostream & out_) const;

    /// Return the cell ID
    cell_id get_cell_id() const;
    
    // Attributes:
    int id = -1;
    int side_id = -1;
    int layer_id = -1;
    int row_id = -1;
    double drift_radius = std::numeric_limits<double>::quiet_NaN();
    double drift_radius_err = std::numeric_limits<double>::quiet_NaN();
    double z = std::numeric_limits<double>::quiet_NaN();
    double z_err = std::numeric_limits<double>::quiet_NaN();
    bool delayed = false;
    
  };

  struct tracker_hit_collection : public std::vector<tracker_hit>
  {
    bool has_hit_with_id(int id_) const;
    bool has_hit_with_cell_id(const cell_id & cell_id_) const;
    bool add_hit(const tracker_hit & hit_);
    int max_hit_id() const;
    void print(std::ostream & out_) const;
  };
  
  struct tracker_hit_drawer
  {
  public:
    tracker_hit_drawer(const tracker_hit & hit_,
                       const tracker & trk_); 
    void draw(std::ostream & out_, uint32_t flags_ = 0);
  private:
    const tracker_hit & _hit_;     
    const tracker & _trk_;
  };

} // end of namespace lttc

#endif // FALAISE_LTTC_PLUGIN_TRACKER_HIT_HH
