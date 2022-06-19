/** \file lttc/sngeometry.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2021-11-16
 * Last modified: 2021-12-15
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
 *   SuperNEMO demonstrator's simplified geometry for LTTC
 *
 */

#ifndef FALAISE_LTTC_PLUGIN_SNGEOMETRY_HH
#define FALAISE_LTTC_PLUGIN_SNGEOMETRY_HH

// Standard library:
#include <iostream>
#include <set>
#include <vector>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>
#include <geomtools/utils.h>

// This project:
#include <falaise/geometry/point.hh>
#include <falaise/geometry/rectangle.hh>
#include <falaise/snemo/geometry/gg_locator.h>

namespace lttc {
 
  /// \brief Cell ID (layer+row numbers)
  struct cell_id
  {
  public:
    cell_id() = default;
    cell_id(int side_, int layer_, int row_);
    bool is_valid() const;
    int side() const;
    int layer() const;
    int row() const;
    /// \brief Compute some ortho-distance between 2 valid cell IDs on the same side
    ///
    /// cell #1                          dist(cell#1,cell#2) == 5
    ///    *---->+---->+---->+
    ///       1     2     3  | 4
    ///                      v
    ///                      +
    ///                      | 5
    ///                      v
    ///                      *
    ///                   cell_2
    ///
    int distance(const cell_id & other_) const;
    bool operator<(const cell_id & id_) const;
    bool operator==(const cell_id & id_) const;
    friend std::ostream & operator<<(std::ostream & out_, const cell_id & cid_);
  private:
    int _side_  = -1; ///< Side number [0..1]
    int _layer_ = -1; ///< Layer number [0..8]
    int _row_   = -1; ///< Row number [0..112]
  };

  /// \brief Tracker running conditions (dead cells...)
  struct tracker_conditions
  {
  public:
    tracker_conditions() = default;      
    bool has_dead_cell(const cell_id & id_) const;
    void add_dead_cell(const cell_id & id_);
    void remove_dead_cell(const cell_id & id_);
    const std::set<cell_id> & get_dead_cells() const;
    void clear_dead_cells();  
  private:     
    std::set<cell_id> _dead_cells_; ///< List of dead cells    
  };
 
  /// \brief Tracker geometry description and more
  struct tracker
  {
  public:

    tracker(const snemo::geometry::gg_locator & locator_);
    tracker(const snemo::geometry::gg_locator & locator_,
            const tracker_conditions & trackconds_);
    bool has_tracker_conditions() const;
    void set_tracker_conditions(const tracker_conditions & trackconds_);
    const tracker_conditions & get_tracker_conditions() const;
    bool locate(const falaise::geometry::point2 & p_, cell_id & id_) const;
    bool locate(const falaise::geometry::point2 & p_, int & iside_, int & ilayer_, int & irow_) const;   
    bool locate(double x_, double y_, int & iside_, int & ilayer_, int & irow_) const;
    bool contains(const falaise::geometry::point2 & p_) const;   
    falaise::geometry::point2 cell_position(int iside_, int ilayer_, int irow_) const;   
    falaise::geometry::point2 cell_position(const cell_id & id_) const;   
    bool cell_contains(const cell_id & id_,
                       const falaise::geometry::point2 & p_,
                       double tolerance_) const;
    falaise::geometry::rectangle cell_rectangle(const cell_id & id_) const;
    void draw(std::ostream & out_) const;
    
    // Build the list of cell IDs intersected by a linear segment
    bool intersect_segment(const falaise::geometry::point2 & p1_,
                           const falaise::geometry::point2 & p2_,
                           std::set<cell_id> & cids_) const;

  private:
    
    const snemo::geometry::gg_locator * _locator_ = nullptr;
    const tracker_conditions * _trackconds_ = nullptr;
     
  public: 

    size_t nsides  = 0;
    size_t nlayers = 0;
    size_t nrows   = 0;
    double rcell   = 0.0;
    double dcell   = 0.0;
    double hcell  = 0.0;
    double xmin   = 0.0;
    double xmax   = 0.0;
    double ymin   = 0.0;
    double ymax   = 0.0;
    falaise::geometry::rectangle halfCells[2];
    
  };

  struct tracker_conditions_drawer
  {
  public:
    tracker_conditions_drawer(const tracker_conditions & trkcond_, const tracker & trk_); 
    void draw(std::ostream & out_, uint32_t flags_ = 0);
  private:
    const tracker_conditions & _trkcond_;
    const tracker & _trk_;     
  };
  
  struct tracker_drawer
  {
  public:
    tracker_drawer(const tracker & trk_);
    void draw(std::ostream & out_, uint32_t flags_ = 0);
  private:     
    const tracker & _trk_;     
  };
 
} // end of namespace lttc

#endif // FALAISE_LTTC_PLUGIN_SNGEOMETRY_HH
