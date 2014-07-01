// -*- mode: c++ ; -*-
/// \file falaise/snemo/geometry/calo_tapered_scin_box_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-25
 * Last modified: 2010-03-25
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
// - Bayeux/geomtools:
#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/polycone.h>

namespace snemo {

  namespace geometry {

    /// \brief The geometry model for SuperNEMO calorimeter tapered scintillator block
    GEOMTOOLS_MODEL_CLASS_DECLARE(calo_tapered_scin_box_model)
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
    protected:

      /// Main construction
      virtual void _at_construct (const std::string & name_,
                                  const datatools::properties & setup_,
                                  geomtools::models_col_type * models_ = 0);
    private:

      /// Gnuplot rendering method
      static void gnuplot_draw_user_function (std::ostream &,
                                              const geomtools::vector_3d &,
                                              const geomtools::rotation_3d &,
                                              const geomtools::i_object_3d &,
                                              void * = 0);

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
      double                      _h_;
      double                      _z_;
      double                      _r_;
      double                      _tapered_r_;
      double                      _tapered_angle_;
      double                      _optical_glue_thickness_;

      // registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(calo_tapered_scin_box_model);
    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_CALO_TAPERED_SCIN_BOX_MODEL_H
