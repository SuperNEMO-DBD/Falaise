// -*- mode: c++ ; -*-
/// \file snemo/geometry/calo_scin_box_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef FALAISE_SNEMO_GEOMETRY_CALO_SCIN_BOX_MODEL_H
#define FALAISE_SNEMO_GEOMETRY_CALO_SCIN_BOX_MODEL_H 1

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/geomtools:
#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_wires_drawer.h>

namespace snemo {

  namespace geometry {

    /// \brief The geometry model for a SuperNEMO calorimeter block
    class calo_scin_box_model : public geomtools::i_model
    {
    public:

      const std::string & get_material () const;

      const geomtools::subtraction_3d & get_solid () const;

      virtual std::string get_model_id () const;

      /// Default constructor
      calo_scin_box_model ();

      /// Destructor
      virtual ~calo_scin_box_model ();

      /// Smart print
      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

      /// \brief Special Gnuplot wires 3D rendering
      struct wires_drawer : public geomtools::i_wires_drawer
      {
        wires_drawer(const calo_scin_box_model & model_);
        virtual ~wires_drawer();
        virtual void generate_wires(std::ostream & out_,
                                    const geomtools::vector_3d & position_,
                                    const geomtools::rotation_3d & rotation_);
      private:
        const calo_scin_box_model * _model_; //!< Handle to the model
      };

    protected:

      /// Main construction
      virtual void _at_construct (const std::string & name_,
                                  const datatools::properties & setup_,
                                  geomtools::models_col_type * models_ = 0);

    private:

      std::string               _material_;
      geomtools::sphere         _extrusion_;
      geomtools::box            _mother_box_;
      geomtools::subtraction_3d _solid_;
      double                    _h_;
      double                    _optical_glue_thickness_;

      boost::scoped_ptr<wires_drawer> _drawer_;

      // Registration interface :
      GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(calo_scin_box_model);

    };

  } // end of namespace geometry

} // end of namespace snemo

#endif // FALAISE_SNEMO_GEOMETRY_CALO_SCIN_BOX_MODEL_H
