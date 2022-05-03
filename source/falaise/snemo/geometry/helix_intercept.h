/// \file falaise/snemo/geometry/helix_intercept.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2021-11-06
 * Last modified : 2021-11-06
 *
 * Copyright (C) 2021 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Algorithm to search the intercept of an helix with an arbitrary  shape
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_HELIX_INTERCEPT_H
#define FALAISE_SNEMO_GEOMETRY_HELIX_INTERCEPT_H 1

// - Bayeux
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/geomtools/helix_3d.h>
#include <bayeux/geomtools/i_shape_3d.h>
#include <bayeux/geomtools/utils.h>

// This project
#include "falaise/snemo/geometry/utils.h"

namespace snemo {

  namespace geometry {

    /// \brief Helix interception algorithm
    class helix_intercept
    {
    public:

      /// \brief Extrapolation result
      struct extrapolation_info
      {
        void reset();
        geomtools::face_intercept_info fii; ///< Face intercept info object
        double extrapolated_length = datatools::invalid_real(); ///< Length of the extrapolated path along the helix
        double extrapolated_xy_length = datatools::invalid_real(); ///< Length of the extrapolated path along the helix (XY-projection)
      };
     
      helix_intercept(const geomtools::helix_3d & h_,
                      const geomtools::i_shape_3d & sh_,
                      const geomtools::placement & shape_placement_,
                      double step_ = datatools::invalid_real(),
                      double precision_ = datatools::invalid_real(),
                      datatools::logger::priority verbosity_ = datatools::logger::PRIO_FATAL);

      bool find_intercept(extrapolation_info & ei_,
                          snemo::geometry::vertex_info::from_bit_type from_bit_);
      
    private:

      void _init_();
      
      datatools::logger::priority _verbosity_ = datatools::logger::PRIO_FATAL; ///< Verbosity
      const geomtools::helix_3d & _helix_; ///< Reference to the helix pattern
      const geomtools::i_shape_3d & _shape_; ///< Reference to the shape on which the intercept is searched for
      geomtools::placement _shape_placement_; ///< Placement of the shape
      double _step_ = datatools::invalid_real(); ///< Maximal step length of the search algorithm
      double _precision_ = datatools::invalid_real(); ///< Precision of the vertex (criterion for convergence)
      uint16_t _max_niter_ = 100; ///< Maximum number of iterations
      double _max_extrapolated_xy_length_ = datatools::invalid_real(); ///< Maximum extrapolated length on the XY plane

    };

  }  // end of namespace geometry

}  // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_HELIX_INTERCEPTx_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/