/** \file falaise/snemo/reconstruction/gamma_tracking_driver.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2012-10-07
 * Last modified : 2014-02-09
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
 *   A driver class that wraps the GammaTracking algorithm.
 *
 * History:
 *
 */

#ifndef FALAISE_GAMMA_TRACKING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_TRACKING_DRIVER_H
#define FALAISE_GAMMA_TRACKING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_TRACKING_DRIVER_H 1

// Standard library:
#include <string>

// This project:
#include <falaise/snemo/processing/base_gamma_builder.h>

// Gamma_Tracking library
#include <GammaTracking/gamma_tracking.h>

namespace snemo {

  namespace reconstruction {

    /// Driver for the gamma tracking algorithms
    class gamma_tracking_driver : public ::snemo::processing::base_gamma_builder
    {
    public:

      /// Dedicated algorithm id
      static const std::string & gamma_tracking_id();

      /// Constructor
      gamma_tracking_driver();

      /// Destructor
      virtual ~gamma_tracking_driver();

      /// Initialize the gamma tracker through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();

      /// OCD support:
      static void init_ocd(datatools::object_configuration_description & ocd_);

    protected:

      /// Set default values to class members
      void _set_defaults();

      /// Prepare cluster for processing
      virtual int _prepare_process(const base_gamma_builder::hit_collection_type & calo_hits_,
                                   snemo::datamodel::particle_track_data & ptd_);

      /// Main tracking method
      virtual int _process_algo(const base_gamma_builder::hit_collection_type & calo_hits_,
                                snemo::datamodel::particle_track_data & ptd_);

    private:

      gt::gamma_tracking _gt_; //!< The Gamma Tracking algorithm
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo


// Declare the OCD interface of the module
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snemo::reconstruction::gamma_tracking_driver)

#endif // FALAISE_GAMMA_TRACKING_PLUGIN_SNEMO_RECONSTRUCTION_GAMMA_TRACKING_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
