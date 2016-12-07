/// \file falaise/snemo/digitization/digitization_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 *               : Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
 * Creation date : 2016-12-06
 * Last modified : 2014-12-07
 *
 * Copyright (C) 2012-2016 Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
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
 *   A driver class that wraps the Digitization algorithms, trigger and readout.
 *
 * History:
 *
 */

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_DRIVER_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_DRIVER_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/signal/signal_data.h>

// This project:
#include <falaise/snemo/digitization/signal_to_calo_tp_algo.h>
#include <falaise/snemo/digitization/signal_to_geiger_tp_algo.h>
#include <falaise/snemo/digitization/calo_tp_to_ctw_algo.h>
#include <falaise/snemo/digitization/geiger_tp_to_ctw_algo.h>
#include <falaise/snemo/digitization/trigger_algorithm_test_time.h>


namespace snemo {

  namespace digitization {

    /// Driver for the gamma tracking algorithms
    class digitization_driver
    {
    public:

      /// Constructor
      digitization_driver();

      /// Destructor
      virtual ~digitization_driver();

      /// Check the geometry manager
      bool has_geometry_manager() const;

      /// Address the geometry manager
      void set_geometry_manager(const geomtools::manager & gmgr_);

      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Initialize the gamma tracker through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();
      
      /// Check the initialization status
      bool is_initialized() const;

      /// Process digitization algorithm (sd to trigger for the moment)
      void process_digitization_algorithms(const mctools::signal::signal_data & SSD_);

    private:
      
      // Configuration:
      bool _initialized_; //!< Initialization status
      const geomtools::manager * _geometry_manager_;  //!< The SuperNEMO geometry manager

      // Algorithms:
      
      snemo::digitization::signal_to_calo_tp_algo _calo_signal_to_tp_algo_; //!< Calo signal algo to calo trigger primitive
      snemo::digitization::signal_to_geiger_tp_algo _geiger_signal_to_tp_algo_; //!< Geiger signal algo to geiger trigger primitive
      snemo::digitization::calo_tp_to_ctw_algo _calo_tp_to_ctw_algo_; //!< Calo TP to crate trigger word algo
      snemo::digitization::geiger_tp_to_ctw_algo _geiger_tp_to_ctw_algo_; //!< Geiger TP to crate trigger word algo
      snemo::digitization::trigger_algorithm_test_time _trigger_algo_; //!< The trigger alogirhtm
    };

  }  // end of namespace digitization

}  // end of namespace snemo


// Declare the OCD interface of the module
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snemo::digitization::digitization_driver)

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_DIGITIZATION_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
