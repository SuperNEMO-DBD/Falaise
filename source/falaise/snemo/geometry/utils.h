/// \file falaise/snemo/geometry/utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2021-11-06
 *
 * License:
 *
 * Copyright 2007-2021 F. Mauger
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
 */

#ifndef FALAISE_SNEMO_GEOMETRY_UTILS_H
#define FALAISE_SNEMO_GEOMETRY_UTILS_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux:
// #include "bayeux/geomtools/visibility.h"
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/bit_mask.h"
#include "bayeux/geomtools/face_intercept_info.h"

// This project
#include "falaise/snemo/datamodels/particle_track.h"

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

    /// Data structure for vertex
    struct vertex_info
    {
      static const int FROM_NONE  = -1; 
      static const int FROM_FIRST =  0; ///< Extrapolation from the first position of the trajectory pattern
      static const int FROM_LAST  =  1; ///< Extrapolation from the last position of the trajectory pattern
      static const int FROM_BEND  =  2; ///< Extrapolation from a bend position along the trajectory pattern
      static std::string from_to_label(const int);
      
      enum from_bit_type
        {
         FROM_LAST_BIT  = datatools::bit_mask::bit00,
         FROM_FIRST_BIT = datatools::bit_mask::bit01,
         FROM_BOTH_BIT  = FROM_LAST_BIT | FROM_FIRST_BIT
        };
      snemo::datamodel::particle_track::vertex_type type = snemo::datamodel::particle_track::VERTEX_NONE;
      int from = FROM_NONE;
      geomtools::geom_id gid;
      geomtools::face_intercept_info face_intercept;
      double distance = datatools::invalid_real();
      double tolerance = datatools::invalid_real();
      void print(std::ostream & out_, const std::string & indent_ = "") const;
    };
    typedef std::vector<vertex_info> vertex_info_list;
  
  }  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
