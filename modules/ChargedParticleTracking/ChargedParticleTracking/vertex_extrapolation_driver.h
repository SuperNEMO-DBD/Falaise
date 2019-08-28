/// \file falaise/snemo/reconstruction/vertex_extrapolation_driver.h
/* Author(s)     : Sophie Blondel <blondel@lal.in2p3.fr>
                   Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   A driver class that extrapolate track vertices on source foil and
 *   calorimeter walls.
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H 1

// This project
#include "falaise/property_set.h"
#include "falaise/snemo/datamodels/particle_track.h"

namespace geomtools {
class manager;
}
namespace datatools {
class properties;
}

namespace snemo {

namespace datamodel {
class tracker_trajectory;
}

namespace geometry {
class locator_plugin;
}

namespace reconstruction {

/// \brief Vertex extrapolation driver
class vertex_extrapolation_driver {
 public:
  /// Return driver id
  static const std::string& get_id();

  /// Default constructor
  vertex_extrapolation_driver() = default;

  /// Initialize the driver through configuration properties
  vertex_extrapolation_driver(const falaise::property_set& ps,
                              const geomtools::manager* gm);

  // Destructor
  ~vertex_extrapolation_driver() = default;

  vertex_extrapolation_driver(const vertex_extrapolation_driver&) = default;
  vertex_extrapolation_driver& operator=(const vertex_extrapolation_driver&) = default;
  vertex_extrapolation_driver(vertex_extrapolation_driver&&) = default;
  vertex_extrapolation_driver& operator=(vertex_extrapolation_driver&&) = default;

  /// Main driver method
  void process(const snemo::datamodel::tracker_trajectory& trajectory_,
               snemo::datamodel::particle_track& particle_);

  /// OCD support:
  static void init_ocd(datatools::object_configuration_description& ocd_);

 private:
  /// Return a non-mutable reference to the geometry manager
  const geomtools::manager& geoManager() const;

  /// Check reliability of vertices extrapolation given Geiger cells
  void _check_vertices_(const snemo::datamodel::tracker_trajectory& trajectory_);

  /// Measure vertices on the calorimeter walls and source foil
  void _measure_vertices_(const snemo::datamodel::tracker_trajectory& trajectory_,
                          snemo::datamodel::particle_track::vertex_collection_type& vertices_);

 private:
  datatools::logger::priority logPriority_ = datatools::logger::PRIO_WARNING;  //!< Logging priority
  const geomtools::manager* geoManager_ = nullptr;               //!< The SuperNEMO geometry manager
  const snemo::geometry::locator_plugin* geoLocator_ = nullptr;  //!< The SuperNEMO locator plugin
  std::map<std::string, bool> _use_vertices_ = {};               //!< Vertices reliability
};

}  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::vertex_extrapolation_driver)

#endif  // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_VERTEX_EXTRAPOLATION_DRIVER_H

// end of falaise/snemo/reconstruction/vertex_extrapolation_driver.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
