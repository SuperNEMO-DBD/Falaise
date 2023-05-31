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
#include "bayeux/geomtools/geom_id.h"

// // This project
// #include <falaise/snemo/datamodels/vertex_utils.h>

namespace snemo {

  namespace geometry {

    /// \brief Direction identifier constants in the SuperNEMO reference frame
    enum class direction_t
      {
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
      enum side_t_enum_
        {
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
      enum grid_mask_t_enum_
        {
          NONE = 0x0,           ///< None
          SIDE = 0x1,           ///< S : nearest neighbours (side)
          DIAG = 0x2,           ///< D : nearest diagonal
          FIRST = SIDE | DIAG,  ///< S or D
          SECOND = 0x4          ///< s : second ranked neighbours
        };
    };

    /// \brief Some geometry utility
    class utils
    {
    public:
      static const unsigned int NSIDES = 2;

      /// Return a label representing the "back" side
      static const std::string & side_back_label();

      /// Return a label representing the "front" side
      static const std::string & side_front_label();

      /// Check if a label represents a valid side
      static bool is_side_label_valid(const std::string & label_);

      /// Get the side code from a label
      static int get_side_from_label(const std::string & label_);
    };

    /// Plain data structure for a vertex extrapolated from a curve in 3D space
    struct vertex_info
    {
      static const int FROM_NONE  = -1; 
      static const int FROM_FIRST =  0; ///< Extrapolation from the first position of the trajectory pattern
      static const int FROM_LAST  =  1; ///< Extrapolation from the last position of the trajectory pattern
      static const int FROM_BEND  =  2; ///< Extrapolation from a bend position along the trajectory pattern
      static std::string from_to_label(const int);

      /// Identifier bit of a curve's ends (example: line or helix) from which a vertex must be extrapolated
      enum from_bit_type
        {
          FROM_UNDEF     = 0x0,
          FROM_LAST_BIT  = datatools::bit_mask::bit00,
          FROM_FIRST_BIT = datatools::bit_mask::bit01,
          FROM_BOTH_BIT  = FROM_LAST_BIT | FROM_FIRST_BIT
        };

      /// Identifier bit of a specific extrapolation mode
      enum extrapolation_mode_type
        {
          EXTRAPOLATION_UNDEF = 0x0,
          EXTRAPOLATION_LINE  = datatools::bit_mask::bit00,
          EXTRAPOLATION_HELIX = datatools::bit_mask::bit01,
          EXTRAPOLATION_OTHER = datatools::bit_mask::bit30
        };

      static std::string to_label(const extrapolation_mode_type);

      enum category_type {
        CATEGORY_UNDEF = 0x0, ///< Undefined category
        CATEGORY_ON_SOURCE_FOIL = datatools::bit_mask::bit00, ///< Maybe a source strip, pad or pad bulk physica volume
        CATEGORY_ON_MAIN_CALORIMETER = datatools::bit_mask::bit01, ///< The entrance window of a main calorimeter block (front)
        CATEGORY_ON_X_CALORIMETER = datatools::bit_mask::bit02, ///< The entrance window of a main X-calorimeter block
        CATEGORY_ON_GAMMA_VETO = datatools::bit_mask::bit03, ///< The bulk volume of a gamma veto block
        CATEGORY_ON_WIRE = datatools::bit_mask::bit04, ///< The vicinity of a wire in the tracking chamber
        CATEGORY_ON_CALIBRATION_SOURCE = datatools::bit_mask::bit05, ///< A calibration source
        CATEGORY_ON_SOURCE_GAP = datatools::bit_mask::bit06 ///< A gap between source pads
      };
 
      static std::string to_label(const category_type);

      // Attributes:
      
      category_type category = CATEGORY_UNDEF;
      int from = FROM_NONE;
      extrapolation_mode_type extrapolation_mode = EXTRAPOLATION_UNDEF;
      geomtools::geom_id gid;
      geomtools::face_intercept_info face_intercept;
      double distance = datatools::invalid_real();
      double distance_xy = datatools::invalid_real();
      double tolerance = datatools::invalid_real();
      bool best = false;
      bool edge = false;

      // Utilities:
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      
    };
    
    typedef std::vector<vertex_info> vertex_info_list;
  
  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
