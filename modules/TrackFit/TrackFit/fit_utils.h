// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/fit_utils.h
 * Author(s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                 Arnaud Chapon   <chapon@lpccaen.in2p3.fr>
 * Creation date: 2010-05-19
 * Last modified: 2014-02-08
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
 *   Some fit utilities
 *
 */

#ifndef FALAISE_TRACKFIT_FIT_UTILS_H
#define FALAISE_TRACKFIT_FIT_UTILS_H 1

// Standard library:
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
// - Bayeux/geomtools:
#include <geomtools/clhep.h>

// This project:
#include <TrackFit/gg_hit.h>

namespace TrackFit {

/// \brief Flag type
enum flag_type {
  DEBUG = datatools::bit_mask::bit00,
  DEVEL = datatools::bit_mask::bit01,
  WARNING = datatools::bit_mask::bit02,
  TEST = datatools::bit_mask::bit03
};

/// \brief Geiger hit wrapper
struct gg_hit_info {
  gg_hit_info(size_t max_nb_closest_hits_ = 2);
  void update_closest_hits(const gg_hit *other_hit_, double dist_);
  void dump(std::ostream &out_ = std::clog) const;
  // Attributes:
  int rank;                                           /// Rank
  const gg_hit *hit;                                  /// Handle to a Geiger hit
  size_t max_nb_closest_hits;                         /// Maximum number of closest hits
  std::map<double, const gg_hit *> closest_hits_map;  /// Map closest hits with distance
};

/// Dictionary of Geiger hit information, addressed by hit Id
typedef std::map<int, gg_hit_info> gg_hit_info_dict_type;

/// Build the Dictionary of Geiger hit information from the collection of hits
void build_hit_info_map(const gg_hits_col &hits_, gg_hit_info_dict_type &dict_,
                        size_t max_nb_closest_hits_ = 2);

/// \brief Track fit utilities
class fit_utils {
 public:
  /// Default guess Bottom/Top factor
  static double default_guess_bt_factor();

  /// Default vicinity factor
  static double default_vicinity_factor();

  /// \brief Type of guess hypothesis
  enum guess_hypothesis_type { INVALID_HYPOTHESIS = -1, BOTTOM_HYPOTHESIS = 0, TOP_HYPOTHESIS = 1 };

  /// \brief Type of trust mode
  enum guess_trust_mode {
    GUESS_TRUST_MODE_INVALID = -1,
    GUESS_TRUST_MODE_BARYCENTER = 0,
    GUESS_TRUST_MODE_COUNTER = 1
  };

  /// Not documented yet
  static int compute_guess_trust_barycenter(const gg_hit_info_dict_type &hits_infos_,
                                            const gg_hit &hit_,
                                            const geomtools::vector_3d &hit_pos_,
                                            const geomtools::vector_3d &bottom_pos_,
                                            const geomtools::vector_3d &top_pos_);

  /// Not documented yet
  static int compute_guess_trust_counter(const gg_hits_col &hits_,
                                         const geomtools::vector_3d &hit_pos_,
                                         const geomtools::vector_3d &bottom_pos_,
                                         const geomtools::vector_3d &top_pos_);
  /// Check debug flag
  bool is_debug() const;

  /// Set debug flag
  void set_debug(bool debug_);

  /// Return the vicinity factor
  double get_vicinity_factor() const;

  /// Set the vicinity factor
  void set_vicinity_factor(double factor_);

  /// Reset
  void reset();

  /// Constructor
  fit_utils(bool debug_ = false);

  /// Destructor
  virtual ~fit_utils();

 private:
  bool _debug_;                    ///< Debug flag
  double _vicinity_factor_;        ///< Vicinity factor
  datatools::properties _config_;  ///< Configuration parameters
};

}  // end of namespace TrackFit

#endif  // FALAISE_TRACKFIT_FIT_UTILS_H
