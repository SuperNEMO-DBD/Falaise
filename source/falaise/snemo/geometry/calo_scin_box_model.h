// -*- mode: c++ ; -*-
/* calo_scin_box_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-25
 * Last modified: 2010-03-25
 *
 * License:
 *
 * Description:
 *   A simplified model of a calorimeter scintillator box
 *   extruded using the spherical shape of a PMT entrance
 *   window
 *
 * History:
 *
 */

#ifndef FLSNGEOMETRY_CALO_SCIN_BOX_MODEL_H_
#define FLSNGEOMETRY_CALO_SCIN_BOX_MODEL_H_ 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/gnuplot_draw.h>

namespace snemo {

  namespace geometry {

    GEOMTOOLS_MODEL_CLASS_DECLARE(calo_scin_box_model)
    {
    public:

      const std::string & get_material () const;

      const geomtools::subtraction_3d & get_solid () const;

      virtual std::string get_model_id () const;

      // ctor:
      calo_scin_box_model ();

      // dtor:
      virtual ~calo_scin_box_model ();

      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;
    protected:

      virtual void _at_construct (const std::string & name_,
                                  const datatools::properties & setup_,
                                  geomtools::models_col_type * models_ = 0);
    private:

      static void gnuplot_draw_user_function (std::ostream &,
                                              const geomtools::vector_3d &,
                                              const geomtools::rotation_3d &,
                                              const geomtools::i_object_3d &,
                                              void * = 0);

    private:

      std::string               _material_;
      geomtools::sphere         _extrusion_;
      geomtools::box            _mother_box_;
      geomtools::subtraction_3d _solid_;
      double                    _h_;
      double                    _optical_glue_thickness_;

      // registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(calo_scin_box_model);

    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FLSNGEOMETRY_CALO_SCIN_BOX_MODEL_H_

// end of calo_scin_box_model.h
