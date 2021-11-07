// falaise/snemo/geometry/helix_intercept.cc
/*
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
 */

// Standard library
#include <cmath>

// Ourselves:
#include <falaise/snemo/geometry/helix_intercept.h>

namespace snemo {

  namespace geometry {
    
    helix_intercept::helix_intercept(const geomtools::helix_3d & h_,
                                     const geomtools::i_shape_3d & sh_,
                                     const geomtools::placement & shape_placement_,
                                     double step_,
                                     double precision_,
                                     datatools::logger::priority verbosity_)
      : _verbosity_(verbosity_)
      , _helix_(h_)
      , _shape_(sh_)
      , _shape_placement_(shape_placement_)
      , _step_(step_)
      , _precision_(precision_)
      , _max_niter_(100)
      , _max_extrapolated_xy_length_(datatools::invalid_real())
    {
      _init_();
    }

    void helix_intercept::_init_()
    {
      double bd_min_thickness = datatools::invalid_real();    
      if (_shape_.has_bounding_data()) {
        const geomtools::bounding_data & bd = _shape_.get_bounding_data();
        bd_min_thickness = bd.get_zmax() - bd.get_zmin();
        if (bd.is_cylinder()) {
          double t = 2 * bd.get_rmax();
          if (t < bd_min_thickness) {
            bd_min_thickness = t;
          }
        } else {
          double t = bd.get_xmax() - bd.get_xmin();
          if (t < bd_min_thickness) {
            bd_min_thickness = t;
          }
          t = bd.get_ymax() - bd.get_ymin();
          if (t < bd_min_thickness) {
            bd_min_thickness = t;
          }
        }
        DT_LOG_DEBUG(_verbosity_, "Bounding data min thickness = " << bd_min_thickness / CLHEP::mm << " mm");
      }
      if (datatools::is_valid(_step_)) {
        if (_step_ < 0.0) {
          if (datatools::is_valid(bd_min_thickness)) {
            if (std::abs(_precision_) > bd_min_thickness) {
              // Force a safe precision computed from the minimal dimension of the shape
              _step_ = 0.05 * bd_min_thickness;
            }
          }
        }
      }
      if (not datatools::is_valid(_step_)) {
        _step_ = 5.0 * CLHEP::mm;
      }
      if (not datatools::is_valid(_precision_)) {
        _precision_ = 1.0 * CLHEP::mm;
      }
      if (_precision_ >= _step_) {
        DT_THROW(std::logic_error, "Precision is larger than step!");
      }
      _max_extrapolated_xy_length_ = 10.0 * CLHEP::cm;
      DT_LOG_DEBUG(_verbosity_, "Step = " << _step_ / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_verbosity_, "Precision = " << _precision_ / CLHEP::mm << " mm");
      DT_LOG_DEBUG(_verbosity_, "Max extrapolated XY length = " << _max_extrapolated_xy_length_ / CLHEP::cm << " cm");
      return;
    }
 
    bool helix_intercept::find_intercept(geomtools::face_intercept_info & fii_,
                                         double & extrapolated_length_,
                                         snemo::geometry::vertex_info::from_bit_type from_bit_)
    {
      fii_.reset();
      datatools::invalidate(extrapolated_length_);
      double hStep   = _helix_.get_step();
      double hRadius = _helix_.get_radius();
      double initDeltaT = _step_ / hypot(hStep, 2 * M_PI * hRadius);
      DT_LOG_DEBUG(_verbosity_, "hStep = " << hStep);
      DT_LOG_DEBUG(_verbosity_, "initDeltaT = " << initDeltaT);
      DT_LOG_DEBUG(_verbosity_, "helix t1   = " << _helix_.get_t1());
      DT_LOG_DEBUG(_verbosity_, "helix t2   = " << _helix_.get_t2());
      double currentT = _helix_.get_t2();
      if (from_bit_ == snemo::geometry::vertex_info::FROM_FIRST_BIT) {
        DT_LOG_DEBUG(_verbosity_, "From first point");
        currentT = _helix_.get_t1();
        initDeltaT *= -1;
      } else {
        DT_LOG_DEBUG(_verbosity_, "From last point");
      }
      int      deltaTFactor = 1;
      uint16_t niters = 0;
      geomtools::face_intercept_info shapeFii;
      geomtools::vector_3d shapeLastImpact;
      geomtools::invalidate(shapeLastImpact);
      double extrapolated_length = 0.0;
      double extrapolated_xy_length = 0.0;
      while (true) {
        DT_LOG_DEBUG(_verbosity_, "Starting loop #" << niters << " : ");
        DT_LOG_DEBUG(_verbosity_, "  currentT = " << currentT);
        DT_LOG_DEBUG(_verbosity_, "  deltaTFactor=" << deltaTFactor);
        double deltaT = initDeltaT / deltaTFactor;
        DT_LOG_DEBUG(_verbosity_, "  deltaT = " << deltaT);
        bool stepFront = true;
        geomtools::vector_3d refPoint     = _helix_.get_point(currentT);
        geomtools::vector_3d refPostPoint = _helix_.get_point(currentT + deltaT);
        geomtools::vector_3d direction = (refPostPoint - refPoint).unit();
        // DT_LOG_DEBUG(_verbosity_, "refPoint     =" << geomtools::to_xyz(refPoint));
        // DT_LOG_DEBUG(_verbosity_, "refPostPoint =" << geomtools::to_xyz(refPostPoint));
        // DT_LOG_DEBUG(_verbosity_, "direction    =" << geomtools::to_xyz(direction));
        // Work in the shape reference frame
        geomtools::vector_3d shapeRefPoint;
        geomtools::vector_3d shapeRefPostPoint;
        geomtools::vector_3d shapeDirection;
        _shape_placement_.mother_to_child(refPoint, shapeRefPoint);
        _shape_placement_.mother_to_child(refPostPoint, shapeRefPostPoint);
        _shape_placement_.mother_to_child_direction(direction, shapeDirection);
        DT_LOG_DEBUG(_verbosity_, "  shapeRefPoint     = " << geomtools::to_xyz(shapeRefPoint));
        DT_LOG_DEBUG(_verbosity_, "  shapeRefPostPoint = " << geomtools::to_xyz(shapeRefPostPoint));
        DT_LOG_DEBUG(_verbosity_, "  shapeDirection    = " << geomtools::to_xyz(shapeDirection));
        shapeFii.reset();
        bool success = _shape_.find_intercept(shapeRefPoint, shapeDirection, shapeFii, _precision_);
        if (success) {
          const geomtools::vector_3d & shapeImpact = shapeFii.get_impact();
          DT_LOG_DEBUG(_verbosity_, "  Found a candidate intercept at " << geomtools::to_xyz(shapeImpact) << " ...");
          double impactDist = (shapeImpact - shapeRefPoint).mag();
          DT_LOG_DEBUG(_verbosity_, "  impactDist = " << impactDist / CLHEP::mm << " mm");
          if (impactDist <= _step_ / deltaTFactor) {
            // We found a possible region of interest for the intercept
            DT_LOG_DEBUG(_verbosity_, "  ... which is in the region of interest");
            stepFront = false;
            if (geomtools::is_valid(shapeLastImpact)) {
              double impactDiff = (shapeLastImpact - shapeImpact).mag();
              DT_LOG_DEBUG(_verbosity_, "  impact to last distance = " << impactDiff / CLHEP::mm << " mm");
              if (impactDiff < _precision_) {
                DT_LOG_DEBUG(_verbosity_, "Break loop because of intercept was found in requested precision.");
                extrapolated_length += impactDist;
                break;
              }
            }
            // Memorizing the impact
            shapeLastImpact = shapeImpact;
          } else {
            DT_LOG_DEBUG(_verbosity_, "  ... but it is too far from the starting point");
            shapeLastImpact = shapeImpact;
          }
        } else {
          // DT_LOG_DEBUG(_verbosity_, "  Did not find a candidate intercept...");
        }
        // Prepare a new step:
        if (stepFront) {
          // We forget the previous found impact (likely an artefact)
          // if (geomtools::is_valid(shapeLastImpact)) {  
          //   // DT_LOG_DEBUG(_verbosity_, "  ... and forget the former candidate intercept.");
          //   geomtools::invalidate(shapeLastImpact);
          // }
          refPoint = refPostPoint;
          currentT += deltaT;
          extrapolated_length += _step_ / deltaTFactor;
          extrapolated_xy_length += 2 * M_PI * deltaT * hRadius;
          // if (! geomtools::is_valid(shapeLastImpact)) {
            // Relax the step factor:
            if (deltaTFactor > 1) {
              deltaTFactor /= 2;
            }
          // }
        } else {
          DT_LOG_DEBUG(_verbosity_, "  Restep with a reduced step...");
          deltaTFactor *= 2;
          shapeFii.reset();
          if (geomtools::is_valid(shapeLastImpact)) {
            geomtools::invalidate(shapeLastImpact);
          }
        }
        if (extrapolated_xy_length > _max_extrapolated_xy_length_) {
          DT_LOG_DEBUG(_verbosity_, "Break loop because the extrapolated XY length=" << extrapolated_xy_length / CLHEP::mm << " is too large.");
          shapeFii.reset();
          break;
        }
        niters++;
        if (niters == _max_niter_) {
          DT_LOG_DEBUG(_verbosity_, "Break loop because of too many iterations.");
          shapeFii.reset();
          break;
        }
      } // while
      if (shapeFii.is_valid()) {
        fii_.set_face_id(shapeFii.get_face_id());
        geomtools::vector_3d impact;
        _shape_placement_.child_to_mother(shapeFii.get_impact(), impact);
        fii_.set_impact(impact);
        DT_LOG_DEBUG(_verbosity_, "Exit on success: ");
        fii_.print(std::cerr, "[debug] ");
        extrapolated_length_ = extrapolated_length;
        DT_LOG_DEBUG(_verbosity_, "  - extrapolated length    = " << extrapolated_length_ / CLHEP::mm << " mm");
        DT_LOG_DEBUG(_verbosity_, "  - extrapolated XY length = " << extrapolated_xy_length / CLHEP::mm << " mm");
      } else {
        DT_LOG_DEBUG(_verbosity_, "Exit on failure.");
      }
      return fii_.is_valid();
    }
    
  }  // namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
