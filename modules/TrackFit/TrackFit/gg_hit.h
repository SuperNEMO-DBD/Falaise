// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/gg_hit.h
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
 *
 *   Geiger hit data in a NEMO-like detector
 *
 * History:
 *
 */

#ifndef FALAISE_TRACKFIT_GG_HIT_H
#define FALAISE_TRACKFIT_GG_HIT_H 1

// Standard library:
#include <iostream>
#include <list>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace TrackFit {

class gg_hit : public datatools::i_tree_dumpable {
 public:
  /// Return the label of for the boolean property associated to prompt geiger hit
  static const std::string& prompt_flag();

  /// Return the label for the boolean property associated to delayed geiger hit
  static const std::string& delayed_flag();

  /// Return the label of the 'last flag' boolean property (for a hit)
  static const std::string& last_flag();

  /// Return the label of the 'first flag' boolean property (for a hit)
  static const std::string& first_flag();

  /// Return the hit Id
  int get_id() const;

  /// Set the hit Id
  void set_id(int);

  /// Check the last flag
  bool is_last() const;

  /// Set the last flag
  void set_last(bool);

  /// Check the first flag
  bool is_first() const;

  /// Set the first flag
  void set_first(bool);

  /// Return the X position
  double get_x() const;

  /// Set the X position
  void set_x(double);

  /// Return the Y position
  double get_y() const;

  /// Set the Y position
  void set_y(double);

  /// Return the Z position
  double get_z() const;

  /// Set the Z position
  void set_z(double);

  /// Return the error on Z position
  double get_sigma_z() const;

  /// Set the error on Z position
  void set_sigma_z(double);

  /// Return the drift time
  double get_t() const;

  /// Set the drift time
  void set_t(double);

  /// Return the maximum radius
  double get_rmax() const;

  /// Set the maximum radius
  void set_rmax(double);

  /// Return the maximum length
  double get_max_length() const;

  /// Set the maximum length
  void set_max_length(double);

  /// Return the drift radius
  double get_r() const;

  /// Set the drift radius
  void set_r(double);

  /// Return the error on drift radius
  double get_sigma_r() const;

  /// Set the error on drift radius
  void set_sigma_r(double);

  /// Check if the reference angle is available
  bool has_phi_ref() const;

  /// Reset the reference angle
  void reset_phi_ref();

  /// Return the reference angle
  double get_phi_ref() const;

  /// Set the reference angle
  void set_phi_ref(double phi_ref_);

  /// Return a non mutable reference on embedded properties container
  const datatools::properties& get_properties() const;

  /// Return a mutable reference on embedded properties container
  datatools::properties& grab_properties();

  /// Constructor
  gg_hit();

  /// Destructor
  virtual ~gg_hit();

  /// Reset
  void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

  /// Default print
  void dump() const;

 protected:
  /// Set default attribute values
  void _set_defaults();

 private:
  int _id_;                            /// Hit Id
  double _x_;                          /// X position
  double _y_;                          /// Y position
  double _z_;                          /// Z position
  double _sigma_z_;                    /// Error on Z position
  double _t_;                          /// Drift time
  double _rmax_;                       /// Radius of the cell
  double _max_length_;                 /// Mength of the cell's anodic wire
  double _r_;                          /// Drift radius
  double _sigma_r_;                    /// Error on drift radius
  double _phi_ref_;                    /// Reference angle
  bool _first_;                        /// Flag for the first cell along a trajectory
  bool _last_;                         /// Flag for the last cell along a trajectory
  datatools::properties _properties_;  /// Auxiliary properties
};

/// Collection of Geiger hits
typedef std::vector<gg_hit> gg_hits_col;

/// Generate draw data for a collection of Geiger hits
void draw_hits(std::ostream& out_, const gg_hits_col& hits_);

}  // end of namespace TrackFit

#endif  // TRACKFIT_GG_HIT_H
