/** \file lttc/sngeometry.h
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
#include <lttc/point.hh>

namespace lttc {
 
  /// \brief Cell ID (layer+row numbers)
  struct cell_id
  {
  public:
    cell_id() = default;
    cell_id(int side_, int layer_, int row_);
    int side() const;
    int layer() const;
    int row() const;
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
 
  /// \brief Tracker geometry
  struct tracker
  {
  public:
    tracker();
    bool locate(const point & p_, cell_id & id_) const;
    bool locate(const point & p_, int & iside_, int & ilayer_, int & irow_) const;   
    bool locate(double x_, double y_, int & iside_, int & ilayer_, int & irow_) const;
    bool contains(const point & p_) const;   
    point cell_position(int iside_, int ilayer_, int irow_) const;   
    point cell_position(const cell_id & id_) const;   
    void draw(std::ostream & out_) const;
  public:
    size_t nsides  = 2;
    size_t nlayers = 9;
    size_t nrows   = 113;
    double rcell   = 22.0 * CLHEP::mm;
    double source_xskip = 30.0 * CLHEP::mm;
    double calo_xskip   = 10.0 * CLHEP::mm;
    double xcalo_yskip  = 10.0 * CLHEP::mm;
    double xmin   = 0.0;
    double xmax   = 0.0;
    double ymin   = 0.0;
    double ymax   = 0.0;
    double hcell  = 2700.0 * CLHEP::mm;
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
