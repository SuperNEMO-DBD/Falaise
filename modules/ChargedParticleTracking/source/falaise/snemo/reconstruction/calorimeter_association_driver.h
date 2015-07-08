// \file falaise/snemo/reconstruction/calorimeter_association_driver.h
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
 *   A driver class that associate a particle track with a set of calorimeter
 *   block.
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H 1

// this project
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

  namespace reconstruction {

    /// \brief Driver for associating particle track with calorimeter hit
    class calorimeter_association_driver
    {
    public:

      /// Name of the property to store calorimeter association flag
      static const std::string & associated_flag();

      /// Return driver id
      static const std::string & get_id();

      /// Setting initialization flag
      void set_initialized(const bool initialized_);

      /// Getting initialization flag
      bool is_initialized() const;

      /// Setting logging priority
      void set_logging_priority(const datatools::logger::priority priority_);

      /// Getting logging priority
      datatools::logger::priority get_logging_priority() const;

      /// Check the geometry manager
      bool has_geometry_manager() const;

      /// Address the geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);

      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Constructor:
      calorimeter_association_driver ();

      /// Destructor:
      ~calorimeter_association_driver ();

      /// Initialize the driver through configuration properties
      void initialize(const datatools::properties & setup_);

      /// Reset the driver
      void reset();

      /// Main driver method
      void process(const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
                   snemo::datamodel::particle_track & particle_);

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

    protected:

      /// Set default values to class members:
      void _set_defaults();

    private:

      /// Find matching calorimeters:
      void _measure_matching_calorimeters_(const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
                                           snemo::datamodel::particle_track & particle_);

    private:

      bool _initialized_;                             //<! Initialize flag
      datatools::logger::priority _logging_priority_; //<! Logging flag
      const geomtools::manager * _geometry_manager_;  //<! The SuperNEMO geometry manager
      double _matching_tolerance_;                    //<! Matching distance between vertex and calorimeter
      bool _use_last_geiger_cell_;                    //<! Use only last geiger cells to find matching calo
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::calorimeter_association_driver)

#endif // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CALORIMETER_ASSOCIATION_DRIVER_H

// end of falaise/snemo/reconstruction/calorimeter_association_driver.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
