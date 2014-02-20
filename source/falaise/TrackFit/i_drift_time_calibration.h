// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/i_drift_time_calibration.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-15
 * Last modified: 2014-02-08
 *
 * Copyright 2012-2014 F. Mauger
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
 *  Custom Geiger drift time calibration
 *
 * History:
 *
 */

#ifndef FALAISE_TRACKFIT_I_DRIFT_TIME_CALIBRATION_H
#define FALAISE_TRACKFIT_I_DRIFT_TIME_CALIBRATION_H 1

// Standard library:
#include <functional>

namespace TrackFit {

  /// \brief Abstract interface for any object for drift time calibration
  struct i_drift_time_calibration
  {
    virtual ~i_drift_time_calibration() {}
    /// Return the fiducial radius of a cell for Geiger avalanche drift
    virtual double get_sensitive_cell_radius() const = 0;
    /// Return the maximum radius of the Geiger cell
    virtual double get_max_cell_radius() const = 0;
    /// Check the validity of a drift time value
    virtual bool   drift_time_is_valid(double drift_time_) const;
    /// Check the validity of a drift radius value
    virtual bool   radius_is_valid(double radius_) const;
    /// Convert the drift time to drift radius
    virtual void   drift_time_to_radius(double time_,
                                        double & radius_,
                                        double & sigma_radius_) const = 0;
    /// Convert the drift radius to drift time
    virtual void   radius_to_drift_time(double radius_,
                                        double & time_,
                                        double & sigma_time_) const = 0;
  };

  /// \brief Functor for drift time to radius conversion
  struct f_time_radius : public std::unary_function<double,double>
  {
    /// \brief Drift time to radius conversion mode
    enum mode_type {
      DRIFT_TIME_TO_RADIUS = 0,
      RADIUS_TO_DRIFT_TIME = 1
    };
    /// Constructor
    f_time_radius();
    /// Set the computing mode to t->r
    void set_drift_time_to_radius();
    /// Set the computing mode to r->t
    void set_radius_to_drift_time();
    /// Overloaded functor operator
    double operator()(double) const;
    // Attributes:
    int mode; /// Computing mode
    double x0, x1, x2, x3, x4, x5; /// Set of internal parameters
    double y0, y1, y2, y3, y4;     /// Set of internal parameters
  };

  /// \brief Default drift time to radius calibration
  struct default_drift_time_calibration : public i_drift_time_calibration
  {
  public:
    /// Constructor
    default_drift_time_calibration();
    /// Return the fiducial radius of a cell for Geiger avalanche drift
    virtual double get_sensitive_cell_radius() const;
    /// Return the maximum radius of the Geiger cell
    virtual double get_max_cell_radius() const;
    /// Convert the drift time to drift radius
    virtual void   drift_time_to_radius(double   time_,
                                        double & radius_,
                                        double & sigma_radius_) const;
    /// Convert the drift radius to drift time
    virtual void   radius_to_drift_time(double   radius_,
                                        double & time_,
                                        double & sigma_time_) const;
    // Attributes:
    double rmax; /// Maximum drift radius
    double sigma_r; /// Default error on drift radius
    f_time_radius td; /// Time to radius conversion functor
    f_time_radius dt; /// Radius to time conversion functor
  };

  /// \brief Alternative drift time to radius calibration
  struct new_drift_time_calibration : public i_drift_time_calibration
  {
  public:
    /// Constructor
    new_drift_time_calibration();
    /// Return the fiducial radius of a cell for Geiger avalanche drift
    virtual double get_sensitive_cell_radius() const;
    /// Return the maximum radius of the Geiger cell
    virtual double get_max_cell_radius() const;
    /// Convert the drift time to drift radius
    virtual void   drift_time_to_radius(double   time_,
                                        double & radius_,
                                        double & sigma_radius_) const;
    /// Convert the drift radius to drift time
    virtual void   radius_to_drift_time(double   radius_,
                                        double & time_,
                                        double & sigma_time_) const;
    // Attributes:
    double rmax; /// Maximum drift radius
    double sigma_r; /// Default error on drift radius
    f_time_radius td; /// Time to radius conversion functor
    f_time_radius dt;/// Radius to time conversion functor
  };

} // end of namespace TrackFit

#endif // FALAISE_TRACKFIT_I_DRIFT_TIME_CALIBRATION_H
