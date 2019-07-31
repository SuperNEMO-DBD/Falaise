/// \file falaise/snemo/reconstruction/charge_computation_driver.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-11-13
 * Last modified : 2014-06-05
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   A driver class that compute the particle charge given the
 *   detector geometry.
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGE_COMPUTATION_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGE_COMPUTATION_DRIVER_H 1

#include "falaise/property_set.h"

namespace snemo {

namespace datamodel {
class tracker_trajectory;
class particle_track;
}  // namespace datamodel

namespace reconstruction {

/// \brief Electric charge determination driver
class charge_computation_driver {
 public:
  /// Return driver id
  static const std::string& get_id();

  charge_computation_driver() = default;
  explicit charge_computation_driver(const falaise::property_set& ps);

  ~charge_computation_driver() = default;
  charge_computation_driver(const charge_computation_driver&) = default;
  charge_computation_driver& operator=(const charge_computation_driver&) = default;
  charge_computation_driver(charge_computation_driver&&) = default;
  charge_computation_driver& operator=(charge_computation_driver&&) = default;

  /// Main driver method
  void process(const snemo::datamodel::tracker_trajectory& trajectory_,
               snemo::datamodel::particle_track& particle_);

 private:
  bool chargeFromSource_ = true;     //<! Convention flag for charge measurement
  int magneticFieldDirection_ = +1;  //<! Magnetic field direction (+/-1)
};

}  // end of namespace reconstruction

}  // end of namespace snemo

#endif  // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGE_COMPUTATION_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
