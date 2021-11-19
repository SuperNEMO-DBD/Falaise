/** \file lttc/lttc_tracker_clustering_module.hh
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2021-11-16
 * Last modified: 2021-11-16
 *
 * Copyright (C) 2021 François Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Module for Geiger hits clustering
 *
 */

#ifndef FALAISE_LTTC_PLUGIN_LTTC_TRACKER_CLUSTERING_MODULE_H
#define FALAISE_LTTC_PLUGIN_LTTC_TRACKER_CLUSTERING_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/dpp:
#include <bayeux/dpp/base_module.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_data.h>

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace datamodel {
    class tracker_clustering_data;
  }

  namespace processing {
    class base_tracker_clusterizer;
  }

  namespace reconstruction {

    /// \brief Tracker clustering module using the LTTC algorithm
    class lttc_tracker_clustering_module
      : public dpp::base_module
    {
    public:
      /// Constructor
      lttc_tracker_clustering_module(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~lttc_tracker_clustering_module();

      /// Set the 'calibrated data' bank label
      void set_cd_label(const std::string &);

      /// Return the 'calibrated data' bank label
      const std::string & get_cd_label() const;

      /// Set the 'tracker clustering data' bank label
      void set_tcd_label(const std::string &);

      /// Return the 'tracker clustering data' bank label
      const std::string & get_tcd_label() const;

      /// Setting geometry manager
      void set_geometry_manager(const geomtools::manager &gm);

      /// Getting geometry manager
      const geomtools::manager & get_geometry_manager() const;

      /// Initialization
      virtual void initialize(const datatools::properties & config,
                              datatools::service_manager & services,
                              dpp::module_handle_dict_type & /* unused */);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things &event);

    protected:
      /// Main process function
      void _process(const snemo::datamodel::calibrated_data &calib_data,
                    snemo::datamodel::tracker_clustering_data &clustering_data);

      /// Give default values to specific class members
      void _set_defaults();

    private:
      const geomtools::manager *geoManager_ = nullptr;  ///< The geometry manager
      std::string CDTag_;                     ///< The label of the input calibrated data bank
      std::string TCDTag_;                    ///< The label of the output tracker clustering data bank
      /// Handle to the embedded clustering algorithm with dynamic memory auto-deletion
      std::unique_ptr<snemo::processing::base_tracker_clusterizer> lttcAlgo_; 

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(lttc_tracker_clustering_module)
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::lttc_tracker_clustering_module)

#endif  // FALAISE_LTTC_PLUGIN_LTTC_TRACKER_CLUSTERING_MODULE_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
