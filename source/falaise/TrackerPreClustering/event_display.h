// -*- mode: c++ ; -*-
/** \file falaise/TrackerPreClustering/event_display.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  Randomization of fake GG hits.
 *
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_H
#define FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_H 1

// Standard library:
#include <iostream>
#include <vector>

// This project:
#include "falaise/TrackerPreClustering/gg_hit.h"

namespace TrackerPreClustering {

/// \brief Toy display for Geiger hits in a simplified SuperNEMO like geometry
class event_display {
 public:
  /// \brief Mode
  enum mode_type { prompt = 0x1, delayed = 0x2 };

  /// Set the dimension of the cell
  void set_cell_size(double cell_size_);

  /// Set the number of layers
  void set_nb_layers(unsigned int nb_layers_);

  /// Set the number of rows
  void set_nb_rows(unsigned int nb_rows_);

  /// Default constructor
  event_display();

  /// Display Geiger hits
  template <typename Hit>
  void display_gg_hits(std::ostream &out_, const std::vector<const Hit *> &hits_,
                       uint32_t mode_ = prompt | delayed) const;

  /// Display clusters of Geiger hits
  template <class Hit>
  void display_cluster(std::ostream &out_, const std::vector<const Hit *> &hits_,
                       uint32_t mode_ = prompt | delayed) const;

  /// Plot Geiger hit
  void plot_gg_hit(std::ostream &out_, int id_, double r_, double er_, double x_, double y_,
                   double z_, double ez_, double delayed_time_) const;

  /// Plot clustered object
  void plot_clustered_item(std::ostream &out_, int id_, double x_, double y_, double z_, double ez_,
                           double delayed_time_) const;

  /// Display event
  void display_event();

 private:
  unsigned int _nb_layers_;  /// Number of layers
  unsigned int _nb_rows_;    /// Number of rows
  double _cell_size_;        /// Cell size
};

}  // end of namespace TrackerPreClustering

#include "falaise/TrackerPreClustering/event_display.tpp"

#endif  // FALAISE_TRACKERPRECLUSTERING_EVENT_DISPLAY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
