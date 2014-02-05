// -*- mode: c++ ; -*-
/** \file falaise/snemo/geometry/utils.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <geomtools/visibility.h>

namespace snemo {

  namespace geometry {

    /// \brief Some geometry utility
    class utils
    {
    public:

      static const unsigned int NSIDES      = 2;
      static const unsigned int NSUBMODULES = NSIDES;

      /// \brief Direction identifier constants in the SuperNEMO reference frame
      enum direction_index
        {
          DIRECTION_INVALID = -1, //!< Invalid/undefined direction
          DIRECTION_BACK    = 0, //!< Back direction (X-)
          DIRECTION_FRONT   = 1, //!< Front direction (X+)
          DIRECTION_LEFT    = 2, //!< Left direction (Y-)
          DIRECTION_RIGHT   = 3, //!< Right direction (Y+)
          DIRECTION_BOTTOM  = 4, //!< Bottom direction (Z-)
          DIRECTION_TOP     = 5, //!< Top direction (Z+)
        };

      /// \brief Side identifier constants (X axis in the SuperNEMO reference frame)
      enum side_index
        {
          SIDE_INVALID = -1, //!< Invalid/undefined side
          SIDE_BACK    = DIRECTION_BACK, //!< Back side (X-)
          SIDE_FRONT   = DIRECTION_FRONT //!< Front side (X+)
        };

      /// Return a label representing the "back" side
      static const std::string & side_back_label();

      /// Return a label representing the "front" side
      static const std::string & side_front_label();

      /// Check if a label represents a valid side
      static bool is_side_label_valid(const std::string & label_);

      /// Get the side code from a label
      static int get_side_from_label(const std::string & label_);

      /** \brief Neighbourg flag on a 2D-grid
       *  [s][s][s][s][s]
       *  [s][D][S][D][s]
       *  [s][S][x][S][s]
       *  [s][D][S][D][s]
       *  [s][s][s][s][s]
       *
       */
      enum grid_neighbour_mask_type
        {
          NEIGHBOUR_NONE = 0x0,   ///
          NEIGHBOUR_SIDE = 0x1,   /// S : nearest neighbours (side)
          NEIGHBOUR_DIAG = 0x2,   /// D : nearest diagonal
          NEIGHBOUR_FIRST = NEIGHBOUR_SIDE | NEIGHBOUR_DIAG, /// S or D
          NEIGHBOUR_SECOND = 0x4, /// s : second ranked neighbours
        };

    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_UTILS_H

// end of falaise/snemo/geometry/utils.h
