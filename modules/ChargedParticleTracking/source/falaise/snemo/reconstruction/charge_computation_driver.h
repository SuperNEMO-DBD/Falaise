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

// Third party:
// - Bayeux/datatools
#include <datatools/logger.h>

namespace datatools {
  class properties;
}

namespace snemo {

  namespace datamodel {
    class tracker_trajectory;
    class particle_track;
  }

  namespace reconstruction {

    /// \brief Electric charge determination driver
    class charge_computation_driver
    {
    public:

      /// Setting initialization flag
      void set_initialized(const bool initialized_);

      /// Getting initialization flag
      bool is_initialized() const;

      /// Setting logging priority
      void set_logging_priority(const datatools::logger::priority priority_);

      /// Getting logging priority
      datatools::logger::priority get_logging_priority() const;

      /// Constructor:
      charge_computation_driver();

      /// Destructor:
      ~charge_computation_driver();

      /// Initialize the driver through configuration properties
      void initialize(const datatools::properties & setup_);

      /// Reset the driver
      void reset();

      /// Main driver method
      void process(const snemo::datamodel::tracker_trajectory & trajectory_,
                   snemo::datamodel::particle_track           & particle_);

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

    protected:

      /// Set default values to class members:
      void _set_defaults();

    private:

      /// Measure particle charge:
      void _measure_particle_charge_(const snemo::datamodel::tracker_trajectory  & trajectory_,
                                     snemo::datamodel::particle_track            & particle_);

    private:

      bool _initialized_;                             //<! Initialize flag
      datatools::logger::priority _logging_priority_; //<! Logging flag
      bool _charge_from_source_;                      //<! Convention flag for charge measurement

    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::charge_computation_driver)

#endif // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_CHARGE_COMPUTATION_DRIVER_H

// end of falaise/snemo/reconstruction/charge_computation_driver.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
