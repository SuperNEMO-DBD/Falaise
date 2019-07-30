/// \file falaise/TrackerPreClustering/event_generator.h
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
 *  Randomization of fake GG hits.
 *
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_EVENT_GENERATOR_H
#define FALAISE_TRACKERPRECLUSTERING_EVENT_GENERATOR_H 1

// Standard library:
#include <iostream>
#include <map>
#include <vector>

// Third party:
// - Boost:

// This is for current crappy Boost 1.55 (and can\'t forward declare their f'ing
// typedef. Needs review for later versions
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/random/mersenne_twister.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// This project:
#include "gg_hit.h"

namespace TrackerPreClustering {
/// \brief Generator of mock events (collections of Geiger hits)
class event_generator {
 public:
  /// Set seed
  void set_seed(long);

  /// Set the cell size
  void set_cell_size(double cell_size_);

  /// Set the number of layers
  void set_nb_layers(unsigned int nb_layers_);

  /// Set the number of rows
  void set_nb_rows(unsigned int nb_rows_);

  /// Default constructor
  event_generator();

  /// Constructor
  event_generator(long seed_);

  /// Check initialization status
  bool is_initialized();

  /// Initialization
  void initialize();

  /// Reset
  void reset();

  /// Generate the mock prompt Geiger hits
  void generate_prompt_gg_hits(std::vector<const gg_hit *> &hits_);

  /// Generate the mock delayed Geiger hits
  void generate_delayed_gg_hits(std::vector<const gg_hit *> &hits_);

  /// Generate an event with many Geiger hits
  void shoot_event(std::vector<const gg_hit *> &hits_);

  /// Build a unique geometry Id (GID) from cell Ids
  int build_gid(int side_, int layer_, int row_) const;

  /// Register a given geometry Id (GID) as prompt or delayed
  void register_gid(int gid_, bool delayed_ = false);

  /// Check if some geometry Id (GID) is registered
  bool has_gid(int gid_) const;

 private:
  unsigned int _nb_layers_;                               /// Number of cell layers
  unsigned int _nb_rows_;                                 /// Number of cell rows
  double _cell_size_;                                     /// Cell size (diameter)
  long _seed_;                                            /// Seed for the embedded PRNG
  boost::scoped_ptr<boost::random::mt19937> _generator_;  /// Embedded PRNG
  std::vector<boost::shared_ptr<gg_hit> > _hits_gc_;      /// Internal collection of Geiger hits
  std::map<int, bool> _gids_;  /// Internal dictionnary of prompt/delayed status
};

}  // end of namespace TrackerPreClustering

#endif  // FALAISE_TRACKERPRECLUSTERING_EVENT_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
