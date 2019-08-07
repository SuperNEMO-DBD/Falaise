/// \file falaise/snemo/geometry/utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2014-01-29
 *
 * License:
 *
 * Copyright 2007-2014 F. Mauger
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
 *   Utilities.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_UTILS_H
#define FALAISE_SNEMO_GEOMETRY_UTILS_H 1

// Third party:
// - Bayeux/geomtools:
#include <geomtools/visibility.h>

namespace snemo {

namespace geometry {

/// \brief Direction identifier constants in the SuperNEMO reference frame
enum class direction_t {
  INVALID = -1,  //!< Invalid/undefined direction
  BACK = 0,      //!< Back direction (X-)
  FRONT = 1,     //!< Front direction (X+)
  LEFT = 2,      //!< Left direction (Y-)
  RIGHT = 3,     //!< Right direction (Y+)
  BOTTOM = 4,    //!< Bottom direction (Z-)
  TOP = 5        //!< Top direction (Z+)
};

/// \brief Side identifier constants (X axis in the SuperNEMO reference frame)
/// Strictly speaking should be scoped enumeration, but code doesn't seem to
/// know whether it wants an int or enum, so introduce scope via a struct
/// temporarily
struct side_t {
  enum side_t_enum_ {
    INVALID = -1,  //!< Invalid/undefined side
    BACK = 0,      // direction_t::BACK,   //!< Back side (X-)
    FRONT = 1      // direction_t::FRONT  //!< Front side (X+)
  };
};

/** \brief Neighbourg flag on a 2D-grid
/// Strictly speaking should be scoped enumeration, but code doesn't seem to
/// know whether it wants an int or enum, so introduce scope via a struct
/// temporarily
 *  [s][s][s][s][s]
 *  [s][D][S][D][s]
 *  [s][S][x][S][s]
 *  [s][D][S][D][s]
 *  [s][s][s][s][s]
 *
 */
struct grid_mask_t {
  enum grid_mask_t_enum_ {
    NONE = 0x0,           ///
    SIDE = 0x1,           /// S : nearest neighbours (side)
    DIAG = 0x2,           /// D : nearest diagonal
    FIRST = SIDE | DIAG,  /// S or D
    SECOND = 0x4          /// s : second ranked neighbours
  };
};

/// \brief Some geometry utility
class utils {
 public:
  static const unsigned int NSIDES = 2;

  /// Return a label representing the "back" side
  static const std::string& side_back_label();

  /// Return a label representing the "front" side
  static const std::string& side_front_label();

  /// Check if a label represents a valid side
  static bool is_side_label_valid(const std::string& label_);

  /// Get the side code from a label
  static int get_side_from_label(const std::string& label_);
};

}  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
