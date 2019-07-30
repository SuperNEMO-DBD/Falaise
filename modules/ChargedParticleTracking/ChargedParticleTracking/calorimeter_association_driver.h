// \file falaise/snemo/reconstruction/calorimeter_association_driver.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-11-13
 * Last modified : 2015-07-08
 *
 * Copyright (C) 2012-2015 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   A driver class that associate a particle track with a set of calorimeter
 *   block.
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H 1

#include <CLHEP/Units/SystemOfUnits.h>

// this project
#include <falaise/config/property_set.h>
#include <falaise/snemo/datamodels/calibrated_data.h>

namespace datatools {
class properties;
}
namespace geomtools {
class manager;
}
namespace snemo {

namespace datamodel {
class particle_track;
}

namespace geometry {
class locator_plugin;
}

namespace reconstruction {

/// \brief Calorimeter utilities to be stored within calorimeter hit auxiliaries
struct calorimeter_utils {
  /// Name of the property to store calorimeter association flag
  static const std::string& associated_flag();

  /// Name of the property to store calorimeter vicinity flag
  static const std::string& neighbor_flag();

  /// Name of the property to store calorimeter isolation flag
  static const std::string& isolated_flag();

  /// Tag a calorimeter with a given flag
  static void flag_as(const snemo::datamodel::calibrated_calorimeter_hit& hit_,
                      const std::string& flag_);

  /// Check if a calorimeter has a given flag
  static bool has_flag(const snemo::datamodel::calibrated_calorimeter_hit& hit_,
                       const std::string& flag_);
};

/// \brief Driver for associating particle track with calorimeter hit
class calorimeter_association_driver {
 public:
  /// Return driver id
  static const std::string& get_id();

  /// Default constructor
  calorimeter_association_driver() = default;

  /// Construct from configuration and geometry
  calorimeter_association_driver(const falaise::config::property_set& ps,
                                 const geomtools::manager* gm);

  /// Destructor
  ~calorimeter_association_driver() = default;

  /// Copy constructor
  calorimeter_association_driver(const calorimeter_association_driver&) = default;

  /// Copy assignment operator
  calorimeter_association_driver& operator=(const calorimeter_association_driver&) = default;

  /// Move constructor
  calorimeter_association_driver(calorimeter_association_driver&&) = default;

  /// Move assignment operator
  calorimeter_association_driver& operator=(calorimeter_association_driver&&) = default;

  /// Main driver method
  void process(
      const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type& calorimeter_hits_,
      snemo::datamodel::particle_track& particle_);

  /// OCD support:
  static void init_ocd(datatools::object_configuration_description& ocd_);

 private:
  /// Return a valid reference to the geometry manager
  const geomtools::manager& geoManager() const;

  /// Find matching calorimeters:
  void _measure_matching_calorimeters_(
      const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type& calorimeter_hits_,
      snemo::datamodel::particle_track& particle_);

 private:
  datatools::logger::priority logPriority_ = datatools::logger::PRIO_WARNING;  //<! Logging flag
  const geomtools::manager* geoManager_ = nullptr;               //<! The SuperNEMO geometry manager
  const snemo::geometry::locator_plugin* geoLocator_ = nullptr;  //!< The SuperNEMO locator plugin
  double matchTolerance_ = 50 * CLHEP::mm;  //<! Matching distance between vertex and calorimeter
};

}  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::calorimeter_association_driver)

#endif  // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H

// end of falaise/snemo/reconstruction/calorimeter_association_driver.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
