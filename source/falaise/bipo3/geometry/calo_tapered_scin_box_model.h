// -*- mode: c++ ; -*-
/// \file falaise/bipo3/geometry/calo_tapered_scin_box_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-25
 * Last modified: 2015-02-10
 *
 * License:
 *
 * Copyright 2007-2015 F. Mauger
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
 *   A simplified model of a calorimeter scintillator box
 *   extruded using the spherical shape of a PMT entrance
 *   and tapered using a conic shape
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_CALO_TAPERED_SCIN_BOX_MODEL_H
#define FALAISE_SNEMO_GEOMETRY_CALO_TAPERED_SCIN_BOX_MODEL_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/geomtools:
#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/polycone.h>
#include <geomtools/i_wires_drawer.h>

namespace snemo {

  namespace geometry {

    /// \brief The geometry model for SuperNEMO calorimeter tapered scintillator block
    class calo_tapered_scin_box_model : public geomtools::i_model
    {
    public:

      /// Return the mother solid shape
      const geomtools::intersection_3d & get_solid () const;

      /// Return the model identifier
      virtual std::string get_model_id () const;

      /// Default constructor
      calo_tapered_scin_box_model ();

      /// Destructor
      virtual ~calo_tapered_scin_box_model ();

      /// Smart print
      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

      /// \brief  Special Gnuplot wires 3D rendering
      struct wires_drawer : public geomtools::i_wires_drawer<calo_tapered_scin_box_model>
      {
        //! \brief Rendering options
        enum wires_rendering_option_type {
          WR_CTSBM_NO_SCIN_BLOCK = (WR_BASE_LAST << 1), //!< Do not render the scintillator block faces
          WR_CTSBM_NO_EXTRUSION  = (WR_BASE_LAST << 2)  //!< Do not render the extrusion
        };

        //! Constructor
        wires_drawer(const calo_tapered_scin_box_model & model_);

        //! Destructor
        virtual ~wires_drawer();

        //! Generate a list of polylines representing the contour of the shape (for display clients)
        virtual void generate_wires_self(geomtools::wires_type & wires_,
                                         uint32_t options_ = 0) const;
      };

    protected:

      /// Main construction
      virtual void _at_construct(const std::string & name_,
                                 const datatools::properties & setup_,
                                 geomtools::models_col_type * models_ = 0);

    private:

      std::string                 _material_;
      geomtools::sphere           _extrusion_;
      geomtools::box              _mother_box_;
      geomtools::polycone         _removed_cone_;
      geomtools::subtraction_3d   _solid_0_;
      geomtools::intersection_3d  _solid_;
      double                      _w_;
      double                      _x_;
      double                      _y_;
      double                      _z_;
      double                      _h_;
      double                      _r_;
      double                      _tapered_r_;
      double                      _tapered_angle_;
      double                      _optical_glue_thickness_;
      double                      _zt_;
      double                      _ze_;
      double                      _re_;
      double                      _angle_e_;

      boost::scoped_ptr<wires_drawer> _drawer_;

      // registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(calo_tapered_scin_box_model)
    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_CALO_TAPERED_SCIN_BOX_MODEL_H
