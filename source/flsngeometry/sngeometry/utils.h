// -*- mode: c++ ; -*-
/* utils.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 *
 * License:
 *
 * Copyright 2007-2011 F. Mauger
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

#ifndef FLSNGEOMETRY_UTILS_H_
#define FLSNGEOMETRY_UTILS_H_ 1

#include <geomtools/visibility.h>

namespace snemo {

  namespace geometry {

    /// \brief Some geometry utility
    class utils
    {
    public:

      enum direction_index
        {
          DIRECTION_INVALID = -1,
          DIRECTION_BACK    = 0,
          DIRECTION_FRONT   = 1,
          DIRECTION_LEFT    = 2,
          DIRECTION_RIGHT   = 3,
          DIRECTION_BOTTOM  = 4,
          DIRECTION_TOP     = 5,
        };

      enum side_index
        {
          SIDE_INVALID = -1,
          SIDE_BACK    = DIRECTION_BACK,
          SIDE_FRONT   = DIRECTION_FRONT
        };

      static const std::string SIDE_BACK_LABEL;

      static const std::string SIDE_FRONT_LABEL;

      static bool is_side_label_valid (const std::string & label_);

      static int get_side_from_label (const std::string & label_);

    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FLSNGEOMETRY_UTILS_H_

// end of utils.h
